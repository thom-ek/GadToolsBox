/*
** GadToolsBox v3 - Screen Managment
*/

#include"includes.h"

/*
** Defines
*/

extern ULONG ScreenNum;

/*
** Functions
*/

ULONG Dispatcher_Screen(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval=NULL;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=INIT_Screen(cl,o,msg);
			break;
		case OM_APPLY:
			retval=APPLY_Screen(cl,o,msg);
			break;
		case OM_NEW:
			retval=NEW_Screen(cl,o,(struct piSet *)msg);
			break;
		case OM_SET:
			retval=SET_Screen(cl,o,(struct piSet *)msg);
			break;
		case OM_GET:
			retval=GET_Screen(cl,o,(struct piGet *)msg);
			break;
		case OM_DISPOSE:
			retval=DISPOSE_Screen(cl,o,(struct piSet *)msg);
			break;
		case OM_CREATE:
			retval=OPEN_Screen(cl,o,msg);
			break;
		case OM_DELETE:
			retval=CLOSE_Screen(cl,o,msg);
			break;
		case OM_READ:
			retval=READ_Screen(cl,o,(struct piIO *)msg);
			break;
		case OM_WRITE:
			retval=WRITE_Screen(cl,o,(struct piIO *)msg);
			break;
	}
	return retval;
}

ULONG NEW_Screen(piClass *cl,piObject *o,struct piSet *msg)
{
	piObject *obj;
	struct Node_Screen *ns;

	if(obj=MyAllocVec(sizeof(piObject)+cl->Size,MEMF_PUBLIC|MEMF_CLEAR))
	{
		ns=(struct Node_Screen *)OBJECT_DATA(obj);
		obj->ln_Name=ns->ns_Title;
		ns->ns_Width=ScrWidth;
		ns->ns_Height=ScrHeight;
		ns->ns_Depth=ScrDepth;
		ns->ns_Type=PUBLICSCREEN;
		ns->ns_DisplayID=ScrDisplayID;
		strcpy(ns->ns_Title,GetCatString(MSG_CHANGEME));
		strcpy(ns->ns_Public,GetCatString(MSG_CHANGEME));
		strcpy(ns->ns_FontName,GUI.gui_Prefs->pr_FontName);
		ns->ns_MyFlags=MYSC_ShowTitle|MYSC_Draggable|MYSC_SharePens|MYSC_LikeWB;
		ns->ns_FontSize=GUI.gui_Prefs->pr_FontSize;

		if(EditFontTF) CloseFont(EditFontTF);
		EditFontTA.ta_Name=ns->ns_FontName;
		EditFontTA.ta_YSize=ns->ns_FontSize;
		EditFontTA.ta_Style=FS_NORMAL;
		EditFontTA.ta_Flags=FPF_DISKFONT|FPF_DESIGNED;
		EditFontTF=OpenDiskFont(&EditFontTA);
	}
	return (ULONG) obj;
}

ULONG DISPOSE_Screen(piClass *cl,piObject *o,Msg msg)
{
	piDoMethod(o,OM_DELETE);
	MyFreeVec(o);
	return TRUE;
}

ULONG OPEN_Screen(piClass *cl,piObject *o,Msg msg)
{
	if(!ScreenWnd)
	{
		OpenWindow_Screen();
		piInitObjectGUI(o);
		SetWndSignals();
	}
	else
		ActivateWindow(ScreenWnd);
	return TRUE;
}

ULONG CLOSE_Screen(piClass *cl,piObject *o,Msg msg)
{
	if(ScreenWnd)
	{
		piApplyObject(o);
		CloseWindow_Screen();
		SetWndSignals();
	}
	return TRUE;
}

ULONG SET_Screen(piClass *cl,piObject *o,struct piSet *msg)
{
//	struct TagItem *t;
//	struct Node_Screen *ns=(struct Node_Screen *)OBJECT_DATA(o);

	return TRUE;
}

ULONG GET_Screen(piClass *cl,piObject *o,struct piGet *msg)
{
	struct Node_Screen *ns=(struct Node_Screen *)OBJECT_DATA(o);

	switch(msg->TagID)
	{
		case OBJ_Node: *(msg->TagData)=(ULONG)ns; return TRUE;
		case GA_Width: *(msg->TagData)=(ULONG)ns->ns_Width; return TRUE;
		case GA_Height: *(msg->TagData)=(ULONG)ns->ns_Height; return TRUE;
		default: *(msg->TagData)=NULL; return FALSE;
	}
}

ULONG READ_Screen(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0;
	struct Node_Screen *ns;

	piGetAttr(o,OBJ_Node,(ULONG *)&ns);
	ReadChunkBytes(msg->IFFh,(APTR)&ns->ns_Width,SIZE(Node_Screen,ns_Width));

	if(EditFontTF) CloseFont(EditFontTF);
	EditFontTA.ta_Name=ns->ns_FontName;
	EditFontTA.ta_YSize=ns->ns_FontSize;
	EditFontTA.ta_Style=FS_NORMAL;
	EditFontTA.ta_Flags=FPF_DISKFONT|FPF_DESIGNED;
	EditFontTF=OpenDiskFont(&EditFontTA);

	return (ULONG)error;
}

ULONG WRITE_Screen(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0;
	struct Node_Screen *ns;
	
	piGetAttr(o,OBJ_Node,(ULONG *)&ns);
	if(error=PushChunk(msg->IFFh,NULL,ID_SCRN,SIZE(Node_Screen,ns_Width))) return (ULONG)error;
	WriteChunkBytes(msg->IFFh,(APTR)&ns->ns_Width,SIZE(Node_Screen,ns_Width));
	if(error=PopChunk(msg->IFFh)) return (ULONG) error;
	return (ULONG)error;
}

extern struct NameInfo NameInfo;

ULONG INIT_Screen(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Screen *ns;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ns);
	GetAttr(PAGE_Active,ScreenGadgets[GD_SCREEN_PAGES], &active);

// General
	if(active==0) PropsWnd=ScreenWnd; else PropsWnd=NULL;
	if(ns->ns_Type==TYPE_WORKBENCH)
	{
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_WIDTH],PropsWnd,TRUE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_HEIGHT],PropsWnd,TRUE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_DEPTH],PropsWnd,TRUE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_DEFAULTID],PropsWnd,TRUE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MAKEPUBLIC],PropsWnd,TRUE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MODEID_GET],PropsWnd,TRUE);
	}
	else
	{
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_WIDTH],PropsWnd,FALSE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_HEIGHT],PropsWnd,FALSE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_DEPTH],PropsWnd,FALSE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_DEFAULTID],PropsWnd,FALSE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MAKEPUBLIC],PropsWnd,FALSE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MODEID_GET],PropsWnd,ns->ns_MyFlags&MYSC_DefaultID);
	}
	SetString(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_TITLE],ns->ns_Title);
	SetString(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_PUBNAME],ns->ns_Public);
	SetInteger(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_WIDTH],ns->ns_Width);
	SetInteger(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_HEIGHT],ns->ns_Height);
	SetInteger(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_DEPTH],ns->ns_Depth);
	SetCheckMark(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_MAKEPUBLIC],ns->ns_MyFlags&MYSC_MakePublic);
	SetCheckMark(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_DEFAULTID],ns->ns_MyFlags&MYSC_DefaultID?FALSE:TRUE);
	SetCycle(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_USE],ns->ns_Type);
	if(ns->ns_MyFlags&MYSC_DefaultID)
	{
		SetText(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_MODEID],"---");
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MODEID_GET],PropsWnd,TRUE);
	}
	else
	{
		GetDisplayInfoData(FindDisplayInfo(ns->ns_DisplayID),(UBYTE *)&NameInfo,sizeof(struct NameInfo),DTAG_NAME,NULL);
		SetText(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_MODEID],NameInfo.Name);
		if(ns->ns_Type!=TYPE_WORKBENCH) DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MODEID_GET],PropsWnd,FALSE);
	}

// Flags
	if(active==1) PropsWnd=ScreenWnd; else PropsWnd=NULL;
	if(ns->ns_Type==TYPE_WORKBENCH)
	{
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_SHOWTITLE],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_BEHIND],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_QUIET],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_AUTOSCROLL],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_FULLPALETTE],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_DRAGGABLE],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_EXCLUSIVE],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_SHAREPENS],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_INTERLEAVED],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_LIKEWORKBENCH],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_MINIMIZEISG],PropsWnd,TRUE);
	}
	else
	{
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_SHOWTITLE],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_BEHIND],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_QUIET],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_AUTOSCROLL],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_FULLPALETTE],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_DRAGGABLE],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_EXCLUSIVE],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_SHAREPENS],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_INTERLEAVED],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_LIKEWORKBENCH],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_MINIMIZEISG],PropsWnd,FALSE);
	}
	SetCheckMark(PropsWnd,PGSCR_FlagsGadgets[GD_PGSCR_SHOWTITLE],ns->ns_MyFlags&MYSC_ShowTitle);
	SetCheckMark(PropsWnd,PGSCR_FlagsGadgets[GD_PGSCR_BEHIND],ns->ns_MyFlags&MYSC_Behind);
	SetCheckMark(PropsWnd,PGSCR_FlagsGadgets[GD_PGSCR_QUIET],ns->ns_MyFlags&MYSC_Quiet);
	SetCheckMark(PropsWnd,PGSCR_FlagsGadgets[GD_PGSCR_AUTOSCROLL],ns->ns_MyFlags&MYSC_AutoScroll);
	SetCheckMark(PropsWnd,PGSCR_FlagsGadgets[GD_PGSCR_FULLPALETTE],ns->ns_MyFlags&MYSC_FullPalette);
	SetCheckMark(PropsWnd,PGSCR_FlagsGadgets[GD_PGSCR_DRAGGABLE],ns->ns_MyFlags&MYSC_Draggable);
	SetCheckMark(PropsWnd,PGSCR_FlagsGadgets[GD_PGSCR_EXCLUSIVE],ns->ns_MyFlags&MYSC_Exclusive);
	SetCheckMark(PropsWnd,PGSCR_FlagsGadgets[GD_PGSCR_SHAREPENS],ns->ns_MyFlags&MYSC_SharePens);
	SetCheckMark(PropsWnd,PGSCR_FlagsGadgets[GD_PGSCR_INTERLEAVED],ns->ns_MyFlags&MYSC_Interleaved);
	SetCheckMark(PropsWnd,PGSCR_FlagsGadgets[GD_PGSCR_LIKEWORKBENCH],ns->ns_MyFlags&MYSC_LikeWB);
	SetCheckMark(PropsWnd,PGSCR_FlagsGadgets[GD_PGSCR_MINIMIZEISG],ns->ns_MyFlags&MYSC_MinimizeISG);
	return TRUE;
}

ULONG APPLY_Screen(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Screen *ns;

	piGetAttr(o,OBJ_Node,(ULONG *)&ns);

	strcpy(ns->ns_Title,GetString(PGSCR_GeneralGadgets[GD_PGSCR_TITLE]));
	strcpy(ns->ns_Public,GetString(PGSCR_GeneralGadgets[GD_PGSCR_PUBNAME]));
	ns->ns_Width=GetInteger(PGSCR_GeneralGadgets[GD_PGSCR_WIDTH]);
	ns->ns_Height=GetInteger(PGSCR_GeneralGadgets[GD_PGSCR_HEIGHT]);
	ns->ns_Depth=GetInteger(PGSCR_GeneralGadgets[GD_PGSCR_DEPTH]);

	return TRUE;
}

/******************************************************************************/

extern UWORD ScrWidth;
extern UWORD ScrHeight;
extern UWORD ScrDepth;
extern ULONG ScrDisplayID;
extern struct BevelBox BBOX_PROPERTIES_PAGES;
extern struct BevelBox BBOX_PROPERTIES_PAGES_PGWN_Style[];
extern struct BevelBox BBOX_PROPERTIES_PAGES_PGWN_Flags[];
extern struct BevelBox BBOX_PROPERTIES_PAGES_PGWN_Flags3[];
extern struct BevelBox BBOX_PREFS_PAGES;
extern struct BevelBox BBOX_SCREEN_PAGES;
extern struct BevelBox BBOX_SCREEN_PAGES_PGSCR_General[];

int FN_CloseWindow_Screen(void)
{
	piApplyObject(CurrentScreen);
	piCloseWindow(CurrentScreen);
	return 2;
}

int FN_PGSCR_DEFAULTID_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	struct Window *PropsWnd;

	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code?FALSE:TRUE;
	DoFlag(ns->ns_MyFlags,MYSC_DefaultID);

	PropsWnd=ScreenWnd;
	if(ns->ns_MyFlags&MYSC_DefaultID)
	{
		SetText(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_MODEID],"---");
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MODEID_GET],PropsWnd,TRUE);
	}
	else
	{
		GetDisplayInfoData(FindDisplayInfo(ns->ns_DisplayID),(UBYTE *)&NameInfo,sizeof(struct NameInfo),DTAG_NAME,NULL);
		SetText(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_MODEID],NameInfo.Name);
		if(ns->ns_Type!=TYPE_WORKBENCH) DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MODEID_GET],PropsWnd,FALSE);
	}
	return 1;
}

int FN_PGSCR_MODEID_GET_Clicked(void)
{
	struct Node_Screen *ns;
	struct Window *PropsWnd;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	PropsWnd=ScreenWnd;
	if(AslRequestTags(ScreenModeReq,
		ASLSM_PubScreenName,GUI.gui_PubScreenName,
		ASLSM_InitialHeight,Scr->Height-50,
		ASLSM_InitialDisplayID,ns->ns_DisplayID,
		ASLSM_InitialDisplayWidth,ns->ns_Width,
		ASLSM_InitialDisplayHeight,ns->ns_Height,
		ASLSM_InitialDisplayDepth,ns->ns_Depth,
		ASLSM_DoWidth,TRUE,
		ASLSM_DoHeight,TRUE,
		ASLSM_DoDepth,TRUE,
		ASLSM_PropertyMask,NULL,
		TAG_DONE))
	{
		ns->ns_DisplayID=ScreenModeReq->sm_DisplayID;
		ns->ns_Width=ScreenModeReq->sm_DisplayWidth;
		ns->ns_Height=ScreenModeReq->sm_DisplayHeight;
		ns->ns_Depth=ScreenModeReq->sm_DisplayDepth;
		SetInteger(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_WIDTH],ns->ns_Width);
		SetInteger(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_HEIGHT],ns->ns_Height);
		SetInteger(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_DEPTH],ns->ns_Depth);
		GetDisplayInfoData(FindDisplayInfo(ns->ns_DisplayID),(UBYTE *)&NameInfo,sizeof(struct NameInfo),DTAG_NAME,NULL);
		SetText(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_MODEID],NameInfo.Name);
	}
	return 1;
}

int FN_PGSCR_USE_Clicked(void)
{
	struct Node_Screen *ns;
	struct Window *PropsWnd;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	ns->ns_Type=ScreenMsg.Code;
	PropsWnd=ScreenWnd;
	if(ns->ns_Type==TYPE_WORKBENCH)
	{
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_WIDTH],PropsWnd,TRUE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_HEIGHT],PropsWnd,TRUE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_DEPTH],PropsWnd,TRUE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_DEFAULTID],PropsWnd,TRUE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MAKEPUBLIC],PropsWnd,TRUE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MODEID_GET],PropsWnd,TRUE);
		PropsWnd=NULL;
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_SHOWTITLE],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_BEHIND],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_QUIET],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_AUTOSCROLL],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_FULLPALETTE],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_DRAGGABLE],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_EXCLUSIVE],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_SHAREPENS],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_INTERLEAVED],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_LIKEWORKBENCH],PropsWnd,TRUE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_MINIMIZEISG],PropsWnd,TRUE);
	}
	else
	{
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_WIDTH],PropsWnd,FALSE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_HEIGHT],PropsWnd,FALSE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_DEPTH],PropsWnd,FALSE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_DEFAULTID],PropsWnd,FALSE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MAKEPUBLIC],PropsWnd,FALSE);
		DisableGadget(PGSCR_GeneralGadgets[GD_PGSCR_MODEID_GET],PropsWnd,ns->ns_MyFlags&MYSC_DefaultID);
		PropsWnd=NULL;
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_SHOWTITLE],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_BEHIND],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_QUIET],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_AUTOSCROLL],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_FULLPALETTE],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_DRAGGABLE],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_EXCLUSIVE],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_SHAREPENS],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_INTERLEAVED],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_LIKEWORKBENCH],PropsWnd,FALSE);
		DisableGadget(PGSCR_FlagsGadgets[GD_PGSCR_MINIMIZEISG],PropsWnd,FALSE);
	}
	return 1;
}

int FN_PGSCR_PUBNAME_Clicked(void)
{
	return 1;
}

int FN_PGSCR_WIDTH_Clicked(void)
{
	return 1;
}

int FN_PGSCR_HEIGHT_Clicked(void)
{
	return 1;
}

int FN_PGSCR_DEPTH_Clicked(void)
{
	return 1;
}

int FN_PGSCR_TITLE_Clicked(void)
{
	return 1;
}

int FN_PGSCR_MINIMIZEISG_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_MinimizeISG);
	return 1;
}

int FN_PGSCR_SHAREPENS_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_SharePens);
	return 1;
}

int FN_PGSCR_FULLPALETTE_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_FullPalette);
	return 1;
}

int FN_PGSCR_EXCLUSIVE_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_Exclusive);
	return 1;
}

int FN_PGSCR_QUIET_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_Quiet);
	return 1;
}

int FN_PGSCR_LIKEWORKBENCH_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_LikeWB);
	return 1;
}

int FN_PGSCR_SHOWTITLE_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_ShowTitle);
	return 1;
}

int FN_PGSCR_MAKEPUBLIC_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_MakePublic);
	return 1;
}

int FN_PGSCR_BEHIND_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_Behind);
	return 1;
}

int FN_PGSCR_DRAGGABLE_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_Draggable);
	return 1;
}

int FN_PGSCR_AUTOSCROLL_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_AutoScroll);
	return 1;
}

int FN_PGSCR_INTERLEAVED_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG code;
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	code=ScreenMsg.Code;
	DoFlag(ns->ns_MyFlags,MYSC_Interleaved);
	return 1;
}

int FN_SCREEN_PAGES_Clicked(void)
{
	struct Node_Screen *ns;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);
	GetAttr(PAGE_Active,ScreenGadgets[GD_SCREEN_PAGES], &active);

// General
	if(active==0) PropsWnd=ScreenWnd; else PropsWnd=NULL;
	if(ns->ns_MyFlags&MYSC_DefaultID)
	{
		SetText(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_MODEID],"---");
	}
	else
	{
		GetDisplayInfoData(FindDisplayInfo(ns->ns_DisplayID),(UBYTE *)&NameInfo,sizeof(struct NameInfo),DTAG_NAME,NULL);
		SetText(PropsWnd,PGSCR_GeneralGadgets[GD_PGSCR_MODEID],NameInfo.Name);
	}
	return 1;
}

int FN_ActiveWindow_Screen( void )
{
	ActiveObject=ID_SCRN;
	PropertiesMode=PM_NONE;
	RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
	if(TB_WindowWnd) CloseWindow_TB_Window();
	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	return 1;
}
