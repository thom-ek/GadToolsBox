/*
** Page Class
** $VER: page.c 37.1 (15.1.98)
*/

#include<dos.h>
#include<string.h>
#include<clib/macros.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<proto/graphics.h>
#include<proto/intuition.h>
#include<proto/gadtools.h>
#include<proto/layers.h>
#include<gadgets/page.h>
#include<graphics/gfxmacros.h>
#include<exec/memory.h>
#include<libraries/gadtools.h>
#include<intuition/imageclass.h>
#include<intuition/gadgetclass.h>
#include<intuition/intuitionbase.h>
#include<intuition/icclass.h>
#include"page.gadget_rev.h"

TEXT version[]=VERSTAG;

/*
** Defines
*/

#define abs(x) ((x)<0?-(x):(x))
#define unless(x) if(!(x))
#define PAGE_SetTagArg(tag,id,data)	{ tag.ti_Tag = (ULONG)(id); tag.ti_Data = (ULONG)(data); }
typedef ULONG (*HookFunction)(void);
#define SAVEDS 
#define ASMFUNC __saveds __asm
#define REG(x) register __## x
#define PAGEGADGET "page.gadget"

/*
** Prototypes
*/

Class * ASMFUNC GetPageClass(void);
APTR ASMFUNC PAGE_GetCopyright(void);
ULONG ASMFUNC PAGE_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG PAGE_NEW(Class *,Object *,struct opSet *);
ULONG STRG_NEW(Class *,Object *,struct opSet *);
ULONG PAGE_DISPOSE(Class *, Object *, Msg);
ULONG PAGE_SET(Class *, Object *, struct opSet *);
ULONG PAGE_GET(Class *,Object *,struct opGet *);
ULONG PAGE_UPDATE(Class *,Object *,struct opUpdate *);
ULONG PAGE_NOTIFY(Class *,Object *,struct opUpdate *);
ULONG PAGE_RENDER(Class *,Object *,struct gpRender *);
ULONG PAGE_GOACTIVE(Class *,Object *,struct gpInput *);
void PAGE_GetGadgetRect( Object *,struct GadgetInfo *,struct Rectangle *);
void SPrintf(Class *,STRPTR buffer, STRPTR format, ...);
ULONG TextFitColor(struct RastPort *,STRPTR,ULONG,struct TextExtent *,struct TextExtent *,WORD,UWORD,UWORD);
void TextColor(Class *,Object *,struct RastPort *,STRPTR,ULONG,struct DrawInfo *,struct IBox *);

/*
** Variables
*/

struct PAGEData
{
	struct BevelBox *pg_ClearBBox;
	ULONG pg_Layout;
	ULONG pg_Active;
	ULONG pg_ActiveStyle;
	BOOL pg_Underscore;
	BOOL pg_GadgetsAdded;
	struct TextFont *pg_TextFont;
	struct TextAttr *pg_TextAttr;
	struct Page *pg_Pages;
	struct Page *pg_ActivePage;
	Object *pg_FrameImage;
	ULONG pg_NumPages;	
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
				if(GadToolsBase=OpenLibrary("gadtools.library",36L))
				{
					if(LayersBase=OpenLibrary("layers.library",36L))
					{
						if (cl = MakeClass (NULL, GADGETCLASS, NULL, sizeof(struct PAGEData), 0L))
						{
							cl->cl_Dispatcher.h_Entry = (HookFunction)  PAGE_Dispatcher;
							cl->cl_UserData = (ULONG) libbase;
//							AddClass (cl);
							return(FALSE);
						}
						CloseLibrary(LayersBase);
					}
					CloseLibrary(GadToolsBase);
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


Class * ASMFUNC GetPageClass()
{
	return cl;
}

APTR ASMFUNC PAGE_GetCopyright()
{
	return (APTR) VERS;
}

/*
** Dispatcher
*/

ULONG ASMFUNC PAGE_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = PAGE_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = PAGE_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = PAGE_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = PAGE_GET(cl, o, (struct opGet *)msg );
			break;
		case OM_UPDATE:
			retval = PAGE_UPDATE(cl, o, (struct opUpdate *)msg );
			break;
		case OM_NOTIFY:
			retval = PAGE_NOTIFY(cl, o, (struct opUpdate *)msg );
			break;
		case GM_RENDER:
			retval = PAGE_RENDER(cl, o, (struct gpRender *)msg );
			break;
		case GM_GOACTIVE:
			retval = PAGE_GOACTIVE(cl, o, (struct gpInput *)msg );
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

ULONG SAVEDS PAGE_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct PAGEData *PG;
	struct Page *pages;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		PG = INST_DATA( cl, object );

		PG->pg_GadgetsAdded=FALSE;
		PG->pg_Active=0;
		PG->pg_ActivePage=NULL;
		PG->pg_Pages=(struct Page *)GetTagData( PAGE_Pages, NULL, ops->ops_AttrList );
		for(pages=PG->pg_Pages,PG->pg_NumPages=0;pages;pages=pages->NextPage,PG->pg_NumPages++);
		PG->pg_Layout=GetTagData( PAGE_Layout, NULL, ops->ops_AttrList );
		PG->pg_Underscore=GetTagData( PAGE_Underscore, NULL, ops->ops_AttrList );
		PG->pg_Active=GetTagData( PAGE_Active, NULL, ops->ops_AttrList );
		if(PG->pg_Active>PG->pg_NumPages) PG->pg_Active=PG->pg_NumPages;
		PG->pg_TextFont=(struct TextFont *)GetTagData( PAGE_TextFont, NULL, ops->ops_AttrList );
		PG->pg_TextAttr=(struct TextAttr *)GetTagData( PAGE_TextAttr, NULL, ops->ops_AttrList );
		PG->pg_ActiveStyle=GetTagData( PAGE_ActiveStyle, NULL, ops->ops_AttrList );
		PG->pg_ClearBBox=(struct BevelBox *)GetTagData( PAGE_ClearBBox, NULL, ops->ops_AttrList );
		{
			unless(PG->pg_FrameImage=NewObject(NULL,"frameiclass",
				IA_Recessed,    FALSE,
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

ULONG SAVEDS PAGE_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct PAGEData *PG = INST_DATA( cl, o );

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	if(PG->pg_FrameImage) DisposeObject(PG->pg_FrameImage);
	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS PAGE_SET(Class *cl, Object *o, struct opSet *ops)
{
	ULONG retval,u=FALSE;
	struct PAGEData *PG = INST_DATA( cl, o );
	struct TagItem *tag, *tstate, notify;
	struct RastPort *rp;
	struct Page *pages;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	tstate=ops->ops_AttrList;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case PAGE_Pages:
				PG->pg_Pages=(struct Page *)tag->ti_Data;
				for(pages=PG->pg_Pages,PG->pg_NumPages=0;pages;pages=pages->NextPage,PG->pg_NumPages++);
				if(PG->pg_Active>PG->pg_NumPages) PG->pg_Active=PG->pg_NumPages;
				retval=FALSE; u=FALSE;
				break;
			case PAGE_Layout:
				PG->pg_Layout=tag->ti_Data;
				retval=FALSE; u=FALSE;
				break;
			case PAGE_Underscore:
				PG->pg_Underscore=tag->ti_Data;
				retval=FALSE; u=FALSE;
				break;
			case PAGE_Active:
				PG->pg_Active=tag->ti_Data;
				if(PG->pg_Active>PG->pg_NumPages) PG->pg_Active=PG->pg_NumPages;
				retval=FALSE; u=FALSE;
				break;
			case PAGE_TextFont:
				PG->pg_TextFont=(struct TextFont *)tag->ti_Data;
				retval=FALSE; u=FALSE;
				break;
			case PAGE_TextAttr:
				PG->pg_TextAttr=(struct TextAttr *)tag->ti_Data;
				retval=FALSE; u=FALSE;
				break;
			case PAGE_ActiveStyle:
				PG->pg_ActiveStyle=tag->ti_Data;
				retval=FALSE; u=FALSE;
				break;
			case PAGE_ClearBBox:
				PG->pg_ClearBBox=(struct BevelBox *)tag->ti_Data;
				retval=FALSE; u=FALSE;
				break;
		}
	}
	PAGE_SetTagArg( notify, TAG_END, NULL );
	DoMethod( o, OM_NOTIFY, &notify, ops->ops_GInfo, 0 );

	if(!retval)
	{
		if(rp=ObtainGIRPort( ops->ops_GInfo ))
		{
			DoMethod(o,GM_RENDER,ops->ops_GInfo, rp, u?GREDRAW_UPDATE:GREDRAW_REDRAW);
			ReleaseGIRPort(rp);
		}
	}

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS PAGE_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval;
	struct PAGEData *PG = INST_DATA( cl, o );

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	switch( opg->opg_AttrID )
	{
		case PAGE_Pages:
			*opg->opg_Storage = (ULONG)PG->pg_Pages;
			break;
		case PAGE_Layout:
			*opg->opg_Storage = (ULONG)PG->pg_Layout;
			retval = TRUE;
			break;
		case PAGE_Underscore:
			*opg->opg_Storage = (ULONG)PG->pg_Underscore;
			retval = TRUE;
			break;
		case PAGE_Active:
			*opg->opg_Storage = (ULONG)PG->pg_Active;
			retval = TRUE;
			break;
		case PAGE_TextFont:
			*opg->opg_Storage = (ULONG)PG->pg_TextFont;
			retval = TRUE;
			break;
		case PAGE_TextAttr:
			*opg->opg_Storage = (ULONG)PG->pg_TextAttr;
			retval = TRUE;
			break;
		case PAGE_ActiveStyle:
			*opg->opg_Storage = (ULONG)PG->pg_ActiveStyle;
			retval = TRUE;
			break;
		case PAGE_ClearBBox:
			*opg->opg_Storage = (ULONG)PG->pg_ClearBBox;
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

ULONG SAVEDS PAGE_UPDATE(Class *cl,Object *o,struct opUpdate *opu )
{
	ULONG retval;
	struct PAGEData *PG = INST_DATA( cl, o );
	struct TagItem *tag, notify;
	struct RastPort *rp;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA(cl, o, (Msg) opu);

	if( opu->opu_AttrList )
	{
		if(tag = FindTagItem( PAGE_Active, opu->opu_AttrList ))
		{
			if( tag->ti_Data != PG->pg_Active)
			{
				PG->pg_Active = tag->ti_Data;

				rp = ObtainGIRPort( opu->opu_GInfo );
				if( rp )
				{
					DoMethod( o, GM_RENDER, opu->opu_GInfo, rp, GREDRAW_UPDATE );
					ReleaseGIRPort( rp );
				}

				/* Notify the change. */
				PAGE_SetTagArg( notify, TAG_END, NULL );
				(void)DoMethod( o, OM_NOTIFY, &notify, opu->opu_GInfo, 0 );
			}
		}
	}
	return( retval );
}

ULONG SAVEDS PAGE_NOTIFY(Class *cl,Object *o,struct opUpdate *opu )
{
	struct TagItem tags[3];
	struct PAGEData *PG = INST_DATA( cl, o );

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	PAGE_SetTagArg(tags[0], GA_ID, ((struct Gadget *)o)->GadgetID);
	PAGE_SetTagArg(tags[1], PAGE_Active, PG->pg_Active);

	if( opu->opu_AttrList == NULL ) { PAGE_SetTagArg(tags[2], TAG_END, NULL); }
	else PAGE_SetTagArg(tags[2], TAG_MORE, opu->opu_AttrList );

	return( DoSuperMethod(cl, o, OM_NOTIFY, tags, opu->opu_GInfo, opu->opu_Flags) );
}

ULONG SAVEDS PAGE_RENDER(Class *cl,Object *o,struct gpRender *gpr )
{
	ULONG retval,State,a,len,offset;
	struct Gadget *gad = (struct Gadget *)o;
	struct Rectangle rect;
	struct DrawInfo *dri;
	struct IBox container,box;
	struct TextExtent temp_te;
	struct RastPort *RP = gpr->gpr_RPort;
	struct PAGEData *PG = INST_DATA( cl, o );
	UWORD patterndata[2] = { 0x2222, 0x8888 };
	UWORD fillpatterndata[2] = { 0x5555, 0xaaaa };
	struct Page *page;
	struct BevelBox *bbox;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA(cl, o, (Msg) gpr);

	PAGE_GetGadgetRect( o, gpr->gpr_GInfo, &rect );

	container.Left = rect.MinX; container.Top = rect.MinY;
	container.Width = 1 + rect.MaxX - rect.MinX;
	container.Height = 1 + rect.MaxY - rect.MinY;

	dri = gpr->gpr_GInfo->gi_DrInfo;

	if(gad->Flags & GFLG_DISABLED)
		State = IDS_DISABLED;
	else
    State = IDS_NORMAL;

	/* Do we have a proper font. */
	if(PG->pg_TextFont) SetFont(RP,PG->pg_TextFont);
	else PG->pg_TextFont=RP->Font;

	if(PG->pg_Pages)
	{
		SetAPen(RP,dri->dri_Pens[BACKGROUNDPEN]);
		RectFill(RP,container.Left,container.Top,container.Left+container.Width,container.Top+container.Height);

		if(PG->pg_ClearBBox)
		{
			SetAPen(RP,dri->dri_Pens[BACKGROUNDPEN]);
			RectFill(RP,PG->pg_ClearBBox->Left,PG->pg_ClearBBox->Top,PG->pg_ClearBBox->Left+PG->pg_ClearBBox->Width,PG->pg_ClearBBox->Top+PG->pg_ClearBBox->Height);
			SetAttrs( PG->pg_FrameImage,
				IA_Left,    PG->pg_ClearBBox->Left,
				IA_Top,     PG->pg_ClearBBox->Top,
				IA_Width,   PG->pg_ClearBBox->Width,
				IA_Height,  PG->pg_ClearBBox->Height,
				TAG_END);
			DrawImageState( RP, (struct Image *)PG->pg_FrameImage, 0, 0, IDS_NORMAL, dri);
		}

		if(gpr->gpr_Redraw!=GREDRAW_UPDATE)
		{
			if(PG->pg_GadgetsAdded)
			{
				RemoveGList(gpr->gpr_GInfo->gi_Window,PG->pg_ActivePage->FirstGadget,-1);
				PG->pg_GadgetsAdded=FALSE;
			}
		}
		for(a=0,page=PG->pg_Pages;a<PG->pg_NumPages;a++,page=page->NextPage)
		{
			offset=1;
			switch(PG->pg_Layout)
			{
				case PGLYT_SIMPLE:
					if(a==PG->pg_Active)
					{
						offset=0;
						SetAPen(RP,dri->dri_Pens[BACKGROUNDPEN]);
						Move(RP,container.Left+a*(container.Width/PG->pg_NumPages)+1,container.Top+container.Height-1);
						if(a==PG->pg_NumPages-1) Draw(RP,container.Left+container.Width-1,container.Top+container.Height-1);
						else Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-3,container.Top+container.Height-1);
						PG->pg_ActivePage=page;
					}
					SetAPen(RP,dri->dri_Pens[SHINEPEN]);
					Move(RP,container.Left+a*(container.Width/PG->pg_NumPages),container.Top+container.Height-2);
					Draw(RP,container.Left+a*(container.Width/PG->pg_NumPages),container.Top+offset+2);
					Draw(RP,container.Left+a*(container.Width/PG->pg_NumPages)+2,container.Top+offset);
					if(a==PG->pg_NumPages-1)
					{
						Draw(RP,container.Left+container.Width-2,container.Top+offset);
						SetAPen(RP,dri->dri_Pens[SHADOWPEN]);
						Draw(RP,container.Left+container.Width,container.Top+offset+2);
						Draw(RP,container.Left+container.Width,container.Top+container.Height-2);
					}
					else
					{
						Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-2-2,container.Top+offset);
						SetAPen(RP,dri->dri_Pens[SHADOWPEN]);
						Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-2,container.Top+offset+2);
						Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-2,container.Top+container.Height-2);
					}
					break;
				case PGLYT_MUISTYLE:
					if(a==PG->pg_Active)
					{
						offset=0;
						SetAPen(RP,dri->dri_Pens[BACKGROUNDPEN]);
						Move(RP,container.Left+a*(container.Width/PG->pg_NumPages)+1,container.Top+container.Height-1);
						if(a==PG->pg_NumPages-1) Draw(RP,container.Left+container.Width-1,container.Top+container.Height-1);
						else Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-3,container.Top+container.Height-1);
						PG->pg_ActivePage=page;
					}
					SetAPen(RP,dri->dri_Pens[SHINEPEN]);
					Move(RP,container.Left+a*(container.Width/PG->pg_NumPages),container.Top+container.Height-2);
					if(a==PG->pg_Active)
					{
						Draw(RP,container.Left+a*(container.Width/PG->pg_NumPages)+2,container.Top+container.Height-2-2);
						Draw(RP,container.Left+a*(container.Width/PG->pg_NumPages)+2,container.Top+offset+2);
						Draw(RP,container.Left+a*(container.Width/PG->pg_NumPages)+4,container.Top+offset);
					}
					else
					{
						Move(RP,container.Left+a*(container.Width/PG->pg_NumPages)+2,container.Top+container.Height-2);
						Draw(RP,container.Left+a*(container.Width/PG->pg_NumPages)+2,container.Top+offset+2);
						Draw(RP,container.Left+a*(container.Width/PG->pg_NumPages)+4,container.Top+offset);
					}
					if(a==PG->pg_NumPages-1)
					{
						Draw(RP,container.Left+container.Width-4,container.Top+offset);
						SetAPen(RP,dri->dri_Pens[SHADOWPEN]);
						if(a==PG->pg_Active)
						{
							Draw(RP,container.Left+container.Width-2,container.Top+2);
							Draw(RP,container.Left+container.Width-2,container.Top+container.Height-2-2);
							Draw(RP,container.Left+container.Width,container.Top+container.Height-2);
						}
						else
						{
							Draw(RP,container.Left+container.Width-2,container.Top+2);
							Draw(RP,container.Left+container.Width-2,container.Top+container.Height-2);
						}
					}
					else
					{
						Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-2-4,container.Top+offset);
						SetAPen(RP,dri->dri_Pens[SHADOWPEN]);
						if(a==PG->pg_Active)
						{
							Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-2-2,container.Top+offset+2);
							Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-2-2,container.Top+container.Height-2-2);
							Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-2,container.Top+container.Height-2);
						}
						else
						{
							Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-2-2,container.Top+offset+2);
							Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-2-2,container.Top+container.Height-2);
						}
					}
					break;
				case PGLYT_GADTOOLS:
					if(a==PG->pg_Active)
					{
						offset=0;
						SetAPen(RP,dri->dri_Pens[BACKGROUNDPEN]);
						Move(RP,container.Left+a*(container.Width/PG->pg_NumPages)+1,container.Top+container.Height-1);
						if(a==PG->pg_NumPages-1) Draw(RP,container.Left+container.Width-1,container.Top+container.Height-1);
						else Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-3,container.Top+container.Height-1);
						PG->pg_ActivePage=page;
					}
					SetAPen(RP,dri->dri_Pens[SHINEPEN]);
					Move(RP,container.Left+a*(container.Width/PG->pg_NumPages),container.Top+container.Height-2);
					Draw(RP,container.Left+a*(container.Width/PG->pg_NumPages),container.Top+offset);
					if(a==PG->pg_NumPages-1)
					{
						Draw(RP,container.Left+container.Width,container.Top+offset);
						SetAPen(RP,dri->dri_Pens[SHADOWPEN]);
						Draw(RP,container.Left+container.Width,container.Top+container.Height-2);
					}
					else
					{
						Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-2,container.Top+offset);
						SetAPen(RP,dri->dri_Pens[SHADOWPEN]);
						Draw(RP,container.Left+(a+1)*(container.Width/PG->pg_NumPages)-2,container.Top+container.Height-2);
					}
					break;
			}
			if(page->Name)
			{
				switch(PG->pg_ActiveStyle)
				{
					case PGAST_NORMAL:
						SetAPen(RP,dri->dri_Pens[TEXTPEN]);
						break;
					case PGAST_HILIGHT:
						if(PG->pg_Active==a) SetAPen(RP,dri->dri_Pens[HIGHLIGHTTEXTPEN]);
						else SetAPen(RP,dri->dri_Pens[TEXTPEN]);
						break;
					case PGAST_SHADOW:
						SetAPen(RP,dri->dri_Pens[SHADOWPEN]);
						break;
				}
				len=TextFit(RP,page->Name,strlen(page->Name),&temp_te,NULL,1,container.Width/PG->pg_NumPages-4, 1+PG->pg_TextFont->tf_YSize);
				Move(RP,container.Left+a*(container.Width/PG->pg_NumPages)+((container.Width/PG->pg_NumPages)-(1+temp_te.te_Extent.MaxX-temp_te.te_Extent.MinX))/2-temp_te.te_Extent.MinX,PG->pg_TextFont->tf_Baseline+(1+container.Top+rect.MaxY-PG->pg_TextFont->tf_YSize)/2+offset);
				Text(RP,page->Name,len);
//				box.Left=container.Left+a*(container.Width/PG->pg_NumPages);
//				box.Top=(1+container.Top+rect.MaxY-PG->pg_TextFont->tf_YSize)/2+offset;
//				box.Width=container.Width/PG->pg_NumPages-4;
//				box.Height=1+PG->pg_TextFont->tf_YSize;
//				TextColor(cl,o,RP,page->Name,strlen(page->Name),dri,&box);
				switch(PG->pg_ActiveStyle)
				{
					case PGAST_SHADOW:
						SetAPen(RP,dri->dri_Pens[SHINEPEN]);
						Move(RP,container.Left+a*(container.Width/PG->pg_NumPages)+((container.Width/PG->pg_NumPages)-(1+temp_te.te_Extent.MaxX-temp_te.te_Extent.MinX))/2-temp_te.te_Extent.MinX-1,PG->pg_TextFont->tf_Baseline+(1+container.Top+rect.MaxY-PG->pg_TextFont->tf_YSize)/2+offset-1);
						Text(RP,page->Name,len);
						break;
				}
			}
			if(a==PG->pg_Active)
			{
				if(page->ThisPageHook) CallHook(page->ThisPageHook,o,gpr->gpr_GInfo,page);
				if(page->FirstIText) PrintIText(RP,page->FirstIText,0,0);
				if(page->FirstImage) DrawImage(RP,page->FirstImage,0,0);
				for(bbox=page->FirstBBox;bbox;bbox=bbox->NextBBox)
				{
					if(bbox->Flags&BBFLG_BACKFILL)
					{
						if(bbox->Flags&BBFLG_PATTERNFILL)
						{
							SetABPenDrMd(RP,bbox->PtrnAPen,bbox->PtrnBPen,JAM1);
							SetAfPt(RP,fillpatterndata,1);
						}
						else
							SetAPen(RP,dri->dri_Pens[BACKGROUNDPEN]);
						RectFill(RP,bbox->Left,bbox->Top,bbox->Left+bbox->Width,bbox->Top+bbox->Height);
						SetAfPt(RP, NULL, 0 );
					}
					if(bbox->Flags&BBFLG_FRAMEBUTTON)
					{
						SetAPen(RP,(bbox->Flags&BBFLG_RECESSED)?bbox->BoxShadow:bbox->BoxShine);
						Move(RP,bbox->Left,bbox->Top+bbox->Height);
						Draw(RP,bbox->Left,bbox->Top);
						Draw(RP,bbox->Left+bbox->Width,bbox->Top);
						SetAPen(RP,(bbox->Flags&BBFLG_RECESSED)?bbox->BoxShine:bbox->BoxShadow);
						Draw(RP,bbox->Left+bbox->Width,bbox->Top+bbox->Height);
						Draw(RP,bbox->Left,bbox->Top+bbox->Height);
					}
					if(bbox->Flags&BBFLG_FRAMERIDGE)
					{
						// Draw Ridge
					}
				}
			}
		}
		if(gpr->gpr_Redraw!=GREDRAW_UPDATE)
		{
			if(PG->pg_ActivePage->FirstGadget)
			{
				AddGList(gpr->gpr_GInfo->gi_Window,PG->pg_ActivePage->FirstGadget,-1,-1,NULL);
				RefreshGList(PG->pg_ActivePage->FirstGadget,gpr->gpr_GInfo->gi_Window,NULL,-1);
				PG->pg_GadgetsAdded=TRUE;
			}
		}
	}

	if(State==IDS_DISABLED)
	{
		container.Width = MAX( 1, container.Width );
		container.Height = MAX( 1, container.Height );

		SetDrMd(RP,JAM1);
		SetAfPt(RP, patterndata, 1);
		SetAPen(RP,dri->dri_Pens[SHADOWPEN]);

		RectFill(RP,  (LONG)container.Left, (LONG)container.Top,
			container.Left + container.Width-1,
			container.Top + container.Height-1);
		SetAfPt(RP, NULL, 0 );
	}

	return( retval );
}

ULONG SAVEDS PAGE_GOACTIVE(Class *cl,Object *o,struct gpInput *gpi )
{
	ULONG retval = GMR_MEACTIVE, Left, Top;
	struct RastPort *rp;
	struct PAGEData *PG = INST_DATA( cl, o );
	struct GadgetInfo *gi = gpi->gpi_GInfo;
	struct Gadget *gad = (struct Gadget *)o;
	struct Rectangle rect;
	struct IBox container;
	ULONG oldactive;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	/* Test if we are disabled. */
	if(gad->Flags & GFLG_DISABLED) return( GMR_NOREUSE );

	/* Call first our parent class. */
	DoSuperMethodA(cl, o, (Msg)gpi);

	PAGE_GetGadgetRect( o, gpi->gpi_GInfo, &rect );
	container.Left = rect.MinX; container.Top = rect.MinY;
	container.Width = 1 + rect.MaxX - rect.MinX;
	container.Height = 1 + rect.MaxY - rect.MinY;

	/* Select this gadget. */
	gad->Flags |= GFLG_SELECTED;

	if(PG->pg_GadgetsAdded) RemoveGList(gpi->gpi_GInfo->gi_Window,PG->pg_ActivePage->FirstGadget,-1);
	PG->pg_GadgetsAdded=FALSE;

	Left = gi->gi_Domain.Left;
	Top = gi->gi_Domain.Top;
	oldactive=PG->pg_Active;
	PG->pg_Active=(gpi->gpi_Mouse.X-Left)/(container.Width/PG->pg_NumPages);

	if(oldactive!=PG->pg_Active)
	{
		if(rp=ObtainGIRPort(gi))
		{
			DoMethod( o, GM_RENDER, gi, rp, GREDRAW_REDRAW);
			ReleaseGIRPort( rp );
			retval = GMR_NOREUSE;
		}
		else retval = GMR_NOREUSE;
	}

	return(retval);
}

void SAVEDS PAGE_GetGadgetRect( Object *o,struct GadgetInfo *gi,struct Rectangle *rect )
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

ULONG TextFitColor(struct RastPort *RP,STRPTR columnstring,ULONG len,struct TextExtent *te,struct TextExtent *te2,WORD dir,UWORD bwidth,UWORD bheight)
{
	int c,b;
	ULONG chars;
	STRPTR copytext;

	if(copytext=AllocMem(len+100,MEMF_ANY|MEMF_CLEAR))
	{
		for(c=0,b=0;c<len;c++) if(columnstring[c]!='' && columnstring[c]!='_') copytext[b++]=columnstring[c]; else if(columnstring[c]=='') c++;
		copytext[b]='\0';
		chars=TextFit(RP,copytext,strlen(copytext),te,te2,dir,bwidth,bheight);
		FreeMem(copytext,len+100);
		return chars;
	}
	return 0;
}

void TextColor(Class *cl,Object *o,struct RastPort *RP,STRPTR text,ULONG len,struct DrawInfo *dri,struct IBox *container)
{
	ULONG flg,a,b,skipx;
	UBYTE Justify,Underline;
	struct PAGEData *PG = INST_DATA( cl, o );

	SetAPen(RP,dri->dri_Pens[TEXTPEN]);
	flg=FS_NORMAL;
	SetSoftStyle(RP,flg,AskSoftStyle(RP));
	Justify=1;
	Underline=0;

	for(a=0,skipx=0;a<strlen(text);a++)
	{
		if(text[a]!='' && text[a]!=((PG->pg_Underscore && Underline==0) ?'_':'\0'))
		{
			for(b=0;(text[a+b]!='') && (text[a+b]!='\0') && (text[a+b]!=((PG->pg_Underscore && Underline==0) ?'_':'\0')) && (b<len);b++);
			Move(RP,container->Left+skipx+container->Width/2-TextLength(RP,text+a,b)/2,container->Top+RP->TxBaseline);
			if(Underline==1 && PG->pg_Underscore)
			{
				Text(RP,"_",1);
				Move(RP,container->Left+skipx,container->Top+RP->TxBaseline);
				Underline=2;
			}
			Text(RP,text+a,b);
			Move(RP,container->Left+skipx,container->Top+RP->TxBaseline);
			skipx+=TextLength(RP,text+a,b);
			for(b=0;(text[a+b]!='') && (text[a+b]!='\0');b++);
			a+=b-1;
		}
		else
		{
			if(text[a]==(PG->pg_Underscore?'_':'\0'))
			{
				if(Underline==0) Underline=1;
			}
			else
			{
				a++;
				switch(text[a])
				{
					case 'p': flg=FS_NORMAL; SetSoftStyle(RP,flg,AskSoftStyle(RP)); break;				// Plain
					case 'i': flg|=FSF_ITALIC; SetSoftStyle(RP,flg,AskSoftStyle(RP)); break;			// Italic
					case 'u': flg|=FSF_UNDERLINED; SetSoftStyle(RP,flg,AskSoftStyle(RP)); break;	// Underline
					case 'b': flg|=FSF_BOLD; SetSoftStyle(RP,flg,AskSoftStyle(RP)); break;				// Bold
		
					case '0': SetAPen(RP,0); break;									// Set color
					case '1': SetAPen(RP,1); break;
					case '2': SetAPen(RP,2); break;
					case '3': SetAPen(RP,3); break;
					case '4': SetAPen(RP,4); break;
					case '5': SetAPen(RP,5); break;
					case '6': SetAPen(RP,6); break;
					case '7': SetAPen(RP,7); break;
					case '8': SetAPen(RP,8); break;
					case '9': SetAPen(RP,9); break;
				}
			}
		}
	}
}
