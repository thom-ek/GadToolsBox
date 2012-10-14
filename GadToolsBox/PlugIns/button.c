/*
** Button PlugIn
*/

#include"includes.h"
#include"button.h"

/*
** OwnTag definitions
*/

#define OWNTAG_TitleType (TAG_USER+1)
// 0 - Normal, 1 - ToolBar, 2 - Image
#define OWNTAG_Toggle (TAG_USER+2)

/*
** Default Tags
*/

struct TagItem ButtonTags[]=
{
	TAG_IGNORE,NULL, //GT_Underscore,'_',
	GA_Disabled,FALSE,
	GA_Immediate,FALSE,
	TAG_DONE,0
};

UBYTE ButtonTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem ButtonOwnTags[]=
{
	OWNTAG_TitleType,FALSE,
	OWNTAG_Toggle,FALSE,
	TAG_DONE,0
};

UBYTE ButtonOwnTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 3
#define GSIZE_NUMOWNTAGS 2

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGGT_General[];

struct Page PAGES_PROPERTIES_PG_BUTT_PAGES[]=
{
	&PAGES_PROPERTIES_PG_BUTT_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG BUTTON_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval=0;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=BUTTON_INIT_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_BUTT_Flags();
			PAGES_PROPERTIES_PG_BUTT_PAGES[0].FirstGadget=PGGT_GeneralGList;
			PAGES_PROPERTIES_PG_BUTT_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L13_GAD);
			PAGES_PROPERTIES_PG_BUTT_PAGES[1].FirstGadget=PG_BUTT_FlagsGList;
			PAGES_PROPERTIES_PG_BUTT_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L1_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_BUTT_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_BUTT_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_BUTT_Flags();
			break;
		case OM_NEW:
			retval=BUTTON_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_CREATE:
			retval=BUTTON_CREATE_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_DELETE:
			retval=BUTTON_DELETE_Gadget(cl,o,msg);
			break;
		case OM_READ:
			retval=BUTTON_READ_Gadget(cl,o,(struct piIO *)msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG BUTTON_READ_Gadget(piClass *cl,piObject *o,struct piIO *msg)
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

ULONG BUTTON_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
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
	SetCheckMark(PropsWnd,PG_BUTT_FlagsGadgets[GD_PG_BUTT_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_BUTT_FlagsGadgets[GD_PG_BUTT_FLAGS_UNDERSCORE],GetTagData(GT_Underscore,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_BUTT_FlagsGadgets[GD_PG_BUTT_FLAGS_IMMEDIATE],GetTagData(GA_Immediate,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_BUTT_FlagsGadgets[GD_PG_BUTT_FLAGS_TOGGLE],GetTagData(OWNTAG_Toggle,0,ng->ng_OwnTags));
	SetMX(PropsWnd,PG_BUTT_FlagsGadgets[GD_PG_BUTT_FLAGS_TITLETYPE],GetTagData(OWNTAG_TitleType,0,ng->ng_OwnTags));
	return TRUE;
}

ULONG BUTTON_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_GADTOOLS,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)ButtonTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)ButtonOwnTagsTypes,
		GAOBJ_ID,BUTTON_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_BUTT_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);
		gtd=(struct GadToolsData *)GadgetData(ng);

		CopyTagItems(ButtonTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(ButtonOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		gtd->g_Kind=BUTTON_KIND;
		gtd->g_Flags=PLACETEXT_IN;
		return (ULONG)obj;
	}
	return FALSE;
}

extern struct SmartBitMapIRGB ToolBarPalette[];
extern Class *SmartBitMapClass;

ULONG BUTTON_CREATE_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
{
	WORD x,y,w,h;
	ULONG type;
	struct TagItem *ti,*oti;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	struct Node_Image *ni;
	struct GadToolsData *gtd;
	struct NewGadget newg;
	piObject *io;

	piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&oti);
	if((type=GetTagData(OWNTAG_TitleType,0,oti))>0)
	{
		piGetAttr(o,OBJ_Node,(ULONG *)&ng);
		piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
		piGetAttr(o,GAOBJ_NodeData,(ULONG *)&gtd);
		piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);

		if(ng->ng_RelX) x=-(nw->nw_Window->Width-nw->nw_Window->BorderLeft-ng->ng_X-1);
		else x=piCX(o);
		if(ng->ng_RelY) y=-(nw->nw_Window->Height-nw->nw_Window->BorderTop-ng->ng_Y-1);
		else y=piCY(o);
		if(ng->ng_RelWidth) w=-(nw->nw_Window->BorderLeft+nw->nw_Window->BorderRight+ng->ng_X+(nw->nw_Window->Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight-ng->ng_Width)-1);
		else w=piCW(o)+1;
		if(ng->ng_RelHeight) h=-(nw->nw_Window->BorderTop+nw->nw_Window->BorderBottom+ng->ng_Y+(nw->nw_Window->Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom-ng->ng_Height)-1);
		else h=piCH(o)+1;

		newg.ng_LeftEdge=x;
		newg.ng_TopEdge=y;
		newg.ng_Width=w;
		newg.ng_Height=h;
		newg.ng_GadgetText=NULL;
		newg.ng_TextAttr=Font;	// ZMIANA
		newg.ng_GadgetID=0;
		newg.ng_Flags=gtd->g_Flags;
		newg.ng_VisualInfo=VisualInfo;
		newg.ng_UserData=NULL;

		ng->ng_Gadget=CreateGadgetA(gtd->g_Kind,(struct Gadget *)msg->Window,&newg,ti);
		if(GetTagData(OWNTAG_Toggle,0,oti)) ng->ng_Gadget->Activation|=GACT_TOGGLESELECT;
		if(ng->ng_RelX) ng->ng_Gadget->Flags|=GFLG_RELRIGHT;
		if(ng->ng_RelY) ng->ng_Gadget->Flags|=GFLG_RELBOTTOM;
		if(ng->ng_RelWidth) ng->ng_Gadget->Flags|=GFLG_RELWIDTH;
		if(ng->ng_RelHeight) ng->ng_Gadget->Flags|=GFLG_RELHEIGHT;
		ng->ng_Gadget->Flags|=GFLG_GADGIMAGE|GFLG_GADGHIMAGE;
		ng->ng_Gadget->Activation|=GACT_RELVERIFY;
		
		if((io=(piObject *)FindName(List(&GUI.gui_List_Image),gtd->g_Text)) && (!(GUI.gui_Prefs->pr_Flags&PRFLAG_FASTREDRAW)))
		{
			piGetAttr(io,OBJ_Node,(ULONG *)&ni);
			switch(type)
			{
				case 1:
					ng->ng_Gadget->GadgetRender=ng->ng_Gadget->SelectRender=NewObject(SmartBitMapClass,NULL,IA_Width,20,IA_Height,20,SMBM_Border,TRUE,SMBM_SelBorder,TRUE,SMBM_BitMap,(ULONG)ni->ni_BitMap,SMBM_Screen,Scr,SMBM_Palette,(ULONG)ToolBarPalette,TAG_DONE);
					break;
				case 2:
					ng->ng_Gadget->GadgetRender=ng->ng_Gadget->SelectRender=NewObject(SmartBitMapClass,NULL,IA_Width,ni->ni_Width,SMBM_Border,FALSE,SMBM_BitMap,(ULONG)ni->ni_BitMap,SMBM_Screen,Scr,SMBM_Palette,(ULONG)ni->ni_CT,TAG_DONE);
					break;
			}
		}
		else
		{
			newg.ng_GadgetText=gtd->g_Text;
		}
		return (ULONG) (ng->ng_Gadget);
	}
	else
	{
		return piDoSuperMethodA(cl,o,msg);
	}
}

ULONG BUTTON_DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct TagItem *oti;
	struct Node_Gadget *ng;

	piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&oti);
	switch(GetTagData(OWNTAG_TitleType,0,oti))
	{
		case 0:
			break;
		case 1:
		case 2:
			piGetAttr(o,OBJ_Node,(ULONG *)&ng);
			if(ng->ng_Gadget)
			{
				if(ng->ng_Gadget->GadgetRender) DisposeObject(ng->ng_Gadget->GadgetRender);
				ng->ng_Gadget->GadgetRender=ng->ng_Gadget->SelectRender=NULL;
			}
			ng->ng_Gadget=NULL;
			break;
	}
	return TRUE;
}


int FN_PG_BUTT_FLAGS_UNDERSCORE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);

	ti[0].ti_Tag=PropertiesMsg.Code?GT_Underscore:TAG_IGNORE;
	ti[0].ti_Data='_';
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}


int FN_PG_BUTT_FLAGS_IMMEDIATE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Immediate,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_BUTT_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_BUTT_FLAGS_TITLETYPE_Clicked( void )
{
	struct TagItem *oti;
	piGetAttr(CurrentGadget,GAOBJ_OwnTags,(ULONG *)&oti);
	FindTagItem(OWNTAG_TitleType,oti)->ti_Data=PropertiesMsg.Code;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_BUTT_FLAGS_TOGGLE_Clicked( void )
{
	struct TagItem *oti;
	piGetAttr(CurrentGadget,GAOBJ_OwnTags,(ULONG *)&oti);
	FindTagItem(OWNTAG_Toggle,oti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
