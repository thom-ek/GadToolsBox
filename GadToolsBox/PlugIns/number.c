/*
** Number PlugIn
*/

#include"includes.h"
#include"number.h"

/*
** Defines
*/

/*
** OwnTag definitions
*/

#define OWNTAG_ImageTitle 1

/*
** Default Tags
*/

struct TagItem NumberTags[]=
{
	GA_Disabled,FALSE,
	GTNM_Border,FALSE,
	GTNM_Justification,GTJ_LEFT,
	GTNM_Clipped,FALSE,
	GTNM_Number,0,
	GTNM_MaxNumberLen,0,
	GTNM_Format,NULL,
	TAG_DONE,0
};

UBYTE NumberTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_TEXT,TAGTYPE_DONE
};

struct TagItem NumberOwnTags[]=
{
	TAG_DONE,0
};

UBYTE NumberOwnTagsTypes[]=
{
	TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 7
#define GSIZE_NUMOWNTAGS 0

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGGT_General[];

struct Page PAGES_PROPERTIES_PG_NUMB_PAGES[]=
{
	&PAGES_PROPERTIES_PG_NUMB_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG NUMBER_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=NUMBER_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=NUMBER_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_NUMB_Flags();
			PAGES_PROPERTIES_PG_NUMB_PAGES[0].FirstGadget=PGGT_GeneralGList;
			PAGES_PROPERTIES_PG_NUMB_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L13_GAD);
			PAGES_PROPERTIES_PG_NUMB_PAGES[1].FirstGadget=PG_NUMB_FlagsGList;
			PAGES_PROPERTIES_PG_NUMB_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L12_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_NUMB_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_NUMB_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_NUMB_Flags();
			break;
		case OM_NEW:
			retval=NUMBER_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_READ:
			retval=NUMBER_READ_Gadget(cl,o,(struct piIO *)msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG NUMBER_READ_Gadget(piClass *cl,piObject *o,struct piIO *msg)
{
	if(CurrentChunk(msg->IFFh)->cn_Type==ID_GXGA && CurrentChunk(msg->IFFh)->cn_ID==ID_GADA)
	{
		struct TagItem *ti,*oti,tag,*sti;
		struct Node_Gadget *ng;
		int a;

		piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
		piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&oti);
		piGetAttr(o,OBJ_Node,(ULONG *)&ng);

		for(a=0;a<ng->ng_Type-1;a++)
		{
			ReadChunkBytes(msg->IFFh,(APTR)&tag,sizeof(struct TagItem));
			switch(tag.ti_Tag)
			{
				case GTNM_Format: break;
				default:
					if(sti=FindTagItem(tag.ti_Tag,ti)) sti->ti_Data=tag.ti_Data;
					break;
			}
		}
		return 0;
	}
	else
		return piDoSuperMethodA(cl,o,(Msg)msg);
}

ULONG NUMBER_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
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
	SetCheckMark(PropsWnd,PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_HIGHLABEL],gtd->g_Flags&NG_HIGHLABEL);
	SetCheckMark(PropsWnd,PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_BORDER],GetTagData(GTNM_Border,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_CLIPPED],GetTagData(GTNM_Clipped,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_DEFAULT],GetTagData(GTNM_Number,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_MAXNUMLEN],GetTagData(GTNM_MaxNumberLen,0,ng->ng_Tags));
	SetString(PropsWnd,PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_FORMAT],GetTagData(GTNM_Format,0,ng->ng_Tags));
	switch(GetTagData(GTNM_Justification,GTJ_LEFT,ng->ng_Tags))
	{
		case GTJ_LEFT: SetCycle(PropsWnd,PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_JUSTIFICATION],GTJ_LEFT); break;
		case GTJ_RIGHT: SetCycle(PropsWnd,PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_JUSTIFICATION],GTJ_RIGHT); break;
		case GTJ_CENTER: SetCycle(PropsWnd,PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_JUSTIFICATION],GTJ_CENTER); break;
	}
	return TRUE;
}

ULONG NUMBER_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	FindTagItem(GTNM_Number,ng->ng_Tags)->ti_Data=GetInteger(PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_DEFAULT]);
	FindTagItem(GTNM_MaxNumberLen,ng->ng_Tags)->ti_Data=GetInteger(PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_MAXNUMLEN]);
	if(FindTagItem(GTNM_Format,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(GTNM_Format,ng->ng_Tags)->ti_Data);
	FindTagItem(GTNM_Format,ng->ng_Tags)->ti_Data=NULL;
	if(FindTagItem(GTNM_Format,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(strlen(GetString(PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_FORMAT]))+1,MEMF_PUBLIC))
		strcpy((char *)((struct TagItem *)FindTagItem(GTNM_Format,ng->ng_Tags)->ti_Data),GetString(PG_NUMB_FlagsGadgets[GD_PG_NUMB_FLAGS_FORMAT]));

	return TRUE;
}

ULONG NUMBER_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_GADTOOLS,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)NumberTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)NumberOwnTagsTypes,
		GAOBJ_ID,NUMBER_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_NUMB_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);
		gtd=(struct GadToolsData *)GadgetData(ng);

		CopyTagItems(NumberTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(NumberOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		gtd->g_Kind=NUMBER_KIND;
		return (ULONG)obj;
	}
	return FALSE;
}

int FN_PG_NUMB_FLAGS_FORMAT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_NUMB_FLAGS_DEFAULT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_NUMB_FLAGS_MAXNUMLEN_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_NUMB_FLAGS_BORDER_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GTNM_Border,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_NUMB_FLAGS_JUSTIFICATION_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	switch(PropertiesMsg.Code)
	{
		case 0: FindTagItem(GTNM_Justification,ti)->ti_Data=GTJ_LEFT; break;
		case 1: FindTagItem(GTNM_Justification,ti)->ti_Data=GTJ_RIGHT; break;
		case 2: FindTagItem(GTNM_Justification,ti)->ti_Data=GTJ_CENTER; break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_NUMB_FLAGS_CLIPPED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GTNM_Clipped,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_NUMB_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_NUMB_FLAGS_HIGHLABEL_Clicked( void )
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
