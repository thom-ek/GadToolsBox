/*
** TreeView PlugIn
*/

#include"includes.h"
#include"treeview.h"

/*
** OwnTag definitions
*/

/*
** Default Tags
*/

struct TagItem TreeViewTags[]=
{
	GA_Disabled,FALSE,
	LIST_Labels,NULL,
	LIST_ReadOnly,FALSE,
	LIST_Border,FALSE,
	LIST_Columns,1,
	LIST_MultiSelect,FALSE,
	LIST_ShowTreeLines,FALSE,
	LIST_Format,NULL,
	LIST_ShowSelected,FALSE,
	LIST_IsTree,FALSE,
	TAG_DONE,0
};

UBYTE TreeViewTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_LIST,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_TEXT,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem TreeViewOwnTags[]=
{
	TAG_DONE,0
};

UBYTE TreeViewOwnTagsTypes[]=
{
	TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 10
#define GSIZE_NUMOWNTAGS 0

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGBO_General[];

struct Page PAGES_PROPERTIES_PG_TRLV_PAGES[]=
{
	&PAGES_PROPERTIES_PG_TRLV_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG TREEVIEW_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_DEFAULT:
			retval=TREEVIEW_DEFAULT_Gadget(cl,o,msg);
			break;
		case OM_INIT:
			retval=TREEVIEW_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=TREEVIEW_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_TRLV_Flags();
			PAGES_PROPERTIES_PG_TRLV_PAGES[0].FirstGadget=PGBO_GeneralGList;
			PAGES_PROPERTIES_PG_TRLV_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L0_GAD);
			PAGES_PROPERTIES_PG_TRLV_PAGES[1].FirstGadget=PG_TRLV_FlagsGList;
			PAGES_PROPERTIES_PG_TRLV_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L40_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_TRLV_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_TRLV_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_TRLV_Flags();
			break;
		case OM_NEW:
			retval=TREEVIEW_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_DISPOSE:
			retval=TREEVIEW_DISPOSE_Gadget(cl,o,msg);
			break;
		case OM_CREATE:
			retval=TREEVIEW_CREATE_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_DELETE:
			retval=TREEVIEW_DELETE_Gadget(cl,o,msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG TREEVIEW_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PG_TRLV_FlagsGadgets[GD_PG_TRLV_FLAGS_BORDER],GetTagData(LIST_Border,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TRLV_FlagsGadgets[GD_PG_TRLV_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TRLV_FlagsGadgets[GD_PG_TRLV_FLAGS_SHOWSELECTED],GetTagData(LIST_ShowSelected,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TRLV_FlagsGadgets[GD_PG_TRLV_FLAGS_MULTISELECT],GetTagData(LIST_MultiSelect,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TRLV_FlagsGadgets[GD_PG_TRLV_FLAGS_SHOWTREELINES],GetTagData(LIST_ShowTreeLines,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TRLV_FlagsGadgets[GD_PG_TRLV_FLAGS_TREE],GetTagData(LIST_IsTree,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_TRLV_FlagsGadgets[GD_PG_TRLV_FLAGS_COLUMNS],GetTagData(LIST_Columns,0,ng->ng_Tags));
	SetString(PropsWnd,PG_TRLV_FlagsGadgets[GD_PG_TRLV_FLAGS_FORMAT],GetTagData(LIST_Format,0,ng->ng_Tags));

	return TRUE;
}

ULONG TREEVIEW_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	FindTagItem(LIST_Columns,ng->ng_Tags)->ti_Data=GetInteger(PG_TRLV_FlagsGadgets[GD_PG_TRLV_FLAGS_COLUMNS]);
	if(FindTagItem(LIST_Format,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(LIST_Format,ng->ng_Tags)->ti_Data);
	FindTagItem(LIST_Format,ng->ng_Tags)->ti_Data=NULL;
	if(FindTagItem(LIST_Format,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(strlen(GetString(PG_TRLV_FlagsGadgets[GD_PG_TRLV_FLAGS_FORMAT]))+1,MEMF_PUBLIC))
		strcpy((char *)FindTagItem(LIST_Format,ng->ng_Tags)->ti_Data,GetString(PG_TRLV_FlagsGadgets[GD_PG_TRLV_FLAGS_FORMAT]));

	return TRUE;
}

ULONG TREEVIEW_DEFAULT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	
	ng->ng_Width=100;
	ng->ng_Height=100;
	return TRUE;
}

ULONG TREEVIEW_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_BOOPSI,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)TreeViewTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)TreeViewOwnTagsTypes,
		GAOBJ_ID,TREEVIEW_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_TRLV_PAGES,
		GAOBJLIST_List,1,
		GAOBJLIST_Active,0,
		GAOBJLIST_Tags,0,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);

		CopyTagItems(TreeViewTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(TreeViewOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		FindTagItem(LIST_Labels,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(sizeof(struct LVList),MEMF_PUBLIC);
		NewList(List(FindTagItem(LIST_Labels,ng->ng_Tags)->ti_Data));
		return (ULONG)obj;
	}
	return FALSE;
}

ULONG TREEVIEW_DISPOSE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct TagItem *ti;
	struct LVList *l;
	struct LVNode *n;

	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
	l=(struct LVList *)FindTagItem(LIST_Labels,ti)->ti_Data;
	while(n=(struct LVNode *)RemTail(List(l)))
	{
		if(n->ln_Name) MyFreeVec(n->ln_Name);
		MyFreeVec(n);
	}
	MyFreeVec(l);
	return piDoSuperMethodA(cl,o,msg);
}

extern Class *ListViewClass;

ULONG TREEVIEW_CREATE_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
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

	return (ULONG) (ng->ng_Gadget=NewObject(ListViewClass, NULL,
									ng->ng_RelX?GA_RelRight:GA_Left,x,
									ng->ng_RelY?GA_RelBottom:GA_Top,y,
									ng->ng_RelWidth?GA_RelWidth:GA_Width,w,
									ng->ng_RelHeight?GA_RelHeight:GA_Height,h,
									(msg->Window) ? GA_Previous : TAG_IGNORE, msg->Window,
									GA_ID,0,
									TAG_MORE,ti));
}

ULONG TREEVIEW_DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	if(ng->ng_Gadget && ng->ng_Window) RemoveGadget(((struct Node_Window *)OBJECT_DATA(ng->ng_Window))->nw_Window,ng->ng_Gadget);
	if(ng->ng_Gadget) DisposeObject(ng->ng_Gadget);
	ng->ng_Gadget=NULL;
	return piDoSuperMethodA(cl,o,msg);
}

int FN_PG_TRLV_FLAGS_COLUMNS_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TRLV_FLAGS_FORMAT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TRLV_FLAGS_SHOWSELECTED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(LIST_ShowSelected,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TRLV_FLAGS_READONLY_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(LIST_ReadOnly,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TRLV_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TRLV_FLAGS_SHOWTREELINES_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(LIST_ShowTreeLines,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TRLV_FLAGS_TREE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(LIST_IsTree,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TRLV_FLAGS_MULTISELECT_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(LIST_MultiSelect,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TRLV_FLAGS_BORDER_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(LIST_Border,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

