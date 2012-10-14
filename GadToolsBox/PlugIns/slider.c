/*
** Slider PlugIn
*/

#include"includes.h"
#include"slider.h"

/*
** OwnTag definitions
*/

/*
** Default Tags
*/

struct TagItem SliderTags[]=
{
	TAG_IGNORE,NULL, //GT_Underscore,'_',
	TAG_IGNORE,0, //GTSL_MaxPixelLen,0,
	GTSL_Min,0,
	GTSL_Max,0,
	GTSL_Level,0,
	GTSL_MaxLevelLen,0,
	GTSL_LevelFormat,NULL,
	GTSL_LevelPlace,PLACETEXT_LEFT,
	GTSL_Justification,GTJ_LEFT,
	GA_Disabled,FALSE,
	GA_Immediate,FALSE,
	GA_RelVerify,FALSE,
//	PGA_NewLook,FALSE,
	PGA_Freedom,LORIENT_HORIZ,
	TAG_DONE,0
};

UBYTE SliderTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_TEXT,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem SliderOwnTags[]=
{
	TAG_DONE,0
};

UBYTE SliderOwnTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 13
#define GSIZE_NUMOWNTAGS 0

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGGT_General[];

struct Page PAGES_PROPERTIES_PG_SLIR_PAGES[]=
{
	&PAGES_PROPERTIES_PG_SLIR_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PG_SLIR_PAGES[2], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG SLIDER_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=SLIDER_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=SLIDER_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_SLIR_Flags();
			OpenWindow_PG_SLIR_Var();
			PAGES_PROPERTIES_PG_SLIR_PAGES[0].FirstGadget=PGGT_GeneralGList;
			PAGES_PROPERTIES_PG_SLIR_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L13_GAD);
			PAGES_PROPERTIES_PG_SLIR_PAGES[1].FirstGadget=PG_SLIR_FlagsGList;
			PAGES_PROPERTIES_PG_SLIR_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L21_GAD);
			PAGES_PROPERTIES_PG_SLIR_PAGES[2].FirstGadget=PG_SLIR_VarGList;
			PAGES_PROPERTIES_PG_SLIR_PAGES[2].Name=GetCatString(MSG_PROPERTIES_PAGES_L22_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_SLIR_PAGES[2].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_SLIR_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_SLIR_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_SLIR_Var();
			CloseWindow_PG_SLIR_Flags();
			break;
		case OM_NEW:
			retval=SLIDER_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_READ:
			retval=SLIDER_READ_Gadget(cl,o,(struct piIO *)msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG SLIDER_READ_Gadget(piClass *cl,piObject *o,struct piIO *msg)
{
	if(CurrentChunk(msg->IFFh)->cn_Type==ID_GXGA && CurrentChunk(msg->IFFh)->cn_ID==ID_GADA)
	{
		struct TagItem *ti,*oti,tag,*sti;
		struct Node_Gadget *ng;
		int a;
		UWORD lenstr;

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
				case GTSL_MaxPixelLen:
					ti[1].ti_Tag=GTSL_MaxPixelLen;
					ti[1].ti_Data=tag.ti_Data;
					break;
				case GTSL_LevelFormat: break;
				default:
					if(sti=FindTagItem(tag.ti_Tag,ti)) sti->ti_Data=tag.ti_Data;
					break;
			}
		}
		for(;a<11;a++) ReadChunkBytes(msg->IFFh,(APTR)&tag,sizeof(struct TagItem));
		ReadChunkBytes(msg->IFFh,(APTR)&lenstr,sizeof(UWORD));
		if(lenstr)
		{
			if(FindTagItem(GTSL_LevelFormat,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(GTSL_LevelFormat,ng->ng_Tags)->ti_Data);
			FindTagItem(GTSL_LevelFormat,ng->ng_Tags)->ti_Data=NULL;
			if(FindTagItem(GTSL_LevelFormat,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(lenstr,MEMF_PUBLIC))
				ReadChunkBytes(msg->IFFh,(APTR)FindTagItem(GTSL_LevelFormat,ng->ng_Tags)->ti_Data,lenstr);
		}
		return 0;
	}
	else
		return piDoSuperMethodA(cl,o,(Msg)msg);
}

ULONG SLIDER_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
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
	SetCheckMark(PropsWnd,PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_HIGHLABEL],gtd->g_Flags&NG_HIGHLABEL);
	SetCheckMark(PropsWnd,PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_UNDERSCORE],GetTagData(GT_Underscore,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_IMMEDIATE],GetTagData(GA_Immediate,0,ng->ng_Tags));
//	SetCheckMark(PropsWnd,PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_NEWLOOK],GetTagData(PGA_NewLook,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_RELVERIFY],GetTagData(GA_RelVerify,0,ng->ng_Tags));

	if(active==2) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetInteger(PropsWnd,PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_MIN],GetTagData(GTSL_Min,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_MAX],GetTagData(GTSL_Max,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_LEVEL],GetTagData(GTSL_Level,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_MAXLEVLEN],GetTagData(GTSL_MaxLevelLen,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_SLIR_VarGadgets[GD_PG_SLIR_FLAGS_MAXPIXELLEN],GetTagData(GTSL_MaxPixelLen,0,ng->ng_Tags));
	if(FindTagItem(GTSL_MaxPixelLen,ng->ng_Tags)) DisableGadget(PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_MAXPIXELLEN],PropsWnd,FALSE);
	else DisableGadget(PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_MAXPIXELLEN],PropsWnd,TRUE);
	SetString(PropsWnd,PG_SLIR_VarGadgets[GD_PG_SLIR_FLAGS_FORMAT],GetTagData(GTSL_LevelFormat,0,ng->ng_Tags));
	SetCycle(PropsWnd,PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_FREEDOM],(GetTagData(PGA_Freedom,0,ng->ng_Tags)==LORIENT_HORIZ)?0:1);
	switch(GetTagData(GTSL_Justification,0,ng->ng_Tags))
	{
		case GTJ_LEFT: SetCycle(PropsWnd,PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_JUSTIFICATION],0); break;
		case GTJ_RIGHT: SetCycle(PropsWnd,PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_JUSTIFICATION],1); break;
		case GTJ_CENTER: SetCycle(PropsWnd,PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_JUSTIFICATION],2); break;
	}
	return TRUE;
}

ULONG SLIDER_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	FindTagItem(GTSL_Min,ng->ng_Tags)->ti_Data=GetInteger(PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_MIN]);
	FindTagItem(GTSL_Max,ng->ng_Tags)->ti_Data=GetInteger(PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_MAX]);
	FindTagItem(GTSL_Level,ng->ng_Tags)->ti_Data=GetInteger(PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_LEVEL]);
	FindTagItem(GTSL_MaxLevelLen,ng->ng_Tags)->ti_Data=GetInteger(PG_SLIR_VarGadgets[GD_PG_SLIR_VAR_MAXLEVLEN]);
	if(FindTagItem(GTSL_MaxPixelLen,ng->ng_Tags))
		FindTagItem(GTSL_MaxPixelLen,ng->ng_Tags)->ti_Data=GetInteger(PG_SLIR_VarGadgets[GD_PG_SLIR_FLAGS_MAXPIXELLEN]);

	if(FindTagItem(GTSL_LevelFormat,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(GTSL_LevelFormat,ng->ng_Tags)->ti_Data);
	FindTagItem(GTSL_LevelFormat,ng->ng_Tags)->ti_Data=NULL;
	if(FindTagItem(GTSL_LevelFormat,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(strlen(GetString(PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_FORMAT]))+1,MEMF_PUBLIC))
		strcpy((char *)((struct TagItem *)FindTagItem(GTSL_LevelFormat,ng->ng_Tags)->ti_Data),GetString(PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_FORMAT]));

	return TRUE;
}

ULONG SLIDER_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_GADTOOLS,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)SliderTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)SliderOwnTagsTypes,
		GAOBJ_ID,SLIDER_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_SLIR_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);
		gtd=(struct GadToolsData *)GadgetData(ng);

		CopyTagItems(SliderTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(SliderOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		gtd->g_Kind=SLIDER_KIND;
		return (ULONG)obj;
	}
	return FALSE;
}

int FN_PG_SLIR_FLAGS_FORMAT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_VAR_MAX_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_VAR_MIN_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_VAR_LEVEL_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_VAR_MAXLEVLEN_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_FLAGS_MAXPIXELLEN_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_FLAGS_MAXPIXELLEN_SET_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	ti[1].ti_Tag=PropertiesMsg.Code?GTSL_MaxPixelLen:TAG_IGNORE;
	if(PropertiesMsg.Code) DisableGadget(PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_MAXPIXELLEN],PropertiesWnd,FALSE);
	else DisableGadget(PG_SLIR_FlagsGadgets[GD_PG_SLIR_FLAGS_MAXPIXELLEN],PropertiesWnd,TRUE);
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_FLAGS_UNDERSCORE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	ti[0].ti_Tag=PropertiesMsg.Code?GT_Underscore:TAG_IGNORE;
	ti[0].ti_Data='_';
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_VAR_LEVELPLACE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	switch(PropertiesMsg.Code)
	{
		case 0: FindTagItem(GTSL_LevelPlace,ti)->ti_Data=PLACETEXT_LEFT; break;
		case 1: FindTagItem(GTSL_LevelPlace,ti)->ti_Data=PLACETEXT_RIGHT; break;
		case 2: FindTagItem(GTSL_LevelPlace,ti)->ti_Data=PLACETEXT_ABOVE; break;
		case 3: FindTagItem(GTSL_LevelPlace,ti)->ti_Data=PLACETEXT_BELOW; break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_VAR_JUSTIFICATION_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	switch(PropertiesMsg.Code)
	{
		case 0: FindTagItem(GTSL_Justification,ti)->ti_Data=GTJ_LEFT; break;
		case 1: FindTagItem(GTSL_Justification,ti)->ti_Data=GTJ_RIGHT; break;
		case 2: FindTagItem(GTSL_Justification,ti)->ti_Data=GTJ_CENTER; break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_VAR_FREEDOM_Clicked( void )
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

int FN_PG_SLIR_FLAGS_IMMEDIATE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Immediate,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

/*
int FN_PG_SLIR_FLAGS_NEWLOOK_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(PGA_NewLook,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
*/
int FN_PG_SLIR_FLAGS_RELVERIFY_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_RelVerify,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_SLIR_FLAGS_HIGHLABEL_Clicked( void )
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
