/*
** ListView PlugIn
*/

#include"includes.h"
#include"listview.h"

/*
** OwnTag definitions
*/

#define OWNTAG_Arrows (TAG_USER+1)

/*
** Default Tags
*/

struct TagItem ListViewTags[]=
{
	TAG_IGNORE,NULL, //GT_Underscore,'_',
	TAG_IGNORE,NULL, //GTLV_ShowSelected,FALSE,
	GA_Disabled,FALSE,
	GTLV_Labels,NULL,
	GTLV_ReadOnly,FALSE,
	GTLV_ScrollWidth,16,
	LAYOUTA_Spacing,0,
	TAG_DONE,0
};

UBYTE ListViewTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_LIST,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem ListViewOwnTags[]=
{
	OWNTAG_Arrows, 0,
	TAG_DONE,0
};

UBYTE ListViewOwnTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 7
#define GSIZE_NUMOWNTAGS 1

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGGT_General[];

struct Page PAGES_PROPERTIES_PG_LVIE_PAGES[]=
{
	&PAGES_PROPERTIES_PG_LVIE_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PG_LVIE_PAGES[2], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG LISTVIEW_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_DEFAULT:
			retval=LISTVIEW_DEFAULT_Gadget(cl,o,msg);
			break;
		case OM_INIT:
			retval=LISTVIEW_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=LISTVIEW_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_LVIE_Flags();
			PAGES_PROPERTIES_PG_LVIE_PAGES[0].FirstGadget=PGGT_GeneralGList;
			PAGES_PROPERTIES_PG_LVIE_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L13_GAD);
			PAGES_PROPERTIES_PG_LVIE_PAGES[1].FirstGadget=PG_LVIE_FlagsGList;
			PAGES_PROPERTIES_PG_LVIE_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L10_GAD);
			PAGES_PROPERTIES_PG_LVIE_PAGES[2].FirstGadget=PG_ListsGList;
			PAGES_PROPERTIES_PG_LVIE_PAGES[2].Name=GetCatString(MSG_PROPERTIES_PAGES_L5_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_LVIE_PAGES[2].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_LVIE_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_LVIE_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_LVIE_Flags();
			break;
		case OM_NEW:
			retval=LISTVIEW_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_DISPOSE:
			retval=LISTVIEW_DISPOSE_Gadget(cl,o,msg);
			break;
		case OM_READ:
			retval=LISTVIEW_READ_Gadget(cl,o,(struct piIO *)msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG LISTVIEW_READ_Gadget(piClass *cl,piObject *o,struct piIO *msg)
{
	if(CurrentChunk(msg->IFFh)->cn_Type==ID_GXGA && CurrentChunk(msg->IFFh)->cn_ID==ID_GADA)
	{
		struct TagItem *ti,tag,*sti;
		struct Node_Gadget *ng;
		struct Node *node;
		int a;
		UWORD numlabels,lablen;

		piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
		piGetAttr(o,OBJ_Node,(ULONG *)&ng);

		for(a=0;a<ng->ng_Type-1;a++)
		{
			ReadChunkBytes(msg->IFFh,(APTR)&tag,sizeof(struct TagItem));
			switch(tag.ti_Tag)
			{
				case GT_Underscore:
					ti[0].ti_Tag=GT_Underscore;
					ti[0].ti_Data='_';
					break;
				case GTLV_Labels: break;
				case GTLV_ShowSelected:
					ti[1].ti_Tag=GTLV_ShowSelected;
					ti[1].ti_Data=NULL;
					break;
				default:
					if(sti=FindTagItem(tag.ti_Tag,ti)) sti->ti_Data=tag.ti_Data;
					break;
			}
		}
		for(;a<6;a++) ReadChunkBytes(msg->IFFh,(APTR)&tag,sizeof(struct TagItem));
		ReadChunkBytes(msg->IFFh,(APTR)&numlabels,sizeof(UWORD));
		for(a=0;a<numlabels;a++)
		{
			ReadChunkBytes(msg->IFFh,(APTR)&lablen,sizeof(UWORD));
			if(sti=FindTagItem(GTLV_Labels,ti))
				if(node=(struct Node *)MyAllocVec(sizeof(struct Node),MEMF_ANY))
					if(node->ln_Name=MyAllocVec(lablen,MEMF_ANY))
					{
						ReadChunkBytes(msg->IFFh,(APTR)node->ln_Name,lablen);
						AddTail(List(sti->ti_Data),node);
					}
		}
		return 0;
	}
	else
		return piDoSuperMethodA(cl,o,(Msg)msg);
}

ULONG LISTVIEW_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(o,GAOBJ_NodeData,(ULONG *)&gtd);
	piDoSuperMethodA(cl,o,msg);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_HIGHLABEL],gtd->g_Flags&NG_HIGHLABEL);
	SetCheckMark(PropsWnd,PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_UNDERSCORE],GetTagData(GT_Underscore,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_SPACING],GetTagData(LAYOUTA_Spacing,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_ARROWS],GetTagData(GTLV_ScrollWidth,0,ng->ng_Tags));

	SetCycle(PropsWnd,PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_ARROWS_SEL],GetTagData(OWNTAG_Arrows,0,ng->ng_OwnTags));
	if(GetTagData(OWNTAG_Arrows,0,ng->ng_OwnTags)==1) DisableGadget(PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_ARROWS],PropsWnd,FALSE);
	else DisableGadget(PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_ARROWS],PropsWnd,TRUE);

	if(active==2) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetListView(PropsWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],GetTagData(GTLV_Labels,0,ng->ng_Tags));
//	SetInteger(PropsWnd,PG_ListsGadgets[GD_PG_LISTS_ACTIVE],GetTagData(GTLV_Active,0,ng->ng_Tags));
	SetString(PropsWnd,PG_ListsGadgets[GD_PG_LISTS_EDIT],"");
	DisableGadget(PG_ListsGadgets[GD_PG_LISTS_UP],PropsWnd,TRUE);
	DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DOWN],PropsWnd,TRUE);
	DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DEL],PropsWnd,TRUE);
	DisableGadget(PG_ListsGadgets[GD_PG_LISTS_EDIT],PropsWnd,TRUE);
	return TRUE;
}

ULONG LISTVIEW_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	FindTagItem(LAYOUTA_Spacing,ng->ng_Tags)->ti_Data=GetInteger(PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_SPACING]);
	FindTagItem(GTLV_ScrollWidth,ng->ng_Tags)->ti_Data=GetInteger(PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_ARROWS]);

	return TRUE;
}

ULONG LISTVIEW_DEFAULT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	
	ng->ng_Width=100;
	ng->ng_Height=100;
	return TRUE;
}

ULONG LISTVIEW_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_GADTOOLS,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)ListViewTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)ListViewOwnTagsTypes,
		GAOBJ_ID,LISTVIEW_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_LVIE_PAGES,
		GAOBJLIST_List,3,
		GAOBJLIST_Active,0,
		GAOBJLIST_Tags,0,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);
		gtd=(struct GadToolsData *)GadgetData(ng);

		CopyTagItems(ListViewTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(ListViewOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		FindTagItem(GTLV_Labels,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(sizeof(struct List),MEMF_PUBLIC);
		NewList(List(FindTagItem(GTLV_Labels,ng->ng_Tags)->ti_Data));
		gtd->g_Kind=LISTVIEW_KIND;
		return (ULONG)obj;
	}
	return FALSE;
}

ULONG LISTVIEW_DISPOSE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct TagItem *ti;
	struct List *l;
	struct Node *n;

	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
	l=(struct List *)FindTagItem(GTLV_Labels,ti)->ti_Data;
	while(n=RemTail(l))
	{
		if(n->ln_Name) MyFreeVec(n->ln_Name);
		MyFreeVec(n);
	}
	MyFreeVec(l);
	return piDoSuperMethodA(cl,o,msg);
}

int FN_PG_LVIE_FLAGS_SPACING_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LVIE_FLAGS_ARROWS_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LVIE_FLAGS_UNDERSCORE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);

	ti[0].ti_Tag=PropertiesMsg.Code?GT_Underscore:TAG_IGNORE;
	ti[0].ti_Data='_';
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LVIE_FLAGS_SHOWSELECTED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	ti[1].ti_Tag=PropertiesMsg.Code?GTLV_ShowSelected:TAG_IGNORE;
	ti[1].ti_Data=NULL;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LVIE_FLAGS_READONLY_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GTLV_ReadOnly,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LVIE_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LVIE_FLAGS_HIGHLABEL_Clicked( void )
{
	ULONG code;
	struct GadToolsData *gtd;

	piGetAttr(CurrentGadget,GAOBJ_NodeData,(ULONG *)&gtd);
	code=PropertiesMsg.Code;
	DoFlag(gtd->g_Flags,NG_HIGHLABEL);
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LVIE_FLAGS_ARROWS_SEL_Clicked( void )
{
	struct TagItem *ti,*oti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	piGetAttr(CurrentGadget,GAOBJ_OwnTags,(ULONG *)&oti);
	FindTagItem(OWNTAG_Arrows,oti)->ti_Data=PropertiesMsg.Code;
	switch(PropertiesMsg.Code)
	{
		case 1:
			DisableGadget(PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_ARROWS],PropertiesWnd,FALSE);
			break;
		case 2:
			DisableGadget(PG_LVIE_FlagsGadgets[GD_PG_LVIE_FLAGS_ARROWS],PropertiesWnd,TRUE);
			break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
