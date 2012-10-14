/*
** Progress Class
** $VER: progress.c 37.1 (15.1.98)
*/

#include<dos.h>
#include<string.h>
#include<clib/macros.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<proto/graphics.h>
#include<proto/intuition.h>
#include<gadgets/progress.h>
#include<graphics/gfxmacros.h>
#include<exec/memory.h>
#include<libraries/gadtools.h>
#include<intuition/imageclass.h>
#include<intuition/gadgetclass.h>
#include<intuition/intuitionbase.h>
#include"progress.gadget_rev.h"

TEXT version[]=VERSTAG;

/*
** Defines
*/

#define unless(x) if(!(x))
#define PROG_SetTagArg(tag,id,data)	{ tag.ti_Tag = (ULONG)(id); tag.ti_Data = (ULONG)(data); }
typedef ULONG (*HookFunction)(void);
#define SAVEDS 
#define ASMFUNC __saveds __asm
#define REG(x) register __## x
#define PROGRESSGADGET "progress.gadget"

/*
** Prototypes
*/

Class * ASMFUNC GetProgressClass(void);
ULONG ASMFUNC PROG_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG PROG_NEW(Class *,Object *,struct opSet *);
ULONG PROG_DISPOSE(Class *, Object *, Msg);
ULONG PROG_SET(Class *, Object *, struct opSet *);
ULONG PROG_GET(Class *,Object *,struct opGet *);
ULONG PROG_UPDATE(Class *,Object *,struct opUpdate *);
ULONG PROG_NOTIFY(Class *,Object *,struct opUpdate *);
ULONG PROG_RENDER(Class *,Object *,struct gpRender *);
void PROG_GetGadgetRect( Object *,struct GadgetInfo *,struct Rectangle *);
void SPrintf(Class *,STRPTR buffer, STRPTR format, ...);

/*
** Variables
*/

struct PROGData
{
	ULONG pd_MaxVal;
	ULONG pd_CurVal;
	ULONG pd_Placement;
	UBYTE pd_ConvertToPercents;
	UBYTE pd_Border;
	TEXT pd_Format[256];
	Object *pd_FrameImage;
	struct TextFont *pd_TextFont;
};

Class *cl=NULL;

/*
** Create Class
*/


int ASMFUNC __UserLibInit(REG(a6) struct MyLibrary *libbase)
{
	if(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",36L))
	{
		if(UtilityBase=OpenLibrary("utility.library",36L))
		{
			if(GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",36L))
			{
				if (cl = MakeClass (NULL, GADGETCLASS, NULL, sizeof(struct PROGData), 0L))
				{
					cl->cl_Dispatcher.h_Entry = (HookFunction)  PROG_Dispatcher;
					cl->cl_UserData = (ULONG) libbase;
//					AddClass (cl);
					return(FALSE);
				}
				CloseLibrary((struct Library *)GfxBase);
			}
			CloseLibrary(UtilityBase);
		}
		CloseLibrary((struct Library *)IntuitionBase);
	}
	return(TRUE);
}

void ASMFUNC __UserLibCleanup(REG(a6) struct MyLibrary *libbase)
{
	FreeClass(cl);
	if(IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
	if(UtilityBase) CloseLibrary(UtilityBase);
	if(GfxBase) CloseLibrary((struct Library *)GfxBase);
}


Class * ASMFUNC GetProgressClass()
{
	return cl;
}

/*
** Dispatcher
*/

ULONG ASMFUNC PROG_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = PROG_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = PROG_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = PROG_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = PROG_GET(cl, o, (struct opGet *)msg );
			break;
		case OM_UPDATE:
			retval = PROG_UPDATE(cl, o, (struct opUpdate *)msg );
			break;
		case OM_NOTIFY:
			retval = PROG_NOTIFY(cl, o, (struct opUpdate *)msg );
			break;
		case GM_RENDER:
			retval = PROG_RENDER(cl, o, (struct gpRender *)msg );
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

ULONG SAVEDS PROG_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct PROGData *PD;
	char *ble;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		PD = INST_DATA( cl, object );

		PD->pd_Border = GetTagData( PROG_Border, TRUE, ops->ops_AttrList );
		PD->pd_ConvertToPercents = GetTagData( PROG_ConvertToPercents, TRUE, ops->ops_AttrList );
		PD->pd_MaxVal = GetTagData( PROG_MaxVal, 100, ops->ops_AttrList);
		PD->pd_CurVal = GetTagData( PROG_CurVal, 0, ops->ops_AttrList);
		PD->pd_Placement = GetTagData( PROG_Placement, PLACETEXT_IN, ops->ops_AttrList);
		ble=(char *)GetTagData( PROG_Format, (ULONG) "%ld" , ops->ops_AttrList);
		if(ble) strcpy(PD->pd_Format,ble);
		else PD->pd_Format[0]='\0';

		if(PD->pd_CurVal>PD->pd_MaxVal) PD->pd_CurVal=PD->pd_MaxVal;

		PD->pd_TextFont = (struct TextFont *)GetTagData( PROG_TextFont, NULL, ops->ops_AttrList);

		if(PD->pd_Border)
		{
			unless(PD->pd_FrameImage=NewObject(NULL,"frameiclass",
				IA_Recessed,    TRUE,
				IA_EdgesOnly,   TRUE,
				IA_FrameType,   FRAME_BUTTON,
				TAG_END))
			{
				CoerceMethod(cl, o, OM_DISPOSE);
				object = NULL;
			}
		}
	}
	return( (ULONG)object );
}

/*
** OM_DISPOSE
*/

ULONG SAVEDS PROG_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct PROGData *PD = INST_DATA( cl, o );

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	if(PD->pd_FrameImage) DisposeObject(PD->pd_FrameImage);
	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS PROG_SET(Class *cl, Object *o, struct opSet *ops)
{
	ULONG retval;
	struct PROGData *PD = INST_DATA( cl, o );
	struct TagItem *tag, *tstate, notify;
	struct RastPort *rp;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	tstate=ops->ops_AttrList;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case PROG_CurVal:
				PD->pd_CurVal = tag->ti_Data;
				retval = FALSE;
				break;
			case PROG_MaxVal:
				PD->pd_MaxVal = tag->ti_Data;
				retval = FALSE;
				break;
			case PROG_ConvertToPercents:
				PD->pd_ConvertToPercents = tag->ti_Data;
				retval = FALSE;
				break;
			case PROG_Format:
				strcpy(PD->pd_Format,(char *)tag->ti_Data);
				retval = FALSE;
				break;
		}
	}

	if(PD->pd_CurVal>PD->pd_MaxVal) PD->pd_CurVal=PD->pd_MaxVal;
	PROG_SetTagArg( notify, TAG_END, NULL );
	DoMethod( o, OM_NOTIFY, &notify, ops->ops_GInfo, 0 );

	if(!retval)
	{
		if(rp=ObtainGIRPort( ops->ops_GInfo ))
		{
			DoMethod(o,GM_RENDER,ops->ops_GInfo, rp, GREDRAW_UPDATE);
			ReleaseGIRPort(rp);
		}
	}

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS PROG_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval;
	struct PROGData *PD = INST_DATA( cl, o );

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	switch( opg->opg_AttrID )
	{
		case PROG_MaxVal:
			*opg->opg_Storage = (ULONG)PD->pd_MaxVal;
			retval = TRUE;
			break;
		case PROG_CurVal:
			*opg->opg_Storage = (ULONG)PD->pd_CurVal;
			retval = TRUE;
			break;
		case PROG_TextFont:
			*opg->opg_Storage = (ULONG)PD->pd_TextFont;
			retval = TRUE;
			break;
		case PROG_Border:
			*opg->opg_Storage = (ULONG)PD->pd_Border;
			retval = TRUE;
			break;
		case PROG_Format:
			*opg->opg_Storage = (ULONG)PD->pd_Format;
			retval = TRUE;
			break;
		case PROG_Placement:
			*opg->opg_Storage = (ULONG)PD->pd_Placement;
			retval = TRUE;
			break;
		case PROG_ConvertToPercents:
			*opg->opg_Storage = (ULONG)PD->pd_ConvertToPercents;
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

ULONG SAVEDS PROG_UPDATE(Class *cl,Object *o,struct opUpdate *opu )
{
	ULONG retval;
	struct PROGData *PD = INST_DATA( cl, o );
	struct TagItem *tag, notify;
	struct RastPort *rp;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA(cl, o, (Msg) opu);

	if( opu->opu_AttrList )
	{
		if(tag = FindTagItem( PROG_CurVal, opu->opu_AttrList ))
		{
			if( tag->ti_Data != PD->pd_CurVal)
			{
				PD->pd_CurVal = tag->ti_Data;

				rp = ObtainGIRPort( opu->opu_GInfo );
				if( rp )
				{
					DoMethod( o, GM_RENDER, opu->opu_GInfo, rp, GREDRAW_UPDATE );
					ReleaseGIRPort( rp );
				}

				/* Notify the change. */
				PROG_SetTagArg( notify, TAG_END, NULL );
				(void)DoMethod( o, OM_NOTIFY, &notify, opu->opu_GInfo, 0 );
			}
		}
	}
	return( retval );
}

ULONG SAVEDS PROG_NOTIFY(Class *cl,Object *o,struct opUpdate *opu )
{
	struct TagItem tags[3];
	struct PROGData *PD = INST_DATA( cl, o );

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	PROG_SetTagArg(tags[0], GA_ID, ((struct Gadget *)o)->GadgetID);
	PROG_SetTagArg(tags[1], PROG_CurVal, PD->pd_CurVal);

	if( opu->opu_AttrList == NULL )
	{
		PROG_SetTagArg(tags[2], TAG_END, NULL);
	}
	else PROG_SetTagArg(tags[2], TAG_MORE, opu->opu_AttrList );

	return( DoSuperMethod(cl, o, OM_NOTIFY, tags, opu->opu_GInfo, opu->opu_Flags) );
}

ULONG SAVEDS PROG_RENDER(Class *cl,Object *o,struct gpRender *gpr )
{
	ULONG retval,State;
	struct Gadget *gad = (struct Gadget *)o;
	struct Rectangle rect;
	struct DrawInfo *dri;
	struct IBox container;
	struct TextExtent temp_te;
	struct RastPort *RP = gpr->gpr_RPort;
	struct PROGData *PD = INST_DATA( cl, o );
	UWORD BorderWidth, BorderHeight;
	UWORD patterndata[2] = { 0x2222, 0x8888 };
	ULONG TextPen, FillPen, BackgroundPen, len, DrawWidth, InPercents;
	TEXT TempText[256],TestText[256];

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA(cl, o, (Msg) gpr);

	PROG_GetGadgetRect( o, gpr->gpr_GInfo, &rect );

	container.Left = rect.MinX; container.Top = rect.MinY;
	container.Width = 1 + rect.MaxX - rect.MinX;
	container.Height = 1 + rect.MaxY - rect.MinY;

	dri = gpr->gpr_GInfo->gi_DrInfo;

	if(gad->Flags & GFLG_DISABLED)
		State = IDS_DISABLED;
	else
    State = IDS_NORMAL;

	SetAttrs( PD->pd_FrameImage,
		IA_Left,    container.Left,
		IA_Top,     container.Top,
		IA_Width,   container.Width,
		IA_Height,  container.Height,
		TAG_END);

	DrawImageState( RP, (struct Image *)PD->pd_FrameImage, 0, 0, State, dri);

	if( dri )
	{
		TextPen = dri->dri_Pens[TEXTPEN];
		FillPen = dri->dri_Pens[FILLPEN];
		BackgroundPen = dri->dri_Pens[BACKGROUNDPEN];
	}
	else
	{
  	TextPen = 1;
		FillPen = 3;
		BackgroundPen = 0;
	}

	DrawWidth=UDivMod32(UMult32(PD->pd_CurVal,(ULONG)container.Width-4),PD->pd_MaxVal);

	/* Do we have a proper font. */
	if(PD->pd_TextFont) SetFont(RP,PD->pd_TextFont);
	else PD->pd_TextFont=RP->Font;

	if(PD->pd_ConvertToPercents)
	{
		InPercents=UDivMod32(UMult32(PD->pd_CurVal,100L),PD->pd_MaxVal);
		SPrintf(cl,TempText,PD->pd_Format,InPercents);
		SPrintf(cl,TestText,PD->pd_Format,100);
	}
	else
	{
		SPrintf(cl,TempText,PD->pd_Format,PD->pd_CurVal);
		SPrintf(cl,TestText,PD->pd_Format,PD->pd_MaxVal);
	}

	SetBPen(RP,BackgroundPen);
	SetDrMd(RP,JAM1);
	SetAPen(RP,FillPen);
	if(container.Left+2<container.Left+2+DrawWidth-1) RectFill(RP,container.Left+2,container.Top+1,container.Left+2+DrawWidth-1,container.Top+2+container.Height-4);
	else RectFill(RP,container.Left+2,container.Top+1,container.Left+2,container.Top+2+container.Height-4);
	SetAPen(RP,BackgroundPen);
	RectFill(RP,container.Left+2+DrawWidth,container.Top+1,container.Left+container.Width-4,container.Top+2+container.Height-4);

	if(TempText[0])
	{
		SetAPen(RP,TextPen);
		switch(PD->pd_Placement)
		{
			case PLACETEXT_IN:
				SetDrMd(RP,JAM1);
				len=TextFit(RP,TempText,strlen(TempText),&temp_te,NULL,1,container.Width-28, 1+PD->pd_TextFont->tf_YSize);
				Move(RP,container.Left+(container.Width-(1+temp_te.te_Extent.MaxX-temp_te.te_Extent.MinX))/2-temp_te.te_Extent.MinX,PD->pd_TextFont->tf_Baseline+(1+container.Top+rect.MaxY-PD->pd_TextFont->tf_YSize)/2);
				Text(RP,TempText,len);
				break;
			case PLACETEXT_LEFT:
				SetDrMd(RP,JAM2);
				len=TextLength(RP,TestText,strlen(TestText));
				Move(RP,container.Left-len-8,PD->pd_TextFont->tf_Baseline+(1+container.Top+rect.MaxY-PD->pd_TextFont->tf_YSize)/2);
				Text(RP,TempText,strlen(TempText));
				break;
			case PLACETEXT_RIGHT:
				SetDrMd(RP,JAM2);
				len=TextLength(RP,TempText,strlen(TempText));
				Move(RP,container.Left+container.Width+8,PD->pd_TextFont->tf_Baseline+(1+container.Top+rect.MaxY-PD->pd_TextFont->tf_YSize)/2);
				Text(RP,TempText,strlen(TempText));
				break;
			case PLACETEXT_ABOVE:
				SetDrMd(RP,JAM2);
				len=TextLength(RP,TempText,strlen(TempText));
				Move(RP,container.Left+(container.Width/2)-len/2,container.Top-6);
				Text(RP,TempText,strlen(TempText));
				break;
			case PLACETEXT_BELOW:
				SetDrMd(RP,JAM2);
				len=TextLength(RP,TempText,strlen(TempText));
				Move(RP,container.Left+(container.Width/2)-len/2,container.Top+container.Height+PD->pd_TextFont->tf_Baseline+3);
				Text(RP,TempText,strlen(TempText));
				break;
		}
	}

	if(State==IDS_DISABLED)
	{
		BorderHeight = 1;
		BorderWidth = (IntuitionBase->LibNode.lib_Version < 39) ? 1 : 2;

		container.Left += BorderWidth;
		container.Top += BorderHeight;
		container.Width = MAX( 1, container.Width - 2*BorderWidth );
		container.Height = MAX( 1, container.Height - 2*BorderHeight );

		SetDrMd(RP,JAM1);
		SetAfPt(RP, patterndata, 1);

		RectFill(RP,  (LONG)container.Left, (LONG)container.Top,
			container.Left + container.Width,
			container.Top + container.Height);
		SetAfPt(RP, NULL, 0 );
	}

	return( retval );
}

void SAVEDS PROG_GetGadgetRect( Object *o,struct GadgetInfo *gi,struct Rectangle *rect )
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

void SAVEDS SPrintf(Class *cl, STRPTR buffer, STRPTR format, ...)
{
	putreg( REG_A6, cl->cl_UserData );
	geta4();

	RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", buffer);
}
