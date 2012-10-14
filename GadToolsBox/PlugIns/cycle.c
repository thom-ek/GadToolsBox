/*
** Cycle PlugIn
*/

#include"includes.h"
#include"cycle.h"

/*
** OwnTag definitions
*/

#define OWNTAG_Labels (TAG_USER+1)

/*
** Default Tags
*/

struct TagItem CycleTags[]=
{
	TAG_IGNORE,NULL, //GT_Underscore,'_',
	GA_Disabled,FALSE,
	GTCY_Labels,NULL,
	GTCY_Active,0,
	TAG_DONE,0
};

UBYTE CycleTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NONE,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem CycleOwnTags[]=
{
	OWNTAG_Labels,NULL,
	TAG_DONE,0
};

UBYTE CycleOwnTagsTypes[]=
{
	TAGTYPE_LIST,TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 4
#define GSIZE_NUMOWNTAGS 1

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGGT_General[];

struct Page PAGES_PROPERTIES_PG_CYCE_PAGES[]=
{
	&PAGES_PROPERTIES_PG_CYCE_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PG_CYCE_PAGES[2], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG CYCLE_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_DEFAULT:
			retval=CYCLE_DEFAULT_Gadget(cl,o,msg);
			break;
		case OM_INIT:
			retval=CYCLE_INIT_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_CYCE_Flags();
			PAGES_PROPERTIES_PG_CYCE_PAGES[0].FirstGadget=PGGT_GeneralGList;
			PAGES_PROPERTIES_PG_CYCE_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L13_GAD);
			PAGES_PROPERTIES_PG_CYCE_PAGES[1].FirstGadget=PG_CYCE_FlagsGList;
			PAGES_PROPERTIES_PG_CYCE_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L4_GAD);
			PAGES_PROPERTIES_PG_CYCE_PAGES[2].FirstGadget=PG_ListsGList;
			PAGES_PROPERTIES_PG_CYCE_PAGES[2].Name=GetCatString(MSG_PROPERTIES_PAGES_L5_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_CYCE_PAGES[2].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_CYCE_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_CYCE_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_CYCE_Flags();
			break;
		case OM_NEW:
			retval=CYCLE_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_DISPOSE:
			retval=CYCLE_DISPOSE_Gadget(cl,o,msg);
			break;
		case OM_CREATE:
			retval=CYCLE_CREATE_Gadget(cl,o,msg);
			break;
		case OM_DELETE:
			retval=CYCLE_DELETE_Gadget(cl,o,msg);
			break;
		case OM_READ:
			retval=CYCLE_READ_Gadget(cl,o,(struct piIO *)msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG CYCLE_READ_Gadget(piClass *cl,piObject *o,struct piIO *msg)
{
	if(CurrentChunk(msg->IFFh)->cn_Type==ID_GXGA && CurrentChunk(msg->IFFh)->cn_ID==ID_GADA)
	{
		struct TagItem *ti,*oti,tag,*sti;
		struct Node_Gadget *ng;
		struct Node *node;
		int a;
		UWORD numlabels,lablen;

		piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
		piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&oti);
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
				case GTCY_Labels: break;
				default:
					if(sti=FindTagItem(tag.ti_Tag,ti)) sti->ti_Data=tag.ti_Data;
					break;
			}
		}
		for(;a<4;a++) ReadChunkBytes(msg->IFFh,(APTR)&tag,sizeof(struct TagItem));
		ReadChunkBytes(msg->IFFh,(APTR)&numlabels,sizeof(UWORD));
		for(a=0;a<numlabels;a++)
		{
			ReadChunkBytes(msg->IFFh,(APTR)&lablen,sizeof(UWORD));
			if(sti=FindTagItem(OWNTAG_Labels,oti))
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

ULONG CYCLE_DEFAULT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	struct Node *n;
	
	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	if(n=MyAllocVec(sizeof(struct Node),MEMF_ANY))
	{
		if(n->ln_Name=MyAllocVec(8,MEMF_ANY))
		{
			strcpy(n->ln_Name,"<empty>");
			AddTail(List(FindTagItem(OWNTAG_Labels,ng->ng_OwnTags)->ti_Data),n);
		}
		else MyFreeVec(n);	
	}
	return TRUE;
}

ULONG CYCLE_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
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
	SetCheckMark(PropsWnd,PG_CYCE_FlagsGadgets[GD_PG_CYCE_FLAGS_HIGHLABEL],gtd->g_Flags&NG_HIGHLABEL);
	SetCheckMark(PropsWnd,PG_CYCE_FlagsGadgets[GD_PG_CYCE_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_CYCE_FlagsGadgets[GD_PG_CYCE_FLAGS_UNDERSCORE],GetTagData(GT_Underscore,0,ng->ng_Tags));

	if(active==2) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetListView(PropsWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],GetTagData(OWNTAG_Labels,0,ng->ng_OwnTags));
	SetInteger(PropsWnd,PG_ListsGadgets[GD_PG_LISTS_ACTIVE],GetTagData(GTCY_Active,0,ng->ng_Tags));
	SetString(PropsWnd,PG_ListsGadgets[GD_PG_LISTS_EDIT],"");
	DisableGadget(PG_ListsGadgets[GD_PG_LISTS_UP],PropsWnd,TRUE);
	DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DOWN],PropsWnd,TRUE);
	DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DEL],PropsWnd,TRUE);
	DisableGadget(PG_ListsGadgets[GD_PG_LISTS_EDIT],PropsWnd,TRUE);
	return TRUE;
}

ULONG CYCLE_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_GADTOOLS,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)CycleTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)CycleOwnTagsTypes,
		GAOBJ_ID,CYCLE_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_CYCE_PAGES,
		GAOBJLIST_List,0,
		GAOBJLIST_Active,3,
		GAOBJLIST_Tags,1,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);
		gtd=(struct GadToolsData *)GadgetData(ng);

		CopyTagItems(CycleTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(CycleOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		gtd->g_Kind=CYCLE_KIND;
		FindTagItem(OWNTAG_Labels,ng->ng_OwnTags)->ti_Data=(ULONG)MyAllocVec(sizeof(struct List),MEMF_PUBLIC);
		NewList(List(FindTagItem(OWNTAG_Labels,ng->ng_OwnTags)->ti_Data));
		return (ULONG)obj;
	}
	return FALSE;
}

ULONG CYCLE_DISPOSE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct TagItem *ti;
	struct List *l;
	struct Node *n;

	piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&ti);
	if(l=(struct List *)FindTagItem(OWNTAG_Labels,ti)->ti_Data)
	{
		while(n=RemTail(l))
		{
			if(n->ln_Name) MyFreeVec(n->ln_Name);
			MyFreeVec(n);
		}
		MyFreeVec(l);
	}
	return piDoSuperMethodA(cl,o,msg);
}

ULONG CYCLE_CREATE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG a;
	STRPTR *tab;
	struct TagItem *ti,*oti;
	struct Node *n;

	piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&oti);
	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);

	for(a=0,n=GetHead(List(GetTagData(OWNTAG_Labels,0,oti)));GetSucc(n);n=GetSucc(n),a++);

	FindTagItem(GTCY_Labels,ti)->ti_Data=(ULONG)MyAllocVec((a+1)*sizeof(STRPTR),MEMF_PUBLIC|MEMF_CLEAR);
	tab=(STRPTR *)FindTagItem(GTCY_Labels,ti)->ti_Data;
	for(a=0,n=GetHead(List(GetTagData(OWNTAG_Labels,0,oti)));GetSucc(n);n=GetSucc(n),a++) tab[a]=n->ln_Name;
	tab[a]=NULL;
	return piDoSuperMethodA(cl,o,msg);
}

ULONG CYCLE_DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct TagItem *ti;
	APTR tab;

	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);

	tab=(APTR)GetTagData(GTCY_Labels,0,ti);
	if(tab) MyFreeVec(tab);
	FindTagItem(GTCY_Labels,ti)->ti_Data=NULL;
	return piDoSuperMethodA(cl,o,msg);
}

int FN_PG_CYCE_FLAGS_UNDERSCORE_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	ti[0].ti_Tag=PropertiesMsg.Code?GT_Underscore:TAG_IGNORE;
	ti[0].ti_Data='_';
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_CYCE_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_CYCE_FLAGS_HIGHLABEL_Clicked( void )
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
