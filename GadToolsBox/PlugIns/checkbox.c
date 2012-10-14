/*
** CheckBox PlugIn
*/

#include"includes.h"
#include"checkbox.h"

/*
** OwnTag definitions
*/

#define OWNTAG_Scaled (TAG_USER+1)

/*
** Default Tags
*/

struct TagItem CheckBoxTags[]=
{
	TAG_IGNORE,NULL, //GT_Underscore,'_',
	GA_Disabled,FALSE,
	GTCB_Scaled,FALSE,
	GTCB_Checked,FALSE,
	TAG_DONE,0
};

UBYTE CheckBoxTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem CheckBoxOwnTags[]=
{
	OWNTAG_Scaled,0,
	TAG_DONE,0
};

UBYTE CheckBoxOwnTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 4
#define GSIZE_NUMOWNTAGS 1

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGGT_General[];

struct Page PAGES_PROPERTIES_PG_CBOX_PAGES[]=
{
	&PAGES_PROPERTIES_PG_CBOX_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG CHECKBOX_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_DEFAULT:
			retval=CHECKBOX_DEFAULT_Gadget(cl,o,msg);
			break;
		case OM_INIT:
			retval=CHECKBOX_INIT_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_CBOX_Flags();
			PAGES_PROPERTIES_PG_CBOX_PAGES[0].FirstGadget=PGGT_GeneralGList;
			PAGES_PROPERTIES_PG_CBOX_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L13_GAD);
			PAGES_PROPERTIES_PG_CBOX_PAGES[1].FirstGadget=PG_CBOX_FlagsGList;
			PAGES_PROPERTIES_PG_CBOX_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L2_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_CBOX_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_CBOX_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_CBOX_Flags();
			break;
		case OM_NEW:
			retval=CHECKBOX_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_READ:
			retval=CHECKBOX_READ_Gadget(cl,o,(struct piIO *)msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG CHECKBOX_READ_Gadget(piClass *cl,piObject *o,struct piIO *msg)
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

ULONG CHECKBOX_DEFAULT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	
	ng->ng_Width=CHECKBOX_WIDTH;
	ng->ng_Height=CHECKBOX_HEIGHT;
	return TRUE;
}

ULONG CHECKBOX_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(o,GAOBJ_NodeData,(ULONG *)&gtd);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(ng->ng_OwnTags[0].ti_Tag==1L) ng->ng_OwnTags[0].ti_Tag=OWNTAG_Scaled;

	piDoSuperMethodA(cl,o,msg);

	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PG_CBOX_FlagsGadgets[GD_PG_CBOX_FLAGS_HIGHLABEL],gtd->g_Flags&NG_HIGHLABEL);
	SetCheckMark(PropsWnd,PG_CBOX_FlagsGadgets[GD_PG_CBOX_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_CBOX_FlagsGadgets[GD_PG_CBOX_FLAGS_UNDERSCORE],GetTagData(GT_Underscore,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_CBOX_FlagsGadgets[GD_PG_CBOX_FLAGS_CHECKED],GetTagData(GTCB_Checked,0,ng->ng_Tags));
	SetCycle(PropsWnd,PG_CBOX_FlagsGadgets[GD_PG_CBOX_FLAGS_SCALED],GetTagData(OWNTAG_Scaled,0,ng->ng_OwnTags));

	if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	if(GetTagData(OWNTAG_Scaled,0,ng->ng_OwnTags)==1) DisableGadget(PGGT_GeneralGadgets[GD_PGGT_WIDTH],PropsWnd,FALSE);
	else DisableGadget(PGGT_GeneralGadgets[GD_PGGT_WIDTH],PropsWnd,TRUE);
	if(GetTagData(OWNTAG_Scaled,0,ng->ng_OwnTags)==1) DisableGadget(PGGT_GeneralGadgets[GD_PGGT_HEIGHT],PropsWnd,FALSE);
	else DisableGadget(PGGT_GeneralGadgets[GD_PGGT_HEIGHT],PropsWnd,TRUE);
	return TRUE;
}

ULONG CHECKBOX_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_GADTOOLS,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)CheckBoxTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)CheckBoxOwnTagsTypes,
		GAOBJ_ID,CHECKBOX_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_CBOX_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);
		gtd=(struct GadToolsData *)GadgetData(ng);

		CopyTagItems(CheckBoxTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(CheckBoxOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		gtd->g_Kind=CHECKBOX_KIND;
		return (ULONG)obj;
	}
	return FALSE;
}

int FN_PG_CBOX_FLAGS_UNDERSCORE_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	ti[0].ti_Tag=PropertiesMsg.Code?GT_Underscore:TAG_IGNORE;
	ti[0].ti_Data='_';
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_CBOX_FLAGS_CHECKED_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GTCB_Checked,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_CBOX_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_CBOX_FLAGS_HIGHLABEL_Clicked( void )
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

int FN_PG_CBOX_FLAGS_SCALED_Clicked( void )
{
	struct Window *wnd=NULL;
	ULONG active;
	struct TagItem *ti,*oti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	piGetAttr(CurrentGadget,GAOBJ_OwnTags,(ULONG *)&oti);
	FindTagItem(OWNTAG_Scaled,oti)->ti_Data=PropertiesMsg.Code;
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);
	if(active==0) wnd=PropertiesWnd;
	switch(PropertiesMsg.Code)
	{
		case 0:
			FindTagItem(GTCB_Scaled,ti)->ti_Data=FALSE;
			DisableGadget(PGGT_GeneralGadgets[GD_PGGT_WIDTH],wnd,TRUE);
			DisableGadget(PGGT_GeneralGadgets[GD_PGGT_HEIGHT],wnd,TRUE);
			break;
		case 1:
			FindTagItem(GTCB_Scaled,ti)->ti_Data=TRUE;
			DisableGadget(PGGT_GeneralGadgets[GD_PGGT_WIDTH],wnd,FALSE);
			DisableGadget(PGGT_GeneralGadgets[GD_PGGT_HEIGHT],wnd,FALSE);
			break;
		case 2:
			FindTagItem(GTCB_Scaled,ti)->ti_Data=TRUE;
			DisableGadget(PGGT_GeneralGadgets[GD_PGGT_WIDTH],wnd,TRUE);
			DisableGadget(PGGT_GeneralGadgets[GD_PGGT_HEIGHT],wnd,TRUE);
			break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
