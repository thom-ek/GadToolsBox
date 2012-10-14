/*
** Palette PlugIn
*/

#include"includes.h"
#include"palette.h"

/*
** OwnTag definitions
*/

#define OWNTAG_Color (TAG_USER+1)
#define OWNTAG_ShowSelected (TAG_USER+2)

/*
** Default Tags
*/

struct TagItem PaletteTags[]=
{
	TAG_IGNORE,NULL, //GT_Underscore,'_',
	TAG_IGNORE,NULL, //GTPA_NumColors,0,
	GTPA_Depth,1,
	TAG_IGNORE,0, //GTPA_IndicatorWidth,0,
	TAG_IGNORE,0, //GTPA_IndicatorHeight,0,
	GA_Disabled,FALSE,
	GTPA_Color,0,
	GTPA_ColorOffset,0,
	TAG_DONE,0
};

UBYTE PaletteTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem PaletteOwnTags[]=
{
	OWNTAG_Color,0,
	OWNTAG_ShowSelected,0,
	TAG_DONE,0
};

UBYTE PaletteOwnTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 8
#define GSIZE_NUMOWNTAGS 2

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGGT_General[];

struct Page PAGES_PROPERTIES_PG_PALT_PAGES[]=
{
	&PAGES_PROPERTIES_PG_PALT_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG PALETTE_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=PALETTE_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=PALETTE_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_PALT_Flags();
			PAGES_PROPERTIES_PG_PALT_PAGES[0].FirstGadget=PGGT_GeneralGList;
			PAGES_PROPERTIES_PG_PALT_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L13_GAD);
			PAGES_PROPERTIES_PG_PALT_PAGES[1].FirstGadget=PG_PALT_FlagsGList;
			PAGES_PROPERTIES_PG_PALT_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L16_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_PALT_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_PALT_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_PALT_Flags();
			break;
		case OM_NEW:
			retval=PALETTE_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_READ:
			retval=PALETTE_READ_Gadget(cl,o,(struct piIO *)msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG PALETTE_READ_Gadget(piClass *cl,piObject *o,struct piIO *msg)
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
				case GTPA_NumColors:
					ti[1].ti_Tag=GTPA_NumColors;
					ti[1].ti_Data=tag.ti_Data;
					ti[2].ti_Tag=TAG_IGNORE;
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

ULONG PALETTE_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
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
	SetCheckMark(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_HIGHLABEL],gtd->g_Flags&NG_HIGHLABEL);
	SetCheckMark(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_UNDERSCORE],GetTagData(GT_Underscore,0,ng->ng_Tags));
	SetCycle(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_TYPE],GetTagData(OWNTAG_Color,0,ng->ng_OwnTags));
	switch(GetTagData(OWNTAG_Color,0,ng->ng_OwnTags))
	{
		case 0:
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR],PropsWnd,FALSE);
			SetInteger(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR],GetTagData(GTPA_Depth,0,ng->ng_Tags));
			break;
		case 1:
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR],PropsWnd,FALSE);
			SetInteger(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR],GetTagData(GTPA_NumColors,0,ng->ng_Tags));
			break;
		case 2:
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR],PropsWnd,TRUE);
			SetInteger(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR],Scr->BitMap.Depth);
			break;
	}
	SetCycle(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED],GetTagData(OWNTAG_ShowSelected,0,ng->ng_OwnTags));
	switch(GetTagData(OWNTAG_ShowSelected,0,ng->ng_OwnTags))
	{
		case 0:
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED_VAR],PropsWnd,TRUE);
			SetInteger(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED_VAR],GetTagData(GTPA_IndicatorWidth,0,ng->ng_Tags));
			break;
		case 1:
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED_VAR],PropsWnd,FALSE);
			SetInteger(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED_VAR],0);
			break;
		case 2:
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED_VAR],PropsWnd,FALSE);
			SetInteger(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED_VAR],GetTagData(GTPA_IndicatorHeight,0,ng->ng_Tags));
			break;
	}
	SetInteger(PropsWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_OFFSET],GetTagData(GTPA_ColorOffset,0,ng->ng_Tags));
	return TRUE;
}

ULONG PALETTE_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	switch(GetTagData(OWNTAG_Color,0,ng->ng_OwnTags))
	{
		case 0: FindTagItem(GTPA_Depth,ng->ng_Tags)->ti_Data=GetInteger(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR]); break;
		case 1: FindTagItem(GTPA_NumColors,ng->ng_Tags)->ti_Data=GetInteger(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR]); break;
		case 2: FindTagItem(GTPA_Depth,ng->ng_Tags)->ti_Data=Scr->BitMap.Depth;
	}
	switch(GetTagData(OWNTAG_ShowSelected,0,ng->ng_OwnTags))
	{
		case 1: FindTagItem(GTPA_IndicatorWidth,ng->ng_Tags)->ti_Data=GetInteger(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED_VAR]); break;
		case 2: FindTagItem(GTPA_IndicatorHeight,ng->ng_Tags)->ti_Data=GetInteger(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED_VAR]); break;
	}
	FindTagItem(GTPA_ColorOffset,ng->ng_Tags)->ti_Data=GetInteger(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_OFFSET]);
	return TRUE;
}

ULONG PALETTE_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_GADTOOLS,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)PaletteTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)PaletteOwnTagsTypes,
		GAOBJ_ID,PALETTE_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_PALT_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);
		gtd=(struct GadToolsData *)GadgetData(ng);

		CopyTagItems(PaletteTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(PaletteOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		gtd->g_Kind=PALETTE_KIND;
		return (ULONG)obj;
	}
	return FALSE;
}

int FN_PG_PALT_FLAGS_OFFSET_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PALT_FLAGS_COLOR_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PALT_FLAGS_SHOWSELECTED_VAR_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PALT_FLAGS_UNDERSCORE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	ti[0].ti_Tag=PropertiesMsg.Code?GT_Underscore:TAG_IGNORE;
	ti[0].ti_Data='_';
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PALT_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PALT_FLAGS_HIGHLABEL_Clicked( void )
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

int FN_PG_PALT_FLAGS_TYPE_Clicked( void )
{
	UBYTE old;
	struct TagItem *ti,*oti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	piGetAttr(CurrentGadget,GAOBJ_OwnTags,(ULONG *)&oti);

	old=FindTagItem(OWNTAG_Color,oti)->ti_Data;
	FindTagItem(OWNTAG_Color,oti)->ti_Data=PropertiesMsg.Code;
	switch(PropertiesMsg.Code)
	{
		case 0:
			ti[1].ti_Tag=TAG_IGNORE;
			ti[2].ti_Tag=GTPA_Depth;
			if(old==1) 
			{
				switch(ti[1].ti_Data)
				{
					case 256: ti[2].ti_Data=8; break;
					case 128: ti[2].ti_Data=7; break;
					case 64: ti[2].ti_Data=6; break;
					case 32: ti[2].ti_Data=5; break;
					case 16: ti[2].ti_Data=4; break;
					case 8: ti[2].ti_Data=3; break;
					case 4: ti[2].ti_Data=2; break;
					case 2: ti[2].ti_Data=1; break;
				}
			}
			SetInteger(PropertiesWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR],GetTagData(GTPA_Depth,0,ti));
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR],PropertiesWnd,FALSE);
			break;	
		case 1:
			ti[1].ti_Tag=GTPA_NumColors;
			ti[2].ti_Tag=TAG_IGNORE;
			if(old!=PropertiesMsg.Code) ti[1].ti_Data=2<<(ti[2].ti_Data-1);
			SetInteger(PropertiesWnd,PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR],GetTagData(GTPA_NumColors,0,ti));
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR],PropertiesWnd,FALSE);
			break;	
		case 2:
			ti[1].ti_Tag=TAG_IGNORE;
			ti[2].ti_Tag=GTPA_Depth;
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_COLOR],PropertiesWnd,TRUE);
			break;	
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PALT_FLAGS_SHOWSELECTED_Clicked( void )
{
	struct TagItem *ti,*oti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	piGetAttr(CurrentGadget,GAOBJ_OwnTags,(ULONG *)&oti);
	FindTagItem(OWNTAG_ShowSelected,oti)->ti_Data=PropertiesMsg.Code;
	switch(PropertiesMsg.Code)
	{
		case 0:
			ti[3].ti_Tag=TAG_IGNORE;
			ti[4].ti_Tag=TAG_IGNORE;
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED_VAR],PropertiesWnd,TRUE);
			break;	
		case 1:
			ti[3].ti_Tag=GTPA_IndicatorWidth;
			ti[4].ti_Tag=TAG_IGNORE;
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED_VAR],PropertiesWnd,FALSE);
			break;	
		case 2:
			ti[3].ti_Tag=TAG_IGNORE;
			ti[4].ti_Tag=GTPA_IndicatorHeight;
			DisableGadget(PG_PALT_FlagsGadgets[GD_PG_PALT_FLAGS_SHOWSELECTED_VAR],PropertiesWnd,FALSE);
			break;	
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
