/*
** Scroller PlugIn
*/

#include"includes.h"
#include"scroller.h"

/*
** OwnTag definitions
*/

#define OWNTAG_Arrows (TAG_USER+1)

/*
** Default Tags
*/

struct TagItem ScrollerTags[]=
{
	TAG_IGNORE,NULL, //GT_Underscore,'_',
	TAG_IGNORE,NULL, //GTSC_Arrows,16,
	GTSC_Top,0,
	GTSC_Total,0,
	GTSC_Visible,0,
	GA_Disabled,FALSE,
	GA_Immediate,FALSE,
	GA_RelVerify,FALSE,
//	PGA_NewLook,FALSE,
	PGA_Freedom,LORIENT_HORIZ,
	TAG_DONE,0
};

UBYTE ScrollerTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem ScrollerOwnTags[]=
{
	OWNTAG_Arrows,0,
	TAG_DONE,0
};

UBYTE ScrollerOwnTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 9
#define GSIZE_NUMOWNTAGS 1

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGGT_General[];

struct Page PAGES_PROPERTIES_PG_SCRL_PAGES[]=
{
	&PAGES_PROPERTIES_PG_SCRL_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PG_SCRL_PAGES[2], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG SCROLLER_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=SCROLLER_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=SCROLLER_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_SCRL_Flags();
			OpenWindow_PG_SCRL_Var();
			PAGES_PROPERTIES_PG_SCRL_PAGES[0].FirstGadget=PGGT_GeneralGList;
			PAGES_PROPERTIES_PG_SCRL_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L13_GAD);
			PAGES_PROPERTIES_PG_SCRL_PAGES[1].FirstGadget=PG_SCRL_FlagsGList;
			PAGES_PROPERTIES_PG_SCRL_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L19_GAD);
			PAGES_PROPERTIES_PG_SCRL_PAGES[2].FirstGadget=PG_SCRL_VarGList;
			PAGES_PROPERTIES_PG_SCRL_PAGES[2].Name=GetCatString(MSG_PROPERTIES_PAGES_L20_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_SCRL_PAGES[2].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_SCRL_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_SCRL_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_SCRL_Var();
			CloseWindow_PG_SCRL_Flags();
			break;
		case OM_NEW:
			retval=SCROLLER_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_READ:
			retval=SCROLLER_READ_Gadget(cl,o,(struct piIO *)msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG SCROLLER_READ_Gadget(piClass *cl,piObject *o,struct piIO *msg)
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
				case GT_Underscore:
					ti[0].ti_Tag=GT_Underscore;
					ti[0].ti_Data='_';
					break;
				case GTSC_Arrows:
					ti[1].ti_Tag=GTSC_Arrows;
					ti[1].ti_Data=tag.ti_Data;
					if(sti=FindTagItem(OWNTAG_Arrows,oti)) sti->ti_Data=1;
					break;
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

ULONG SCROLLER_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
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
	SetCheckMark(PropsWnd,PG_SCRL_FlagsGadgets[GD_PG_SCRL_FLAGS_HIGHLABEL],gtd->g_Flags&NG_HIGHLABEL);
	SetCheckMark(PropsWnd,PG_SCRL_FlagsGadgets[GD_PG_SCRL_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_SCRL_FlagsGadgets[GD_PG_SCRL_FLAGS_UNDERSCORE],GetTagData(GT_Underscore,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_SCRL_FlagsGadgets[GD_PG_SCRL_FLAGS_IMMEDIATE],GetTagData(GA_Immediate,0,ng->ng_Tags));
//	SetCheckMark(PropsWnd,PG_SCRL_FlagsGadgets[GD_PG_SCRL_FLAGS_NEWLOOK],GetTagData(PGA_NewLook,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_SCRL_FlagsGadgets[GD_PG_SCRL_FLAGS_RELVERIFY],GetTagData(GA_RelVerify,0,ng->ng_Tags));

	if(active==2) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetInteger(PropsWnd,PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_TOP],GetTagData(GTSC_Top,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_TOTAL],GetTagData(GTSC_Total,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_VISIBLE],GetTagData(GTSC_Visible,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_ARROWS],GetTagData(GTSC_Arrows,0,ng->ng_Tags));
	SetCycle(PropsWnd,PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_ARROW_TYPE],GetTagData(OWNTAG_Arrows,0,ng->ng_OwnTags));
	if(GetTagData(OWNTAG_Arrows,0,ng->ng_OwnTags)==1) DisableGadget(PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_ARROWS],PropsWnd,FALSE);
	else DisableGadget(PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_ARROWS],PropsWnd,TRUE);
	SetCycle(PropsWnd,PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_FREEDOM],(GetTagData(PGA_Freedom,0,ng->ng_Tags)==LORIENT_HORIZ)?0:1);
	return TRUE;
}

ULONG SCROLLER_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	FindTagItem(GTSC_Top,ng->ng_Tags)->ti_Data=GetInteger(PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_TOP]);
	FindTagItem(GTSC_Total,ng->ng_Tags)->ti_Data=GetInteger(PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_TOTAL]);
	FindTagItem(GTSC_Visible,ng->ng_Tags)->ti_Data=GetInteger(PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_VISIBLE]);
	if(GetTagData(OWNTAG_Arrows,0,ng->ng_OwnTags)==1)
		FindTagItem(GTSC_Arrows,ng->ng_Tags)->ti_Data=GetInteger(PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_ARROWS]);

	return TRUE;
}

ULONG SCROLLER_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_GADTOOLS,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)ScrollerTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)ScrollerOwnTagsTypes,
		GAOBJ_ID,SCROLLER_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_SCRL_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);
		gtd=(struct GadToolsData *)GadgetData(ng);

		CopyTagItems(ScrollerTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(ScrollerOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		gtd->g_Kind=SCROLLER_KIND;
		return (ULONG)obj;
	}
	return FALSE;
}

int FN_PG_SCRL_VAR_TOP_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SCRL_VAR_TOTAL_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SCRL_VAR_VISIBLE_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SCRL_VAR_ARROWS_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SCRL_VAR_ARROW_TYPE_Clicked( void )
{
	struct TagItem *oti,*ti;
	piGetAttr(CurrentGadget,GAOBJ_OwnTags,(ULONG *)&oti);
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(OWNTAG_Arrows,oti)->ti_Data=PropertiesMsg.Code;
	ti[1].ti_Tag=PropertiesMsg.Code?GTSC_Arrows:TAG_IGNORE;
	if(PropertiesMsg.Code!=1)
		DisableGadget(PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_ARROWS],PropertiesWnd,TRUE);
	else
		DisableGadget(PG_SCRL_VarGadgets[GD_PG_SCRL_VAR_ARROWS],PropertiesWnd,FALSE);
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SCRL_FLAGS_UNDERSCORE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	ti[0].ti_Tag=PropertiesMsg.Code?GT_Underscore:TAG_IGNORE;
	ti[0].ti_Data='_';
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SCRL_VAR_FREEDOM_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	switch(PropertiesMsg.Code)
	{
		case 0: FindTagItem(PGA_Freedom,ti)->ti_Data=LORIENT_HORIZ; break;
		case 1: FindTagItem(PGA_Freedom,ti)->ti_Data=LORIENT_VERT; break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SCRL_FLAGS_IMMEDIATE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Immediate,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SCRL_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

/*
int FN_PG_SCRL_FLAGS_NEWLOOK_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(PGA_NewLook,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
*/

int FN_PG_SCRL_FLAGS_RELVERIFY_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_RelVerify,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SCRL_FLAGS_HIGHLABEL_Clicked( void )
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
