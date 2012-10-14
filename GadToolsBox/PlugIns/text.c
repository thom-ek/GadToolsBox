/*
** Text PlugIn
*/

#include"includes.h"
#include"text.h"

/*
** Defines
*/

/*
** OwnTag definitions
*/

#define OWNTAG_ImageTitle 1

/*
** Default TextTags
*/

struct TagItem TextTags[]=
{
	GA_Disabled,FALSE,
	GTTX_Border,FALSE,
	GTTX_CopyText,FALSE,
	GTTX_Justification,GTJ_LEFT,
	GTTX_Clipped,FALSE,
	GTTX_Text,NULL,
	TAG_DONE,0
};

UBYTE TextTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_TEXT,TAGTYPE_DONE
};

struct TagItem TextOwnTags[]=
{
	TAG_DONE,0
};

UBYTE TextOwnTagsTypes[]=
{
	TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 6
#define GSIZE_NUMOWNTAGS 0

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGGT_General[];

struct Page PAGES_PROPERTIES_PG_TEXT_PAGES[]=
{
	&PAGES_PROPERTIES_PG_TEXT_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG TEXT_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=TEXT_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=TEXT_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_TEXT_Flags();
			PAGES_PROPERTIES_PG_TEXT_PAGES[0].FirstGadget=PGGT_GeneralGList;
			PAGES_PROPERTIES_PG_TEXT_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L13_GAD);
			PAGES_PROPERTIES_PG_TEXT_PAGES[1].FirstGadget=PG_TEXT_FlagsGList;
			PAGES_PROPERTIES_PG_TEXT_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L26_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_TEXT_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_TEXT_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_TEXT_Flags();
			break;
		case OM_NEW:
			retval=TEXT_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_READ:
			retval=TEXT_READ_Gadget(cl,o,(struct piIO *)msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG TEXT_READ_Gadget(piClass *cl,piObject *o,struct piIO *msg)
{
	if(CurrentChunk(msg->IFFh)->cn_Type==ID_GXGA && CurrentChunk(msg->IFFh)->cn_ID==ID_GADA)
	{
		struct TagItem *ti,tag,*sti;
		struct Node_Gadget *ng;
		int a;
		UWORD lenstr;

		piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
		piGetAttr(o,OBJ_Node,(ULONG *)&ng);

		for(a=0;a<ng->ng_Type-1;a++)
		{
			ReadChunkBytes(msg->IFFh,(APTR)&tag,sizeof(struct TagItem));
			switch(tag.ti_Tag)
			{
				case GTTX_Text: break;
				default:
					if(sti=FindTagItem(tag.ti_Tag,ti)) sti->ti_Data=tag.ti_Data;
					break;
			}
		}
		for(;a<3;a++) ReadChunkBytes(msg->IFFh,(APTR)&tag,sizeof(struct TagItem));
		ReadChunkBytes(msg->IFFh,(APTR)&lenstr,sizeof(UWORD));
		if(lenstr)
		{
			if(FindTagItem(GTTX_Text,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(GTTX_Text,ng->ng_Tags)->ti_Data);
			FindTagItem(GTTX_Text,ng->ng_Tags)->ti_Data=NULL;
			if(FindTagItem(GTTX_Text,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(lenstr,MEMF_PUBLIC))
				ReadChunkBytes(msg->IFFh,(APTR)FindTagItem(GTTX_Text,ng->ng_Tags)->ti_Data,lenstr);
		}
		return 0;
	}
	else
		return piDoSuperMethodA(cl,o,(Msg)msg);
}

ULONG TEXT_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
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
	SetCheckMark(PropsWnd,PG_TEXT_FlagsGadgets[GD_PG_TEXT_FLAGS_HIGHLABEL],gtd->g_Flags&NG_HIGHLABEL);
	SetCheckMark(PropsWnd,PG_TEXT_FlagsGadgets[GD_PG_TEXT_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TEXT_FlagsGadgets[GD_PG_TEXT_FLAGS_BORDER],GetTagData(GTTX_Border,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TEXT_FlagsGadgets[GD_PG_TEXT_FLAGS_CLIPPED],GetTagData(GTTX_Clipped,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TEXT_FlagsGadgets[GD_PG_TEXT_FLAGS_COPYTEXT],GetTagData(GTTX_CopyText,0,ng->ng_Tags));
	SetString(PropsWnd,PG_TEXT_FlagsGadgets[GD_PG_TEXT_FLAGS_DEFAULT],GetTagData(GTTX_Text,0,ng->ng_Tags));
	switch(GetTagData(GTTX_Justification,GTJ_LEFT,ng->ng_Tags))
	{
		case GTJ_LEFT: SetCycle(PropsWnd,PG_TEXT_FlagsGadgets[GD_PG_TEXT_FLAGS_JUSTIFICATION],GTJ_LEFT); break;
		case GTJ_RIGHT: SetCycle(PropsWnd,PG_TEXT_FlagsGadgets[GD_PG_TEXT_FLAGS_JUSTIFICATION],GTJ_RIGHT); break;
		case GTJ_CENTER: SetCycle(PropsWnd,PG_TEXT_FlagsGadgets[GD_PG_TEXT_FLAGS_JUSTIFICATION],GTJ_CENTER); break;
	}
	return TRUE;
}

ULONG TEXT_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	if(FindTagItem(GTTX_Text,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(GTTX_Text,ng->ng_Tags)->ti_Data);
	FindTagItem(GTTX_Text,ng->ng_Tags)->ti_Data=NULL;
	if(FindTagItem(GTTX_Text,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(strlen(GetString(PG_TEXT_FlagsGadgets[GD_PG_TEXT_FLAGS_DEFAULT]))+1,MEMF_PUBLIC))
		strcpy((char *)((struct TagItem *)FindTagItem(GTTX_Text,ng->ng_Tags)->ti_Data),GetString(PG_TEXT_FlagsGadgets[GD_PG_TEXT_FLAGS_DEFAULT]));

	return TRUE;
}

ULONG TEXT_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_GADTOOLS,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)TextTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)TextOwnTagsTypes,
		GAOBJ_ID,TEXT_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_TEXT_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);
		gtd=(struct GadToolsData *)GadgetData(ng);

		CopyTagItems(TextTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(TextOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		gtd->g_Kind=TEXT_KIND;
		return (ULONG)obj;
	}
	return FALSE;
}

int FN_PG_TEXT_FLAGS_DEFAULT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TEXT_FLAGS_BORDER_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GTTX_Border,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TEXT_FLAGS_COPYTEXT_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GTTX_CopyText,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TEXT_FLAGS_JUSTIFICATION_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	switch(PropertiesMsg.Code)
	{
		case 0: FindTagItem(GTTX_Justification,ti)->ti_Data=GTJ_LEFT; break;
		case 1: FindTagItem(GTTX_Justification,ti)->ti_Data=GTJ_RIGHT; break;
		case 2: FindTagItem(GTTX_Justification,ti)->ti_Data=GTJ_CENTER; break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TEXT_FLAGS_CLIPPED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GTTX_Clipped,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TEXT_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TEXT_FLAGS_HIGHLABEL_Clicked( void )
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
