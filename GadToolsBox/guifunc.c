/* This file contains empty template routines that
 * the IDCMP handler will call uppon. Fill out these
 * routines with your code or use them as a reference
 * to create your program.
 */

#include"includes.h"

struct List *GenList;
ULONG ActiveGadget=NULL,PropertiesMode=PM_NONE,ActiveObject=NULL;
piObject *CurrentWindow=NULL;
piObject *CurrentGadget=NULL;
piObject *CurrentFrame=NULL;
piObject *CurrentMenu=NULL;
piObject *CurrentScreen=NULL;
piObject *CurrentString=NULL;
piObject *CurrentVersion=NULL;
piObject *CurrentImage=NULL;
struct Node_Window *CurrentWindowNode=NULL;
BOOL Shift=FALSE;
LONG SelectedLV=-1;
TEXT ProjectName[256]="\0",FontName[256]="\0";
struct NameInfo NameInfo;
#define Bit32(x) ((ULONG)(x|(x<<8)|(x<<16)|(x<<24)))
struct TextAttr EditFontTA;
struct TextFont *EditFontTF=NULL;
BOOL ProjectChanged=FALSE;

/*********** Status ***********************************************************/

void SetStatusWindow(int changed,STRPTR status)
{
	switch(changed)
	{
		case 0: ProjectChanged=changed; break;
		case 1: ProjectChanged=changed; break;
	}

	if(StatusWnd)
	{
		switch(changed)
		{
			case 0: SetText(StatusWnd,StatusGadgets[GD_STATUS_CHANGED]," "); break;
			case 1: SetText(StatusWnd,StatusGadgets[GD_STATUS_CHANGED],"*"); break;
		}
		if(status!=(STRPTR)-1) SetText(StatusWnd,StatusGadgets[GD_STATUS_TEXT],status);
	}
}

int OpenStatusWindow(void)
{
	int retval;

	if(!StatusWnd)
	{
		if(!(retval=OpenWindow_Status()))
		{
			SetStatusWindow(ProjectChanged,(STRPTR)-1);
			SetWndSignals();
			MenuCheck(MainMenus,FULLMENUNUM(3,2,0),TRUE);
			SetFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_STATUSOPEN);
			return 0;
		}
		else return retval;
	}
	else
	{
		ActivateWindow(StatusWnd);
		WindowToFront(StatusWnd);
	}
	return 0;
}

void CloseStatusWindow(void)
{
	if(StatusWnd)
	{
		CloseWindow_Status();
		MenuCheck(MainMenus,FULLMENUNUM(3,2,0),FALSE);
		ClearFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_STATUSOPEN);
	}
}

int FN_ChangeWindow_Status(void)
{
	GUI.gui_Prefs->pr_WinPos[WIN_STATUS].X=StatusWnd->LeftEdge;
	GUI.gui_Prefs->pr_WinPos[WIN_STATUS].Y=StatusWnd->TopEdge;
	FindTagItem(WA_Left,(struct TagItem *)StatusWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_STATUS].X;
	FindTagItem(WA_Top,(struct TagItem *)StatusWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_STATUS].Y;
	return 1;
}

/*********** Prefs ************************************************************/

int FN_PG_PREFS_GENERAL_SIZEZOOM_Clicked(void)
{
	return 1;
}

int FN_PG_PREFS_GENERAL_FASTREDRAW_Clicked(void)
{
	return 1;
}

int FN_PG_PREFS_GENERAL_AUTOBACKUP_Clicked(void)
{
	return 1;
}

int FN_PG_PREFS_GENERAL_WRITEICONS_Clicked( void )
{
	return 1;
}

int FN_PG_PREFS_GENERAL_GETSCREEN_Clicked( void )
{
	if(AslRequestTags(ScreenModeReq,
		ASLSM_PubScreenName,GUI.gui_PubScreenName,
		ASLSM_InitialHeight,Scr->Height-50,
		ASLSM_InitialDisplayID,GUI.gui_Prefs->pr_DisplayID,
		ASLSM_InitialDisplayWidth,GUI.gui_Prefs->pr_Width,
		ASLSM_InitialDisplayHeight,GUI.gui_Prefs->pr_Height,
		ASLSM_InitialDisplayDepth,GUI.gui_Prefs->pr_Depth,
		ASLSM_DoWidth,TRUE,
		ASLSM_DoHeight,TRUE,
		ASLSM_DoDepth,TRUE,
		ASLSM_PropertyMask,NULL,
		TAG_DONE))
	{
		GUI.gui_Prefs->pr_DisplayID=ScreenModeReq->sm_DisplayID;
		GUI.gui_Prefs->pr_Width=ScreenModeReq->sm_DisplayWidth;
		GUI.gui_Prefs->pr_Height=ScreenModeReq->sm_DisplayHeight;
		GUI.gui_Prefs->pr_Depth=ScreenModeReq->sm_DisplayDepth;
		GetDisplayInfoData(FindDisplayInfo(GUI.gui_Prefs->pr_DisplayID),(UBYTE *)&NameInfo,sizeof(struct NameInfo),DTAG_NAME,NULL);
		SetText(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_SCREENMODE],NameInfo.Name);
	}
	return 1;
}

UWORD RecalcObjectSizeX(struct TextFont *from,struct TextFont *to,UWORD value)
{
	return ((UWORD) (to->tf_XSize*value+(from->tf_XSize))/from->tf_XSize);
}

UWORD RecalcObjectSizeY(struct TextFont *from,struct TextFont *to,UWORD value)
{
	return ((UWORD) (to->tf_YSize*value+(from->tf_YSize))/from->tf_YSize);
}

void RecalculateGUI(struct TextFont *from,struct TextFont *to)
{
	struct Node_Gadget *ng;
	struct Node_Frame *nf;
	struct Node_Window *nw;
	piObject *wo,*go,*fo;

	for(wo=(piObject *)GetHead(&GUI.gui_List_Window);GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
		nw->nw_X=RecalcObjectSizeX(from,to,nw->nw_X);
		nw->nw_Y=RecalcObjectSizeY(from,to,nw->nw_Y);
		nw->nw_Width=RecalcObjectSizeX(from,to,nw->nw_Width);
		nw->nw_Height=RecalcObjectSizeY(from,to,nw->nw_Height);

		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
		{
			piGetAttr(go,OBJ_Node,(ULONG *)&ng);
			ng->ng_X=RecalcObjectSizeX(from,to,ng->ng_X);
			ng->ng_Y=RecalcObjectSizeY(from,to,ng->ng_Y);
			ng->ng_Width=RecalcObjectSizeX(from,to,ng->ng_Width);
			ng->ng_Height=RecalcObjectSizeY(from,to,ng->ng_Height);
		}
		for(fo=(piObject *)GetHead(List(&nw->nw_List_Frame));GetSucc(fo);fo=(piObject *)GetSucc(fo))
		{
			piGetAttr(fo,OBJ_Node,(ULONG *)&nf);
			nf->nf_X=RecalcObjectSizeX(from,to,nf->nf_X);
			nf->nf_Y=RecalcObjectSizeY(from,to,nf->nf_Y);
			nf->nf_Width=RecalcObjectSizeX(from,to,nf->nf_Width);
			nf->nf_Height=RecalcObjectSizeY(from,to,nf->nf_Height);
		}
	}
}

int FN_PG_PREFS_GENERAL_EDITFONT_Clicked( void )
{
	struct Node_Screen *ns;
	UWORD OldSize;
	struct TextFont oldtf;

	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);
	if(AslRequestTags(FontReq,
		ASLFO_PubScreenName,GUI.gui_PubScreenName,
		ASLFO_InitialHeight,Scr->Height-50,
		ASLFO_InitialName,GUI.gui_Prefs->pr_FontName,
		ASLFO_InitialSize,GUI.gui_Prefs->pr_FontSize,
		ASLFO_MaxHeight,128,
		TAG_DONE))
	{
		OldSize=ns->ns_FontSize;
		strncpy(ns->ns_FontName,FontReq->fo_Attr.ta_Name,30);
		strncpy(GUI.gui_Prefs->pr_FontName,FontReq->fo_Attr.ta_Name,30);
		ns->ns_FontSize=GUI.gui_Prefs->pr_FontSize=FontReq->fo_Attr.ta_YSize;
		sprintf(FontName,"%s %ld, %s %ld",GUI.gui_Prefs->pr_FontName,GUI.gui_Prefs->pr_FontSize, ns->ns_FontName, ns->ns_FontSize);
		SetText(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_FONTNAME],FontName);

		CopyMem(EditFontTF,&oldtf,sizeof(struct TextFont));
		if(EditFontTF) CloseFont(EditFontTF);
		EditFontTA.ta_Name=ns->ns_FontName;
		EditFontTA.ta_YSize=ns->ns_FontSize;
		EditFontTA.ta_Style=FS_NORMAL;
		EditFontTA.ta_Flags=FPF_DISKFONT|FPF_DESIGNED;
		EditFontTF=OpenDiskFont(&EditFontTA);

		// GUI Font size, adapt GUI!
		if(ns->ns_FontSize!=OldSize)
		{
			if(RequestOk(/*GetCatString(MSG_INFORMATION),GetCatString(MSG_FONTSIZECHANGED)*/"Information","Font size has been changed\nOld size was: %ld, New size is: %ld\nDo you want to recalculate all objects?",GetCatString(MSG_YESNO),oldtf.tf_YSize,EditFontTF->tf_YSize))
				RecalculateGUI(&oldtf,EditFontTF);
			LayoutAllWindows();
		}
	}
	return 1;
}

int FN_PG_PREFS_GEN_LIST_Clicked( void )
{
	ULONG code;
	TEXT executestr[256],buf2[256];

	NameFromLock(GetProgramDir(),executestr,256);
	GetListView(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GEN_LIST],&code);
	sprintf(buf2,"generators/%s",FindNodeNum(GenList,code)->ln_Name);
	AddPart(executestr,buf2,256);
	sprintf(buf2," SETPREFS SEMAPHORE=\"%s\"",GTBSem.gs_Semaphore.ss_Link.ln_Name);
	strcat(executestr,buf2);
	Execute(executestr,NULL,NULL);
	return 1;
}

int FN_PREFS_SAVE_Clicked( void )
{
	FN_PREFS_USE_Clicked();
	SavePrefs("ENV:GadToolsBox/GadToolsBox3.prefs");
	SavePrefs("ENVARC:GadToolsBox/GadToolsBox3.prefs");
	return 0;
}

int FN_PREFS_USE_Clicked( void )
{
	ULONG code;

	GetListView(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GEN_LIST],&code);
	strcpy(GUI.gui_Prefs->pr_Generator,FindNodeNum(GenList,code)->ln_Name);
//	GetCheckMark(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_WRITEICONS],&code);
//	DoFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_WRITEICONS);
	GetCheckMark(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_AUTOBACKUP],&code);
	DoFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_AUTOBACKUP);
	GetCheckMark(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_FASTREDRAW],&code);
	DoFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_FASTREDRAW);
	GetCheckMark(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_SIZEZOOM],&code);
	DoFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_SIZEZOOM);
	
	return 0;
}

int FN_PREFS_CANCEL_Clicked( void )
{
	return 0;
}

int FN_CloseWindow_Preferences( void )
{
	return FN_PREFS_USE_Clicked();
}

void MakeGeneratorsList(struct List *l)
{
	struct FileInfoBlock *fib;
	struct Node *n;
	BPTR lock;

	if(fib=AllocDosObjectTags(DOS_FIB,TAG_DONE))
	{
		if(lock=Lock("generators",ACCESS_READ))
		{
			if(Examine(lock,fib))
			{
				while(ExNext(lock,fib))
				{
					if(fib->fib_DirEntryType<0)
					{
						if(strstr(fib->fib_FileName,".generator"))
						{
							n=MyAllocVec(sizeof(struct Node)+108,MEMF_PUBLIC);
							n->ln_Name=((UBYTE *)n)+(sizeof(struct Node));
							strcpy(n->ln_Name,fib->fib_FileName);
							AddTail(l,n);
						}
					}
				}
			}
			UnLock(lock);
		}
		FreeDosObject(DOS_FIB,fib);
	}
}

void FreeGeneratorsList(struct List *l)
{
	struct Node *n;

	while(n=RemTail(l)) MyFreeVec(n);
}

/*********** Grid *************************************************************/

int FN_GRID_OK_Clicked( void )
{
	GUI.gui_Grid[0]=GetInteger(GridGadgets[GD_GRID_X]);
	GUI.gui_Grid[1]=GetInteger(GridGadgets[GD_GRID_Y]);
	if(GUI.gui_Grid[0]<1) GUI.gui_Grid[0]=2;
	if(GUI.gui_Grid[1]<1) GUI.gui_Grid[1]=2;
	GUI.gui_Prefs->pr_Grid[0]=GUI.gui_Grid[0];
	GUI.gui_Prefs->pr_Grid[1]=GUI.gui_Grid[1];
	switch(ActiveObject)
	{
		case ID_WIND:
			piEraseObject(CurrentWindow);
			break;
		case ID_IMGE:
			break;
	}

	LayoutAllWindows();
	return 0;
}

int FN_GRID_CANCEL_Clicked( void )
{
	return 0;
}

int FN_GRID_X_Clicked( void )
{
	/* routine when gadget "X Size:" is clicked. */
	return 1;
}

int FN_GRID_Y_Clicked( void )
{
	/* routine when gadget "Y Size:" is clicked. */
	return 1;
}

int FN_GRID_SNAP_Clicked( void )
{
	/* routine when gadget "Grid On" is clicked. */
	GUI.gui_Prefs->pr_GridOn=GUI.gui_GridOn=GridMsg.Code;
	return 1;
}

int FN_GRID_SHOW_Clicked( void )
{
	/* routine when gadget "Show Grid" is clicked. */
	GUI.gui_Prefs->pr_GridShow=GUI.gui_GridShow=GridMsg.Code;
	return 1;
}

int FN_CloseWindow_Grid(void)
{
	return FN_GRID_OK_Clicked();
}

/*********** About ************************************************************/

int FN_ABOUT_OK_Clicked( void )
{
	return 0;
}

int FN_ABOUT_TEXT_Clicked( void )
{
	return 1;
}

int FN_CloseWindow_About( void )
{
	return 0;
}

/*********** Lists ************************************************************/

ULONG ListSelected;

int FN_LIST_Clicked(void)
{
	ListSelected=ListMsg.Code;
	return 2;
}

int FN_CloseWindow_List(void)
{
	return 0;
}

/*********** Menus ************************************************************/

int FN_MainProjectNew_Selected( void )
{
	FreeProject();
	ProjectName[0]='\0';
	return 1;
}

extern struct GTBSemaphore GTBSem;

int FN_MainProjectOpen_Selected( void )
{
	if(ProjectChanged)
		if(!RequestOk(GetCatString(MSG_CONFIRMATION),GetCatString(MSG_NOTSAVED),GetCatString(MSG_YESNO),(int)FilePart(ProjectName)))
			return 1;

	if(AslRequestTags(OpenReq,
		ASLFR_PubScreenName,GUI.gui_PubScreenName,
		ASLFR_InitialPattern,"#?.(gtr|gui)",
		ASLFR_RejectIcons,TRUE,
		ASLFR_InitialFile,FilePart(ProjectName),
		ASLFR_TitleText,GetCatString(MSG_SELECTPROJECTFILE),
		TAG_DONE))
	{
		OpenWindow_Working();
		ObtainSemaphore(&GTBSem.gs_Semaphore);
		GTBSem.gs_Window=WorkingWnd;
		GTBSem.gs_Progress=WorkingGadgets[GD_WORK_PROGRESS];
		ReleaseSemaphore(&GTBSem.gs_Semaphore);
		SetWindowTitles(WorkingWnd,GetCatString(MSG_LOADING),(UBYTE *)-1);
		strcpy(ProjectName,OpenReq->fr_Drawer);
		AddPart(ProjectName,OpenReq->fr_File,108);

		SetStatusWindow(0,GetCatString(MSG_LOADING));

		FreeProject();

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
	return 2;
}

int FN_MainProjectSave_Selected( void )
{
	TEXT bakname[256];

	if(ProjectName[0]=='\0') FN_MainProjectSaveAs_Selected();
	else
	{
		OpenWindow_Working();
		ObtainSemaphore(&GTBSem.gs_Semaphore);
		GTBSem.gs_Window=WorkingWnd;
		GTBSem.gs_Progress=WorkingGadgets[GD_WORK_PROGRESS];
		ReleaseSemaphore(&GTBSem.gs_Semaphore);
		SetWindowTitles(WorkingWnd,GetCatString(MSG_SAVING),(UBYTE *)-1);
		GetFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_AUTOBACKUP)
		{
			sprintf(bakname,"%s.bak",ProjectName);
			Rename(ProjectName,bakname);
		}
		SetStatusWindow(0,GetCatString(MSG_SAVING));
		SaveGUI(ProjectName);
		ObtainSemaphore(&GTBSem.gs_Semaphore);
		GTBSem.gs_Window=NULL;
		GTBSem.gs_Progress=NULL;
		ReleaseSemaphore(&GTBSem.gs_Semaphore);
		CloseWindow_Working();
		SetStatusWindow(0,GetCatString(MSG_STATUS_RESSAVED));
	}
	return 1;
}

int Exists(STRPTR fname)
{
	BPTR lock;
	if(lock=Lock(fname,ACCESS_READ))
	{
		UnLock(lock);
		return 1;
	}
	return 0;
}

int FN_MainProjectSaveAs_Selected( void )
{
	TEXT bakname[256];

	if(AslRequestTags(SaveReq,
		ASLFR_PubScreenName,GUI.gui_PubScreenName,
		ASLFR_InitialPattern,"#?.gtr",
		ASLFR_RejectIcons,TRUE,
		ASLFR_DoSaveMode,TRUE,
		ASLFR_InitialFile,FilePart(ProjectName),
		ASLFR_TitleText,GetCatString(MSG_SAVEPROJECTAS),
		TAG_DONE))
	{
		strcpy(ProjectName,SaveReq->fr_Drawer);
		AddPart(ProjectName,SaveReq->fr_File,108);
		if(!strstr(ProjectName,".gtr")) strcat(ProjectName,".gtr");

		if(Exists(ProjectName))
			if(!RequestOk(GetCatString(MSG_CONFIRMATION),GetCatString(MSG_REPLACEFILE),GetCatString(MSG_YESNO),(int)FilePart(ProjectName)))
				return 1;

		OpenWindow_Working();
		ObtainSemaphore(&GTBSem.gs_Semaphore);
		GTBSem.gs_Window=WorkingWnd;
		GTBSem.gs_Progress=WorkingGadgets[GD_WORK_PROGRESS];
		ReleaseSemaphore(&GTBSem.gs_Semaphore);
		SetWindowTitles(WorkingWnd,GetCatString(MSG_SAVING),(UBYTE *)-1);
		GetFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_AUTOBACKUP)
		{
			sprintf(bakname,"%s.bak",ProjectName);
			Rename(ProjectName,bakname);
		}
		SetStatusWindow(0,GetCatString(MSG_SAVING));
		SaveGUI(ProjectName);
		ObtainSemaphore(&GTBSem.gs_Semaphore);
		GTBSem.gs_Window=NULL;
		GTBSem.gs_Progress=NULL;
		ReleaseSemaphore(&GTBSem.gs_Semaphore);
		CloseWindow_Working();
		SetStatusWindow(0,GetCatString(MSG_STATUS_RESSAVED));
	}
	return 1;
}

int FN_MainProjectSaveSource_Selected( void )
{
	TEXT fname[108];
	TEXT executestr[256],buf2[256];
	TEXT bakname[256];
	TEXT baksname[256];
	char *c;

	if(GUI.gui_Prefs->pr_Generator[0])
	{
		if(AslRequestTags(GenReq,
			ASLFR_PubScreenName,GUI.gui_PubScreenName,
			ASLFR_InitialPattern,"(#?.c|#?.h|#?.gtr)",
			ASLFR_RejectIcons,TRUE,
			ASLFR_DoSaveMode,TRUE,
			ASLFR_InitialFile,FilePart(ProjectName),
			ASLFR_TitleText,GetCatString(MSG_ENTERBASENAME),
			TAG_DONE))
		{
			strcpy(fname,GenReq->fr_Drawer);
			AddPart(fname,GenReq->fr_File,108);
			if(c=strstr(fname,".c")) *c='\0';
			if(c=strstr(fname,".h")) *c='\0';
			if(c=strstr(fname,".gtr")) *c='\0';

			NameFromLock(GetProgramDir(),executestr,256);
			sprintf(buf2,"generators/%s",GUI.gui_Prefs->pr_Generator);
			AddPart(executestr,buf2,256);
			sprintf(buf2," FILE=\"%s\" SEMAPHORE=\"%s\"",fname,GTBSem.gs_Semaphore.ss_Link.ln_Name);
			strcat(executestr,buf2);

			strcpy(fname,GenReq->fr_Drawer);
			AddPart(fname,GenReq->fr_File,108);
			if(c=strstr(fname,".c")) *c='\0';
			if(c=strstr(fname,".h")) *c='\0';
			if(c=strstr(fname,".gtr")) *c='\0';
			strcat(fname,".c");

			if(Exists(fname))
				if(!RequestOk(GetCatString(MSG_CONFIRMATION),GetCatString(MSG_REPLACEFILE),GetCatString(MSG_YESNO),(int)FilePart(fname)))
					return 1;

			OpenWindow_Working();
			ObtainSemaphore(&GTBSem.gs_Semaphore);
			GTBSem.gs_Window=WorkingWnd;
			GTBSem.gs_Progress=WorkingGadgets[GD_WORK_PROGRESS];
			GTBSem.gs_FontTF=EditFontTF;
			ReleaseSemaphore(&GTBSem.gs_Semaphore);
			SetWindowTitles(WorkingWnd,GetCatString(MSG_GENERATING),(UBYTE *)-1);
			GetFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_AUTOBACKUP)
			{
				sprintf(baksname,"%s.c",fname);
				sprintf(bakname,"%s.c.bak",fname);
				Rename(baksname,bakname);
				sprintf(baksname,"%s.h",fname);
				sprintf(bakname,"%s.h.bak",fname);
				Rename(baksname,bakname);
			}
			SetStatusWindow(0,GetCatString(MSG_GENERATING));
			Execute(executestr,NULL,NULL);
			ObtainSemaphore(&GTBSem.gs_Semaphore);
			GTBSem.gs_Window=NULL;
			GTBSem.gs_Progress=NULL;
			ReleaseSemaphore(&GTBSem.gs_Semaphore);
			CloseWindow_Working();
			SetStatusWindow(2,GetCatString(MSG_STATUS_RESGENERATED));
		}
	}
	return 1;
}

int FN_MainProjectPreferences_Selected( void )
{
	int run=1,a;
	struct List l;
	struct Node *n;
	struct Node_Screen *ns;

	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);
	NewList(&l);
	MakeGeneratorsList(&l);
	GenList=&l;
	if(!OpenWindow_Preferences())
	{
		GetDisplayInfoData(FindDisplayInfo(GUI.gui_Prefs->pr_DisplayID),(UBYTE *)&NameInfo,sizeof(struct NameInfo),DTAG_NAME,NULL);
		SetText(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_SCREENMODE],NameInfo.Name);
		sprintf(FontName,"%s %ld, %s %ld",GUI.gui_Prefs->pr_FontName,GUI.gui_Prefs->pr_FontSize, ns->ns_FontName, ns->ns_FontSize);
		SetText(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_FONTNAME],FontName);
		SetListView(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GEN_LIST],&l);
		SetCheckMark(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_SIZEZOOM],GUI.gui_Prefs->pr_Flags&PRFLAG_SIZEZOOM);
		SetCheckMark(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_AUTOBACKUP],GUI.gui_Prefs->pr_Flags&PRFLAG_AUTOBACKUP);
		SetCheckMark(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_FASTREDRAW],GUI.gui_Prefs->pr_Flags&PRFLAG_FASTREDRAW);
//		SetCheckMark(PreferencesWnd,PreferencesGadgets[GD_PG_PREFS_GENERAL_WRITEICONS],GUI.gui_Prefs->pr_Flags&PRFLAG_WRITEICONS);
		if(n=FindName(&l,GUI.gui_Prefs->pr_Generator))
		{
			a=GetNumNode(&l,n);
			GT_SetGadgetAttrs(PreferencesGadgets[GD_PG_PREFS_GEN_LIST],PreferencesWnd,NULL,GTLV_Selected,a,GTLV_MakeVisible,a,TAG_DONE);
		}
		while(run)
		{
			Wait(1L<<PreferencesWnd->UserPort->mp_SigBit);
			run=HandleIDCMP_Preferences();
		}
		CloseWindow_Preferences();
	}
	GenList=NULL;
	FreeGeneratorsList(&l);
	return 1;
}

int FN_MainProjectAbout_Selected( void )
{
	int run=1;
	char regtext[2000];

	if(!OpenWindow_About())
	{
		if(CheckRegistration())
			sprintf(regtext,GetCatString(MSG_ABOUT),VER_VERSION,VER_REVISION,VER_AUTHOR,_FakeUserName,_FakeAddress,_FakeSerial);
		else
			sprintf(regtext,GetCatString(MSG_ABOUTUNREG),VER_VERSION,VER_REVISION,VER_AUTHOR);
		SetGadgetAttrs(AboutGadgets[GD_ABOUT_TEXT],AboutWnd,NULL,TEXTFIELD_Text,regtext,TAG_DONE);
		SetAttrs(GTBLOGOImg,SMBM_Recessed,TRUE,SMBM_Border,TRUE,TAG_DONE);
		DrawImageState(AboutWnd->RPort,(struct Image *) GTBLOGOImg, AboutWnd->BorderLeft + AdaptX(8) , AboutWnd->BorderTop + AdaptY(4), IDS_NORMAL, DrawInfo);
		while(run)
		{
			Wait(1L<<AboutWnd->UserPort->mp_SigBit);
			run=HandleIDCMP_About();
		}
		CloseWindow_About();
	}
	return 1;
}

int FN_MainProjectQuit_Selected( void )
{
	if(ProjectChanged)
		if(!RequestOk(GetCatString(MSG_CONFIRMATION),GetCatString(MSG_NOTSAVED),GetCatString(MSG_YESNO),(int)FilePart(ProjectName)))
			return 1;
	return 3;
}

int FN_MainEditUndo_Selected( void )
{
	ULONG a,error=0;
	struct ContextNode *cn;
	struct IFFHandle *iffh;
	piObject *wo;
	BOOL refresh=FALSE;

	switch(LastCommand)
	{
		case CMD_CUT:
			wo=CurrentWindow;
			if(iffh=AllocIFF())
			{
				iffh->iff_Stream=(ULONG)UndoHandle;
				InitIFFasClip(iffh);
				if(!OpenIFF(iffh,IFFF_READ))
				{
					if(error=StopChunk(iffh,ID_GTBX,ID_WIND)) goto skip;
					if(error=StopChunk(iffh,ID_GTBX,ID_FRME)) goto skip;
					if(error=StopChunk(iffh,ID_GTBX,ID_LSTR)) goto skip;
					if(error=StopChunk(iffh,ID_GTBX,ID_MENU)) goto skip;
					if(error=StopChunk(iffh,ID_GTBX,ID_IMGE)) goto skip;
					for(a=0;a<PLUGIN_GADGETS;a++) if(error=StopChunk(iffh,ID_GTBX,piGadgets[a].piID)) goto skip;
					while(1)
					{
						error=ParseIFF(iffh,IFFPARSE_SCAN);
						if(error==IFFERR_EOC) continue;       /* enter next context */
						else if(error) break;
						if(cn=CurrentChunk(iffh))
						{
							if(cn->cn_Type==ID_GTBX)
							{
								switch(cn->cn_ID)
								{
									case ID_WIND:
										refresh=TRUE;
										piReadWindow(iffh,&wo);
										break;
									case ID_FRME:
										piReadFrame(iffh,wo); break;
										break;
									case ID_MENU:
										piReadMenu(iffh); break;
										break;
									case ID_IMGE:
										piReadPointer(iffh); break;
										break;
									case ID_LSTR:
										piReadObject(CurrentString,iffh); break;
										break;
									case BUTTON_ID:
									case CHECKBOX_ID:
									case COLORWHEEL_ID:
									case CYCLE_ID:
									case DATATYPE_ID:
									case GRADIENTSLIDER_ID:
									case INTEGER_ID:
									case LISTVIEW_ID:
									case MX_ID:
									case NUMBER_ID:
									case PAGE_ID:
									case PALETTE_ID:
									case POPUPMENU_ID:
									case PROGRESS_ID:
									case SCROLLER_ID:
									case SLIDER_ID:
									case STRING_ID:
									case TAPEDECK_ID:
									case TEXT_ID:
									case TEXTFIELD_ID:
									case TREEVIEW_ID:
										refresh=TRUE;
										piReadGadget(iffh,wo);
										break;
								}
							}
						}
					}
skip:			CloseIFF(iffh);
				}
				FreeIFF(iffh);
			}
			if(refresh)
			{
				piOpenWindow(wo);
				piRenderObject(wo);
				RefreshProperties();
			}
			break;
	}
	LastCommand=CMD_NONE;
	return 1;
}

int FN_MainEditRedo_Selected( void )
{
	return 1;
}

int FN_MainEditCut_Selected( void )
{
	switch(ActiveObject)
	{
		case ID_WIND:
			switch(GUI.gui_Mode)
			{
				case MODE_TEST:
					break;
				case MODE_GADGET:
				case MODE_FRAME:
					LastCommand=CMD_CUT;
					if(piCutObject(CurrentWindow)==2) CurrentWindow=NULL;
					else CurrentGadget=NULL;
					break;
			}
			break;
		case ID_MENU:
			if(CurrentMenu)
			{
				LastCommand=CMD_CUT;
				if(piCutObject(CurrentMenu)==2) CurrentMenu=NULL;
				else piRenderObject(CurrentMenu);
			}
			break;
		case ID_IMGE:
			if(CurrentImage)
			{
				LastCommand=CMD_CUT;
				piCutObject(CurrentImage);
				CurrentImage=NULL;
			}
			break;
	}
	PropertiesMode=PM_NONE;
	ActiveObject=NULL;
	RefreshProperties();
	UpdateObjectList();
	if(TB_WindowWnd) CloseWindow_TB_Window();
	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	return 2;
}

int FN_MainEditCopy_Selected( void )
{
	switch(ActiveObject)
	{
		case ID_WIND:
			switch(GUI.gui_Mode)
			{
				case MODE_TEST:
					break;
				case MODE_GADGET:
				case MODE_FRAME:
					piCopyObject(CurrentWindow,COPY_CLIP);
					break;
			}
			break;
		case ID_MENU:
			if(CurrentMenu)
				piCopyObject(CurrentMenu,COPY_CLIP);
			break;
		case ID_IMGE:
			if(CurrentImage)
				piCopyObject(CurrentImage,COPY_CLIP);
			break;
	}
	return 1;
}

int FN_MainEditPaste_Selected( void )
{
	ULONG a,error=0;
	struct ContextNode *cn;
	struct IFFHandle *iffh;
	piObject *wo;
	BOOL refresh=FALSE,unknown=TRUE;

	LastCommand=CMD_PASTE;
	wo=CurrentWindow;
	if(!(ClipHandle=OpenClipboard(PRIMARY_CLIP))) return 1;
	if(iffh=AllocIFF())
	{
		iffh->iff_Stream=(ULONG)ClipHandle;
		InitIFFasClip(iffh);
		if(!OpenIFF(iffh,IFFF_READ))
		{
			if(error=StopChunk(iffh,ID_GTBX,ID_WIND)) goto skip;
			if(error=StopChunk(iffh,ID_GTBX,ID_FRME)) goto skip;
			if(error=StopChunk(iffh,ID_GTBX,ID_LSTR)) goto skip;
			if(error=StopChunk(iffh,ID_GTBX,ID_MENU)) goto skip;
			if(error=StopChunk(iffh,ID_GTBX,ID_IMGE)) goto skip;
			for(a=0;a<PLUGIN_GADGETS;a++) if(error=StopChunk(iffh,ID_GTBX,piGadgets[a].piID)) goto skip;
			while(1)
			{
				error=ParseIFF(iffh,IFFPARSE_SCAN);
				if(error==IFFERR_EOC) continue;       /* enter next context */
				else if(error) break;
				if(cn=CurrentChunk(iffh))
				{
					if(cn->cn_Type==ID_GTBX)
					{
						unknown=FALSE;
						switch(cn->cn_ID)
						{
							case ID_WIND:
								refresh=TRUE;
								piReadWindow(iffh,&wo);
								break;
							case ID_FRME:
								piReadFrame(iffh,wo); break;
								break;
							case ID_MENU:
								piReadMenu(iffh); break;
								break;
							case ID_IMGE:
								piReadImage(iffh); break;
								break;
							case ID_LSTR:
								piReadObject(CurrentString,iffh); break;
								break;
							case BUTTON_ID:
							case CHECKBOX_ID:
							case COLORWHEEL_ID:
							case CYCLE_ID:
							case DATATYPE_ID:
							case GRADIENTSLIDER_ID:
							case INTEGER_ID:
							case LISTVIEW_ID:
							case MX_ID:
							case NUMBER_ID:
							case PAGE_ID:
							case PALETTE_ID:
							case POPUPMENU_ID:
							case PROGRESS_ID:
							case SCROLLER_ID:
							case SLIDER_ID:
							case STRING_ID:
							case TAPEDECK_ID:
							case TEXT_ID:
							case TEXTFIELD_ID:
							case TREEVIEW_ID:
								refresh=TRUE;
								piReadGadget(iffh,wo);
								break;
						}
					}
				}
			}
skip:	CloseIFF(iffh);
		}
		FreeIFF(iffh);
	}
	if(unknown)
	{
		// Paste unknown object, currently only picture objects are supported
		Object *dtobj;
		piObject *io;
		char *name;
		struct BitMap *bm,*newbm;
		struct Node_Image *ni;
		struct ColorRegister *cr;

//		printf("Unknown object\n");

		if(dtobj=NewDTObject(0,
			DTA_SourceType,DTST_CLIPBOARD,
			DTA_GroupID,GID_PICTURE,
			OBP_Precision,PRECISION_EXACT,
			PDTA_Screen,Scr,
			PDTA_Remap,FALSE,
			TAG_DONE))
		{
//			printf("Picture datatype found!\n");
			if(io=piNewObject(piImageClass,TAG_DONE))
			{
				piAddObject(io,NULL);
				piGetAttr(io,OBJ_Node,(ULONG *)&ni);
				GetDTAttrs(dtobj,
					DTA_ObjName,&name,
					PDTA_BitMap,&bm,
					PDTA_ColorRegisters,&cr,
					TAG_DONE);
				strncpy(ni->ni_Label,name,40);
				if(newbm=AllocBitMap(GetBitMapAttr(bm,BMA_WIDTH),GetBitMapAttr(bm,BMA_HEIGHT),GetBitMapAttr(bm,BMA_DEPTH),BMF_CLEAR,bm))
				{
					ni->ni_Width=GetBitMapAttr(bm,BMA_WIDTH);
					ni->ni_Height=GetBitMapAttr(bm,BMA_HEIGHT);
					ni->ni_Depth=GetBitMapAttr(bm,BMA_DEPTH);
//					printf("Bitmap allocated: %dx%dx%d\n",ni->ni_Width,ni->ni_Height,ni->ni_Depth);
					BltBitMap(bm,0,0,newbm,0,0,ni->ni_Width,ni->ni_Height,0xc0,0xff,NULL);
					if(ni->ni_BitMap) FreeBitMap(ni->ni_BitMap);
					ni->ni_BitMap=newbm;
					for(a=0;a<(2<<ni->ni_Depth);a++)
					{
						ni->ni_Pens[a]=ni->ni_CT[a].cp_I=-1;
						ni->ni_CT[a].cp_R=Bit32(cr[a].red);
						ni->ni_CT[a].cp_G=Bit32(cr[a].green);
						ni->ni_CT[a].cp_B=Bit32(cr[a].blue);
					}
				}
			}
			DisposeDTObject(dtobj);
		}
	}
	if(ClipHandle) CloseClipboard(ClipHandle); ClipHandle=NULL;
	if(refresh)
	{
		piOpenWindow(wo);
		piRenderObject(wo);
	}
	UpdateObjectList();
	RefreshObjectList();
	return 1;
}

int FN_MainEditClear_Selected( void )
{
	piObject *go,*fo,*o;
	struct Node_Gadget *ng;
	struct Node_Frame *nf;
	struct Node_Window *nw;
	BOOL s=FALSE;

	switch(ActiveObject)
	{
		case ID_WIND:
			o=CurrentWindow;
			piGetAttr(o,OBJ_Node,(ULONG *)&nw);
			switch(GUI.gui_Mode)
			{
				case MODE_TEST:
					break;
				case MODE_GADGET:
					for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
					{
						piGetAttr(go,OBJ_Node,(ULONG *)&ng);
						if(ng->ng_Selected) { s=TRUE; break; }
					}
					if(s)
					{
repg:				for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
						{
							piGetAttr(go,OBJ_Node,(ULONG *)&ng);
							if(ng->ng_Selected)
							{
								piRemoveObject(go);
								piDisposeObject(go);
								goto repg;
							}
						}
						piEraseObject(o);
						piRenderObject(o);
						PropertiesMode=PM_NONE;
						RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
						return 1;
					}
					else FN_MainObjWindowDelete_Selected();
					break;
				case MODE_FRAME:
					for(fo=(piObject *)GetHead(List(&nw->nw_List_Frame));GetSucc(fo);fo=(piObject *)GetSucc(fo))
					{
						piGetAttr(fo,OBJ_Node,(ULONG *)&nf);
						if(nf->nf_Selected) { s=TRUE; break; }
					}
					if(s)
					{
repf:				for(fo=(piObject *)GetHead(List(&nw->nw_List_Frame));GetSucc(fo);fo=(piObject *)GetSucc(fo))
						{
							piGetAttr(fo,OBJ_Node,(ULONG *)&nf);
							if(nf->nf_Selected)
							{
								piRemoveObject(fo);
								piDisposeObject(fo);
								goto repg;
							}
						}
						piEraseObject(o);
						piRenderObject(o);
						PropertiesMode=PM_NONE;
						RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
						return 1;
					}
					else FN_MainObjWindowDelete_Selected();
					break;
			}
			break;
		case ID_MENU:
			FN_MainObjMenuDelete_Selected();
			break;
		case ID_IMGE:
			FN_MainObjImageDelete_Selected();
			break;
	}
	PropertiesMode=PM_NONE;
	ActiveObject=NULL;
	RefreshProperties();
	UpdateObjectList();
	return 2;
}

int FN_MainEditDuplicate_Selected( void )
{
	return 1;
}

int FN_MainEditSelectAll_Selected( void )
{
	piObject *o;

	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			for(o=(piObject *)GetHead(&CurrentWindowNode->nw_List_Gadget);GetSucc(o);o=(piObject *)GetSucc(o)) piSetAttrs(o,OBJ_Select,TRUE,TAG_DONE);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			for(o=(piObject *)GetHead(&CurrentWindowNode->nw_List_Frame);GetSucc(o);o=(piObject *)GetSucc(o)) piSetAttrs(o,OBJ_Select,TRUE,TAG_DONE);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainEditGrid_Selected( void )
{
	BOOL ret=TRUE;

	unless(OpenWindow_Grid())
	{
		SetInteger(GridWnd,GridGadgets[GD_GRID_X],GUI.gui_Grid[0]);
		SetInteger(GridWnd,GridGadgets[GD_GRID_Y],GUI.gui_Grid[1]);
		SetCheckMark(GridWnd,GridGadgets[GD_GRID_SNAP],GUI.gui_GridOn);
		SetCheckMark(GridWnd,GridGadgets[GD_GRID_SHOW],GUI.gui_GridShow);
		while(ret)
		{
			Wait(1L<<GridWnd->UserPort->mp_SigBit);
			ret=HandleIDCMP_Grid();
		}
		CloseWindow_Grid();
	}
	return 1;
}

int FN_MainEditAlignToLeft_Selected( void )
{
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			AlignObjects(&CurrentWindowNode->nw_List_Gadget,ALIGN_LEFT);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			AlignObjects(&CurrentWindowNode->nw_List_Frame,ALIGN_LEFT);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainEditAlignToHCenter_Selected( void )
{
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			AlignObjects(&CurrentWindowNode->nw_List_Gadget,ALIGN_HCENTER);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			AlignObjects(&CurrentWindowNode->nw_List_Frame,ALIGN_HCENTER);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainEditAlignToRight_Selected( void )
{
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			AlignObjects(&CurrentWindowNode->nw_List_Gadget,ALIGN_RIGHT);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			AlignObjects(&CurrentWindowNode->nw_List_Frame,ALIGN_RIGHT);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainEditAlignToTop_Selected( void )
{
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			AlignObjects(&CurrentWindowNode->nw_List_Gadget,ALIGN_TOP);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			AlignObjects(&CurrentWindowNode->nw_List_Frame,ALIGN_TOP);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainEditAlignToVCenter_Selected( void )
{
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			AlignObjects(&CurrentWindowNode->nw_List_Gadget,ALIGN_VCENTER);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			AlignObjects(&CurrentWindowNode->nw_List_Frame,ALIGN_VCENTER);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainEditAlignToBottom_Selected( void )
{
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			AlignObjects(&CurrentWindowNode->nw_List_Gadget,ALIGN_BOTTOM);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			AlignObjects(&CurrentWindowNode->nw_List_Frame,ALIGN_BOTTOM);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainEditCloneWidth_Selected( void )
{
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			AlignObjects(&CurrentWindowNode->nw_List_Gadget,CLONE_WIDTH);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			AlignObjects(&CurrentWindowNode->nw_List_Frame,CLONE_WIDTH);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainEditCloneHeight_Selected( void )
{
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			AlignObjects(&CurrentWindowNode->nw_List_Gadget,CLONE_HEIGHT);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			AlignObjects(&CurrentWindowNode->nw_List_Frame,CLONE_HEIGHT);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainEditCloneBoth_Selected( void )
{
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			AlignObjects(&CurrentWindowNode->nw_List_Gadget,CLONE_BOTH);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			AlignObjects(&CurrentWindowNode->nw_List_Frame,CLONE_BOTH);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainEditSpreadHoriz_Selected( void )
{
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			AlignObjects(&CurrentWindowNode->nw_List_Gadget,SPREAD_HORIZ);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			AlignObjects(&CurrentWindowNode->nw_List_Frame,SPREAD_HORIZ);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainEditSpreadVertical_Selected( void )
{
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			AlignObjects(&CurrentWindowNode->nw_List_Gadget,SPREAD_VERT);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
		case MODE_FRAME:
			AlignObjects(&CurrentWindowNode->nw_List_Frame,SPREAD_VERT);
			piEraseObject(CurrentWindow);
			piRenderObject(CurrentWindow);
			break;
	}
	return 1;
}

int FN_MainObjTypeTest_Selected( void )
{
	piObject *wo;
	struct Node_Window *nw;

	PropertiesMode=PM_NONE;
	RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
	SetWndSignals();
	if(GUI.gui_Mode!=MODE_TEST)
	{
		GUI.gui_Mode=MODE_TEST;
		if(!ListEmpty(&GUI.gui_List_Window))
		{
			for(wo=(piObject *)GetHead(&GUI.gui_List_Window);GetSucc(wo);wo=(piObject *)GetSucc(wo))
			{
				piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
				if(nw->nw_Window)
				{
					piCloseWindow(wo);
					piOpenWindow(wo);
				}
			}
		}
	}
	return 2;
}

int FN_MainObjTypeGadget_Selected( void )
{
	piObject *wo;
	struct Node_Window *nw;

	PropertiesMode=PM_NONE;
	RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
	SetWndSignals();
	if(GUI.gui_Mode==MODE_TEST)
	{
		GUI.gui_Mode=MODE_GADGET;
		if(!ListEmpty(&GUI.gui_List_Window))
		{
			for(wo=(piObject *)GetHead(&GUI.gui_List_Window);GetSucc(wo);wo=(piObject *)GetSucc(wo))
			{
				piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
				if(nw->nw_Window)
				{
					piCloseWindow(wo);
					piOpenWindow(wo);
				}
			}
		}
	}

	GUI.gui_Mode=MODE_GADGET;
	return 2;
}

int FN_MainObjTypeFrame_Selected( void )
{
	piObject *wo;
	struct Node_Window *nw;

	PropertiesMode=PM_NONE;
	RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
	SetWndSignals();
	if(GUI.gui_Mode==MODE_TEST)
	{
		GUI.gui_Mode=MODE_FRAME;
		if(!ListEmpty(&GUI.gui_List_Window))
		{
			for(wo=(piObject *)GetHead(&GUI.gui_List_Window);GetSucc(wo);wo=(piObject *)GetSucc(wo))
			{
				piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
				if(nw->nw_Window)
				{
					piCloseWindow(wo);
					piOpenWindow(wo);
				}
			}
		}
	}

	GUI.gui_Mode=MODE_FRAME;
	return 2;
}

int FN_MainObjWindowNew_Selected( void )
{
	/* routine when (sub)item "New..." is selected. */
	piObject *tmp;

	if(tmp=piNewObject(piWindowClass,TAG_DONE))
	{
		CurrentWindow=tmp;
		ActiveObject=ID_WIND;
		PropertiesMode=PM_WINDOW;
		piAddObject(CurrentWindow,NULL);
		piOpenWindow(CurrentWindow);
		if(!TB_WindowWnd)
		{
			OpenWindow_TB_Window();
			ApplySelectionWindow();
		}
		SetWndSignals();
		ApplySelectionWindow();
		UpdateObjectList();
		RefreshObjectList();
	}
	return 1;
}

int FN_MainObjWindowClose_Selected( void )
{
	/* routine when (sub)item "Delete..." is selected. */
	if(CurrentWindow)
	{
		piCloseWindow(CurrentWindow);
		PropertiesMode=PM_NONE;
		ActiveObject=NULL;
		CurrentWindow=NULL;
		RefreshProperties();
	}
	if(TB_WindowWnd) CloseWindow_TB_Window();
	return 2;
}

int FN_MainObjWindowDelete_Selected( void )
{
	/* routine when (sub)item "Delete..." is selected. */
	if(CurrentWindow)
	{
		piRemoveObject(CurrentWindow);
		piDisposeObject(CurrentWindow);
		PropertiesMode=PM_NONE;
		ActiveObject=NULL;
		CurrentWindow=NULL;
		RefreshProperties();
		UpdateObjectList();
	}
	if(TB_WindowWnd) CloseWindow_TB_Window();
	return 2;
}

int FN_MainObjWindowRefresh_Selected( void )
{
	/* routine when (sub)item "Refresh" is selected. */
	piEraseObject(CurrentWindow);
	piRenderObject(CurrentWindow);
	RefreshWindowFrame(CurrentWindowNode->nw_Window);
	return 1;
}

int FN_MainObjMenuNew_Selected( void )
{
	/* routine when (sub)item "New..." is selected. */
	piObject *tmp;

	if(tmp=piNewObject(piMenuClass,TAG_DONE))
	{
		CurrentMenu=tmp;
		ActiveObject=ID_MENU;
		PropertiesMode=PM_MENU;
		piAddObject(CurrentMenu,NULL);
		piOpenWindow(CurrentMenu);
		UpdateObjectList();
		RefreshObjectList();
	}
	return 1;
}

int FN_MainObjMenuClose_Selected( void )
{
	/* routine when (sub)item "Delete..." is selected. */
	if(CurrentMenu)
	{
		piCloseWindow(CurrentMenu);
		PropertiesMode=PM_NONE;
		ActiveObject=NULL;
		CurrentMenu=NULL;
		RefreshProperties();
	}
	return 2;
}

int FN_MainObjMenuDelete_Selected( void )
{
	/* routine when (sub)item "Delete..." is selected. */
	if(CurrentMenu)
	{
		piRemoveObject(CurrentMenu);
		piDisposeObject(CurrentMenu);
		PropertiesMode=PM_NONE;
		ActiveObject=NULL;
		CurrentMenu=NULL;
		RefreshProperties();
		UpdateObjectList();
	}
	return 2;
}

int FN_MainObjImageNew_Selected( void )
{
	/* routine when (sub)item "New..." is selected. */
	piObject *tmp;

	if(tmp=piNewObject(piImageClass,TAG_DONE))
	{
		CurrentImage=tmp;
		ActiveObject=ID_IMGE;
		PropertiesMode=PM_IMAGE;
		piAddObject(CurrentImage,NULL);
		piOpenWindow(CurrentImage);
		FN_ActiveWindow_EditImage();
		UpdateObjectList();
	}
	return 1;
}

int FN_MainObjImageClose_Selected( void )
{
	/* routine when (sub)item "Delete..." is selected. */
	if(CurrentImage)
	{
		piCloseWindow(CurrentImage);
		PropertiesMode=PM_NONE;
		ActiveObject=NULL;
		CurrentImage=NULL;
		RefreshProperties();
	}
	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	return 2;
}

int FN_MainObjImageDelete_Selected( void )
{
	/* routine when (sub)item "Delete..." is selected. */
	if(CurrentImage)
	{
		piRemoveObject(CurrentImage);
		piDisposeObject(CurrentImage);
		PropertiesMode=PM_NONE;
		ActiveObject=NULL;
		CurrentImage=NULL;
		RefreshProperties();
		UpdateObjectList();
	}
	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	return 2;
}

int FN_MainObjVersionOpen_Selected( void )
{
	piOpenWindow(CurrentVersion);
	ActiveObject=ID_SVER;
	PropertiesMode=PM_NONE;
	RefreshProperties();
	return 1;
}

int FN_MainObjVersionClose_Selected( void )
{
	piCloseWindow(CurrentVersion);
	RefreshProperties();
	return 2;
}

int FN_MainObjScreenOpen_Selected( void )
{
	piOpenWindow(CurrentScreen);
	ActiveObject=ID_SCRN;
	PropertiesMode=PM_NONE;
	RefreshProperties();
	return 1;
}

int FN_MainObjScreenClose_Selected( void )
{
	piCloseWindow(CurrentScreen);
	RefreshProperties();
	return 2;
}

int FN_MainObjStringsOpen_Selected( void )
{
	piOpenWindow(CurrentString);
	ActiveObject=ID_LSTR;
	PropertiesMode=PM_STRING;
	RefreshProperties();
	return 1;
}

int FN_MainObjStringsClose_Selected( void )
{
	piCloseWindow(CurrentString);
	PropertiesMode=PM_NONE;
	RefreshProperties();
	return 2;
}

int FN_MainWindowProperties_Selected( void )
{
	if(!PropertiesWnd) OpenWindow_Props();
	else CloseWindow_Props();
	return 2;
}

int FN_MainWindowObjects_Selected( void )
{
	if(!ObjectWnd) OpenObjectList();
	else CloseObjectList();
	return 2;
}

int FN_MainWindowStatus_Selected( void )
{
	if(!StatusWnd) OpenStatusWindow();
	else CloseStatusWindow();
	return 2;
}

int FN_MainWindowHideAll_Selected( void )
{
	DeleteAllWindows();
	DeleteAllMenus();
	DeleteAllImages();
	FN_CloseWindow_Screen();
	FN_CloseWindow_EditVersion();
	FN_CloseWindow_EditString();
	PropertiesMode=PM_NONE;
	CurrentWindow=NULL;
	CurrentMenu=NULL;
	CurrentImage=NULL;
	ActiveObject=NULL;
	if(TB_WindowWnd) CloseWindow_TB_Window();
	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	RefreshProperties();
	return 2;
}

int FN_CloseWindow_Edit( void )
{
	return FN_MainObjWindowClose_Selected();
}

int FN_NewSize_Edit( void )
{
	/* routine for "IDCMP_NEWSIZE". */
	ULONG active;
	struct Window *PropsWnd;

	if(GUI.gui_Mode!=MODE_TEST)
	{
		GetFlag(CurrentWindowNode->nw_MyFlags,MYWA_InnerWidth)
			CurrentWindowNode->nw_Width=CurrentWindowNode->nw_Window->Width-CurrentWindowNode->nw_Window->BorderLeft-CurrentWindowNode->nw_Window->BorderRight;
		else
			CurrentWindowNode->nw_Width=CurrentWindowNode->nw_Window->Width;
		GetFlag(CurrentWindowNode->nw_MyFlags,MYWA_InnerHeight)
			CurrentWindowNode->nw_Height=CurrentWindowNode->nw_Window->Height-CurrentWindowNode->nw_Window->BorderTop-CurrentWindowNode->nw_Window->BorderBottom;
		else
			CurrentWindowNode->nw_Height=CurrentWindowNode->nw_Window->Height;

		switch(CurrentWindowNode->nw_XPos)
		{
			case WPOS_REL_LEFT_TOP:
				CurrentWindowNode->nw_X=CurrentWindowNode->nw_Window->LeftEdge;
				break;
			case WPOS_REL_RIGHT_BOTTOM:
				CurrentWindowNode->nw_X=(Scr->Width-CurrentWindowNode->nw_Window->LeftEdge-WinWidth(CurrentWindow));
				break;
			case WPOS_CENTER:
				CurrentWindowNode->nw_X=Scr->Width/2-WinWidth(CurrentWindow)/2;
				break;
		}		
		switch(CurrentWindowNode->nw_YPos)
		{
			case WPOS_REL_LEFT_TOP:
				CurrentWindowNode->nw_Y=CurrentWindowNode->nw_Window->TopEdge;
				break;
			case WPOS_REL_RIGHT_BOTTOM:
				CurrentWindowNode->nw_Y=(Scr->Height-CurrentWindowNode->nw_Window->TopEdge-WinHeight(CurrentWindow));
				break;
			case WPOS_CENTER:
				CurrentWindowNode->nw_Y=Scr->Height/2-WinHeight(CurrentWindow)/2;
				break;
		}		

		piRenderObject(CurrentWindow);
		if(PropertiesWnd)
		{
			GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

			if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
			SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_X],CurrentWindowNode->nw_X);
			SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_Y],CurrentWindowNode->nw_Y);
			SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_WIDTH],CurrentWindowNode->nw_Width);
			SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_HEIGHT],CurrentWindowNode->nw_Height);
		}
		SetStatusWindow(1,(STRPTR)-1);
//		RefreshPropertiesWindow((UBYTE *)-1);
	}
	return 1;
}

int FN_ChangeWindow_Edit( void )
{
	/* routine for "IDCMP_CHANGEWINDOW". */
	ULONG active;
	struct Window *PropsWnd;

	if(GUI.gui_Mode!=MODE_TEST)
	{
		GetFlag(CurrentWindowNode->nw_MyFlags,MYWA_InnerWidth)
			CurrentWindowNode->nw_Width=CurrentWindowNode->nw_Window->Width-CurrentWindowNode->nw_Window->BorderLeft-CurrentWindowNode->nw_Window->BorderRight;
		else
			CurrentWindowNode->nw_Width=CurrentWindowNode->nw_Window->Width;
		GetFlag(CurrentWindowNode->nw_MyFlags,MYWA_InnerHeight)
			CurrentWindowNode->nw_Height=CurrentWindowNode->nw_Window->Height-CurrentWindowNode->nw_Window->BorderTop-CurrentWindowNode->nw_Window->BorderBottom;
		else
			CurrentWindowNode->nw_Height=CurrentWindowNode->nw_Window->Height;

		switch(CurrentWindowNode->nw_XPos)
		{
			case WPOS_REL_LEFT_TOP:
				CurrentWindowNode->nw_X=CurrentWindowNode->nw_Window->LeftEdge;
				break;
			case WPOS_REL_RIGHT_BOTTOM:
				CurrentWindowNode->nw_X=(Scr->Width-CurrentWindowNode->nw_Window->LeftEdge-WinWidth(CurrentWindow));
				break;
			case WPOS_CENTER:
				CurrentWindowNode->nw_X=Scr->Width/2-WinWidth(CurrentWindow)/2;
				break;
		}		
		switch(CurrentWindowNode->nw_YPos)
		{
			case WPOS_REL_LEFT_TOP:
				CurrentWindowNode->nw_Y=CurrentWindowNode->nw_Window->TopEdge;
				break;
			case WPOS_REL_RIGHT_BOTTOM:
				CurrentWindowNode->nw_Y=(Scr->Height-CurrentWindowNode->nw_Window->TopEdge-WinHeight(CurrentWindow));
				break;
			case WPOS_CENTER:
				CurrentWindowNode->nw_Y=Scr->Height/2-WinHeight(CurrentWindow)/2;
				break;
		}		
		if(PropertiesWnd)
		{
			GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

			if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
			SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_X],CurrentWindowNode->nw_X);
			SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_Y],CurrentWindowNode->nw_Y);
			SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_WIDTH],CurrentWindowNode->nw_Width);
			SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_HEIGHT],CurrentWindowNode->nw_Height);
		}
		SetStatusWindow(1,(STRPTR)-1);
	}
	return 1;
}

int prevsel=-1;

#define IDMN_EDIT 1
#define IDMN_CUT 2
#define IDMN_COPY 3
#define IDMN_ALIGN 10
#define IDMN_CLONE 11
#define IDMN_SPREAD 12

#define IDMN_OBJECT 2
#define IDMN_WINDOW 0
#define IDMN_MENU 1
#define IDMN_IMAGE 2
#define IDMN_STRING 3
#define IDMN_SCREEN 4
#define IDMN_VERSION 5

#define IDMN_NEW 0
#define IDMN_CLOSE 1
#define IDMN_DELETE 2
#define IDMN_REFRESH 4

void RefreshProperties(void)
{
	int sel;

	switch(ActiveObject)
	{
		case ID_WIND:
			if(CurrentWindowNode->nw_Window) PropertiesMode=PM_WINDOW;
			switch(GUI.gui_Mode)
			{
				case MODE_GADGET:
					if(CurrentWindowNode->nw_Window) sel=HowManySelected(&CurrentWindowNode->nw_List_Gadget);
					else sel=0;
					if(sel==1) PropertiesMode=PM_GADGET;
					if(sel>1) PropertiesMode=PM_NONE;
					if(sel==0)
					{
						if(CurrentWindowNode->nw_Window) PropertiesMode=PM_WINDOW;
						RefreshPropertiesWindow(GetCatString(MSG_WINDOWOBJECT));
					}
					else RefreshPropertiesWindow(GetCatString(MSG_GADGETOBJECT));
		 			break;
				case MODE_FRAME:
					sel=HowManySelected(&CurrentWindowNode->nw_List_Frame);
					if(sel==1) PropertiesMode=PM_FRAME;
					if(sel>1) PropertiesMode=PM_NONE;
					RefreshPropertiesWindow(GetCatString(MSG_FRAMEOBJECT));
		 			break;
				case MODE_TEST:
					PropertiesMode=PM_NONE;
					sel=0;
					RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
					break;
			}
			if(sel!=prevsel)
			{
				OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_CUT,-1));
				OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_COPY,-1));
				DisableGadget(TB_MainGadgets[GD_TB_CUT],TB_MainWnd,0);
				DisableGadget(TB_MainGadgets[GD_TB_COPY],TB_MainWnd,0);

				if(sel>1)
				{
					OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_ALIGN,-1));
					OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_CLONE,-1));
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNLEFT],TB_MainWnd,0);
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNHCENTER],TB_MainWnd,0);
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNRIGHT],TB_MainWnd,0);
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNTOP],TB_MainWnd,0);
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNVCENTER],TB_MainWnd,0);
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNBOTTOM],TB_MainWnd,0);
					DisableGadget(TB_MainGadgets[GD_TB_CLONEWIDTH],TB_MainWnd,0);
					DisableGadget(TB_MainGadgets[GD_TB_CLONEHEIGHT],TB_MainWnd,0);
					DisableGadget(TB_MainGadgets[GD_TB_CLONEBOTH],TB_MainWnd,0);
				}
				else
				{
					OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_ALIGN,-1));
					OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_CLONE,-1));
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNLEFT],TB_MainWnd,1);
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNHCENTER],TB_MainWnd,1);
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNRIGHT],TB_MainWnd,1);
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNTOP],TB_MainWnd,1);
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNVCENTER],TB_MainWnd,1);
					DisableGadget(TB_MainGadgets[GD_TB_ALIGNBOTTOM],TB_MainWnd,1);
					DisableGadget(TB_MainGadgets[GD_TB_CLONEWIDTH],TB_MainWnd,1);
					DisableGadget(TB_MainGadgets[GD_TB_CLONEHEIGHT],TB_MainWnd,1);
					DisableGadget(TB_MainGadgets[GD_TB_CLONEBOTH],TB_MainWnd,1);
				}
				if(sel>2)
				{
					OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_SPREAD,-1));
					DisableGadget(TB_MainGadgets[GD_TB_SPREADHORIZ],TB_MainWnd,0);
					DisableGadget(TB_MainGadgets[GD_TB_SPREADVERT],TB_MainWnd,0);
				}
				else
				{
					OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_SPREAD,-1));
					DisableGadget(TB_MainGadgets[GD_TB_SPREADHORIZ],TB_MainWnd,1);
					DisableGadget(TB_MainGadgets[GD_TB_SPREADVERT],TB_MainWnd,1);
				}
			}
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_CLOSE));
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_DELETE));
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_REFRESH));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_MENU,IDMN_CLOSE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_MENU,IDMN_DELETE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_IMAGE,IDMN_CLOSE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_IMAGE,IDMN_DELETE));
			prevsel=sel;
			break;
		case ID_MENU:
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_CUT,-1));
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_COPY,-1));
			DisableGadget(TB_MainGadgets[GD_TB_CUT],TB_MainWnd,0);
			DisableGadget(TB_MainGadgets[GD_TB_COPY],TB_MainWnd,0);
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_ALIGN,-1));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_CLONE,-1));
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNLEFT],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNHCENTER],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNRIGHT],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNTOP],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNVCENTER],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNBOTTOM],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_CLONEWIDTH],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_CLONEHEIGHT],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_CLONEBOTH],TB_MainWnd,1);
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_SPREAD,-1));
			DisableGadget(TB_MainGadgets[GD_TB_SPREADHORIZ],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_SPREADVERT],TB_MainWnd,1);
			prevsel=-1;
			RefreshPropertiesWindow(GetCatString(MSG_MENUOBJECT));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_CLOSE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_DELETE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_REFRESH));
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_MENU,IDMN_CLOSE));
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_MENU,IDMN_DELETE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_IMAGE,IDMN_CLOSE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_IMAGE,IDMN_DELETE));
			break;
		case ID_IMGE:
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_CUT,-1));
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_COPY,-1));
			DisableGadget(TB_MainGadgets[GD_TB_CUT],TB_MainWnd,0);
			DisableGadget(TB_MainGadgets[GD_TB_COPY],TB_MainWnd,0);
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_ALIGN,-1));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_CLONE,-1));
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNLEFT],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNHCENTER],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNRIGHT],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNTOP],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNVCENTER],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNBOTTOM],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_CLONEWIDTH],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_CLONEHEIGHT],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_CLONEBOTH],TB_MainWnd,1);
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_SPREAD,-1));
			DisableGadget(TB_MainGadgets[GD_TB_SPREADHORIZ],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_SPREADVERT],TB_MainWnd,1);
			prevsel=-1;
			RefreshPropertiesWindow(GetCatString(MSG_IMAGEOBJECT));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_CLOSE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_DELETE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_REFRESH));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_MENU,IDMN_CLOSE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_MENU,IDMN_DELETE));
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_IMAGE,IDMN_CLOSE));
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_IMAGE,IDMN_DELETE));
			break;
		default:
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_CUT,-1));
			OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_COPY,-1));
			DisableGadget(TB_MainGadgets[GD_TB_CUT],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_COPY],TB_MainWnd,1);
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_ALIGN,-1));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_CLONE,-1));
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNLEFT],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNHCENTER],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNRIGHT],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNTOP],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNVCENTER],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_ALIGNBOTTOM],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_CLONEWIDTH],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_CLONEHEIGHT],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_CLONEBOTH],TB_MainWnd,1);
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_EDIT,IDMN_SPREAD,-1));
			DisableGadget(TB_MainGadgets[GD_TB_SPREADHORIZ],TB_MainWnd,1);
			DisableGadget(TB_MainGadgets[GD_TB_SPREADVERT],TB_MainWnd,1);
			prevsel=-1;
			RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_CLOSE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_DELETE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_WINDOW,IDMN_REFRESH));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_MENU,IDMN_CLOSE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_MENU,IDMN_DELETE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_IMAGE,IDMN_CLOSE));
			OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_IMAGE,IDMN_DELETE));
			break;
	}
	if(EditStringWnd) OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_STRING,IDMN_CLOSE));
	else OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_STRING,IDMN_CLOSE));
	if(ScreenWnd) OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_SCREEN,IDMN_CLOSE));
	else OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_SCREEN,IDMN_CLOSE));
	if(EditVersionWnd) OnMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_VERSION,IDMN_CLOSE));
	else OffMenu(TB_MainWnd,FULLMENUNUM(IDMN_OBJECT,IDMN_VERSION,IDMN_CLOSE));
	MenuCheck(MainMenus,FULLMENUNUM(3,0,0),PropertiesWnd?TRUE:FALSE);
	MenuCheck(MainMenus,FULLMENUNUM(3,1,0),ObjectWnd?TRUE:FALSE);
	MenuCheck(MainMenus,FULLMENUNUM(3,2,0),StatusWnd?TRUE:FALSE);
}

ULONG SSec=0,SMic=0,ESec=0,EMic=0;
UBYTE GadgetStatus=GS_NOTHING;
WORD StartX,StartY,EndX=(UWORD)-1,EndY=(UWORD)-1,DeltaX,DeltaY,px,py,MoveX,MoveY;
WORD SMMoveX=0,SMMoveY=0,EMMoveX=0,EMMoveY=0;
WORD MinMoveX,MinMoveY,MaxMoveX,MaxMoveY;
WORD ObjX,ObjY,ObjW,ObjH;
BOOL First,Shifted=FALSE;

int FN_MouseButtons_Edit( void )
{
	/* routine for "IDCMP_MOUSEBUTTONS". */
	piObject *o;
	struct Node_Gadget *ng;
	struct Node_Frame *nf;
	int (*func)();
	BOOL running=TRUE,objsel;

	switch(EditMsg.Code)
	{
		case MENUDOWN:
			SetWindowPointer(CurrentWindowNode->nw_Window,TAG_DONE);
			objsel=FALSE;
			switch(GUI.gui_Mode)
			{
				case MODE_GADGET:
					if(o=CurrentGadget=piFindGadget(CurrentWindow,EditMsg.MouseX,EditMsg.MouseY))
					{
						if(HowManySelected(&CurrentWindowNode->nw_List_Gadget)>1)
							func=(void *) PM_OpenPopupMenu(CurrentWindowNode->nw_Window,PM_Menu,PM_ObjSelGadget,PM_Code, EditMsg.Code, TAG_DONE);
						else
							func=(void *) PM_OpenPopupMenu(CurrentWindowNode->nw_Window,PM_Menu,PM_ObjGadget,PM_Code, EditMsg.Code, TAG_DONE);

						if(func)
						{
							piGetAttr(o,OBJ_Node,(ULONG *)&ng);
							if(!ng->ng_Selected)
							{
								objsel=TRUE; ng->ng_Selected=TRUE;
								PropertiesMode=PM_GADGET;
							}
							running = func();
							if(objsel) { ng->ng_Selected=FALSE; }
							if(running > 1) return running;
						}
					}
					else
					{
						PropertiesMode=PM_WINDOW;
						func=(void *) PM_OpenPopupMenu(CurrentWindowNode->nw_Window,PM_Menu,PM_ObjWindow,PM_Code, EditMsg.Code, TAG_DONE);
						if(func)
						{
							running = func();
							if(running > 1) return running;
						}
					}
					break;
				case MODE_FRAME:
					if(o=CurrentFrame=piFindFrame(CurrentWindow,EditMsg.MouseX,EditMsg.MouseY))
					{
						func=(void *) PM_OpenPopupMenu(CurrentWindowNode->nw_Window,PM_Menu,PM_ObjGadget,PM_Code, EditMsg.Code, TAG_DONE);
						if(func)
						{
							piGetAttr(o,OBJ_Node,(ULONG *)&nf);
							if(!nf->nf_Selected)
							{
								objsel=TRUE; nf->nf_Selected=TRUE;
								PropertiesMode=PM_FRAME;
							}
							running = func();
							if(objsel) { nf->nf_Selected=FALSE; }
							if(running > 1) return running;
						}
					}
					else
					{
						PropertiesMode=PM_WINDOW;
						func=(void *) PM_OpenPopupMenu(CurrentWindowNode->nw_Window,PM_Menu,PM_ObjWindow,PM_Code, EditMsg.Code, TAG_DONE);
						if(func)
						{
							running = func();
							if(running > 1) return running;
						}
					}
					break;
			}
			break;
		case MENUUP:
			break;
		case SELECTDOWN:
			if(GUI.gui_Mode!=MODE_TEST)
			{
				CurrentTime(&ESec,&EMic);
				EMMoveX=EditMsg.MouseX;
				EMMoveY=EditMsg.MouseY;
				if(DoubleClick(SSec,SMic,ESec,EMic) && abs(SMMoveX-EMMoveX)<6 && abs(SMMoveY-EMMoveY)<6)
				{
					CurrentTime(&SSec,&SMic);
					if(!PropertiesWnd) OpenWindow_Props();
					goto cancel;
				}
				CurrentTime(&SSec,&SMic);
				SMMoveX=EMMoveX;
				SMMoveY=EMMoveY;
				
				PropertiesMode=PM_WINDOW;
				switch(GUI.gui_Mode)
				{
					case MODE_GADGET:
						if(o=CurrentGadget=piFindGadget(CurrentWindow,EditMsg.MouseX,EditMsg.MouseY))
						{
							PropertiesMode=PM_GADGET;
							SetStatusWindow(1,"");
							HandleSelectDown(CurrentWindow,o);
						}
						else
						{
							if(!Shift)
							{
								if(HowManySelected(&CurrentWindowNode->nw_List_Gadget)>0)
								{
									UnSelectAllObjects(CurrentWindow);
									RefreshProperties();
									piRenderObject(CurrentWindow);
								}
							}
							if(ActiveGadget)
							{
								GadgetStatus=GS_CREATING;
								SetStatusWindow(1,GetCatString(MSG_STATUS_CREATINGOBJECT));
							}
							else
							{
								GadgetStatus=GS_SELECTING;
								SetStatusWindow(2,GetCatString(MSG_STATUS_SELECTINGOBJECTS));
							}
							StartX=GridX(CurrentWindow,EditMsg.MouseX);
							StartY=GridY(CurrentWindow,EditMsg.MouseY);
							EndX=EndY=(UWORD) -1;
						}
						break;
					case MODE_FRAME:
						if(o=CurrentFrame=piFindFrame(CurrentWindow,EditMsg.MouseX,EditMsg.MouseY))
						{
							PropertiesMode=PM_FRAME;
							HandleSelectDown(CurrentWindow,o);
						}
						else
						{
							if(!Shift)
							{
								if(HowManySelected(&CurrentWindowNode->nw_List_Frame)>0)
								{
									UnSelectAllObjects(CurrentWindow);
									RefreshProperties();
									piRenderObject(CurrentWindow);
								}
							}
							GadgetStatus=GS_CREATING;
							SetStatusWindow(1,GetCatString(MSG_STATUS_CREATINGOBJECT));
							StartX=GridX(CurrentWindow,EditMsg.MouseX);
							StartY=GridY(CurrentWindow,EditMsg.MouseY);
							EndX=EndY=(UWORD) -1;
						}
						break;
				}
			}
			break;
		case SELECTUP:
			SetStatusWindow(1,"");
			if(GUI.gui_Mode!=MODE_TEST)
			{
				switch(GadgetStatus)
				{
					case GS_SIZING_LEFT:
					case GS_SIZING_RIGHT:
					case GS_SIZING_UP:
					case GS_SIZING_DOWN:
					case GS_SIZING_LEFTUP:
					case GS_SIZING_RIGHTUP:
					case GS_SIZING_RIGHTDOWN:
					case GS_SIZING_LEFTDOWN:
						GadgetStatus=GS_NOTHING;
						switch(GUI.gui_Mode)
						{
							case MODE_GADGET:
								SetWindowPointer(CurrentWindowNode->nw_Window,TAG_DONE);
								if(Shifted) piEraseObject(CurrentWindow);
								else piEraseObject(CurrentGadget);
								SetObjectDims(CurrentGadget,StartX,StartY,EndX,EndY);
								piRenderObject(CurrentWindow);
								RefreshPropertiesWindow(GetCatString(MSG_GADGETOBJECT));
//								RefreshProperties();
								break;
							case MODE_FRAME:
								SetWindowPointer(CurrentWindowNode->nw_Window,TAG_DONE);
								if(Shifted) piEraseObject(CurrentWindow);
								else piEraseObject(CurrentFrame);
								SetObjectDims(CurrentFrame,StartX,StartY,EndX,EndY);
								piRenderObject(CurrentWindow);
								RefreshPropertiesWindow(GetCatString(MSG_FRAMEOBJECT));
//								RefreshProperties();
								break;
						}
						break;
					case GS_MOVING:
						GadgetStatus=GS_NOTHING;
						switch(GUI.gui_Mode)
						{
							case MODE_GADGET:
								if((StartX!=EndX && StartY!=EndY) || (EndX!=-1 && EndY!=-1))
								{
									SetWindowPointer(CurrentWindowNode->nw_Window,TAG_DONE);
									if(Shifted) piEraseObject(CurrentWindow);
									else piEraseObject(CurrentGadget);
									for(o=(piObject *)GetHead(&CurrentWindowNode->nw_List_Gadget);GetSucc(o);o=(piObject *)GetSucc(o))
									{
										piGetAttr(o,OBJ_Node,(ULONG *)&ng);
										if(ng->ng_Selected)
										{
											ng->ng_X=GridX(CurrentWindow,piCX(o)+MoveX+DeltaX)-CurrentWindowNode->nw_Window->BorderLeft;
											ng->ng_Y=GridY(CurrentWindow,piCY(o)+MoveY+DeltaY)-CurrentWindowNode->nw_Window->BorderTop;
										}
									}
									piRenderObject(CurrentWindow);
//									RefreshProperties();
								}
								break;
							case MODE_FRAME:
								if((StartX!=EndX && StartY!=EndY) || (EndX!=-1 && EndY!=-1))
								{
									SetWindowPointer(CurrentWindowNode->nw_Window,TAG_DONE);
									if(Shifted) piEraseObject(CurrentWindow);
									else piEraseObject(CurrentFrame);
									for(o=(piObject *)GetHead(&CurrentWindowNode->nw_List_Frame);GetSucc(o);o=(piObject *)GetSucc(o))
									{
										piGetAttr(o,OBJ_Node,(ULONG *)&nf);
										if(nf->nf_Selected)
										{
											nf->nf_X=GridX(CurrentWindow,piCX(o)+MoveX+DeltaX)-CurrentWindowNode->nw_Window->BorderLeft;
											nf->nf_Y=GridY(CurrentWindow,piCY(o)+MoveY+DeltaY)-CurrentWindowNode->nw_Window->BorderTop;
										}
									}
									piRenderObject(CurrentWindow);
//									RefreshProperties();
								}
								break;
						}
						break;
					case GS_SELECTING:
						SetWindowPointer(CurrentWindowNode->nw_Window,TAG_DONE);
						GadgetStatus=GS_NOTHING;
						EndX=GridX(CurrentWindow,EditMsg.MouseX);
						EndY=GridY(CurrentWindow,EditMsg.MouseY);
						SetDrPt(CurrentWindowNode->nw_Window->RPort,0xffff);
						if(StartX!=EndX && StartY!=EndY)
						{
							SelectObjects(CurrentWindow,StartX,StartY,EndX,EndY);
							piEraseObject(CurrentWindow);
							piRenderObject(CurrentWindow);
						}
						RefreshProperties();
						break;
					case GS_CREATING:
						SetWindowPointer(CurrentWindowNode->nw_Window,TAG_DONE);
						GadgetStatus=GS_NOTHING;
						EndX=GridX(CurrentWindow,EditMsg.MouseX);
						EndY=GridY(CurrentWindow,EditMsg.MouseY);
						switch(GUI.gui_Mode)
						{
							case MODE_GADGET:
								CurrentGadget=piNewObject(piGadgets[GetGadgetPtr(ActiveGadget)].Class,TAG_DONE);
								piAddObject(CurrentGadget,CurrentWindow);
								if((StartX==EndX) && (StartY==EndY))
								{
									SetObjectDims(CurrentGadget,StartX,StartY,StartX+2,StartY+2);
									piResetToDefaults(CurrentGadget);
								}
								else
								{
									piResetToDefaults(CurrentGadget);
									SetObjectDims(CurrentGadget,StartX,StartY,EndX,EndY);
								}
								piRenderObject(CurrentWindow);
								PropertiesMode=PM_GADGET;
								RefreshPropertiesWindow(GetCatString(MSG_GADGETOBJECT));
//								ActiveGadget=NULL;
//								ApplySelectionWindow();
								break;
							case MODE_FRAME:
								CurrentFrame=piNewObject(piFrameClass,TAG_DONE);
								piAddObject(CurrentFrame,CurrentWindow);
								if((StartX==EndX) && (StartY==EndY))
								{
									SetObjectDims(CurrentFrame,StartX,StartY,StartX+2,StartY+2);
									piResetToDefaults(CurrentFrame);
								}
								else
								{
									piResetToDefaults(CurrentFrame);
									SetObjectDims(CurrentFrame,StartX,StartY,EndX,EndY);
								}
								piRenderObject(CurrentWindow);
								PropertiesMode=PM_FRAME;
								RefreshPropertiesWindow(GetCatString(MSG_FRAMEOBJECT));
								break;
						}	// switch(GUI.gui_Mode)
						break;
				} // switch(GadgetStatus)
			} // if(GUI.gui_Mode!=MODE_TEST)
			break;
	} // switch(EditMsg.Code)
cancel:
	return 1;
}

int FN_MouseMove_Edit( void )
{
	/* routine for "IDCMP_MOUSEMOVE". */

	switch(GUI.gui_Mode)
	{
		case MODE_TEST:
			break;
		case MODE_GADGET:
			HandleMouseMove(CurrentWindow,CurrentGadget);
			break;
		case MODE_FRAME:
			HandleMouseMove(CurrentWindow,CurrentFrame);
			break;
	}
	return 1;
}

int FN_MenuVerify_Edit(void)
{
	return 1;
}

ULONG AP_Window,GType;

void HandleMouseMove(piObject *wo,piObject *o)
{
	struct Node_Window *nw;
	struct Node_Gadget *ng;
	struct RastPort *rport;
	struct List *l;
	struct Window *PropsWnd;
	piObject *go;
	UWORD x,y,w,h;
	ULONG sel;

	switch(GUI.gui_Mode)
	{
		case MODE_TEST:
			return;
		case MODE_GADGET:
			piGetAttr(wo,WNOBJ_GadgetList,(ULONG *)&l);
			break;
		case MODE_FRAME:
			piGetAttr(wo,WNOBJ_FrameList,(ULONG *)&l);
			break;
	}
	piGetAttr(wo,WNOBJ_RastPort,(ULONG *)&rport);
	piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
	if(GadgetStatus==GS_MOVING)
	{
		EndX=GridX(wo,EditMsg.MouseX);
		EndY=GridY(wo,EditMsg.MouseY);
		MoveX=EndX-StartX;
		MoveY=EndY-StartY;
		if(MoveX+DeltaX<MinMoveX) MoveX=MinMoveX-DeltaX;
		if(MoveY+DeltaY<MinMoveY) MoveY=MinMoveY-DeltaY;
		if(MoveX+DeltaX>MaxMoveX) MoveX=MaxMoveX-DeltaX;
		if(MoveY+DeltaY>MaxMoveY) MoveY=MaxMoveY-DeltaY;
		for(go=(piObject *)GetHead(l);GetSucc(go);go=(piObject *)GetSucc(go))
		{
			piGetAttr(go,OBJ_Select,&sel);
			if(sel)
			{
				x=piCX(go); y=piCY(go); w=piCW(go); h=piCH(go);
				SetAPen(rport,1);
				SetDrMd(rport,COMPLEMENT);
				if(!First)
				{
					Move(rport,GridX(wo,x+DeltaX+px),  GridY(wo,y+DeltaY+py));
					Draw(rport,GridX(wo,x+DeltaX+px)+w,GridY(wo,y+DeltaY+py));
					Draw(rport,GridX(wo,x+DeltaX+px)+w,GridY(wo,y+DeltaY+py)+h);
					Draw(rport,GridX(wo,x+DeltaX+px),  GridY(wo,y+DeltaY+py)+h);
					Draw(rport,GridX(wo,x+DeltaX+px),  GridY(wo,y+DeltaY+py)+1);
				}
				Move(rport,GridX(wo,x+DeltaX+MoveX),  GridY(wo,y+DeltaY+MoveY));
				Draw(rport,GridX(wo,x+DeltaX+MoveX)+w,GridY(wo,y+DeltaY+MoveY));
				Draw(rport,GridX(wo,x+DeltaX+MoveX)+w,GridY(wo,y+DeltaY+MoveY)+h);
				Draw(rport,GridX(wo,x+DeltaX+MoveX),  GridY(wo,y+DeltaY+MoveY)+h);
				Draw(rport,GridX(wo,x+DeltaX+MoveX),  GridY(wo,y+DeltaY+MoveY)+1);
				if(PropertiesWnd && !Shifted)
				{
					if(AP_Window==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
					switch(GUI.gui_Mode)
					{
						case MODE_GADGET:
							piGetAttr(o,OBJ_Node,(ULONG *)&ng);
							if(ng->ng_Type==GTYPE_GADTOOLS)
							{
								SetInteger(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_X],GridX(wo,x+DeltaX+MoveX)-nw->nw_Window->BorderLeft);
								SetInteger(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_Y],GridY(wo,y+DeltaY+MoveY)-nw->nw_Window->BorderTop);
							}
							else
							{
								SetInteger(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_X],GridX(wo,x+DeltaX+MoveX)-nw->nw_Window->BorderLeft);
								SetInteger(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_Y],GridY(wo,y+DeltaY+MoveY)-nw->nw_Window->BorderTop);
							}
							break;
						case MODE_FRAME:
							SetInteger(PropsWnd,PGFR_GeneralGadgets[GD_PGFR_X],GridX(wo,x+DeltaX+MoveX)-nw->nw_Window->BorderLeft);
							SetInteger(PropsWnd,PGFR_GeneralGadgets[GD_PGFR_Y],GridY(wo,y+DeltaY+MoveY)-nw->nw_Window->BorderTop);
							break;
					}
				}
			}
		}
		First=FALSE;
		px=MoveX;
		py=MoveY;
	}
	else
	{
		if(GadgetStatus!=GS_NOTHING)
		{
			SetAPen(rport,1);
			SetDrMd(rport,COMPLEMENT);
			if(GadgetStatus==GS_SELECTING) { SetDrPt(rport,0xf0f0); }
			else { SetDrPt(rport,0xffff); }
			if(EndX!=-1 && EndY!=-1)
			{
				Move(rport,StartX,StartY);
				Draw(rport,EndX,StartY);
				Draw(rport,EndX,EndY);
				Draw(rport,StartX,EndY);
				Draw(rport,StartX,StartY+1);
			}
			if(GadgetStatus!=GS_CREATING && GadgetStatus!=GS_SELECTING)
			{
				x=piCX(o); y=piCY(o); w=piCW(o); h=piCH(o);
			}
			switch(GadgetStatus)
			{
				case GS_SIZING_LEFT:
					StartX=GridX(wo,EditMsg.MouseX-DeltaX);
					StartY=y;
					EndX=x+w;
					EndY=y+h;
					if(((WORD)StartX)<nw->nw_Window->BorderLeft) StartX=nw->nw_Window->BorderLeft;
					if(StartX>=nw->nw_Window->Width-nw->nw_Window->BorderRight) StartX=nw->nw_Window->Width-nw->nw_Window->BorderRight-1;
					break;
				case GS_SIZING_RIGHT:
					StartX=x;
					StartY=y;
					EndX=(GridX(wo,EditMsg.MouseX+DeltaX));
					EndY=y+h;
					if(((WORD)EndX)<nw->nw_Window->BorderLeft) EndX=nw->nw_Window->BorderLeft;
					if(EndX>=nw->nw_Window->Width-nw->nw_Window->BorderRight) EndX=nw->nw_Window->Width-nw->nw_Window->BorderRight-1;
					break;
				case GS_SIZING_UP:
					StartX=x;
					StartY=(GridY(wo,EditMsg.MouseY-DeltaY));
					EndX=x+w;
					EndY=y+h;
					if(((WORD)StartY)<nw->nw_Window->BorderTop) StartY=nw->nw_Window->BorderTop;
					if(StartY>=nw->nw_Window->Height-nw->nw_Window->BorderBottom) StartY=nw->nw_Window->Height-nw->nw_Window->BorderBottom-1;
					break;
				case GS_SIZING_DOWN:
					StartX=x;
					StartY=y;
					EndX=x+w;
					EndY=(GridY(wo,EditMsg.MouseY+DeltaY));
					if(((WORD)EndY)<nw->nw_Window->BorderTop) EndY=nw->nw_Window->BorderTop;
					if(EndY>=nw->nw_Window->Height-nw->nw_Window->BorderBottom) EndY=nw->nw_Window->Height-nw->nw_Window->BorderBottom-1;
					break;
				case GS_SIZING_LEFTUP:
					StartX=(GridX(wo,EditMsg.MouseX-DeltaX));
					StartY=(GridY(wo,EditMsg.MouseY-DeltaY));
					EndX=x+w;
					EndY=y+h;
					if(((WORD)StartX)<nw->nw_Window->BorderLeft) StartX=nw->nw_Window->BorderLeft;
					if(((WORD)StartY)<nw->nw_Window->BorderTop) StartY=nw->nw_Window->BorderTop;
					if(StartX>=nw->nw_Window->Width-nw->nw_Window->BorderRight) StartX=nw->nw_Window->Width-nw->nw_Window->BorderRight-1;
					if(StartY>=nw->nw_Window->Height-nw->nw_Window->BorderBottom) StartY=nw->nw_Window->Height-nw->nw_Window->BorderBottom-1;
					break;
				case GS_SIZING_RIGHTUP:
					StartX=x;
					StartY=(GridY(wo,EditMsg.MouseY-DeltaY));
					EndX=(GridX(wo,EditMsg.MouseX+DeltaX));
					EndY=y+h;
					if(((WORD)EndX)<nw->nw_Window->BorderLeft) EndX=nw->nw_Window->BorderLeft;
					if(((WORD)StartY)<nw->nw_Window->BorderTop) StartY=nw->nw_Window->BorderTop;
					if(EndX>=nw->nw_Window->Width-nw->nw_Window->BorderRight) EndX=nw->nw_Window->Width-nw->nw_Window->BorderRight-1;
					if(StartY>=nw->nw_Window->Height-nw->nw_Window->BorderBottom) StartY=nw->nw_Window->Height-nw->nw_Window->BorderBottom-1;
					break;
				case GS_SIZING_RIGHTDOWN:
					StartX=x;
					StartY=y;
					EndX=(GridX(wo,EditMsg.MouseX+DeltaX));
					EndY=(GridY(wo,EditMsg.MouseY+DeltaY));
					if(((WORD)EndX)<nw->nw_Window->BorderLeft) EndX=nw->nw_Window->BorderLeft;
					if(((WORD)EndY)<nw->nw_Window->BorderTop) EndY=nw->nw_Window->BorderTop;
					if(EndX>=nw->nw_Window->Width-nw->nw_Window->BorderRight) EndX=nw->nw_Window->Width-nw->nw_Window->BorderRight-1;
					if(EndY>=nw->nw_Window->Height-nw->nw_Window->BorderBottom) EndY=nw->nw_Window->Height-nw->nw_Window->BorderBottom-1;
					break;
				case GS_SIZING_LEFTDOWN:
					StartX=(GridX(wo,EditMsg.MouseX-DeltaX));
					StartY=y;
					EndX=x+w;
					EndY=(GridY(wo,EditMsg.MouseY+DeltaY));
					if(((WORD)StartX)<nw->nw_Window->BorderLeft) StartX=nw->nw_Window->BorderLeft;
					if(((WORD)EndY)<nw->nw_Window->BorderTop) EndY=nw->nw_Window->BorderTop;
					if(StartX>=nw->nw_Window->Width-nw->nw_Window->BorderRight) StartX=nw->nw_Window->Width-nw->nw_Window->BorderRight-1;
					if(EndY>=nw->nw_Window->Height-nw->nw_Window->BorderBottom) EndY=nw->nw_Window->Height-nw->nw_Window->BorderBottom-1;
					break;
				case GS_CREATING:
				case GS_SELECTING:
					EndX=(GridX(wo,EditMsg.MouseX));
					EndY=(GridY(wo,EditMsg.MouseY));
					break;
			}
			Move(rport,StartX,StartY);
			Draw(rport,EndX,StartY);
			Draw(rport,EndX,EndY);
			Draw(rport,StartX,EndY);
			Draw(rport,StartX,StartY+1);
/*			if(GadgetStatus!=GS_CREATING && GadgetStatus!=GS_SELECTING && PropertiesWnd)
			{
				if(AP_Window==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
				switch(GUI.gui_Mode)
				{
					case MODE_GADGET:
						if(ng->ng_Type==GTYPE_GADTOOLS)
						{
							SetInteger(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_WIDTH],w);
							SetInteger(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_HEIGHT],h);
						}
						else
						{
							SetInteger(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_WIDTH],w);
							SetInteger(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_HEIGHT],h);
						}
						break;
					case MODE_FRAME:
						SetInteger(PropsWnd,PGFR_GeneralGadgets[GD_PGFR_WIDTH],w);
						SetInteger(PropsWnd,PGFR_GeneralGadgets[GD_PGFR_HEIGHT],h);
						break;
				}
			}
*/
		}
	}
}

piObject *HandleSelectDown(piObject *wo,piObject *go)
{
	struct Node_Window *nw;
	ULONG sel;
	UWORD lx,ly,rx,ry;

	piGetAttr(wo,OBJ_Node,(ULONG *)&nw);

	Shifted=FALSE;
	if(!Shift) UnSelectAllObjects(wo);
	else Shifted=TRUE;
	piSetAttrs(go,OBJ_Select,TRUE,TAG_DONE);
	RefreshProperties();
	piRenderObject(go);
	StartX=piCX(go);
	StartY=piCY(go);
	EndX=EndY=(UWORD) -1;
	First=TRUE;
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &AP_Window);
	switch(CheckObjectClickPosition(go,EditMsg.MouseX,EditMsg.MouseY))
	{
		case CP_MOVE:
			SetStatusWindow(1,GetCatString(MSG_STATUS_MOVINGOBJECT));
			DeltaX=-(GridX(wo,EditMsg.MouseX)-piCX(go));
			DeltaY=-(GridY(wo,EditMsg.MouseY)-piCY(go));
			GadgetStatus=GS_MOVING;
			MoveX=-DeltaX;
			MoveY=-DeltaY;
			lx=ly=65535;
			rx=ry=0;
			switch(GUI.gui_Mode)
			{
				case MODE_GADGET:
					for(go=(piObject *)GetHead(&nw->nw_List_Gadget);GetSucc(go);go=(piObject *)GetSucc(go))
					{
						piGetAttr(go,OBJ_Select,&sel);
						if(sel)
						{
							SetWindowPointer(nw->nw_Window,WA_Pointer,MovePtr,TAG_DONE);
							if(piCX(go)<lx) lx=piCX(go);
							if(piCY(go)<ly) ly=piCY(go);
							if(piCX(go)+piCW(go)>rx) rx=piCX(go)+piCW(go);
							if(piCY(go)+piCH(go)>ry) ry=piCY(go)+piCH(go);
						}
					}
					break;
				case MODE_FRAME:
					for(go=(piObject *)GetHead(&nw->nw_List_Frame);GetSucc(go);go=(piObject *)GetSucc(go))
					{
						piGetAttr(go,OBJ_Select,&sel);
						if(sel)
						{
							SetWindowPointer(nw->nw_Window,WA_Pointer,MovePtr,TAG_DONE);
							if(piCX(go)<lx) lx=piCX(go);
							if(piCY(go)<ly) ly=piCY(go);
							if(piCX(go)+piCW(go)>rx) rx=piCX(go)+piCW(go);
							if(piCY(go)+piCH(go)>ry) ry=piCY(go)+piCH(go);
						}
					}
					break;
			}
			MinMoveX=-(lx-nw->nw_Window->BorderLeft);
			MinMoveY=-(ly-nw->nw_Window->BorderTop);
			MaxMoveX=nw->nw_Window->Width-nw->nw_Window->BorderRight-rx-1;
			MaxMoveY=nw->nw_Window->Height-nw->nw_Window->BorderBottom-ry-1;
			break;
		case CP_SIZE_LEFT:
			SetStatusWindow(1,GetCatString(MSG_STATUS_RESIZINGOBJECT));
			SetWindowPointer(nw->nw_Window,WA_Pointer,SizeHorizPtr,TAG_DONE);
			DeltaX=(GridX(wo,EditMsg.MouseX)-piCX(go));
			GadgetStatus=GS_SIZING_LEFT;
			break;
		case CP_SIZE_RIGHT:
			SetStatusWindow(1,GetCatString(MSG_STATUS_RESIZINGOBJECT));
			SetWindowPointer(nw->nw_Window,WA_Pointer,SizeHorizPtr,TAG_DONE);
			DeltaX=GridX(wo,(piCX(go)+piCW(go)-EditMsg.MouseX));
			GadgetStatus=GS_SIZING_RIGHT;
			break;
		case CP_SIZE_UP:
			SetStatusWindow(1,GetCatString(MSG_STATUS_RESIZINGOBJECT));
			SetWindowPointer(nw->nw_Window,WA_Pointer,SizeVertPtr,TAG_DONE);
			DeltaY=(GridY(wo,EditMsg.MouseY)-piCY(go));
			GadgetStatus=GS_SIZING_UP;
			break;
		case CP_SIZE_DOWN:
			SetStatusWindow(1,GetCatString(MSG_STATUS_RESIZINGOBJECT));
			SetWindowPointer(nw->nw_Window,WA_Pointer,SizeVertPtr,TAG_DONE);
			DeltaY=(piCY(go)+piCH(go)-GridY(wo,EditMsg.MouseY));
			GadgetStatus=GS_SIZING_DOWN;
			break;
		case CP_SIZE_LEFTUP:
			SetStatusWindow(1,GetCatString(MSG_STATUS_RESIZINGOBJECT));
			SetWindowPointer(nw->nw_Window,WA_Pointer,Size45UDPtr,TAG_DONE);
			DeltaX=(GridX(wo,EditMsg.MouseX)-piCX(go));
			DeltaY=(GridY(wo,EditMsg.MouseY)-piCY(go));
			GadgetStatus=GS_SIZING_LEFTUP;
			break;
		case CP_SIZE_RIGHTUP:
			SetStatusWindow(1,GetCatString(MSG_STATUS_RESIZINGOBJECT));
			SetWindowPointer(nw->nw_Window,WA_Pointer,Size45DUPtr,TAG_DONE);
			DeltaX=(piCX(go)+piCW(go)-GridX(wo,EditMsg.MouseX));
			DeltaY=(GridY(wo,EditMsg.MouseY)-piCY(go));
			GadgetStatus=GS_SIZING_RIGHTUP;
			break;
		case CP_SIZE_RIGHTDOWN:
			SetStatusWindow(1,GetCatString(MSG_STATUS_RESIZINGOBJECT));
			SetWindowPointer(nw->nw_Window,WA_Pointer,Size45UDPtr,TAG_DONE);
			DeltaX=(piCX(go)+piCW(go)-GridX(wo,EditMsg.MouseX));
			DeltaY=(piCY(go)+piCH(go)-GridY(wo,EditMsg.MouseY));
			GadgetStatus=GS_SIZING_RIGHTDOWN;
			break;
		case CP_SIZE_LEFTDOWN:
			SetStatusWindow(1,GetCatString(MSG_STATUS_RESIZINGOBJECT));
			SetWindowPointer(nw->nw_Window,WA_Pointer,Size45DUPtr,TAG_DONE);
			DeltaX=(GridX(wo,EditMsg.MouseX)-piCX(go));
			DeltaY=(piCY(go)+piCH(go)-GridY(wo,EditMsg.MouseY));
			GadgetStatus=GS_SIZING_LEFTDOWN;
			break;
	}
	return go;
}

int FN_RawKey_Edit( void )
{
	/* routine for "IDCMP_RAWKEY". */

	switch(EditMsg.Code)
	{
		case 0x60:	// Left Shift Down
		case 0x61:	// Right Shift Down
			Shift=TRUE;
			break;
		case 0xE0:	// Left Shift Up
		case 0xE1:	// Right Shift Up
			Shift=FALSE;
			break;
		case 0x46:	// Del
			FN_MainEditClear_Selected();
			break;
		case 0x5f:	// Help
			if(AmigaGuideObj) gtbSendAmigaGuideCmd(AmigaGuideObj,"ALINK main");
			break;
	}
	return 1;
}

int FN_CloseWindow_Working( void )
{
	/* routine for "IDCMP_CLOSEWINDOW". */
	return 1;
}

int FN_ActiveWindow_Edit( void )
{
	/* routine for "IDCMP_CLOSEWINDOW". */
	PropertiesMode=PM_WINDOW;
	RefreshProperties();
	if(!TB_WindowWnd) { OpenWindow_TB_Window(); ApplySelectionWindow(); }
	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	SetWndSignals();
	return 1;
}

void AlignObjects(struct MinList *list,ULONG how)
{
	piObject *o,*ot;
	LONG sel,l,t,w,h,r,b,ml=65535,mt=65535,mr=0,mb=0,mw=0,mh=0,s=0,i,pl,pt,a,aa,la,lb;
	piObject **objs=NULL,*swap;

	for(o=(piObject *)GetHead(list);GetSucc(o);o=(piObject *)GetSucc(o))
	{
		piGetAttr(o,OBJ_Select,(ULONG *)&sel);
		if(sel)
		{
			s++;
			piGetAttr(o,GA_Left,(ULONG *)&l);
			piGetAttr(o,GA_Top,(ULONG *)&t);
			piGetAttr(o,GA_Width,(ULONG *)&w);
			piGetAttr(o,GA_Height,(ULONG *)&h);
			r=l+w;
			b=t+h;
			if(l<ml) { ml=l; }
			if(t<mt) { mt=t; }
			if(r>mr) { mr=r; }
			if(b>mb) { mb=b; }
			if(w>mw) { mw=w; }
			if(h>mh) { mh=h; }
		}
	}
	if(s>1)
	{
		objs=MyAllocVec(sizeof(struct piObject *)*s,MEMF_ANY);
		s=0;
		for(o=(piObject *)GetHead(list);GetSucc(o);o=(piObject *)GetSucc(o))
		{
			piGetAttr(o,OBJ_Select,(ULONG *)&sel);
			if(sel)
			{
				objs[s]=o;
				s++;
			}
		}
		switch(how)
		{
			case SPREAD_HORIZ:
				mw=mr-ml;	// maksymalny rozstaw obiektow
				w=0;			// suma szerokosci obiektow
				i=0;			// ilosc obiektow
				for(o=(piObject *)GetHead(list);GetSucc(o);o=(piObject *)GetSucc(o))
				{
					piGetAttr(o,OBJ_Select,(ULONG *)&sel);
					if(sel)
					{
						piGetAttr(o,GA_Width,(ULONG *)&l);
						w+=l;
						i++;
					}
				}
				l=(mw-w)/(i-1);
				pl=ml;
	
				for(a=0;a<s;a++)
				{
					for(aa=0;aa<s;aa++)
					{
						piGetAttr(objs[a],GA_Left,(ULONG *)&la);
						piGetAttr(objs[aa],GA_Left,(ULONG *)&lb);
						if(la<lb)
						{
							swap=objs[a];
							objs[a]=objs[aa];
							objs[aa]=swap;
						}
					}
				}
				break;
			case SPREAD_VERT:
				mh=mb-mt;	// maksymalny rozstaw obiektow
				h=0;			// suma szerokosci obiektow
				i=0;			// ilosc obiektow
				for(o=(piObject *)GetHead(list);GetSucc(o);o=(piObject *)GetSucc(o))
				{
					piGetAttr(o,OBJ_Select,(ULONG *)&sel);
					if(sel)
					{
						piGetAttr(o,GA_Height,(ULONG *)&t);
						h+=t;
						i++;
					}
				}
				t=(mh-h)/(i-1);
				pt=mt;
	
				for(a=0;a<s;a++)
				{
					for(aa=0;aa<s;aa++)
					{
						piGetAttr(objs[a],GA_Top,(ULONG *)&la);
						piGetAttr(objs[aa],GA_Top,(ULONG *)&lb);
						if(la<lb)
						{
							swap=objs[a];
							objs[a]=objs[aa];
							objs[aa]=swap;
						}
					}
				}
				break;
			default:
				piGetAttr(CurrentGadget,GA_Top,(ULONG *)&mt);
				piGetAttr(CurrentGadget,GA_Left,(ULONG *)&ml);
				piGetAttr(CurrentGadget,GA_Width,(ULONG *)&mw);
				piGetAttr(CurrentGadget,GA_Height,(ULONG *)&mh);
				mr=ml+mw;
				mb=mt+mh;
				ot=CurrentGadget;
				break;
		}
		for(a=0;a<s;a++)
		{
			o=objs[a];
			switch(how)
			{
				case ALIGN_LEFT:
					piSetAttrs(o,GA_Left,ml,TAG_DONE);
					break;
				case ALIGN_RIGHT:
					piGetAttr(o,GA_Width,(ULONG *)&w);
					piSetAttrs(o,GA_Left,mr-w,TAG_DONE);
					break;
				case ALIGN_TOP:
					piSetAttrs(o,GA_Top,mt,TAG_DONE);
					break;
				case ALIGN_BOTTOM:
					piGetAttr(o,GA_Height,(ULONG *)&h);
					piSetAttrs(o,GA_Top,mb-h,TAG_DONE);
					break;
				case ALIGN_HCENTER:
					piGetAttr(o,GA_Width,(ULONG *)&w);
					piSetAttrs(o,GA_Left,ml+((mr-ml)/2)-(w/2),TAG_DONE);
					break;
				case ALIGN_VCENTER:
					piGetAttr(o,GA_Height,(ULONG *)&h);
					piSetAttrs(o,GA_Top,mt+((mb-mt)/2)-(h/2),TAG_DONE);
					break;
				case CLONE_WIDTH:
					piGetAttr(ot,GA_Width,(ULONG *)&w);
					piSetAttrs(o,GA_Width,w,TAG_DONE);
					break;
				case CLONE_HEIGHT:
					piGetAttr(ot,GA_Height,(ULONG *)&h);
					piSetAttrs(o,GA_Height,h,TAG_DONE);
					break;
				case CLONE_BOTH:
					piGetAttr(ot,GA_Width,(ULONG *)&w);
					piGetAttr(ot,GA_Height,(ULONG *)&h);
					piSetAttrs(o,GA_Width,w,GA_Height,h,TAG_DONE);
					break;
				case SPREAD_HORIZ:
					piGetAttr(o,GA_Width,(ULONG *)&w);
					piSetAttrs(o,GA_Left,pl,TAG_DONE);
					pl=pl+l+w;
					break;
				case SPREAD_VERT:
					piGetAttr(o,GA_Height,(ULONG *)&h);
					piSetAttrs(o,GA_Top,pt,TAG_DONE);
					pt=pt+t+h;
					break;
			}
		}
		if(objs) MyFreeVec(objs);
	}
}

int HowManySelected(struct MinList *list)
{
	piObject *o;
	ULONG sel;
	int numsel=0;

	for(o=(piObject *)GetHead(list);GetSucc(o);o=(piObject *)GetSucc(o))
	{
		piGetAttr(o,OBJ_Select,(ULONG *)&sel);
		if(sel) numsel++;
	}
	return numsel;
}
