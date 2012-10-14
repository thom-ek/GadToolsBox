/*
** GadToolsBox v3.0 (C) 1996-98 by Tomasz Muszynski
*/

long __oslibversion = 39;

#include"includes.h"

/*
** Defines
*/

TEXT _FakeUserName[256];
TEXT _FakeAddress[512];
TEXT _FakeSerial[30];

#define Leave(x) { RequestOk(GetCatString(MSG_ERROR),x,GetCatString(MSG_OK),NULL); goto end; }

/*
** Global Variables
*/

struct ClipboardHandle *ClipHandle=NULL;
struct ClipboardHandle *UndoHandle=NULL;
struct FileRequester *OpenReq=NULL,*SaveReq=NULL,*GenReq=NULL;
struct ScreenModeRequester *ScreenModeReq=NULL;
struct FontRequester *FontReq=NULL;
//struct Library *ColorWheelBase=NULL,*GradientSliderBase=NULL;
struct Library *TapeDeckBase=NULL,*CyberGfxBase=NULL;
struct PopupMenuBase *PopupMenuBase;
Class *PopUpMenuClass=NULL,*DrawClass=NULL,*AmigaGuideClass=NULL;
piClass *piWindowClass=NULL,*piGadgetClass=NULL,*piFrameClass=NULL,*piMenuClass=NULL,*piScreenClass=NULL,*piStringClass=NULL,*piVersionClass=NULL,*piImageClass=NULL;
Object *gui_LeftImage=NULL,*gui_RightImage=NULL,*gui_UpImage=NULL,*gui_DownImage=NULL,*gui_SizeImage=NULL,*AmigaGuideObj=NULL;
struct GUI GUI;
struct Prefs Prefs;
BOOL running=TRUE,PropertiesChanged=FALSE,Semaphore=FALSE,PropertiesLock=TRUE;
ULONG WinSig,ScreenNum=0,LastCommand=CMD_NONE;
extern struct WBStartup *WBenchMsg;

extern UBYTE GadgetStatus;
extern struct BevelBox BBOX_PROPERTIES_PAGES;
extern UWORD FontX,FontY;
extern TEXT ProjectName[];

struct GTBSemaphore GTBSem;

#define TEMPLATE "FILE"
#define ARG_FILE 0
#define ARG_COUNT 1

/*
** Functions
*/

/*
void __saveds Trap_FreeAll(void)
{
	struct Task *task;

	geta4();
	task=FindTask(NULL);
	putreg( REG_A6, (long)task->tc_TrapData);

	FreeProject();
	if(Semaphore) RemSemaphore(&GTBSem.gs_Semaphore);
	CloseDownScreen();
	FreeClasses();
	if(PopUpMenuClass) DisposePopUpMenuClass(PopUpMenuClass);
	if(GradientSliderBase) CloseLibrary(GradientSliderBase);
	if(ColorWheelBase) CloseLibrary(ColorWheelBase);
	if(TapeDeckBase) CloseLibrary(TapeDeckBase);
	FreeApplication();
}
*/

//void *GlobalMemoryPool=NULL;

void *MyAllocVec(ULONG size,ULONG flags)
{
//	return AllocVecPooled(GlobalMemoryPool,size);
	return AllocVec(size,flags|MEMF_CLEAR);
//	return calloc(1,size);
}

void MyFreeVec(void *ptr)
{
//	FreeVecPooled(GlobalMemoryPool,ptr);
	FreeVec(ptr);
//	free(ptr);
}

void main(void)
{
	struct RDArgs *rdargs;
	ULONG signal;
	UWORD err;
	long *(opts[ARG_COUNT]);
	BOOL PreLoadFile=FALSE;
	ULONG AGSig;
//	struct Task *task;

//	task=FindTask(NULL);
//	task->tc_TrapData=(APTR)getreg( REG_A6 );
//	task->tc_TrapCode=Trap_FreeAll;

	if(!WBenchMsg)
	{
		opts[ARG_FILE]=(long *)ProjectName;

		if(rdargs=ReadArgs(TEMPLATE,(long *) opts, NULL))
		{
			strcpy(ProjectName,(char *)opts[ARG_FILE]);
			if(ProjectName[0]) PreLoadFile=TRUE;
			FreeArgs(rdargs);
		}
	}

	LoadRegistration();
	CheckRegistration();
	InitGUI();
	if(err=InitApplication(NULL)) Leave(GetErrorString(err));
//	unless(GlobalMemoryPool=CreatePool(MEMF_CLEAR|MEMF_ANY,30000,30000)) Leave("Can't create memory pool");
	CyberGfxBase=OpenLibrary("cybergraphics.library",39L);
	unless(PopupMenuBase=(struct PopupMenuBase *)OpenLibrary(POPUPMENU_NAME,POPUPMENU_VERSION)) Leave(GetCatString(MSG_ERROPENLIB_POPUPMENU));
	unless(TapeDeckBase=OpenLibrary("gadgets/tapedeck.gadget",3L)) Leave(GetCatString(MSG_ERROPENLIB_TAPEDECK));

	unless(PopUpMenuClass=CreatePopUpMenuClass()) Leave(GetCatString(MSG_CANTCREATEPOPUPMENUCLASS));
	unless(DrawClass=CreateDrawClass()) Leave(GetCatString(MSG_CANTCREATEDRAWCLASS));
	AmigaGuideClass=CreateAmigaGuideClass();
	unless(CreatePopupMenus()) Leave(GetCatString(MSG_CANTCREATEPOPUPMENUS));
	if(!(UndoHandle=OpenClipboard(1))) Leave(GetCatString(MSG_CANTOPENCLIPBOARD));

	unless(SaveReq=(struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,TAG_DONE)) Leave(GetCatString(MSG_CANTALLOCREQ));
	unless(OpenReq=(struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,TAG_DONE)) Leave(GetCatString(MSG_CANTALLOCREQ));
	unless(GenReq=(struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,TAG_DONE)) Leave(GetCatString(MSG_CANTALLOCREQ));
	unless(ScreenModeReq=(struct ScreenModeRequester *)AllocAslRequestTags(ASL_ScreenModeRequest,TAG_DONE)) Leave(GetCatString(MSG_CANTALLOCREQ));
	unless(FontReq=(struct FontRequester *)AllocAslRequestTags(ASL_FontRequest,TAG_DONE)) Leave(GetCatString(MSG_CANTALLOCREQ));

	LoadPrefs("ENV:GadToolsBox/GadToolsBox3.prefs");

	do
	{
		ScreenNum++;
		if(ScreenNum>256) Leave(GetErrorString(err));
	}
	while(err=SetupScreen(ScreenNum));

	GUI.gui_PubScreenName=ScrPubName;

	InitClasses();

	InitSemaphore(&GTBSem.gs_Semaphore);
	GTBSem.gs_Semaphore.ss_Link.ln_Name=ScrPubName;
	AddSemaphore(&GTBSem.gs_Semaphore);
	Semaphore=TRUE;

	unless(gui_LeftImage=NewImageObject(LEFTIMAGE)) Leave(GetCatString(MSG_CANTALLOCOBJECT));
	unless(gui_RightImage=NewImageObject(RIGHTIMAGE)) Leave(GetCatString(MSG_CANTALLOCOBJECT));
	unless(gui_UpImage=NewImageObject(UPIMAGE)) Leave(GetCatString(MSG_CANTALLOCOBJECT));
	unless(gui_DownImage=NewImageObject(DOWNIMAGE)) Leave(GetCatString(MSG_CANTALLOCOBJECT));
	unless(gui_SizeImage=NewImageObject(SIZEIMAGE)) Leave(GetCatString(MSG_CANTALLOCOBJECT));

	if(OpenWindow_TB_Main()) Leave(GetCatString(MSG_CANTOPENTOOLBAR));
	GetFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_OBJECTSOPEN) if(OpenObjectList()) Leave(GetCatString(MSG_CANTOPENOBJECTWINDOW));
	GetFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_STATUSOPEN) if(OpenStatusWindow()) Leave(GetCatString(MSG_CANTOPENSTATUSWINDOW));
	GetFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_PROPSOPEN) if(OpenWindow_Props()) Leave(GetCatString(MSG_CANTOPENPROPERTIESWINDOW));

	SetWndSignals();
	ObtainSemaphore(&GTBSem.gs_Semaphore);
	GTBSem.gs_GUI=&GUI;
	ReleaseSemaphore(&GTBSem.gs_Semaphore);

	RefreshProperties();

	if(WBenchMsg)
	{
		if(WBenchMsg->sm_NumArgs>1)
		{
			NameFromLock(WBenchMsg->sm_ArgList[1].wa_Lock,ProjectName,108);
			AddPart(ProjectName,WBenchMsg->sm_ArgList[1].wa_Name,108);

			PreLoadFile=TRUE;
		}
	}

	if(AmigaGuideClass)
	{
		if(AmigaGuideObj=NewObject(AmigaGuideClass,NULL,
				AGUI_Name,"GadToolsBox.guide",
				AGUI_Screen,Scr,
				AGUI_Activate,TRUE,
				AGUI_BaseName,"GadToolsBox",
				TAG_DONE))
			GetAttr(AGUI_SigBit,AmigaGuideObj,&AGSig);
	}

	if(PreLoadFile)
	{
		OpenWindow_Working();
		ObtainSemaphore(&GTBSem.gs_Semaphore);
		GTBSem.gs_Window=WorkingWnd;
		GTBSem.gs_Progress=WorkingGadgets[GD_WORK_PROGRESS];
		ReleaseSemaphore(&GTBSem.gs_Semaphore);
		SetWindowTitles(WorkingWnd,GetCatString(MSG_LOADING),(UBYTE *)-1);

		SetStatusWindow(0,GetCatString(MSG_LOADING));

		LoadGUI(ProjectName);

//		LayoutAllWindows();
		ObtainSemaphore(&GTBSem.gs_Semaphore);
		GTBSem.gs_Window=NULL;
		GTBSem.gs_Progress=NULL;
		ReleaseSemaphore(&GTBSem.gs_Semaphore);
		CloseWindow_Working();
		UpdateObjectList();
		SetStatusWindow(0,GetCatString(MSG_STATUS_RESLOADED));
	}

	while(running==1 || running==2)
	{
		signal=Wait(WinSig|(1L<<SIGBREAKF_CTRL_C)|AGSig);
		if(signal & AGSig) DoMethod(AmigaGuideObj,GTB_HANDLEMESSAGE,signal);
		if(signal & (1L<<SIGBREAKF_CTRL_C)) running=FALSE;
		if(signal & (1L<<ProjectMsgPort->mp_SigBit)) running=HandleWorkIDCMP();
		if(TB_MainWnd) if(signal & (1L<<TB_MainWnd->UserPort->mp_SigBit)) running=HandleIDCMP_TB_Main();
		if(TB_WindowWnd) if(signal & (1L<<TB_WindowWnd->UserPort->mp_SigBit)) running=HandleIDCMP_TB_Window();
		if(TB_PointerWnd) if(signal & (1L<<TB_PointerWnd->UserPort->mp_SigBit)) running=HandleIDCMP_TB_Pointer();
		if(PropertiesWnd) if(signal & (1L<<PropertiesWnd->UserPort->mp_SigBit)) running=HandleIDCMP_Properties();
		if(ScreenWnd) if(signal & (1L<<ScreenWnd->UserPort->mp_SigBit)) HandleIDCMP_Screen();
		if(EditStringWnd) if(signal & (1L<<EditStringWnd->UserPort->mp_SigBit)) running=HandleIDCMP_EditString();
		if(EditVersionWnd) if(signal & (1L<<EditVersionWnd->UserPort->mp_SigBit)) HandleIDCMP_EditVersion();
		if(ObjectWnd) if(signal & (1L<<ObjectWnd->UserPort->mp_SigBit)) running=HandleIDCMP_Object();
		if(StatusWnd) if(signal & (1L<<StatusWnd->UserPort->mp_SigBit)) running=HandleIDCMP_Status();
	}

end:
	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	if(TB_WindowWnd) CloseWindow_TB_Window();
	if(TB_MainWnd) CloseWindow_TB_Main();
	if(ScreenWnd) CloseWindow_Screen();
	CloseStatusWindow();
	CloseObjectList();
	CloseWindow_Props();

	FreeProject();

	if(AmigaGuideObj) DisposeObject(AmigaGuideObj);
	if(Semaphore) RemSemaphore(&GTBSem.gs_Semaphore);
	CloseDownScreen();
	if(gui_SizeImage) DisposeObject(gui_SizeImage);
	if(gui_DownImage) DisposeObject(gui_DownImage);
	if(gui_UpImage) DisposeObject(gui_UpImage);
	if(gui_RightImage) DisposeObject(gui_RightImage);
	if(gui_LeftImage) DisposeObject(gui_LeftImage);
	if(FontReq) FreeAslRequest(FontReq);
	if(ScreenModeReq) FreeAslRequest(ScreenModeReq);
	if(GenReq) FreeAslRequest(GenReq);
	if(OpenReq) FreeAslRequest(OpenReq);
	if(SaveReq) FreeAslRequest(SaveReq);
	FreeClasses();
	if(UndoHandle) CloseClipboard(UndoHandle); UndoHandle=NULL;
	FreePopupMenus();
	if(EditFontTF) CloseFont(EditFontTF);
	if(AmigaGuideClass) DisposeAmigaGuideClass(AmigaGuideClass);
	if(DrawClass) DisposeDrawClass(DrawClass);
	if(PopUpMenuClass) DisposePopUpMenuClass(PopUpMenuClass);
//	if(GradientSliderBase) CloseLibrary(GradientSliderBase);
//	if(ColorWheelBase) CloseLibrary(ColorWheelBase);
	if(TapeDeckBase) CloseLibrary(TapeDeckBase);
	if(PopupMenuBase) CloseLibrary((struct Library *)PopupMenuBase);
	if(CyberGfxBase) CloseLibrary(CyberGfxBase);
//	if(GlobalMemoryPool) DeletePool(GlobalMemoryPool);
	FreeRegistration();
	FreeApplication();
}

Object *NewImageObject(ULONG which)
{
	return NewObject(NULL, SYSICLASS,
		SYSIA_DrawInfo, DrawInfo,
		SYSIA_Which, which,
		SYSIA_Size, SysISize(),
		TAG_DONE);
}

int SysISize()
{
	return (Scr->Flags & SCREENHIRES ? SYSISIZE_MEDRES : SYSISIZE_LOWRES);
}

BOOL HandleWorkIDCMP()
{
	struct IntuiMessage	*m;
	struct MenuItem		*n;
	int			(*func)();
	BOOL			run = TRUE;
	piObject *obj;
	struct Node_Window *nw;
	struct Node_Image *ni;
	struct Node_Menu *nm;

	while( m = GT_GetIMsg( ProjectMsgPort )) {

		CopyMem(( char * )m, ( char * )&EditMsg, (long)sizeof( struct IntuiMessage ));
		if(!EditMsg.IDCMPWindow)
		{
			GT_ReplyIMsg( m );
			return 1;
		}
		if(obj=(piObject *) EditMsg.IDCMPWindow->UserData)
		{
			nw=(struct Node_Window *)OBJECT_DATA(obj);
			ActiveObject=nw->nw_ID;
			switch(nw->nw_ID)
			{
				case ID_WIND:
					CurrentWindow=obj;
					CurrentWindowNode=nw;
					break;
				case ID_MENU:
					CurrentMenu=obj;
					break;
				case ID_IMGE:
					CurrentImage=obj;
					break;
				default:
					CurrentWindow=NULL;
					CurrentImage=NULL;
					CurrentMenu=NULL;
					break;
			}
		}
/*		if(PM_FilterIMsg(EditMsg.IDCMPWindow,PM_ObjGadget,m,TAG_DONE))
		{
			if(n = ItemAddress( MainMenus, EditMsg.Code ))
			{
				func = (void *)(GTMENUITEM_USERDATA( n ));
				run = func();
				if(running > 1)
				{
					GT_ReplyIMsg( m );
					return running;
				}
			}
		}
*/
		if((EditMsg.Class==IDCMP_MENUVERIFY) && obj)
		{
			switch(nw->nw_ID)
			{
				case ID_WIND: 
					if(GadgetStatus!=GS_NOTHING)
					{
						SetWindowPointer(CurrentWindowNode->nw_Window,TAG_DONE);
						GadgetStatus=GS_NOTHING;
						piEraseObject(CurrentWindow);
						piRenderObject(CurrentWindow);
						m->Code=MENUCANCEL;
						EditMsg.Class=NULL;
					}
					else
					{
						if(EditMsg.MouseX>0 && EditMsg.MouseY>0 && EditMsg.MouseX<CurrentWindowNode->nw_Window->Width && EditMsg.MouseY<CurrentWindowNode->nw_Window->Height)
						{
							m->Code=MENUCANCEL;
							EditMsg.Class=IDCMP_MOUSEBUTTONS;
							EditMsg.Code=MENUDOWN;
						}
						else
							EditMsg.Class=NULL;
					}
//					PM_FilterIMsg(EditMsg.IDCMPWindow,PM_ObjGadget,m,TAG_DONE);
					break;
				case ID_MENU:
					piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);
					if(EditMsg.MouseX>0 && EditMsg.MouseY>0 && EditMsg.MouseX<nm->nm_Window->Width && EditMsg.MouseY<nm->nm_Window->Height)
					{
						m->Code=MENUCANCEL;
						EditMsg.Class=IDCMP_MOUSEBUTTONS;
						EditMsg.Code=MENUDOWN;
					}
					else
						EditMsg.Class=NULL;
//					PM_FilterIMsg(EditMsg.IDCMPWindow,PM_ObjMenu,m,TAG_DONE);
					break;
				case ID_IMGE:
					piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
					if(EditMsg.MouseX>0 && EditMsg.MouseY>0 && EditMsg.MouseX<ni->ni_Window->Width && EditMsg.MouseY<ni->ni_Window->Height)
					{
						m->Code=MENUCANCEL;
						EditMsg.Class=IDCMP_MOUSEBUTTONS;
						EditMsg.Code=MENUDOWN;
					}
					else
						EditMsg.Class=NULL;
//					PM_FilterIMsg(EditMsg.IDCMPWindow,PM_ObjMenu,m,TAG_DONE);
					break;
			}
		}
		GT_ReplyIMsg( m );

		switch ( EditMsg.Class ) {

			case	IDCMP_REFRESHWINDOW:
				GT_BeginRefresh( EditMsg.IDCMPWindow );
				GT_EndRefresh( EditMsg.IDCMPWindow, TRUE );
				break;

			case	IDCMP_CLOSEWINDOW:
				switch(nw->nw_ID)
				{
					case ID_WIND: running = FN_CloseWindow_Edit(); break;
					case ID_MENU: running = FN_CloseWindow_EditMenu(); break;
					case ID_IMGE: running = FN_CloseWindow_EditImage(); break;
				}
				break;

			case	IDCMP_ACTIVEWINDOW:
				switch(nw->nw_ID)
				{
					case ID_WIND: running = FN_ActiveWindow_Edit(); break;
					case ID_MENU: running = FN_ActiveWindow_EditMenu(); break;
					case ID_IMGE: running = FN_ActiveWindow_EditImage(); break;
				}
				break;

			case	IDCMP_NEWSIZE:
				switch(nw->nw_ID)
				{
					case ID_WIND: running = FN_NewSize_Edit(); break;
					case ID_MENU: running = FN_NewSize_EditMenu(); break;
					case ID_IMGE: running = FN_NewSize_EditImage(); break;
				}
				break;

			case	IDCMP_CHANGEWINDOW:
				switch(nw->nw_ID)
				{
					case ID_WIND: running = FN_ChangeWindow_Edit(); break;
//					case ID_MENU: running = FN_NewSize_EditMenu(); break;
//					case ID_IMGE: running = FN_NewSize_EditImage(); break;
				}
				break;

			case	IDCMP_IDCMPUPDATE:
				switch(nw->nw_ID)
				{
//					case ID_WIND: running = FN_IDCMPUpdate_Edit(); break;
					case ID_MENU: running = FN_IDCMPUpdate_EditMenu(); break;
					case ID_IMGE: running = FN_IDCMPUpdate_EditImage(); break;
				}
				break;

			case	IDCMP_MOUSEBUTTONS:
				switch(nw->nw_ID)
				{
					case ID_WIND: running = FN_MouseButtons_Edit(); break;
					case ID_MENU: running = FN_MouseButtons_EditMenu(); break;
					case ID_IMGE: running = FN_MouseButtons_EditImage(); break;
				}
				break;

			case	IDCMP_MOUSEMOVE:
				switch(nw->nw_ID)
				{
					case ID_WIND: running = FN_MouseMove_Edit(); break;
					case ID_MENU: running = FN_MouseMove_EditMenu(); break;
					case ID_IMGE: running = FN_MouseMove_EditImage(); break;
				}
				break;

			case	IDCMP_GADGETUP:
			case	IDCMP_GADGETDOWN:
				break;

			case	IDCMP_RAWKEY:
				running = FN_RawKey_Edit();
				break;

			case	IDCMP_MENUPICK:
				while( EditMsg.Code != MENUNULL ) {
					n = ItemAddress( MainMenus, EditMsg.Code );
					func = (void *)(GTMENUITEM_USERDATA( n ));
					run = func();
					if(running > 1) return running;
					EditMsg.Code = n->NextSelect;
				}
				break;
		}
	}
	return( run );
}

void SetWndSignals()
{
	WinSig=NULL;

	WinSig|=1L<<ProjectMsgPort->mp_SigBit;
	if(TB_MainWnd) WinSig|=1L<<TB_MainWnd->UserPort->mp_SigBit;
	if(TB_WindowWnd) WinSig|=1L<<TB_WindowWnd->UserPort->mp_SigBit;
	if(TB_PointerWnd) WinSig|=1L<<TB_PointerWnd->UserPort->mp_SigBit;
	if(PropertiesWnd) WinSig|=1L<<PropertiesWnd->UserPort->mp_SigBit;
	if(ScreenWnd) WinSig|=1L<<ScreenWnd->UserPort->mp_SigBit;
	if(EditStringWnd) WinSig|=1L<<EditStringWnd->UserPort->mp_SigBit;
	if(EditVersionWnd) WinSig|=1L<<EditVersionWnd->UserPort->mp_SigBit;
	if(ObjectWnd) WinSig|=1L<<ObjectWnd->UserPort->mp_SigBit;
	if(StatusWnd) WinSig|=1L<<StatusWnd->UserPort->mp_SigBit;
}

struct Node *FindNodeNum(struct List *l,ULONG num)
{
	struct Node *node;
	ULONG a;

	node=l->lh_Head;

	for(a=0;a<num;a++)
	{
		if(!node->ln_Succ) return NULL;
		node=node->ln_Succ;
	}
	if(!node->ln_Succ) return NULL;
	return node;
}

ULONG GetNumNode(struct List *l,struct Node *n)
{
	ULONG a=0;
	struct Node *node;

	node=l->lh_Head;
	while(node!=n)
	{
		if(!node->ln_Succ) return a;
		node=node->ln_Succ;
		a++;
	}
	return a;
}

void MoveNodeUp(struct List *l,struct Node *n)
{
	struct Node *after;

	if(l->lh_Head==n) return; // already first
	after=GetPred(GetPred(n));
	Remove(n);
	Insert(l,n,after);
}

void MoveNodeDown(struct List *l,struct Node *n)
{
	struct Node *after;

	if(l->lh_TailPred==n) return; // already last
	after=GetSucc(n);
	Remove(n);
	Insert(l,n,after);
}

void ShowGrid(piObject *o)
{
	int x,y;
	struct Node_Window *nw;
	
	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	if(GUI.gui_GridShow && GUI.gui_Grid[0]>0 && GUI.gui_Grid[0]>0 && GUI.gui_Mode!=MODE_TEST)
	{
		SetDrMd(nw->nw_Window->RPort,JAM1);
		SetAPen(nw->nw_Window->RPort,DrawInfo->dri_Pens[SHADOWPEN]);
		for(x=0;x<nw->nw_Window->Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight;x+=GUI.gui_Grid[0])
		{
			for(y=0;y<nw->nw_Window->Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom;y+=GUI.gui_Grid[1])
			{
				WritePixel(nw->nw_Window->RPort,nw->nw_Window->BorderLeft+x,nw->nw_Window->BorderTop+y);
			}
		}
	}
}

UWORD GridX(piObject *o,UWORD x)
{
	struct Node_Window *nw=(struct Node_Window *)OBJECT_DATA(o);

	if(GUI.gui_GridOn)
	{
		return((UWORD)(((x-nw->nw_Window->BorderLeft)-((x-nw->nw_Window->BorderLeft)%GUI.gui_Grid[0]))+nw->nw_Window->BorderLeft));
	}
	else return x;
}

UWORD GridY(piObject *o,UWORD y)
{
	struct Node_Window *nw=(struct Node_Window *)OBJECT_DATA(o);

	if(GUI.gui_GridOn)
	{
		return((UWORD)(((y-nw->nw_Window->BorderTop)-((y-nw->nw_Window->BorderTop)%GUI.gui_Grid[1]))+nw->nw_Window->BorderTop));
	}
	else return y;
}

void InitGUI()
{
	NewList(List(&GUI.gui_List_Window));
	NewList(List(&GUI.gui_List_Menu));
	NewList(List(&GUI.gui_List_Image));
	NewList(List(&GUI.gui_List_Binary));
	GUI.gui_Mode=MODE_GADGET;
	GUI.gui_PubScreenName=ScrPubName;
	GUI.gui_Grid[0]=1;
	GUI.gui_Grid[1]=1;
	GUI.gui_Prefs=&Prefs;
	GUI.gui_Version=VER_VERSION;
	GUI.gui_Revision=VER_REVISION;
}

void FreeProject()
{
	struct Node_Screen *ns;

	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	if(TB_WindowWnd) CloseWindow_TB_Window();
	PropertiesMode=PM_NONE;
	ActiveObject=NULL;
	RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
	DisposeAllWindows(); CurrentWindow=NULL; CurrentGadget=NULL; CurrentFrame=NULL;
	DisposeAllMenus(); CurrentMenu=NULL;
	DisposeAllImages(); CurrentImage=NULL;
	if(GUI.gui_Node_Version) piDisposeObject(GUI.gui_Node_Version);
	if(GUI.gui_Node_String) piDisposeObject(GUI.gui_Node_String);
	if(GUI.gui_Node_Screen) piDisposeObject(GUI.gui_Node_Screen);
	GUI.gui_Node_Screen=CurrentScreen=piNewObject(piScreenClass,TAG_DONE);
	GUI.gui_Node_String=CurrentString=piNewObject(piStringClass,TAG_DONE);
	GUI.gui_Node_Version=CurrentVersion=piNewObject(piVersionClass,TAG_DONE);
	UpdateObjectList();
	RefreshObjectList();

	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);
	if(EditFontTF) CloseFont(EditFontTF);
	EditFontTA.ta_Name=ns->ns_FontName;
	EditFontTA.ta_YSize=ns->ns_FontSize;
	EditFontTA.ta_Style=FS_NORMAL;
	EditFontTA.ta_Flags=FPF_DISKFONT|FPF_DESIGNED;
	EditFontTF=OpenDiskFont(&EditFontTA);
}

struct TagItem *AllocTagItems(ULONG num)
{
	return MyAllocVec((num+1)*sizeof(struct TagItem),MEMF_ANY|MEMF_CLEAR);
}

void MyFreeTagItems(struct TagItem *t)
{
	MyFreeVec(t);
}

void CopyTagItems(struct TagItem *s,struct TagItem *d,ULONG num)
{
	CopyMem(s,d,num*sizeof(struct TagItem));
}

void InitClasses()
{
	int a;

	OpenWindow_PGEMPTY();

	piScreenClass=piMakeClass(CLASS_SCREEN,NULL,sizeof(struct Node_Screen));
	piOpenClass(piScreenClass);
	GUI.gui_Node_Screen=CurrentScreen=piNewObject(piScreenClass,TAG_DONE);

	piVersionClass=piMakeClass(CLASS_VERSION,NULL,sizeof(struct Node_Version));
	piOpenClass(piVersionClass);
	GUI.gui_Node_Version=CurrentVersion=piNewObject(piVersionClass,TAG_DONE);

	piStringClass=piMakeClass(CLASS_STRING,NULL,sizeof(struct Node_String));
	piOpenClass(piStringClass);
	GUI.gui_Node_String=CurrentString=piNewObject(piStringClass,TAG_DONE);

	piWindowClass=piMakeClass(CLASS_WINDOW,NULL,sizeof(struct Node_Window));
	piOpenClass(piWindowClass);

	piFrameClass=piMakeClass(CLASS_FRAME,NULL,sizeof(struct Node_Frame));
	piOpenClass(piFrameClass);

	piImageClass=piMakeClass(CLASS_IMAGE,NULL,sizeof(struct Node_Image));
	piOpenClass(piImageClass);

	piMenuClass=piMakeClass(CLASS_MENU,NULL,sizeof(struct Node_Menu));
	piOpenClass(piMenuClass);

	piGadgetClass=piMakeClass(CLASS_GADGET,NULL,sizeof(struct Node_Gadget));
	piOpenClass(piGadgetClass);
	for(a=0;a<PLUGIN_GADGETS;a++)
	{
		piGadgets[a].Class=piMakeClass(CLASS_GADGET,piGadgetClass,sizeof(struct Node_Gadget));
		piGadgets[a].Class->Dispatcher=piGadgets[a].Dispatcher;
		piOpenClass(piGadgets[a].Class);
	}
}

void FreeClasses()
{
	int a;

	for(a=0;a<PLUGIN_GADGETS;a++)
	{
		if(piGadgets[a].Class)
		{
			piCloseClass(piGadgets[a].Class);
			piFreeClass(piGadgets[a].Class);
		}
	}
	if(piGadgetClass)
	{
		piCloseClass(piGadgetClass);
		piFreeClass(piGadgetClass);
	}
	if(piMenuClass)
	{
		piCloseClass(piMenuClass);
		piFreeClass(piMenuClass);
	}
	if(piImageClass)
	{
		piCloseClass(piImageClass);
		piFreeClass(piImageClass);
	}
	if(piFrameClass)
	{
		piCloseClass(piFrameClass);
		piFreeClass(piFrameClass);
	}
	if(piWindowClass)
	{
		piCloseClass(piWindowClass);
		piFreeClass(piWindowClass);
	}
	if(piStringClass)
	{
		if(GUI.gui_Node_String) piDisposeObject(GUI.gui_Node_String);
		piCloseClass(piStringClass);
		piFreeClass(piStringClass);
		GUI.gui_Node_String=NULL;
	}
	if(piVersionClass)
	{
		if(GUI.gui_Node_Version) piDisposeObject(GUI.gui_Node_Version);
		piCloseClass(piVersionClass);
		piFreeClass(piVersionClass);
		GUI.gui_Node_Version=NULL;
	}
	if(piScreenClass)
	{
		if(GUI.gui_Node_Screen) piDisposeObject(GUI.gui_Node_Screen);
		piCloseClass(piScreenClass);
		piFreeClass(piScreenClass);
		GUI.gui_Node_Screen=NULL;
	}
	CloseWindow_PGEMPTY();
}

/****************************************************************************/

/* Properties Gadget tags */
ULONG PropsGTags[]=
{
	CHECKBOX_KIND, PLACETEXT_LEFT, GTCB_Scaled, TRUE, GA_Text, (ULONG) "", GA_Left, 296, GA_Top, 4, GA_Width, 18, GA_Height, 15, GA_UserData, (ULONG) FN_PROPERTIES_LOCK_Clicked, TAG_DONE,
	BUTTON_KIND, PLACETEXT_IN, GA_ToggleSelect, FALSE, GA_Text, (ULONG) MSG_PROPERTIES_APPLY_GAD, GA_Left, 276, GA_Top, 4, GA_Width, 18, GA_Height, 15, GA_UserData, (ULONG) FN_PROPERTIES_APPLY_Clicked, TAG_DONE,
	PAGE_KIND, NULL, GA_RelVerify, TRUE, PAGE_Pages, (ULONG) NULL, PAGE_Active, 0, PAGE_TextAttr, NULL, PAGE_Layout, PGLYT_SIMPLE, PAGE_ActiveStyle, PGAST_NORMAL, PAGE_ClearBBox, (ULONG) &BBOX_PROPERTIES_PAGES, GA_Left, 8, GA_Top, 4, GA_Width, 257, GA_Height, 20, GA_UserData, (ULONG) FN_PROPERTIES_PAGES_Clicked, TAG_DONE
};

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGEMPTY[];

struct Page PAGES_PROPERTIES_PGEMPTY_PAGES[]=
{
	NULL, "", FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

ULONG AP_NONE=0,AP_WIND=0,AP_GADG=0,AP_FRME=0,AP_IMGE=0,AP_MENU=0,AP_STRG=0;
ULONG CurPMode=0;

void RefreshPropertiesWindow(STRPTR title)
{
	if(PropertiesWnd)
	{
		if(PropertiesGadgets[GD_PROPERTIES_PAGES])
		{
			switch(CurPMode)
			{
				case PM_WINDOW: GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &AP_WIND); break;
				case PM_GADGET: GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &AP_GADG); break;
				case PM_IMAGE: GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &AP_IMGE); break;
				case PM_FRAME: GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &AP_FRME); break;
				case PM_MENU: GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &AP_MENU); break;
				case PM_STRING: GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &AP_STRG); break;
			}
			RemoveGList(PropertiesWnd,PropertiesGadgets[GD_PROPERTIES_PAGES],-1);
			DisposeObject((Object *)PropertiesGadgets[GD_PROPERTIES_PAGES]);
			PropertiesGadgets[GD_PROPERTIES_PAGES]=NULL;
		}
		DisableGadget(PropertiesGadgets[GD_PROPERTIES_APPLY],PropertiesWnd,FALSE);

		switch(PropertiesMode)
		{
			case PM_WINDOW:
				piGetAttr(CurrentWindow,OBJ_Properties,(ULONG *)&PropsGTags[39]);
				PropsGTags[41]=AP_WIND;
				break;
			case PM_GADGET:
				piGetAttr(CurrentGadget,OBJ_Properties,(ULONG *)&PropsGTags[39]);
				PropsGTags[41]=AP_GADG;
				break;
			case PM_IMAGE:
				piGetAttr(CurrentImage,OBJ_Properties,(ULONG *)&PropsGTags[39]);
				PropsGTags[41]=AP_IMGE;
				break;
			case PM_FRAME:
				piGetAttr(CurrentFrame,OBJ_Properties,(ULONG *)&PropsGTags[39]);
				PropsGTags[41]=AP_FRME;
				break;
			case PM_MENU:
				piGetAttr(CurrentMenu,OBJ_Properties,(ULONG *)&PropsGTags[39]);
				PropsGTags[41]=AP_MENU;
				break;
			case PM_STRING:
				piGetAttr(CurrentString,OBJ_Properties,(ULONG *)&PropsGTags[39]);
				PropsGTags[41]=AP_STRG;
				break;
			default:
				PropsGTags[39]=(ULONG)PAGES_PROPERTIES_PGEMPTY_PAGES;
				PropsGTags[41]=0;
				break;
		}

		PropsGTags[43]=(ULONG)Font;
		PropertiesGadgets[GD_PROPERTIES_PAGES]=CreateGadgetExA(PropertiesWnd->BorderLeft,PropertiesWnd->BorderTop,PropertiesWnd->BorderRight,PropertiesWnd->BorderBottom,NULL,(struct TagItem *) &PropsGTags[34]);
		AddGList(PropertiesWnd,PropertiesGadgets[GD_PROPERTIES_PAGES],-1,-1,NULL);
		RefreshGList(PropertiesGadgets[GD_PROPERTIES_PAGES],PropertiesWnd,NULL,-1);

		SetWindowTitles(PropertiesWnd,title,(UBYTE *)-1);

		DisableGadget(PropertiesGadgets[GD_PROPERTIES_APPLY],PropertiesWnd,FALSE);
		switch(PropertiesMode)
		{
			case PM_WINDOW:
				piInitObjectGUI(CurrentWindow);
				break;
			case PM_GADGET:
				piInitObjectGUI(CurrentGadget);
				break;
			case PM_IMAGE:
				piInitObjectGUI(CurrentImage);
				break;
			case PM_FRAME:
				piInitObjectGUI(CurrentFrame);
				break;
			case PM_MENU:
				piInitObjectGUI(CurrentMenu);
				break;
			case PM_STRING:
				piInitObjectGUI(CurrentString);
				break;
			case PM_NONE:
				DisableGadget(PropertiesGadgets[GD_PROPERTIES_APPLY],PropertiesWnd,TRUE);
				break;
		}
	}
	CurPMode=PropertiesMode;
}

UWORD OpenWindow_Props()
{
	struct Gadget *g;
	UWORD lc,tc;
	UWORD OffX, OffY;

	if(PropertiesWnd)
		RefreshPropertiesWindow((UBYTE *)-1);
	else
	{
		if(!(g=CreateContext(&PropertiesGList))) return GTBERR_CREATECONTEXT;

		CalcLeftTop(&OffX,&OffY,(struct TagItem *)PropertiesWTags);
		if(FindTagItem(WA_FontAdaptive,(struct TagItem *)PropertiesWTags)) CalcAdaptivity(GetTagData(WA_Width,GetTagData(WA_InnerWidth,GetTagData(WA_RelWidth,0,(struct TagItem *)PropertiesWTags),(struct TagItem *)PropertiesWTags),(struct TagItem *)PropertiesWTags),GetTagData(WA_Height,GetTagData(WA_InnerHeight,GetTagData(WA_RelHeight,0,(struct TagItem *)PropertiesWTags),(struct TagItem *)PropertiesWTags),(struct TagItem *)PropertiesWTags)); else { FontX=7; FontY=13; }

//		BBOX_PROPERTIES_PAGES.Top=AdaptY()+OffY;

		PropsGTags[22]=(ULONG)GetCatString(MSG_PROPERTIES_APPLY_GAD);
//		PropsGTags[5]=(ULONG)GetCatString(MSG_PROPERTIES_LOCK_GAD);

		for(lc=0,tc=0;lc<CNT_Properties-1;lc++)
		{
//			if(!GadgetLocaleDone[WND_Properties]) FixGadgetTags((struct TagItem *)&PropsGTags[tc]);
			PropertiesGadgets[lc]=g=CreateGadgetExA(OffX,OffY,0,0,g,(struct TagItem *) &PropsGTags[tc]);
			tc+=2;
			while(PropertiesGTags[tc]!=TAG_DONE) tc+=2;
			tc++;
			if(!(g)) return GTBERR_CREATEGADGET;
		}
		PropertiesGadgets[GD_PROPERTIES_PAGES]=NULL;

		SetCheckMark(NULL,PropertiesGadgets[GD_PROPERTIES_LOCK],PropertiesLock);
	
		if(!(PropertiesWnd=OpenWindowTagsEx(
					WA_Gadgets,PropertiesGList,
					WA_CustomScreen,Scr,
					TAG_MORE, PropertiesWTags ))) return GTBERR_OPENWINDOW;
	
		RefreshPropertiesWindow((UBYTE *)-1);
		MenuCheck(MainMenus,FULLMENUNUM(3,0,0),TRUE);
		SetFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_PROPSOPEN);
		SetMenuStrip(PropertiesWnd,MainMenus);
		GT_RefreshWindow(PropertiesWnd,NULL);
		SetWndSignals();
	}
	return GTBERR_OK;
}

void CloseWindow_Props()
{
	if(PropertiesGadgets[GD_PROPERTIES_PAGES])
	{
		RemoveGList(PropertiesWnd,PropertiesGadgets[GD_PROPERTIES_PAGES],-1);
		DisposeObject((Object *)PropertiesGadgets[GD_PROPERTIES_PAGES]);
		PropertiesGadgets[GD_PROPERTIES_PAGES]=NULL;
	}
	if(PropertiesWnd) CloseWindowEx(PropertiesWnd); PropertiesWnd=NULL;
	if(PropertiesGList) { FreeGadgets(PropertiesGList); PropertiesGList=NULL; }
	SetWndSignals();
	MenuCheck(MainMenus,FULLMENUNUM(3,0,0),FALSE);
	ClearFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_PROPSOPEN);
}

int FN_CloseWindow_Properties()
{
	FN_PROPERTIES_APPLY_Clicked();
	CloseWindow_Props();
	return 2;
}

int FN_MouseMove_Properties()
{
	return 1;
}

int FN_ChangeWindow_Properties()
{
	GUI.gui_Prefs->pr_WinPos[WIN_PROPERTIES].X=PropertiesWnd->LeftEdge;
	GUI.gui_Prefs->pr_WinPos[WIN_PROPERTIES].Y=PropertiesWnd->TopEdge;
	FindTagItem(WA_Left,(struct TagItem *)PropertiesWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_PROPERTIES].X;
	FindTagItem(WA_Top,(struct TagItem *)PropertiesWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_PROPERTIES].Y;
	return 1;
}

int FN_IntuiTicks_Properties()
{
	return 1;
}

int FN_InActiveWindow_Properties()
{
	if(!PropertiesLock)
	{
		FN_PROPERTIES_APPLY_Clicked();
		CloseWindow_Props();
		return 2;
	}
	else return 1;
}

int FN_PROPERTIES_APPLY_Clicked()
{
	switch(PropertiesMode)
	{
		case PM_WINDOW:
			piApplyObject(CurrentWindow);
			if(PropertiesChanged)
			{
				piCloseWindow(CurrentWindow);
				piOpenWindow(CurrentWindow);
			}
			break;
		case PM_GADGET:
			piApplyObject(CurrentGadget);
			if(PropertiesChanged)
				piRenderObject(CurrentGadget);
			break;
		case PM_IMAGE:
			piApplyObject(CurrentImage);
			if(PropertiesChanged)
				piRenderObject(CurrentImage);
			break;
		case PM_FRAME:
			piApplyObject(CurrentFrame);
			if(PropertiesChanged)
				piRenderObject(CurrentFrame);
			break;
		case PM_MENU:
			piApplyObject(CurrentMenu);
			if(PropertiesChanged)
				piRenderObject(CurrentMenu);
			break;
		case PM_STRING:
			piApplyObject(CurrentString);
			if(PropertiesChanged)
				piRenderObject(CurrentString);
			break;
		case PM_NONE:
			break;
	}
	SetStatusWindow(1,(STRPTR)-1);
	PropertiesChanged=FALSE;
	return 1;
}

int FN_PROPERTIES_LOCK_Clicked()
{
	PropertiesLock=PropertiesMsg.Code;
	return 1;
}

int FN_PROPERTIES_PAGES_Clicked()
{
	GT_RefreshWindow(PropertiesWnd,NULL);
	return 1;
}

struct PopupMenu *PM_Object=NULL;
struct PopupMenu *PM_ObjGadget=NULL;
struct PopupMenu *PM_ObjSelGadget=NULL;
struct PopupMenu *PM_ObjWindow=NULL;
struct PopupMenu *PM_ObjMenu=NULL;
struct PopupMenu *PM_ObjImage=NULL;

int CreatePopupMenus()
{
	PM_Object=PM_MakeMenu(
			PMItem(GetCatString(MSG_MainEditCut_MITEM)+2),		PM_UserData,	FN_MainEditCut_Selected, End,
			PMItem(GetCatString(MSG_MainEditCopy_MITEM)+2),		PM_UserData,	FN_MainEditCopy_Selected, End,
			PMItem(GetCatString(MSG_MainEditPaste_MITEM)+2),	PM_UserData,	FN_MainEditPaste_Selected, End,
			PMBar,						PMEnd,
			PMItem(GetCatString(MSG_MainObjStringsOpen_MSUB)+2),	PM_UserData,	NULL, End,
			PMItem(GetCatString(MSG_MainObjWindowClose_MSUB)+2),	PM_UserData,	NULL, End,
			End;
	unless(PM_Object) return NULL;

	PM_ObjGadget=PM_MakeMenu(
			PMItem(GetCatString(MSG_MainEditCut_MITEM)+2),		PM_UserData,	FN_MainEditCut_Selected, End,
			PMItem(GetCatString(MSG_MainEditCopy_MITEM)+2),		PM_UserData,	FN_MainEditCopy_Selected, End,
			PMItem(GetCatString(MSG_MainEditPaste_MITEM)+2),	PM_UserData,	FN_MainEditPaste_Selected, End,
			PMBar,						PMEnd,
			PMItem(GetCatString(MSG_MainWindowProperties_MITEM)+2),	PM_UserData,	OpenWindow_Props, End,
			End;
	unless(PM_ObjGadget) return NULL;

	PM_ObjSelGadget=PM_MakeMenu(
			PMItem(GetCatString(MSG_MainEditCut_MITEM)+2),		PM_UserData,	FN_MainEditCut_Selected, End,
			PMItem(GetCatString(MSG_MainEditCopy_MITEM)+2),		PM_UserData,	FN_MainEditCopy_Selected, End,
			PMItem(GetCatString(MSG_MainEditPaste_MITEM)+2),	PM_UserData,	FN_MainEditPaste_Selected, End,
			PMBar,						PMEnd,
			PMItem(GetCatString(MSG_ALIGNLEFT)),	PM_UserData,	FN_MainEditAlignToLeft_Selected, End,
			PMItem(GetCatString(MSG_ALIGNHCENTER)),	PM_UserData,	FN_MainEditAlignToHCenter_Selected, End,
			PMItem(GetCatString(MSG_ALIGNRIGHT)),	PM_UserData,	FN_MainEditAlignToRight_Selected, End,
			PMItem(GetCatString(MSG_ALIGNTOP)),	PM_UserData,	FN_MainEditAlignToTop_Selected, End,
			PMItem(GetCatString(MSG_ALIGNVCENTER)),	PM_UserData,	FN_MainEditAlignToVCenter_Selected, End,
			PMItem(GetCatString(MSG_ALIGNBOTTOM)),	PM_UserData,	FN_MainEditAlignToBottom_Selected, End,
			PMBar,						PMEnd,
			PMItem(GetCatString(MSG_SPREADHORIZ)),	PM_UserData,	FN_MainEditSpreadHoriz_Selected, End,
			PMItem(GetCatString(MSG_SPREADVERT)),	PM_UserData,	FN_MainEditSpreadVertical_Selected, End,
			PMBar,						PMEnd,
			PMItem(GetCatString(MSG_MainWindowProperties_MITEM)+2),	PM_UserData,	OpenWindow_Props, End,
			End;
	unless(PM_ObjSelGadget) return NULL;

	PM_ObjWindow=PM_MakeMenu(
			PMItem(GetCatString(MSG_MainEditCut_MITEM)+2),		PM_UserData,	FN_MainEditCut_Selected, End,
			PMItem(GetCatString(MSG_MainEditCopy_MITEM)+2),		PM_UserData,	FN_MainEditCopy_Selected, End,
			PMItem(GetCatString(MSG_MainEditPaste_MITEM)+2),	PM_UserData,	FN_MainEditPaste_Selected, End,
			PMBar,						PMEnd,
			PMItem(GetCatString(MSG_MainObjWindowClose_MSUB)+2),	PM_UserData,	FN_MainObjWindowClose_Selected, End,
			PMItem(GetCatString(MSG_MainObjWindowRefresh_MSUB)+2),	PM_UserData,	FN_MainObjWindowRefresh_Selected, End,
			PMItem(GetCatString(MSG_MainWindowProperties_MITEM)+2),	PM_UserData,	OpenWindow_Props, End,
			End;
	unless(PM_ObjWindow) return NULL;

	PM_ObjMenu=PM_MakeMenu(
			PMItem(GetCatString(MSG_MainEditCut_MITEM)+2),		PM_UserData,	FN_MainEditCut_Selected, End,
			PMItem(GetCatString(MSG_MainEditCopy_MITEM)+2),		PM_UserData,	FN_MainEditCopy_Selected, End,
			PMItem(GetCatString(MSG_MainEditPaste_MITEM)+2),	PM_UserData,	FN_MainEditPaste_Selected, End,
			PMBar,						PMEnd,
			PMItem(GetCatString(MSG_MainObjMenuClose_MSUB)+2),	PM_UserData,	FN_MainObjMenuClose_Selected, End,
			PMItem(GetCatString(MSG_MainWindowProperties_MITEM)+2),	PM_UserData,	OpenWindow_Props, End,
			End;
	unless(PM_ObjMenu) return NULL;

	PM_ObjImage=PM_MakeMenu(
			PMItem(GetCatString(MSG_MainEditCut_MITEM)+2),		PM_UserData,	FN_MainEditCut_Selected, End,
			PMItem(GetCatString(MSG_MainEditCopy_MITEM)+2),		PM_UserData,	FN_MainEditCopy_Selected, End,
			PMItem(GetCatString(MSG_MainEditPaste_MITEM)+2),	PM_UserData,	FN_MainEditPaste_Selected, End,
			PMBar,						PMEnd,
			PMItem(GetCatString(MSG_MainObjImageClose_MSUB)+2),	PM_UserData,	FN_MainObjImageClose_Selected, End,
			PMItem(GetCatString(MSG_MainWindowProperties_MITEM)+2),	PM_UserData,	OpenWindow_Props, End,
			End;
	unless(PM_ObjImage) return NULL;

	return TRUE;
}

void FreePopupMenus()
{
	if(PM_Object) PM_FreePopupMenu(PM_Object);
	if(PM_ObjImage) PM_FreePopupMenu(PM_ObjImage);
	if(PM_ObjMenu) PM_FreePopupMenu(PM_ObjMenu);
	if(PM_ObjWindow) PM_FreePopupMenu(PM_ObjWindow);
	if(PM_ObjGadget) PM_FreePopupMenu(PM_ObjGadget);
	if(PM_ObjSelGadget) PM_FreePopupMenu(PM_ObjSelGadget);
}

/****************************************************************************/

KEYFILE *key=NULL;
UBYTE *keybuf=NULL;
int keylen;

void LoadRegistration()
{
	int a;
	BPTR in=FALSE;
	BOOL ok=FALSE;
	ULONG chsum,*lens;
	TEXT keyfilename[256];
	STRPTR paths[]=
	{
		"PROGDIR:",
		"S:",
		"L:",
		NULL,
		"KEYFILES:",
		"KEYPATH:",
		NULL
	};
	STRPTR vars[]=
	{
		"KEYFILES",
		"KEYPATH",
		NULL
	};

	strcpy(_FakeUserName,"Not registered");
	strcpy(_FakeAddress,"Please register!");
	strcpy(_FakeSerial,"mailto:thom@union.org.pl");

	for(a=0;vars[a];a++)
	{
		if(GetVar(vars[a],keyfilename,256,NULL)!=-1)
		{
			AddPart(keyfilename,"GadToolsBox3.key",256);
			if(in=Open(keyfilename,MODE_OLDFILE)) goto fileok;
		}
	}

	for(a=0;paths[a];a++)
	{
		strcpy(keyfilename,paths[a]);
		AddPart(keyfilename,"GadToolsBox3.key",256);
		if(in=Open(keyfilename,MODE_OLDFILE)) goto fileok;
	}

fileok:
	if(in)
	{
		Seek(in,0,OFFSET_END);
		keylen=Seek(in,0,OFFSET_BEGINNING);
		if(keybuf=MyAllocVec(keylen,MEMF_ANY))
		{
			Read(in,keybuf,keylen);
			key=(KEYFILE *)keybuf;
			if(key->kf_KeyID==MAKE_ID('K','E','Y','F') && key->kf_KeyVersion==0 &&
					key->kf_ProgramID==0 && key->kf_Version==3 && key->kf_Revision==0)
			{
				chsum=0;
				for(a=0;a<keylen-4;a++) chsum+=(keybuf[a]^(0xcf+a)*a);
				if(chsum==(*((ULONG *)((UBYTE *) keybuf+keylen-4))))
				{
					strcpy(_FakeUserName,keybuf+sizeof(KEYFILE)+4);
					lens=(ULONG *)((UBYTE *)keybuf+sizeof(KEYFILE));
					strcpy(_FakeAddress,keybuf+sizeof(KEYFILE)+4+(*lens)+4);
					strcpy(_FakeSerial,key->kf_Serial);
					ok=TRUE;
				}
			}
		}
		Close(in);
	}
filefail:
	if(!ok) FreeRegistration();
}

void FreeRegistration()
{
	if(keybuf) MyFreeVec(keybuf); keybuf=NULL; key=NULL;
}

BOOL __inline CheckRegistration(void)
{
	int a;
	ULONG chsum;

	if(key)
	{
		chsum=0;
		for(a=0;a<keylen-4;a++) chsum+=(keybuf[a]^(0xcf+a)*a);
		if(chsum==(*((ULONG *)((UBYTE *) keybuf+keylen-4))))
			return TRUE;
	}
	return FALSE;
}

// FULLMENUNUM

void MenuCheck(struct Menu *Menu, UWORD MenuNum,BOOL Enable)
{
	struct MenuItem *mi;

	if(mi=ItemAddress(Menu,MenuNum))
	{
		if(Enable) SetFlag(mi->Flags,CHECKED); else ClearFlag(mi->Flags,CHECKED);
	}
}

int RequestOk(STRPTR title,STRPTR string,STRPTR gads, int ble, ...)
{
	struct EasyStruct es;

	es.es_StructSize=sizeof(struct EasyStruct);
	es.es_Flags=0;
	es.es_Title=title;
	es.es_TextFormat=string;
	es.es_GadgetFormat=gads;
	return EasyRequestArgs(TB_MainWnd,&es,NULL,&ble);
}

STRPTR GetErrorString(UWORD errnum)
{
	switch(errnum)
	{
		case GTBERR_LOCALE: return "Can't open Locale";
		case GTBERR_CREATEPROJECTPORT: return GetCatString(MSG_ERRCREATEPROJECTPORT);
		case GTBERR_CREATEPOINTER: return GetCatString(MSG_ERRCREATEPOINTER);
		case GTBERR_CREATEIMAGE: return GetCatString(MSG_ERRCREATEIMAGE);
		case GTBERR_OPENLIB_COLORWHEEL: return GetCatString(MSG_ERROPENLIB_COLORWHEEL);
		case GTBERR_OPENLIB_GRADIENTSLIDER: return GetCatString(MSG_ERROPENLIB_GRADIENTSLIDER);
		case GTBERR_OPENLIB_PAGE: return GetCatString(MSG_ERROPENLIB_PAGE);
		case GTBERR_OPENLIB_PROGRESS: return GetCatString(MSG_ERROPENLIB_PROGRESS);
		case GTBERR_OPENLIB_TEXTFIELD: return GetCatString(MSG_ERROPENLIB_TEXTFIELD);
		case GTBERR_OPENLIB_TREEVIEW: return GetCatString(MSG_ERROPENLIB_TREEVIEW);
		case GTBERR_OPENLIB_SMARTBITMAP: return GetCatString(MSG_ERROPENLIB_SMARTBITMAP);
		case GTBERR_OPENSCREEN: return GetCatString(MSG_ERR_OPENSCREEN);
		case GTBERR_VISUALINFO: return GetCatString(MSG_ERR_VISUALINFO);
		case GTBERR_SCRDRAWINFO: return GetCatString(MSG_ERR_SCRDRAWINFO);
		default: return GetCatString(MSG_ERR_UNKNOWN);
	}
}
