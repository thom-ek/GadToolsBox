/*
** PopUpMenu PlugIn
*/

#include"includes.h"
#include"popupmenu.h"

/*
** Default Tags
*/

struct TagItem PopUpMenuTags[]=
{
	GA_Disabled,FALSE,
	PUMG_Labels,NULL,
	PUMG_Active,0,
	PUMG_NewLook,FALSE,
	TAG_DONE,0
};

UBYTE PopUpMenuTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_LIST,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem PopUpMenuOwnTags[]=
{
	TAG_DONE,0
};

UBYTE PopUpMenuOwnTagsTypes[]=
{
	TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 4
#define GSIZE_NUMOWNTAGS 0

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGBO_General[];

struct Page PAGES_PROPERTIES_PG_PUPM_PAGES[]=
{
	&PAGES_PROPERTIES_PG_PUPM_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PG_PUPM_PAGES[2], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG POPUPMENU_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=POPUPMENU_INIT_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_PUPM_Flags();
			PAGES_PROPERTIES_PG_PUPM_PAGES[0].FirstGadget=PGBO_GeneralGList;
			PAGES_PROPERTIES_PG_PUPM_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L0_GAD);
			PAGES_PROPERTIES_PG_PUPM_PAGES[1].FirstGadget=PG_PUPM_FlagsGList;
			PAGES_PROPERTIES_PG_PUPM_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L17_GAD);
			PAGES_PROPERTIES_PG_PUPM_PAGES[2].FirstGadget=PG_ListsGList;
			PAGES_PROPERTIES_PG_PUPM_PAGES[2].Name=GetCatString(MSG_PROPERTIES_PAGES_L5_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_PUPM_PAGES[2].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_PUPM_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_PUPM_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_PUPM_Flags();
			break;
		case OM_NEW:
			retval=POPUPMENU_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_DISPOSE:
			retval=POPUPMENU_DISPOSE_Gadget(cl,o,msg);
			break;
		case OM_CREATE:
			retval=POPUPMENU_CREATE_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_DELETE:
			retval=POPUPMENU_DELETE_Gadget(cl,o,msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG POPUPMENU_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PG_PUPM_FlagsGadgets[GD_PG_PUPM_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_PUPM_FlagsGadgets[GD_PG_PUPM_FLAGS_NEWLOOK],GetTagData(PUMG_NewLook,0,ng->ng_Tags));
	return TRUE;
}

ULONG POPUPMENU_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_BOOPSI,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)PopUpMenuTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)PopUpMenuOwnTagsTypes,
		GAOBJ_ID,POPUPMENU_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_PUPM_PAGES,
		GAOBJLIST_List,1,
		GAOBJLIST_Active,2,
		GAOBJLIST_Tags,0,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);

		CopyTagItems(PopUpMenuTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(PopUpMenuOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		FindTagItem(PUMG_Labels,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(sizeof(struct List),MEMF_PUBLIC);
		NewList(List(FindTagItem(PUMG_Labels,ng->ng_Tags)->ti_Data));
		return (ULONG)obj;
	}
	return FALSE;
}

ULONG POPUPMENU_DISPOSE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct TagItem *ti;
	struct List *l;
	struct Node *n;

	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
	l=(struct List *)FindTagItem(PUMG_Labels,ti)->ti_Data;
	while(n=RemTail(l))
	{
		if(n->ln_Name) MyFreeVec(n->ln_Name);
		MyFreeVec(n);
	}
	MyFreeVec(l);
	return piDoSuperMethodA(cl,o,msg);
}

ULONG POPUPMENU_CREATE_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
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

	return (ULONG) (ng->ng_Gadget=NewObject(PopUpMenuClass, NULL,
									ng->ng_RelX?GA_RelRight:GA_Left,x,
									ng->ng_RelY?GA_RelBottom:GA_Top,y,
									ng->ng_RelWidth?GA_RelWidth:GA_Width,w,
									ng->ng_RelHeight?GA_RelHeight:GA_Height,h,
									(msg->Window) ? GA_Previous : TAG_IGNORE, msg->Window,
									GA_ID,0,
									TAG_MORE,ti));
}

ULONG POPUPMENU_DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	if(ng->ng_Gadget && ng->ng_Window) RemoveGadget(((struct Node_Window *)OBJECT_DATA(ng->ng_Window))->nw_Window,ng->ng_Gadget);
	if(ng->ng_Gadget) DisposeObject(ng->ng_Gadget);
	ng->ng_Gadget=NULL;
	return piDoSuperMethodA(cl,o,msg);
}

int FN_PG_PUPM_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PUPM_FLAGS_NEWLOOK_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(PUMG_NewLook,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
