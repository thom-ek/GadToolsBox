/*
** TapeDeck PlugIn
*/

#include"includes.h"
#include"tapedeck.h"

/*
** Defines
*/

/*
** OwnTag definitions
*/

/*
** Default Tags
*/

struct TagItem TapeDeckTags[]=
{
	GA_Disabled,FALSE,
	TDECK_Paused,FALSE,
	TDECK_Tape,FALSE,
	TDECK_Frames,0,
	TDECK_CurrentFrame,0,
	TAG_DONE,0
};

UBYTE TapeDeckTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem TapeDeckOwnTags[]=
{
	TAG_DONE,0
};

UBYTE TapeDeckOwnTagsTypes[]=
{
	TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 5
#define GSIZE_NUMOWNTAGS 0

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGBO_General[];

struct Page PAGES_PROPERTIES_PG_TDEK_PAGES[]=
{
	&PAGES_PROPERTIES_PG_TDEK_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG TAPEDECK_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=TAPEDECK_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=TAPEDECK_APPLY_Gadget(cl,o,msg);
			break;
		case OM_DEFAULT:
			retval=TAPEDECK_DEFAULT_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_TDEK_Flags();
			PAGES_PROPERTIES_PG_TDEK_PAGES[0].FirstGadget=PGBO_GeneralGList;
			PAGES_PROPERTIES_PG_TDEK_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L0_GAD);
			PAGES_PROPERTIES_PG_TDEK_PAGES[1].FirstGadget=PG_TDEK_FlagsGList;
			PAGES_PROPERTIES_PG_TDEK_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L25_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_TDEK_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_TDEK_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_TDEK_Flags();
			break;
		case OM_NEW:
			retval=TAPEDECK_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_CREATE:
			retval=TAPEDECK_CREATE_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_DELETE:
			retval=TAPEDECK_DELETE_Gadget(cl,o,msg);
			break;
		case OM_RENDER:
			retval=TAPEDECK_RENDER_Gadget(cl,o,msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG TAPEDECK_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_TAPE],GetTagData(TDECK_Tape,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_PAUSED],GetTagData(TDECK_Paused,0,ng->ng_Tags));
	if(GetTagData(TDECK_Tape,0,ng->ng_Tags))
	{
		DisableGadget(PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_FRAMES],PropsWnd,TRUE);
		DisableGadget(PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_CURRENT],PropsWnd,TRUE);
	}
	else
	{
		DisableGadget(PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_FRAMES],PropsWnd,FALSE);
		DisableGadget(PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_CURRENT],PropsWnd,FALSE);
	}
	SetInteger(PropsWnd,PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_FRAMES],GetTagData(TDECK_Frames,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_CURRENT],GetTagData(TDECK_CurrentFrame,0,ng->ng_Tags));
	return TRUE;
}

ULONG TAPEDECK_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	FindTagItem(TDECK_Frames,ng->ng_Tags)->ti_Data=GetInteger(PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_FRAMES]);
	FindTagItem(TDECK_CurrentFrame,ng->ng_Tags)->ti_Data=GetInteger(PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_CURRENT]);
	return TRUE;
}

ULONG TAPEDECK_DEFAULT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	
	ng->ng_Width=202;
	ng->ng_Height=14;
	return TRUE;
}

ULONG TAPEDECK_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_BOOPSI,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)TapeDeckTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)TapeDeckOwnTagsTypes,
		GAOBJ_ID,TAPEDECK_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_TDEK_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);

		CopyTagItems(TapeDeckTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(TapeDeckOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);

		return (ULONG)obj;
	}
	return FALSE;
}

ULONG TAPEDECK_CREATE_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
{
	struct TagItem *ti;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	WORD x,y,w,h;

	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
	
	if(ng->ng_RelX) x=-(nw->nw_Window->Width-nw->nw_Window->BorderLeft-ng->ng_X-1);
	else x=piCX(o);
	if(ng->ng_RelY) y=-(nw->nw_Window->Height-nw->nw_Window->BorderTop-ng->ng_Y-1);
	else y=piCY(o);
	if(ng->ng_RelWidth) w=-(nw->nw_Window->BorderLeft+nw->nw_Window->BorderRight+(nw->nw_Window->Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight-ng->ng_Width)-1);
	else w=piCW(o)+1;
	if(ng->ng_RelHeight) h=-(nw->nw_Window->BorderTop+nw->nw_Window->BorderBottom+(nw->nw_Window->Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom-ng->ng_Height)-1);
	else h=piCH(o)+1;

	return (ULONG) (ng->ng_Gadget=NewObject(NULL, "tapedeck.gadget",
									ng->ng_RelX?GA_RelRight:GA_Left,x,
									ng->ng_RelY?GA_RelBottom:GA_Top,y,
									ng->ng_RelWidth?GA_RelWidth:GA_Width,w,
									ng->ng_RelHeight?GA_RelHeight:GA_Height,h,
									(msg->Window) ? GA_Previous : TAG_IGNORE, msg->Window,
									GA_ID,0,
									TAG_MORE,ti));
}

ULONG TAPEDECK_DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	if(ng->ng_Gadget && ng->ng_Window) RemoveGadget(((struct Node_Window *)OBJECT_DATA(ng->ng_Window))->nw_Window,ng->ng_Gadget);
	if(ng->ng_Gadget) DisposeObject(ng->ng_Gadget);
	ng->ng_Gadget=NULL;
	return piDoSuperMethodA(cl,o,msg);
}

extern DrawRect(struct RastPort *,UWORD,UWORD,BOOL);

ULONG TAPEDECK_RENDER_Gadget(piClass *cl,piObject *o,Msg msg)
{
	UWORD x,y,w,h;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	struct Gadget *g,*ga;
	BOOL fill=FALSE;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);

	x=piCX(o);
	y=piCY(o);
	w=piCW(o);
	h=piCH(o);

	SetAPen(nw->nw_Window->RPort,DrawInfo->dri_Pens[BACKGROUNDPEN]);
	SetDrMd(nw->nw_Window->RPort,JAM1);
	RectFill(nw->nw_Window->RPort,x,y,x+w,y+h);

	g=CreateContext(&g);
	if(ga=(struct Gadget *)piDoMethod(o,OM_CREATE,g))
	{
		AddGList(nw->nw_Window,ga,-1,-1,NULL);
		RefreshGList(g,nw->nw_Window,NULL,-1);
		RemoveGList(nw->nw_Window,ga,-1);
	}
	piDoMethod(o,OM_DELETE);
	g->NextGadget=NULL;
	FreeGadgets(g);

	if(ng->ng_Selected && GUI.gui_Mode==MODE_GADGET)
	{
		SetAPen(nw->nw_Window->RPort,1);
		SetDrMd(nw->nw_Window->RPort,COMPLEMENT);
		if(CurrentGadget==o) fill=TRUE;
		DrawRect(nw->nw_Window->RPort,x,y,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x+(w/2)-2,y,fill);
		DrawRect(nw->nw_Window->RPort,x+(w/2)-2,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x,y+(h/2)-2,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y+(h/2)-2,fill);
	}
	return TRUE;
}

int FN_PG_TDEK_FLAGS_FRAMES_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TDEK_FLAGS_CURRENT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TDEK_FLAGS_PAUSED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TDECK_Paused,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TDEK_FLAGS_TAPE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TDECK_Tape,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	if(PropertiesMsg.Code)
	{
		DisableGadget(PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_FRAMES],PropertiesWnd,TRUE);
		DisableGadget(PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_CURRENT],PropertiesWnd,TRUE);
	}
	else
	{
		DisableGadget(PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_FRAMES],PropertiesWnd,FALSE);
		DisableGadget(PG_TDEK_FlagsGadgets[GD_PG_TDEK_FLAGS_CURRENT],PropertiesWnd,FALSE);
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TDEK_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
