/*
** Draw Gadget Class
*/

#include<clib/macros.h>
#include"includes.h"

/*
** Defines
*/

#define DRAW_SetTagArg(tag,id,data)	{ tag.ti_Tag = (ULONG)(id); tag.ti_Data = (ULONG)(data); }
typedef ULONG (*HookFunction)(void);
#define SAVEDS 
#define ASMFUNC __saveds __asm
#define REG(x) register __## x
#define RGB(r,g,b) ((ULONG)((ULONG)r<<16|(ULONG)g<<8|(ULONG)b))

BOOL CyberGraphX=FALSE;

/*
** Prototypes
*/

Class * ASMFUNC GetListViewClass(void);
ULONG ASMFUNC DRAW_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG DRAW_NEW(Class *,Object *,struct opSet *);
ULONG DRAW_DISPOSE(Class *, Object *, Msg);
ULONG DRAW_SET(Class *, Object *, struct opSet *);
ULONG DRAW_GET(Class *,Object *,struct opGet *);
ULONG DRAW_UPDATE(Class *,Object *,struct opUpdate *);
ULONG DRAW_NOTIFY(Class *,Object *,struct opUpdate *);
ULONG DRAW_RENDER(Class *,Object *,struct gpRender *);
ULONG DRAW_GOACTIVE(Class *,Object *,struct gpInput *);
ULONG DRAW_HANDLEINPUT(Class *,Object *,struct gpInput *);
ULONG DRAW_GOINACTIVE(Class *,Object *,struct gpGoInactive *);
void DRAW_GetGadgetRect( Object *,struct GadgetInfo *,struct Rectangle *);
void PutPixel(struct RastPort *,struct ColorPalette *,UWORD,UWORD);
void PutRect(struct RastPort *,struct ColorPalette *,UWORD,UWORD,UWORD,UWORD);

/*
** Variables
*/

struct DRAWData
{
	UWORD dd_Zoom;							// Pixel Size
	UWORD dd_Width,dd_Height;		// Width and Height of BitMap - only when object is creating bitmap
	UWORD dd_Depth;
	UWORD dd_PointX,dd_PointY;	// For mouse pointers
	UWORD dd_Left,dd_Top,dd_OldLeft,dd_OldTop;	// Left and Top visible pixel on edit window
	UWORD dd_VisibleX,dd_VisibleY;	// Visible pixels on edit window
	struct ColorPalette *dd_Pens;		// Color conversion and palette
	ULONG dd_ActualPen;					// Actual pen num (2-8bpp) or color (15-24bpp)
	struct BitMap *dd_BitMap;		// BitMap to edit on
	struct RastPort dd_RastPort;// Raster port of edit bitmap
	BOOL dd_Border;							// Show border frame around gadget
	BOOL dd_SetPoint;						// For mouse pointers
	BOOL dd_ReadOnly;						// Is read only
	BOOL dd_Grid;								// Show grid
	BOOL dd_UseCGX;							// Is bitmap cybergraphics compatible?
	BOOL dd_OwnBitMap;					// Bitmap is created by object
	Object *dd_FrameImage;
	BOOL dd_ActiveFromMouse;
	BOOL dd_ShowPoint;					// Show point on zoomed images
};

/*
** Create Class
*/


Class *CreateDrawClass()
{
	Class *cl=NULL;

	if(cl = MakeClass (NULL, GADGETCLASS, NULL, sizeof(struct DRAWData), 0L))
	{
		cl->cl_Dispatcher.h_Entry = (HookFunction)  DRAW_Dispatcher;
		return(cl);
	}
	return(NULL);
}

void DisposeDrawClass(Class *cl)
{
	FreeClass(cl);
}

/*
** Dispatcher
*/

ULONG ASMFUNC DRAW_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = DRAW_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = DRAW_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = DRAW_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = DRAW_GET(cl, o, (struct opGet *)msg );
			break;
		case OM_UPDATE:
			retval = DRAW_UPDATE(cl, o, (struct opUpdate *)msg );
			break;
		case OM_NOTIFY:
			retval = DRAW_NOTIFY(cl, o, (struct opUpdate *)msg );
			break;
		case GM_RENDER:
			retval = DRAW_RENDER(cl, o, (struct gpRender *)msg );
			break;
		case GM_GOACTIVE:
			retval = DRAW_GOACTIVE(cl, o, (struct gpInput *)msg );
			break;
		case GM_HANDLEINPUT:
			retval = DRAW_HANDLEINPUT(cl, o, (struct gpInput *)msg );
			break;
		case GM_GOINACTIVE:
			retval = DRAW_GOINACTIVE(cl, o, (struct gpGoInactive *)msg );
			break;
		default:
			retval = DoSuperMethodA(cl, o, msg);
			break;
	}
	return(retval);
}

/*
** OM_NEW
*/

ULONG SAVEDS DRAW_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct DRAWData *DD;

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		DD = INST_DATA( cl, object );

		if(DD->dd_BitMap = (struct BitMap *)GetTagData( DRAW_BitMap, NULL, ops->ops_AttrList ))
		{
			DD->dd_OwnBitMap=FALSE;
			DD->dd_Width = (UWORD) GetTagData(DRAW_Width,GetBitMapAttr(DD->dd_BitMap,BMA_WIDTH),ops->ops_AttrList);
			DD->dd_Height = (UWORD) GetBitMapAttr(DD->dd_BitMap,BMA_HEIGHT);
			DD->dd_Depth = (UWORD) GetBitMapAttr(DD->dd_BitMap,BMA_DEPTH);
		}
		else // Bitmap not supplied, create own bitmap
		{
			DD->dd_OwnBitMap=TRUE;
			DD->dd_Width = GetTagData( DRAW_Width, 16, ops->ops_AttrList );
			DD->dd_Height = GetTagData( DRAW_Height, 16, ops->ops_AttrList );
			DD->dd_Depth = GetTagData( DRAW_Depth, 8, ops->ops_AttrList );
			unless(DD->dd_BitMap=AllocBitMap(DD->dd_Width,DD->dd_Height,DD->dd_Depth,BMF_CLEAR,NULL))
			{
				CoerceMethod(cl, o, OM_DISPOSE);
				object = NULL;
				return( (ULONG)object );
			}
		}
		if(DD->dd_Depth>8) DD->dd_UseCGX=TRUE; else DD->dd_UseCGX=FALSE;

		DD->dd_Zoom = GetTagData( DRAW_Zoom, 1, ops->ops_AttrList );
		DD->dd_ShowPoint = GetTagData( DRAW_ShowPoint, FALSE, ops->ops_AttrList );
		DD->dd_Pens = (struct ColorPalette *)GetTagData( DRAW_Palette, NULL, ops->ops_AttrList );
		DD->dd_Border = GetTagData( DRAW_Border, FALSE, ops->ops_AttrList );
		DD->dd_ReadOnly = GetTagData( DRAW_ReadOnly, FALSE, ops->ops_AttrList );
		DD->dd_Grid = GetTagData( DRAW_Grid, FALSE, ops->ops_AttrList );
		DD->dd_ActualPen = GetTagData( DRAW_ActualPen, DD->dd_UseCGX?RGB(0,0,0):1, ops->ops_AttrList );
		DD->dd_OldLeft = DD->dd_Left = GetTagData( DRAW_Left, 0, ops->ops_AttrList );
		DD->dd_OldTop = DD->dd_Top = GetTagData( DRAW_Top, 0, ops->ops_AttrList );

		InitRastPort(&DD->dd_RastPort);
		DD->dd_RastPort.BitMap=DD->dd_BitMap;

		if(DD->dd_Border)
		{
			unless(DD->dd_FrameImage=NewObject(NULL,"frameiclass",
				IA_Recessed,    DD->dd_ReadOnly,
				IA_EdgesOnly,   TRUE,
				IA_FrameType,   FRAME_BUTTON,
				TAG_END))
			{
				CoerceMethod(cl, o, OM_DISPOSE);
				object = NULL;
				return( (ULONG)object );
			}
		}
	}
	return( (ULONG)object );
}

/*
** OM_DISPOSE
*/

ULONG SAVEDS DRAW_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct DRAWData *DD = INST_DATA( cl, o );

	if(DD->dd_OwnBitMap) if(DD->dd_BitMap) FreeBitMap(DD->dd_BitMap);
	if(DD->dd_FrameImage) DisposeObject(DD->dd_FrameImage);
	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS DRAW_SET(Class *cl, Object *o, struct opSet *ops)
{
	ULONG retval;
	struct DRAWData *DD = INST_DATA( cl, o );
	struct TagItem *tag, notify;
	struct RastPort *rp;

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	if(ops->ops_AttrList)
	{
		if(tag = FindTagItem( DRAW_Border, ops->ops_AttrList ))
		{
			DD->dd_Border = (BOOL) tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_Zoom, ops->ops_AttrList ))
		{
			DD->dd_Zoom = (UWORD) tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_BitMap, ops->ops_AttrList ))
		{
			DD->dd_BitMap = (struct BitMap *)tag->ti_Data;
			DD->dd_Width = (UWORD) GetTagData(DRAW_Width,GetBitMapAttr(DD->dd_BitMap,BMA_WIDTH),ops->ops_AttrList);
			DD->dd_Height = (UWORD) GetBitMapAttr(DD->dd_BitMap,BMA_HEIGHT);
			DD->dd_Depth = (UWORD) GetBitMapAttr(DD->dd_BitMap,BMA_DEPTH);
			InitRastPort(&DD->dd_RastPort);
			DD->dd_RastPort.BitMap=DD->dd_BitMap;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_Palette, ops->ops_AttrList ))
		{
			DD->dd_Pens = (struct ColorPalette *)tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_Width, ops->ops_AttrList ))
		{
			if(DD->dd_OwnBitMap) DD->dd_Width = (UWORD)tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_Height, ops->ops_AttrList ))
		{
			if(DD->dd_OwnBitMap) DD->dd_Height = (UWORD)tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_Depth, ops->ops_AttrList ))
		{
			if(DD->dd_OwnBitMap) DD->dd_Depth = (UWORD)tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_Palette, ops->ops_AttrList ))
		{
			DD->dd_Pens = (struct ColorPalette *)tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_ActualPen, ops->ops_AttrList ))
		{
			DD->dd_ActualPen = (ULONG) tag->ti_Data;
			if(DD->dd_Depth<9) SetAPen(&DD->dd_RastPort,(UBYTE)DD->dd_ActualPen);
			retval = TRUE;
		}

		if(tag = FindTagItem( DRAW_SetPoint, ops->ops_AttrList ))
		{
			DD->dd_SetPoint = (BOOL) tag->ti_Data;
			retval = TRUE;
		}

		if(tag = FindTagItem( DRAW_PointX, ops->ops_AttrList ))
		{
			DD->dd_PointX = (UWORD) tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_PointY, ops->ops_AttrList ))
		{
			DD->dd_PointY = (UWORD) tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_ReadOnly, ops->ops_AttrList ))
		{
			DD->dd_ReadOnly = (BOOL) tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_Grid, ops->ops_AttrList ))
		{
			DD->dd_Grid = (BOOL) tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_Left, ops->ops_AttrList ))
		{
			DD->dd_OldLeft = DD->dd_Left;
			DD->dd_Left = (UWORD) tag->ti_Data;
			retval = FALSE;
		}

		if(tag = FindTagItem( DRAW_Top, ops->ops_AttrList ))
		{
			DD->dd_OldTop = DD->dd_Top;
			DD->dd_Top = (UWORD) tag->ti_Data;
			retval = FALSE;
		}

		DRAW_SetTagArg( notify, TAG_END, NULL );
		DoMethod( o, OM_NOTIFY, &notify, ops->ops_GInfo, 0 );

		if(!retval)
		{
			if(rp=ObtainGIRPort( ops->ops_GInfo ))
			{
				DoMethod(o,GM_RENDER,ops->ops_GInfo, rp, GREDRAW_UPDATE);
				ReleaseGIRPort(rp);
			}
		}
	}

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS DRAW_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval;
	struct DRAWData *DD = INST_DATA( cl, o );

	switch( opg->opg_AttrID )
	{
		case DRAW_BitMap:
			*opg->opg_Storage = (ULONG)DD->dd_BitMap;
			retval = TRUE;
			break;
		case DRAW_RastPort:
			*opg->opg_Storage = (ULONG)((ULONG *)&(DD->dd_RastPort));
			retval = TRUE;
			break;
		case DRAW_Depth:
			*opg->opg_Storage = (ULONG)DD->dd_Depth;
			retval = TRUE;
			break;
		case DRAW_Width:
			*opg->opg_Storage = (ULONG)DD->dd_Width;
			retval = TRUE;
			break;
		case DRAW_Height:
			*opg->opg_Storage = (ULONG)DD->dd_Height;
			retval = TRUE;
			break;
		case DRAW_Zoom:
			*opg->opg_Storage = (ULONG)DD->dd_Zoom;
			retval = TRUE;
			break;
		case DRAW_Border:
			*opg->opg_Storage = (ULONG)DD->dd_Border;
			retval = TRUE;
			break;
		case DRAW_ActualPen:
			*opg->opg_Storage = (ULONG)DD->dd_ActualPen;
			retval = TRUE;
			break;
		case DRAW_ReadOnly:
			*opg->opg_Storage = (ULONG)DD->dd_ReadOnly;
			retval = TRUE;
			break;
		case DRAW_Grid:
			*opg->opg_Storage = (ULONG)DD->dd_Grid;
			retval = TRUE;
			break;
		case DRAW_PointX:
			*opg->opg_Storage = (ULONG)DD->dd_PointX;
			retval = TRUE;
			break;
		case DRAW_PointY:
			*opg->opg_Storage = (ULONG)DD->dd_PointY;
			retval = TRUE;
			break;
		case DRAW_VisibleX:
			*opg->opg_Storage = (ULONG)DD->dd_VisibleX;
			retval = TRUE;
			break;
		case DRAW_VisibleY:
			*opg->opg_Storage = (ULONG)DD->dd_VisibleY;
			retval = TRUE;
			break;
		case DRAW_Left:
			*opg->opg_Storage = (ULONG)DD->dd_Left;
			retval = TRUE;
			break;
		case DRAW_Top:
			*opg->opg_Storage = (ULONG)DD->dd_Top;
			retval = TRUE;
			break;
		default:
			retval = DoSuperMethodA(cl, o, (Msg)opg);
			break;
	}
	return( retval );
}

/*
** OM_UPDATE
*/

ULONG SAVEDS DRAW_UPDATE(Class *cl,Object *o,struct opUpdate *opu )
{
	ULONG retval;
	struct DRAWData *DD = INST_DATA( cl, o );
	struct TagItem *tag, notify;
	struct RastPort *rp;

	retval = DoSuperMethodA(cl, o, (Msg) opu);

	if( opu->opu_AttrList )
	{
		if(tag = FindTagItem( DRAW_Top, opu->opu_AttrList ))
		{
			if( tag->ti_Data != DD->dd_Top)
			{
				DD->dd_Top = tag->ti_Data;

				rp = ObtainGIRPort( opu->opu_GInfo );
				if( rp )
				{
					DoMethod( o, GM_RENDER, opu->opu_GInfo, rp, GREDRAW_UPDATE );
					ReleaseGIRPort( rp );
				}

				/* Notify the change. */
				DRAW_SetTagArg( notify, TAG_END, NULL );
				(void)DoMethod( o, OM_NOTIFY, &notify, opu->opu_GInfo, 0 );
			}
		}
		if(tag = FindTagItem( DRAW_Left, opu->opu_AttrList ))
		{
			if( tag->ti_Data != DD->dd_Left)
			{
				DD->dd_Left= tag->ti_Data;

				rp = ObtainGIRPort( opu->opu_GInfo );
				if( rp )
				{
					DoMethod( o, GM_RENDER, opu->opu_GInfo, rp, GREDRAW_UPDATE );
					ReleaseGIRPort( rp );
				}

				/* Notify the change. */
				DRAW_SetTagArg( notify, TAG_END, NULL );
				(void)DoMethod( o, OM_NOTIFY, &notify, opu->opu_GInfo, 0 );
			}
		}
	}
	return( retval );
}

ULONG SAVEDS DRAW_NOTIFY(Class *cl,Object *o,struct opUpdate *opu )
{
	struct TagItem tags[6];
	struct DRAWData *DD = INST_DATA( cl, o );

	DRAW_SetTagArg(tags[0], GA_ID, ((struct Gadget *)o)->GadgetID);
	DRAW_SetTagArg(tags[1], DRAW_BitMap, DD->dd_BitMap);
	DRAW_SetTagArg(tags[2], DRAW_Zoom, DD->dd_Zoom);
	DRAW_SetTagArg(tags[3], DRAW_Left, DD->dd_Left);
	DRAW_SetTagArg(tags[4], DRAW_Top, DD->dd_Top);

	if( opu->opu_AttrList == NULL )
	{
		DRAW_SetTagArg(tags[5], TAG_END, NULL);
	}
	else DRAW_SetTagArg(tags[5], TAG_MORE, opu->opu_AttrList );

	return( DoSuperMethod(cl, o, OM_NOTIFY, tags, opu->opu_GInfo, opu->opu_Flags) );
}

ULONG SAVEDS DRAW_RENDER(Class *cl,Object *o,struct gpRender *gpr )
{
	ULONG retval,State,x,y,movex,movey;
	struct Gadget *gad = (struct Gadget *)o;
	struct Rectangle rect;
	struct DrawInfo *dri;
	struct IBox container;
	struct RastPort *rp = gpr->gpr_RPort;
	struct DRAWData *DD = INST_DATA( cl, o );
	UWORD patterndata[2] = { 0x2222, 0x8888 };
	ULONG TextPen, FillPen, BackgroundPen, HLTextPen,vx,vy;
	BOOL Grid;

	retval = DoSuperMethodA(cl, o, (Msg) gpr);

	DRAW_GetGadgetRect( o, gpr->gpr_GInfo, &rect );

	container.Left = rect.MinX; container.Top = rect.MinY;
	container.Width = 1 + rect.MaxX - rect.MinX;
	container.Height = 1 + rect.MaxY - rect.MinY;

	dri = gpr->gpr_GInfo->gi_DrInfo;

	if(gad->Flags & GFLG_DISABLED)
		State = IDS_DISABLED;
	else
    State = IDS_NORMAL;

	if(DD->dd_Border)
	{
		SetAttrs( DD->dd_FrameImage,
			IA_Left,    container.Left,
			IA_Top,     container.Top,
			IA_Width,   container.Width,
			IA_Height,  container.Height,
			TAG_END);
		DrawImageState( rp, (struct Image *)DD->dd_FrameImage, 0, 0, IDS_NORMAL, dri);
	}

	if( dri )
	{
		BackgroundPen = dri->dri_Pens[BACKGROUNDPEN];
		TextPen = dri->dri_Pens[TEXTPEN];
		HLTextPen = dri->dri_Pens[HIGHLIGHTTEXTPEN];
		FillPen = dri->dri_Pens[FILLPEN];
	}
	else
	{
		BackgroundPen = 0;
 		TextPen = 1;
		HLTextPen = 2;
		FillPen = 3;
	}

	if(DD->dd_Zoom==1)
	{
		Grid=DD->dd_Grid?2:1;
		vx=DD->dd_VisibleX=(container.Width-4)/(Grid);
		vy=DD->dd_VisibleY=(container.Height-4)/(Grid);
	}
	else
	{
		Grid=DD->dd_Grid?1:0;
		vx=DD->dd_VisibleX=(container.Width-4)/(DD->dd_Zoom+Grid);
		vy=DD->dd_VisibleY=(container.Height-4)/(DD->dd_Zoom+Grid);
	}
	if(DD->dd_VisibleX>DD->dd_Width) DD->dd_VisibleX=DD->dd_Width;
	if(DD->dd_VisibleY>DD->dd_Height) DD->dd_VisibleY=DD->dd_Height;

	if(DD->dd_Top>DD->dd_Height-DD->dd_VisibleY) DD->dd_Top=DD->dd_Height-DD->dd_VisibleY;
	if(DD->dd_Left>DD->dd_Width-DD->dd_VisibleX) DD->dd_Left=DD->dd_Width-DD->dd_VisibleX;

	if(vx>=DD->dd_Width)
	{
		SetDrMd(rp,JAM2);
		SetAfPt(rp, patterndata, 1);
		SetAPen(rp,dri->dri_Pens[SHADOWPEN]);
		SetBPen(rp,dri->dri_Pens[BACKGROUNDPEN]);

		RectFill(rp,  container.Left+(DD->dd_Width)*(DD->dd_Zoom+Grid)+2,
			container.Top+2,
			container.Left + container.Width-3,
			container.Top + container.Height-3);
		SetAfPt(rp, NULL, 0 );
	}

	if(vy>=DD->dd_Height)
	{
		SetDrMd(rp,JAM2);
		SetAfPt(rp, patterndata, 1);
		SetAPen(rp,dri->dri_Pens[SHADOWPEN]);
		SetBPen(rp,dri->dri_Pens[BACKGROUNDPEN]);

		RectFill(rp,  container.Left+2,
			container.Top+(DD->dd_Height)*(DD->dd_Zoom+Grid)+2,
			container.Left + container.Width-3,
			container.Top + container.Height-3);
		SetAfPt(rp, NULL, 0 );
	}

	switch(gpr->gpr_Redraw)
	{
		case GREDRAW_UPDATE:
			movex=DD->dd_Left-DD->dd_OldLeft;
			movey=DD->dd_Top-DD->dd_OldTop;
			if(DD->dd_Zoom==1)
			{
				ScrollRaster(rp,movex,movey,
											container.Left+2, container.Top+2,
											container.Left+2+DD->dd_VisibleX, container.Top+2+DD->dd_VisibleY);
			}
			else
			{
				ScrollRaster(rp,movex*(DD->dd_Zoom+Grid),movey*(DD->dd_Zoom+Grid),
											container.Left+2, container.Top+2,
											container.Left+2+DD->dd_VisibleX*(DD->dd_Zoom+Grid), container.Top+2+DD->dd_VisibleY*(DD->dd_Zoom+Grid));
			}
			if(movex!=0 && (DD->dd_OldLeft!=DD->dd_Left))
			for(x=(DD->dd_OldLeft>DD->dd_Left)?DD->dd_Left:((movex<DD->dd_VisibleX)?(DD->dd_Left-movex+DD->dd_VisibleX):DD->dd_Left);x<((DD->dd_OldLeft>DD->dd_Left)?(DD->dd_OldLeft):(DD->dd_Left+DD->dd_VisibleX)) && x<(DD->dd_Left+DD->dd_VisibleX);x++)
			{
				for(y=DD->dd_Top;y<(DD->dd_Top+DD->dd_VisibleY);y++)
				{
					SetAPen(rp,(UBYTE) DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)].cp_I);
					if(DD->dd_Zoom==1)
						PutPixel(rp,&DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)],(x-DD->dd_Left)*Grid+container.Left+2,(y-DD->dd_Top)*Grid+container.Top+2);
					else
						PutRect(rp,&DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)],
											((x-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2,
											((y-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top +2,
											DD->dd_Zoom-1,DD->dd_Zoom-1);
				}
			}

			if(movey!=0 && (DD->dd_OldTop!=DD->dd_Top))
			for(x=DD->dd_Left;x<(DD->dd_Left+DD->dd_VisibleX);x++)
			{
				for(y=(DD->dd_OldTop>DD->dd_Top)?(DD->dd_Top):((movey<DD->dd_VisibleY)?(DD->dd_Top-movey+DD->dd_VisibleY):DD->dd_Top);y<((DD->dd_OldTop>DD->dd_Top)?(DD->dd_OldTop):(DD->dd_Top+DD->dd_VisibleY)) && y<(DD->dd_Top+DD->dd_VisibleY);y++)
				{
					SetAPen(rp,(UBYTE)DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)].cp_I);
					if(DD->dd_Zoom==1)
						PutPixel(rp,&DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)],(x-DD->dd_Left)*Grid+container.Left+2,(y-DD->dd_Top)*Grid+container.Top+2);
					else
						PutRect(rp,&DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)],
											((x-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2,
											((y-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top +2,
											DD->dd_Zoom-1,DD->dd_Zoom-1);
				}
			}
			break;
		case GREDRAW_REDRAW:
			for(x=DD->dd_Left;x<(DD->dd_Left+DD->dd_VisibleX);x++)
			{
				for(y=DD->dd_Top;y<(DD->dd_Top+DD->dd_VisibleY);y++)
				{
					SetAPen(rp,(UBYTE)DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)].cp_I);
					if(DD->dd_Zoom==1)
						PutPixel(rp,&DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)],(x-DD->dd_Left)*Grid+container.Left+2,(y-DD->dd_Top)*Grid+container.Top+2);
					else
						PutRect(rp,&DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)],
											((x-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2,
											((y-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top +2,
											DD->dd_Zoom-1,DD->dd_Zoom-1);
				}
			}
			break;
		case GREDRAW_TOGGLE:
			break;
	}
	DD->dd_OldLeft=DD->dd_Left;
	DD->dd_OldTop=DD->dd_Top;
	if(DD->dd_Zoom!=1 && DD->dd_PointX>DD->dd_Left && DD->dd_PointY>DD->dd_Top)
	{
		SetAPen(rp,2);
		Move(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2-1,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2-1);
		Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2+DD->dd_Zoom,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2-1);
		Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2+DD->dd_Zoom,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2+DD->dd_Zoom);
		Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2-1,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2+DD->dd_Zoom);
		Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2-1,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2-1);
	}

	if(State==IDS_DISABLED)
	{
		container.Width = MAX( 1, container.Width );
		container.Height = MAX( 1, container.Height );

		SetDrMd(rp,JAM1);
		SetAfPt(rp, patterndata, 1);
		SetAPen(rp,dri->dri_Pens[SHADOWPEN]);

		RectFill(rp,  (LONG)container.Left, (LONG)container.Top,
			container.Left + container.Width-1,
			container.Top + container.Height-1);
		SetAfPt(rp, NULL, 0 );
	}

	return( retval );
}

ULONG SAVEDS DRAW_GOACTIVE(Class *cl,Object *o,struct gpInput *gpi )
{
	ULONG retval = GMR_MEACTIVE, x, y;
	struct RastPort *rp;
	struct DRAWData *DD = INST_DATA( cl, o );
	struct GadgetInfo *gi = gpi->gpi_GInfo;
	struct Gadget *gad = (struct Gadget *)o;
	struct Rectangle rect;
	struct IBox container;
	struct TagItem notify;
	BOOL Grid;

	/* Test if we are disabled. */
	if((gad->Flags & GFLG_DISABLED) || DD->dd_ReadOnly) return( GMR_NOREUSE );

	Grid=DD->dd_Grid?1:0;

	if(!((gpi->gpi_Mouse.X-gi->gi_Domain.Left)>2 && (gpi->gpi_Mouse.Y-gi->gi_Domain.Top)>2 &&
			(gpi->gpi_Mouse.X-gi->gi_Domain.Left-2)<(DD->dd_VisibleX*(DD->dd_Zoom+Grid)) &&
			(gpi->gpi_Mouse.Y-gi->gi_Domain.Top-2)<(DD->dd_VisibleY*(DD->dd_Zoom+Grid)))) return( GMR_NOREUSE );

	/* Call first our parent class. */
	DoSuperMethodA(cl, o, (Msg)gpi);

	DRAW_GetGadgetRect( o, gpi->gpi_GInfo, &rect );
	container.Left = rect.MinX; container.Top = rect.MinY;
	container.Width = 1 + rect.MaxX - rect.MinX;
	container.Height = 1 + rect.MaxY - rect.MinY;

	/* Chech whether we were activated from mouse or keyboard. */
	DD->dd_ActiveFromMouse = (gpi->gpi_IEvent != NULL);

	/* Select this gadget. */
	gad->Flags |= GFLG_SELECTED;


	if(rp=ObtainGIRPort(gi))
	{
		x = ((gpi->gpi_Mouse.X-gi->gi_Domain.Left-2)/(DD->dd_Zoom+Grid))+DD->dd_Left;
		y = ((gpi->gpi_Mouse.Y-gi->gi_Domain.Top-2)/(DD->dd_Zoom+Grid))+DD->dd_Top;

		if(DD->dd_Zoom==1) Grid=DD->dd_Grid?2:1;
		else Grid=DD->dd_Grid?1:0;

		if(DD->dd_SetPoint)
		{
			if(DD->dd_Zoom!=1 && DD->dd_PointX>DD->dd_Left && DD->dd_PointY>DD->dd_Top)
			{
				SetAPen(rp,0);
				Move(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2-1,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2-1);
				Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2+DD->dd_Zoom,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2-1);
				Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2+DD->dd_Zoom,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2+DD->dd_Zoom);
				Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2-1,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2+DD->dd_Zoom);
				Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2-1,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2-1);
			}
			if((y*DD->dd_Width+x)<(DD->dd_Width*DD->dd_Height))
			{
				DD->dd_PointX=x;
				DD->dd_PointY=y;
			}
			if(DD->dd_Zoom!=1 && DD->dd_PointX>DD->dd_Left && DD->dd_PointY>DD->dd_Top)
			{
				SetAPen(rp,2);
				Move(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2-1,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2-1);
				Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2+DD->dd_Zoom,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2-1);
				Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2+DD->dd_Zoom,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2+DD->dd_Zoom);
				Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2-1,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2+DD->dd_Zoom);
				Draw(rp,((DD->dd_PointX-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2-1,((DD->dd_PointY-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top+2-1);
			}
			DRAW_SetTagArg( notify, TAG_END, NULL );
			DoMethod( o, OM_NOTIFY, &notify, gpi->gpi_GInfo, 0 );
		}
		else
		{
			if((y*DD->dd_Width+x)<(DD->dd_Width*DD->dd_Height))
			{
				SetAPen(&DD->dd_RastPort,(UBYTE)DD->dd_ActualPen);
				WritePixel(&DD->dd_RastPort,x,y);
			}
			if(DD->dd_Zoom==1)
				PutPixel(rp,&DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)],(x-DD->dd_Left)*Grid+container.Left+2,(y-DD->dd_Top)*Grid+container.Top+2);
			else
				PutRect(rp,&DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)],
									((x-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2,
									((y-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top +2,
									DD->dd_Zoom-1,DD->dd_Zoom-1);
			DRAW_SetTagArg( notify, TAG_END, NULL );
			DoMethod( o, OM_NOTIFY, &notify, gpi->gpi_GInfo, 0 );
		}

		ReleaseGIRPort( rp );
	}
	else retval = GMR_NOREUSE;

	return(retval);
}

ULONG SAVEDS DRAW_HANDLEINPUT(Class *cl,Object *o,struct gpInput *gpi )
{
	ULONG retval = GMR_MEACTIVE,x,y;
	struct InputEvent *ie = gpi->gpi_IEvent;
	struct DRAWData *DD = INST_DATA(cl, o);
	struct GadgetInfo *gi = gpi->gpi_GInfo;
	struct RastPort *rp;
	struct Rectangle rect;
	struct IBox container;
	struct TagItem notify;
	BOOL Grid;

	if( DD->dd_ActiveFromMouse )
	{
		while( ie && (retval == GMR_MEACTIVE) )
		{
			switch( ie->ie_Class )
			{
				case IECLASS_TIMER: // Scroll listview
					break;
				case IECLASS_RAWMOUSE:
					switch(ie->ie_Code)
					{
						case SELECTUP:
							retval=GMR_NOREUSE|GMR_VERIFY;
						case MENUUP:
							retval=GMR_NOREUSE;
							break;
						case IECODE_NOBUTTON:
							DRAW_GetGadgetRect( o, gpi->gpi_GInfo, &rect );
							container.Left = rect.MinX; container.Top = rect.MinY;
							container.Width = 1 + rect.MaxX - rect.MinX;
							container.Height = 1 + rect.MaxY - rect.MinY;

							Grid=DD->dd_Grid?1:0;

							if((gpi->gpi_Mouse.X-gi->gi_Domain.Left)>2 && (gpi->gpi_Mouse.Y-gi->gi_Domain.Top)>2 &&
									(gpi->gpi_Mouse.X-gi->gi_Domain.Left-2)<(DD->dd_VisibleX*(DD->dd_Zoom+Grid)) &&
									(gpi->gpi_Mouse.Y-gi->gi_Domain.Top-2)<(DD->dd_VisibleY*(DD->dd_Zoom+Grid)) && (!DD->dd_SetPoint))
							{
								if(rp=ObtainGIRPort(gi))
								{
									x = ((gpi->gpi_Mouse.X-gi->gi_Domain.Left-2)/(DD->dd_Zoom+Grid))+DD->dd_Left;
									y = ((gpi->gpi_Mouse.Y-gi->gi_Domain.Top-2)/(DD->dd_Zoom+Grid))+DD->dd_Top;

									if(DD->dd_Zoom==1) Grid=DD->dd_Grid?2:1;
									else Grid=DD->dd_Grid?1:0;

									if((y*DD->dd_Width+x)<(DD->dd_Width*DD->dd_Height))
									{
										SetAPen(&DD->dd_RastPort,(UBYTE)DD->dd_ActualPen);
										WritePixel(&DD->dd_RastPort,x,y);
									}
									if(DD->dd_Zoom==1)
										PutPixel(rp,&DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)],(x-DD->dd_Left)*Grid+container.Left+2,(y-DD->dd_Top)*Grid+container.Top+2);
									else
										PutRect(rp,&DD->dd_Pens[ReadPixel(&DD->dd_RastPort,x,y)],
															((x-DD->dd_Left)*(DD->dd_Zoom+Grid))+container.Left+2,
															((y-DD->dd_Top)*(DD->dd_Zoom+Grid))+container.Top +2,
															DD->dd_Zoom-1,DD->dd_Zoom-1);
									ReleaseGIRPort( rp );
									DRAW_SetTagArg( notify, TAG_END, NULL );
									DoMethod( o, OM_NOTIFY, &notify, gpi->gpi_GInfo, 0 );
								}
							}
							break;
					}
					break;
			}
			ie = ie->ie_NextEvent;
		}
	}
	return(retval);
}

ULONG SAVEDS DRAW_GOINACTIVE(Class *cl,Object *o,struct gpGoInactive *gpgi )
{
	ULONG retval;
	struct DRAWData *DD = INST_DATA(cl, o);

	retval = DoSuperMethodA(cl, o, (Msg)gpgi);

	((struct Gadget *)o)->Flags &= ~GFLG_SELECTED;
	DD->dd_SetPoint=FALSE;

	return(retval);
}


void SAVEDS DRAW_GetGadgetRect( Object *o,struct GadgetInfo *gi,struct Rectangle *rect )
{
	struct Gadget *gad = (struct Gadget *)o;
	LONG W, H;

	rect->MinX = gad->LeftEdge;
	rect->MinY = gad->TopEdge;
	W = gad->Width;
	H = gad->Height;

	if( gi )
	{
		if( gad->Flags & GFLG_RELRIGHT ) rect->MinX += gi->gi_Domain.Width - 1;
		if( gad->Flags & GFLG_RELBOTTOM ) rect->MinY += gi->gi_Domain.Height - 1;
		if( gad->Flags & GFLG_RELWIDTH ) W += gi->gi_Domain.Width;
		if( gad->Flags & GFLG_RELHEIGHT ) H += gi->gi_Domain.Height;
	}
	rect->MaxX = rect->MinX + W - (W > 0);
	rect->MaxY = rect->MinY + H - (H > 0);
}

void PutPixel(struct RastPort *rp,struct ColorPalette *cp,UWORD x,UWORD y)
{
	if(CyberGraphX && CyberGfxBase)
	{
		WriteRGBPixel(rp,x,y,RGB(cp->cp_R,cp->cp_G,cp->cp_B));
	}
	else
	{
		SetAPen(rp,cp->cp_I);
		WritePixel(rp,x,y);
	}
}

void PutRect(struct RastPort *rp,struct ColorPalette *cp,UWORD x,UWORD y,UWORD w,UWORD h)
{
	if(CyberGraphX && CyberGfxBase)
	{
		FillPixelArray(rp,x,y,w,h,RGB(cp->cp_R,cp->cp_G,cp->cp_B));
	}
	else
	{
		SetAPen(rp,cp->cp_I);
		RectFill(rp,x,y,x+w,y+w);
	}
}
