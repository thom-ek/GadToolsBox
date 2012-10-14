/*
** String PlugIn
*/

#include"includes.h"
#include"plugins/string.h"

/*
** OwnTag definitions
*/

/*
** Default Tags
*/

struct TagItem StringTags[]=
{
	TAG_IGNORE,NULL, //GT_Underscore,'_',
	GA_Disabled,FALSE,
	GA_TabCycle,FALSE,
	STRINGA_ReplaceMode,FALSE,
	STRINGA_ExitHelp,FALSE,
	STRINGA_Justification,GACT_STRINGLEFT,
	GTST_String,0,
	GTST_MaxChars,0,
	TAG_DONE,0
};

UBYTE StringTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_TEXT,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem StringOwnTags[]=
{
	TAG_DONE,0
};

UBYTE StringOwnTagsTypes[]=
{
	TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 8
#define GSIZE_NUMOWNTAGS 0

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGGT_General[];

struct Page PAGES_PROPERTIES_PG_STRG_PAGES[]=
{
	&PAGES_PROPERTIES_PG_STRG_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PG_STRG_PAGES[2], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG STRING_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=STRING_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=STRING_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_STRG_Flags();
			OpenWindow_PG_STRG_Var();
			PAGES_PROPERTIES_PG_STRG_PAGES[0].FirstGadget=PGGT_GeneralGList;
			PAGES_PROPERTIES_PG_STRG_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L13_GAD);
			PAGES_PROPERTIES_PG_STRG_PAGES[1].FirstGadget=PG_STRG_FlagsGList;
			PAGES_PROPERTIES_PG_STRG_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L23_GAD);
			PAGES_PROPERTIES_PG_STRG_PAGES[2].FirstGadget=PG_STRG_VarGList;
			PAGES_PROPERTIES_PG_STRG_PAGES[2].Name=GetCatString(MSG_PROPERTIES_PAGES_L24_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_STRG_PAGES[2].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_STRG_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_STRG_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_STRG_Var();
			CloseWindow_PG_STRG_Flags();
			break;
		case OM_NEW:
			retval=STRING_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_READ:
			retval=STRING_READ_Gadget(cl,o,(struct piIO *)msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG STRING_READ_Gadget(piClass *cl,piObject *o,struct piIO *msg)
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
				case GT_Underscore:
					ti[0].ti_Tag=GT_Underscore;
					ti[0].ti_Data='_';
					break;
				case GTST_String: break;
				default:
					if(sti=FindTagItem(tag.ti_Tag,ti)) sti->ti_Data=tag.ti_Data;
					break;
			}
		}
		for(;a<7;a++) ReadChunkBytes(msg->IFFh,(APTR)&tag,sizeof(struct TagItem));
		ReadChunkBytes(msg->IFFh,(APTR)&lenstr,sizeof(UWORD));
		if(lenstr)
		{
			if(FindTagItem(GTST_String,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(GTST_String,ng->ng_Tags)->ti_Data);
			FindTagItem(GTST_String,ng->ng_Tags)->ti_Data=NULL;
			if(FindTagItem(GTST_String,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(lenstr,MEMF_PUBLIC))
				ReadChunkBytes(msg->IFFh,(APTR)FindTagItem(GTST_String,ng->ng_Tags)->ti_Data,lenstr);
		}
		return 0;
	}
	else
		return piDoSuperMethodA(cl,o,(Msg)msg);
}

ULONG STRING_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
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
	SetCheckMark(PropsWnd,PG_STRG_FlagsGadgets[GD_PG_STRG_FLAGS_HIGHLABEL],gtd->g_Flags&NG_HIGHLABEL);
	SetCheckMark(PropsWnd,PG_STRG_FlagsGadgets[GD_PG_STRG_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_STRG_FlagsGadgets[GD_PG_STRG_FLAGS_UNDERSCORE],GetTagData(GT_Underscore,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_STRG_FlagsGadgets[GD_PG_STRG_FLAGS_EXITHELP],GetTagData(STRINGA_ExitHelp,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_STRG_FlagsGadgets[GD_PG_STRG_FLAGS_REPLACEMODE],GetTagData(STRINGA_ReplaceMode,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_STRG_FlagsGadgets[GD_PG_STRG_FLAGS_TABCYCLE],GetTagData(GA_TabCycle,0,ng->ng_Tags));

	if(active==2) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetInteger(PropsWnd,PG_STRG_VarGadgets[GD_PG_STRG_VAR_STRING],GetTagData(GTST_String,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_STRG_VarGadgets[GD_PG_STRG_VAR_MAXCHARS],GetTagData(GTST_MaxChars,0,ng->ng_Tags));
	switch(GetTagData(STRINGA_Justification,0,ng->ng_Tags))
	{
		case GACT_STRINGLEFT: SetCycle(PropsWnd,PG_STRG_VarGadgets[GD_PG_STRG_VAR_JUSTIFICATION],0); break;
		case GACT_STRINGRIGHT: SetCycle(PropsWnd,PG_STRG_VarGadgets[GD_PG_STRG_VAR_JUSTIFICATION],1); break;
		case GACT_STRINGCENTER: SetCycle(PropsWnd,PG_STRG_VarGadgets[GD_PG_STRG_VAR_JUSTIFICATION],2); break;
	}
	return TRUE;
}

ULONG STRING_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	if(FindTagItem(GTST_String,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(GTST_String,ng->ng_Tags)->ti_Data);
	FindTagItem(GTST_String,ng->ng_Tags)->ti_Data=NULL;
	if(FindTagItem(GTST_String,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(strlen(GetString(PG_STRG_VarGadgets[GD_PG_STRG_VAR_STRING]))+1,MEMF_PUBLIC))
		strcpy((char *)FindTagItem(GTST_String,ng->ng_Tags)->ti_Data,GetString(PG_STRG_VarGadgets[GD_PG_STRG_VAR_STRING]));
	FindTagItem(GTST_MaxChars,ng->ng_Tags)->ti_Data=GetInteger(PG_STRG_VarGadgets[GD_PG_STRG_VAR_MAXCHARS]);
	return TRUE;
}

ULONG STRING_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_GADTOOLS,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)StringTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)StringOwnTagsTypes,
		GAOBJ_ID,STRING_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_STRG_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);
		gtd=(struct GadToolsData *)GadgetData(ng);

		CopyTagItems(StringTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(StringOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		gtd->g_Kind=STRING_KIND;
		return (ULONG)obj;
	}
	return FALSE;
}

int FN_PG_STRG_VAR_STRING_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_STRG_FLAGS_UNDERSCORE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	ti[0].ti_Tag=PropertiesMsg.Code?GT_Underscore:TAG_IGNORE;
	ti[0].ti_Data='_';
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_STRG_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_STRG_FLAGS_HIGHLABEL_Clicked( void )
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

int FN_PG_STRG_FLAGS_EXITHELP_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(STRINGA_ExitHelp,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_STRG_VAR_JUSTIFICATION_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	switch(PropertiesMsg.Code)
	{
		case 0: FindTagItem(STRINGA_Justification,ti)->ti_Data=GACT_STRINGLEFT; break;
		case 1: FindTagItem(STRINGA_Justification,ti)->ti_Data=GACT_STRINGRIGHT; break;
		case 2: FindTagItem(STRINGA_Justification,ti)->ti_Data=GACT_STRINGCENTER; break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_STRG_FLAGS_REPLACEMODE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(STRINGA_ReplaceMode,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_STRG_FLAGS_TABCYCLE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_TabCycle,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_STRG_FLAGS_NUMBER_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_STRG_VAR_MAXCHARS_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
