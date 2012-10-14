/*
** C Source code generator
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<proto/dos.h>
#include<proto/intuition.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<proto/progress.h>
#include<proto/listview.h>
#include<proto/gadtools.h>
#include<proto/iffparse.h>
#include<prefs/prefhdr.h>
#include"main.h"
#include"gadtools.h"
#include"gadtools_cat.h"

#define Leave(x) {  printf(x); goto end; }

/*
** Protos
*/

void SavePrefs(STRPTR);
void LoadPrefs(STRPTR);
BOOL GenerateSourceCode(STRPTR);
void GenerateMainCode(BPTR);
void GenerateHeader(BPTR);
void GenerateImages(BPTR);
void GenerateTemplate(BPTR);
void GenerateCatalogSource(BPTR);
void GenerateCatalogHeader(BPTR);
void GenerateCatalogDescription(BPTR);
void GenerateCatalogTranslation(BPTR);
BOOL CheckPlugIn(ULONG);
BOOL CheckBOOPSIWindow(piObject *);
BOOL CheckDataTypeWindow(piObject *);
BOOL CheckGadToolsWindow(piObject *);
BOOL CheckPlugInWindow(piObject *,ULONG);
BOOL CheckAppWindow(void);
BOOL CheckToolBarWindows(void);
BOOL CheckToolBarWindow(piObject *);
BOOL CheckProjectWindow(void);

void GADGET_GenerateStruct(BPTR,piObject *);
void GADGET_GenerateTags(BPTR,piObject *);
void GADGET_GenerateOpenLib(BPTR,ULONG);
void GADGET_GenerateCloseLib(BPTR,ULONG);
void GADGET_GenerateProtos(BPTR,piObject *);
void GADGET_GenerateTemplate(BPTR,piObject *);
void GADGET_GenerateCatalogSource(BPTR,piObject *,LONG *);
void GADGET_GenerateCatalogHeader(BPTR,piObject *);
void GADGET_GenerateCatalogDescription(BPTR,piObject *);
void GADGET_GenerateCatalogTranslation(BPTR,piObject *);

void FUNC_GenerateWindowTags(BPTR,piObject *);
void FUNC_GenerateApplication(BPTR);
void FUNC_GenerateScreen(BPTR);
void FUNC_GenerateWindow(BPTR,piObject *);
void FUNC_GenerateErrors(BPTR);
void FUNC_GenerateTemplate(BPTR,piObject *);
void FUNC_GenerateCatalogUpdate(BPTR);

void MENU_GenerateProtos(BPTR,piObject *);
void MENU_GenerateDefs(BPTR,piObject *);
void MENU_GenerateTemplate(BPTR,piObject *);
void MENU_GenerateCatalogSource(BPTR,piObject *,LONG *);
void MENU_GenerateCatalogHeader(BPTR,piObject *);
void MENU_GenerateCatalogDescription(BPTR,piObject *);
void MENU_GenerateCatalogTranslation(BPTR,piObject *);

#define SetProgress(prg) SetGadgetAttrs(GTBSem->gs_Progress,GTBSem->gs_Window,NULL,PROG_CurVal,prg,TAG_DONE)
#define IFF_Error(num)		\
		{											\
			printf("IFFError #%d",num);	\
			goto skip;	\
		}

/*
** Variables
*/

struct GTBSemaphore *GTBSem;
struct GUI *GUI;
extern struct Screen *Scr;
STRPTR BaseName;
BOOL OPT_ARexx,OPT_Template,OPT_Screen,OPT_UseLocale,OPT_GUIFuncs;
struct SavePrefs SP;

#define TEMPLATE "FILE/K,SEMAPHORE/A,SETPREFS/S"
#define ARG_FILE 0
#define ARG_SEMAPHORE 1
#define ARG_SETPREFS 2
#define ARG_COUNT 3

char DEF_FILE[]="\0";
char DEF_SEMAPHORE[]="\0";
ULONG DEF_SETPREFS=FALSE;

BOOL setprefs=FALSE;
TEXT fn[256];
TEXT semaphore[256];


/*
** Main Code
*/

void main(int argc, char *argv[])
{
	struct RDArgs *rdargs;
	long *(opts[ARG_COUNT]);
	int running=1;

	if(argc!=0)
	{
		opts[ARG_FILE]=(long *)DEF_FILE;
		opts[ARG_SEMAPHORE]=(long *)DEF_SEMAPHORE;
		opts[ARG_SETPREFS]=(long *)DEF_SETPREFS;

		if(rdargs=ReadArgs(TEMPLATE,(long *) opts, NULL))
		{
			strcpy(fn,(char *)opts[ARG_FILE]);
			strcpy(semaphore,(char *)opts[ARG_SEMAPHORE]);
			setprefs=(ULONG)opts[ARG_SETPREFS];

			FreeArgs(rdargs);
		}
		else
		{
			PrintFault(IoErr(),argv[0]);
			exit(IoErr());
		}
	}

	if(GTBSem=(struct GTBSemaphore *)FindSemaphore(semaphore)) ObtainSemaphoreShared(&GTBSem->gs_Semaphore); else Leave("GadToolsBox 3 is not running!\n");
	GUI=GTBSem->gs_GUI;
	strcpy(ScrPubName,GUI->gui_PubScreenName);

	LoadPrefs("ENV:GadToolsBox/c.generator.prefs");

	if(setprefs)
	{
		if(!InitApplication(NULL))
		{
			if(!SetupScreen(0))
			{
				if(!OpenWindow_Prefs())
				{
					SetCheckMark(PrefsWnd,PrefsGadgets[GD_TEMPLATE],SP.spr_Flags&MYPR_Template);
					SetCheckMark(PrefsWnd,PrefsGadgets[GD_SCREEN],SP.spr_Flags&MYPR_Screen);
					SetCheckMark(PrefsWnd,PrefsGadgets[GD_LOCALIZE],SP.spr_Flags&MYPR_Localize);
					SetCheckMark(PrefsWnd,PrefsGadgets[GD_GUIFUNCS],SP.spr_Flags&MYPR_GUIFuncs);
					while(running==1 || running==2)
					{
						Wait(1L<<PrefsWnd->UserPort->mp_SigBit);
							running=HandleIDCMP_Prefs();
					}
					CloseWindow_Prefs();
				}
				CloseDownScreen();
			}
			FreeApplication();
		}
		if(GTBSem) ReleaseSemaphore(&GTBSem->gs_Semaphore);
		exit(0);
	}

	if(!(Scr=LockPubScreen(GUI->gui_PubScreenName))) Leave("Can't LockPubScreen\n");
	OPT_Template=SP.spr_Flags&MYPR_Template;
	OPT_Screen=SP.spr_Flags&MYPR_Screen;
	OPT_UseLocale=SP.spr_Flags&MYPR_Localize;
	OPT_GUIFuncs=SP.spr_Flags&MYPR_GUIFuncs;
	if(!IsListEmpty(((struct List *)&GUI->gui_List_Window))) { OPT_Screen=TRUE; OPT_GUIFuncs=TRUE; }
	GenerateSourceCode(fn);
end:
	if(Scr) UnlockPubScreen(NULL,Scr);
	if(GTBSem) ReleaseSemaphore(&GTBSem->gs_Semaphore);
}

BOOL CheckPlugIn(ULONG id)
{
	piObject *wo,*go;
	struct Node_Window *nw;
	struct Node_Gadget *ng;

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
		{
			ng=(struct Node_Gadget *)OBJECT_DATA(go);
			if(ng->ng_PlugInID==id) return TRUE;
		}
	}
	return FALSE;
}

BOOL CheckBOOPSIWindow(piObject *wo)
{
	piObject *go;
	struct Node_Window *nw;
	struct Node_Gadget *ng;

	nw=(struct Node_Window *)OBJECT_DATA(wo);
	for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(go);
		if(	ng->ng_PlugInID==COLORWHEEL_ID ||
				ng->ng_PlugInID==GRADIENTSLIDER_ID ||
				ng->ng_PlugInID==PAGE_ID ||
				ng->ng_PlugInID==POPUPMENU_ID ||
				ng->ng_PlugInID==PROGRESS_ID ||
				ng->ng_PlugInID==TAPEDECK_ID ||
				ng->ng_PlugInID==TEXTFIELD_ID ||
				ng->ng_PlugInID==TREEVIEW_ID) return TRUE;
	}
	return FALSE;
}

BOOL CheckDataTypeWindow(piObject *wo)
{
	piObject *go;
	struct Node_Window *nw;
	struct Node_Gadget *ng;

	nw=(struct Node_Window *)OBJECT_DATA(wo);
	for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(go);
		if(	ng->ng_PlugInID==DATATYPE_ID ) return TRUE;
	}
	return FALSE;
}

BOOL CheckPlugInWindow(piObject *wo,ULONG id)
{
	piObject *go;
	struct Node_Window *nw;
	struct Node_Gadget *ng;

	nw=(struct Node_Window *)OBJECT_DATA(wo);
	for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(go);
		if(	ng->ng_PlugInID==id ) return TRUE;
	}
	return FALSE;
}

BOOL CheckGadToolsWindow(piObject *wo)
{
	piObject *go;
	struct Node_Window *nw;
	struct Node_Gadget *ng;

	nw=(struct Node_Window *)OBJECT_DATA(wo);
	for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(go);
		if(	ng->ng_PlugInID==BUTTON_ID ||
				ng->ng_PlugInID==CHECKBOX_ID ||
				ng->ng_PlugInID==CYCLE_ID ||
				ng->ng_PlugInID==INTEGER_ID ||
				ng->ng_PlugInID==LISTVIEW_ID ||
				ng->ng_PlugInID==NUMBER_ID ||
				ng->ng_PlugInID==MX_ID ||
				ng->ng_PlugInID==PALETTE_ID ||
				ng->ng_PlugInID==SCROLLER_ID ||
				ng->ng_PlugInID==SLIDER_ID ||
				ng->ng_PlugInID==STRING_ID ||
				ng->ng_PlugInID==TEXT_ID ) return TRUE;
	}
	return FALSE;
}

BOOL CheckToolBarWindows()
{
	piObject *go,*wo;
	struct Node_Window *nw;
	struct Node_Gadget *ng;

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
		{
			ng=(struct Node_Gadget *)OBJECT_DATA(go);
			if(	ng->ng_PlugInID==BUTTON_ID)
			{
				if(FindTagItem(TAG_USER+1,ng->ng_OwnTags)->ti_Data>0) return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CheckToolBarWindow(piObject *wo)
{
	piObject *go;
	struct Node_Window *nw;
	struct Node_Gadget *ng;

	nw=(struct Node_Window *)OBJECT_DATA(wo);
	for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(go);
		if(	ng->ng_PlugInID==BUTTON_ID)
		{
			if(FindTagItem(TAG_USER+1,ng->ng_OwnTags)->ti_Data>0) return TRUE;
		}
	}
	return FALSE;
}

BOOL CheckAppWindow()
{
	piObject *wo;
	struct Node_Window *nw;

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		GetFlag(nw->nw_MyFlags,MYWA_AppWindow) return TRUE;
	}
	return FALSE;
}

BOOL CheckProjectWindow()
{
	piObject *wo;
	struct Node_Window *nw;

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		GetFlag(nw->nw_MyFlags,MYWA_Project) return TRUE;
	}
	return FALSE;
}

BOOL GenerateSourceCode(STRPTR basename)
{
	TEXT fname[108];
	BPTR file=NULL;
	struct Node_Version *nv;

	nv=(struct Node_Version *)OBJECT_DATA(GUI->gui_Node_Version);

	BaseName=FilePart(basename);

	sprintf(fname,"%s.c",basename);
	if(file=Open(fname,MODE_NEWFILE))
	{
		GenerateMainCode(file);
		Close(file);
	}

	sprintf(fname,"%s.h",basename);
	if(file=Open(fname,MODE_NEWFILE))
	{
		GenerateHeader(file);
		Close(file);
	}

	if(!IsListEmpty(List(&GUI->gui_List_Image)))
	{
		sprintf(fname,"%s_img.c",basename);
		if(file=Open(fname,MODE_NEWFILE))
		{
			GenerateImages(file);
			Close(file);
		}
	}

	if(OPT_Template)
	{
		sprintf(fname,"%s_temp.c",basename);
		if(file=Open(fname,MODE_NEWFILE))
		{
			GenerateTemplate(file);
			Close(file);
		}
		SetProgress(85L);
	}

	sprintf(fname,"%s_cat.c",basename);
	if(file=Open(fname,MODE_NEWFILE))
	{
		GenerateCatalogSource(file);
		Close(file);
	}

	sprintf(fname,"%s_cat.h",basename);
	if(file=Open(fname,MODE_NEWFILE))
	{
		GenerateCatalogHeader(file);
		Close(file);
	}

	SetProgress(90L);

	if(OPT_UseLocale)
	{
		strcpy(fname,basename);
		sprintf(FilePart(fname),"%s.cd",nv->nv_ProgramName);
		if(file=Open(fname,MODE_NEWFILE))
		{
			GenerateCatalogDescription(file);
			Close(file);
		}

		SetProgress(95L);

		strcpy(fname,basename);
		sprintf(FilePart(fname),"%s.ct",nv->nv_ProgramName);
		if(file=Open(fname,MODE_NEWFILE))
		{
			GenerateCatalogTranslation(file);
			Close(file);
		}
	}

	SetProgress(100L);

	return TRUE;
}

void GenerateMainCode(BPTR handle)
{
	piObject *wo,*go,*mo;
	struct Node_Window *nw;
	struct Node_Menu *nm;
	struct Node_Screen *ns;
	struct Node_Version *nv;

	ns=(struct Node_Screen *)OBJECT_DATA(GUI->gui_Node_Screen);
	nv=(struct Node_Version *)OBJECT_DATA(GUI->gui_Node_Version);

	SetProgress(0L);

/* Header */
	FPrintf(handle,"/*" \
		"\n** Source code generated by GadToolsBox %ld.%ld (c) by Tomasz Muszynski\n" \
		"** GUI designed by: %s\n" \
		"*/\n\n",GUI->gui_Version,GUI->gui_Revision,nv->nv_Author);

	SetProgress(2L);

/* Includes */
	FPuts(handle,"#include<stdio.h>\n" \
				"#include<proto/exec.h>\n" \
				"#include<exec/memory.h>\n" \
				"#include<proto/gadtools.h>\n" \
				"#include<proto/graphics.h>\n" \
				"#include<intuition/pointerclass.h>\n" \
				"#include<intuition/imageclass.h>\n");
	FPuts(handle,"#include<libraries/iffparse.h>\n");
	if(OPT_UseLocale) FPuts(handle,"#include<proto/locale.h>\n");
	FPuts(handle,"#include<proto/intuition.h>\n");
	if(CheckAppWindow()) FPuts(handle,"#include<proto/workbench.h>\n");
	if(CheckToolBarWindows()) FPuts(handle,"#include<proto/smartbitmap.h>\n");
	FPuts(handle,"#include<proto/utility.h>\n");
	if(CheckPlugIn(COLORWHEEL_ID)) FPuts(handle,"#include<proto/colorwheel.h>\n");
	if(CheckPlugIn(DATATYPE_ID)) FPuts(handle,"#include<proto/datatypes.h>\n");
	if(CheckPlugIn(PAGE_ID)) FPuts(handle,"#include<proto/page.h>\n");
	if(CheckPlugIn(POPUPMENU_ID)) FPuts(handle,"#include<gadgets/popupmenu.h>\n");
	if(CheckPlugIn(PROGRESS_ID)) FPuts(handle,"#include<proto/progress.h>\n");
	if(CheckPlugIn(TREEVIEW_ID)) FPuts(handle,"#include<proto/listview.h>\n");
	if(CheckPlugIn(TEXTFIELD_ID)){ FPuts(handle,"#include<proto/textfield.h>\n");
		FPuts(handle,"#include<gadgets/textfield.h>\n");}
	if(CheckPlugIn(TAPEDECK_ID)) FPuts(handle,"#include<gadgets/tapedeck.h>\n");
	if(CheckPlugIn(GRADIENTSLIDER_ID)) FPuts(handle,"#include<gadgets/gradientslider.h>\n");
	FPuts(handle,"#include<intuition/gadgetclass.h>\n");
	FPrintf(handle,"#include\"%s.h\"\n",BaseName);
	FPrintf(handle,"#include\"%s_cat.h\"\n",BaseName);
	FPuts(handle,"\n");

	SetProgress(5L);

	FPuts(handle,"TEXT VER_STRING[]=VER_SVER;\n\n");

	SetProgress(10L);

/* Global variables - available only if screen generation is on */
	if(OPT_Screen)
	{
		FPuts(handle,"struct Screen *Scr=NULL;\n" \
					"struct DrawInfo *DrawInfo=NULL;\n" \
					"APTR VisualInfo=NULL;\n");
		FPuts(handle,"TEXT ScrTitle[256];\n");
		if(ns->ns_Type!=TYPE_CUSTOMSCREEN) FPuts(handle,"TEXT ScrPubName[30];\n");
		if(ns->ns_Type!=TYPE_WORKBENCH)
		{
			FPrintf(handle,"UWORD ScrWidth=%ld;\n",ns->ns_Width);
			FPrintf(handle,"UWORD ScrHeight=%ld;\n",ns->ns_Height);
			FPrintf(handle,"UWORD ScrDepth=%ld;\n",ns->ns_Depth);
			GetFlag(ns->ns_MyFlags,MYSC_DefaultID) FPrintf(handle,"ULONG ScrDisplayID=0x0;\n",ns->ns_DisplayID);
			else FPrintf(handle,"ULONG ScrDisplayID=0x%lx;\n",ns->ns_DisplayID);
		}
	}
	if(OPT_UseLocale)
	{
		FPuts(handle,"struct Catalog *Catalog=NULL;\nstruct Locale *Locale=NULL;\n");
	}

	SetProgress(15L);

/* Gadget library bases */
	if(CheckPlugIn(COLORWHEEL_ID)) FPuts(handle,"struct Library *ColorWheelBase=NULL;\n");
	if(CheckPlugIn(GRADIENTSLIDER_ID)) FPuts(handle,"struct Library *GradientSliderBase=NULL;\n");
	if(CheckPlugIn(PAGE_ID)) FPuts(handle,"struct Library *PageBase=NULL;\n");
	if(CheckPlugIn(PROGRESS_ID)) FPuts(handle,"struct Library *ProgressBase=NULL;\n");
	if(CheckPlugIn(TAPEDECK_ID)) FPuts(handle,"struct Library *TapeDeckBase=NULL;\n");
	if(CheckPlugIn(TEXTFIELD_ID)) FPuts(handle,"struct Library *TextFieldBase=NULL;\n");
	if(CheckPlugIn(TREEVIEW_ID)) FPuts(handle,"struct Library *ListViewBase=NULL;\n");
	if(CheckPlugIn(PAGE_ID)) FPuts(handle,"Class *PageClass=NULL;\n");
	if(CheckPlugIn(POPUPMENU_ID)) FPuts(handle,"Class *PopUpMenuClass=NULL;\n");
	if(CheckPlugIn(PROGRESS_ID)) FPuts(handle,"Class *ProgressClass=NULL;\n");
	if(CheckPlugIn(TEXTFIELD_ID)) FPuts(handle,"Class *TextFieldClass=NULL;\n");
	if(CheckPlugIn(TREEVIEW_ID)) FPuts(handle,"Class *ListViewClass=NULL;\n");
	if(CheckToolBarWindows())
	{
		FPuts(handle,"struct Library *SmartBitMapBase=NULL;\n");
		FPuts(handle,"Class *SmartBitMapClass=NULL;\n");
	}

/* Message ports for App and Project windows */
	if(CheckAppWindow()) FPuts(handle,"struct MsgPort *AppMsgPort=NULL;\n");
	if(CheckProjectWindow()) FPuts(handle,"struct MsgPort *ProjectMsgPort=NULL;\n");
	if(OPT_GUIFuncs) FPuts(handle,"struct TextAttr *Font, Attr;\nUWORD FontX, FontY;\n");
	if(!IsListEmpty(List(&GUI->gui_List_Window)))
	{
		FPuts(handle,"BOOL GadgetLocaleDone[CNTWND_WINDOWS];\n");
		FPuts(handle,"BOOL WindowLocaleDone[CNTWND_WINDOWS];\n");
		FPuts(handle,"BOOL MenuLocaleDone[CNTMN_MENUS];\n");
	}
	FPuts(handle,"\n");

	SetProgress(20L);

/* Menu definitions */
	if(!IsListEmpty(List(&GUI->gui_List_Menu)))
	{
		for(mo=(piObject *)GetHead(List(&GUI->gui_List_Menu));GetSucc(mo);mo=(piObject *)GetSucc(mo))
		{
			nm=(struct Node_Menu *)OBJECT_DATA(mo);
			FPrintf(handle,"struct Menu *%sMenus=NULL;\n",nm->nm_Label);
		}
	}

	SetProgress(25L);

	if(!IsListEmpty(List(&GUI->gui_List_Menu)))
	{
		for(mo=(piObject *)GetHead(List(&GUI->gui_List_Menu));GetSucc(mo);mo=(piObject *)GetSucc(mo))
		{
			nm=(struct Node_Menu *)OBJECT_DATA(mo);
			MENU_GenerateDefs(handle,mo);
		}
	}

	SetProgress(30L);

/* Windows releated definitions */
	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);

		FPrintf(handle,"\n/* %s Window structures */\n",nw->nw_Label);
		if(nw->nw_PageGadget==0) FPrintf(handle,"struct Window *%sWnd=NULL;\n",nw->nw_Label);
		GetFlag(nw->nw_MyFlags,MYWA_Project) FPrintf(handle,"APTR %sWApp;\n",nw->nw_Label);
		if(!IsListEmpty(List(&nw->nw_List_Gadget))) FPrintf(handle,"struct Gadget *%sGList=NULL;\n",nw->nw_Label);
		if(!IsListEmpty(List(&nw->nw_List_Gadget))) FPrintf(handle,"struct Gadget *%sGadgets[CNT_%s];\n",nw->nw_Label,nw->nw_Label);
		if(nw->nw_PageGadget==0) FPrintf(handle,"struct IntuiMessage %sMsg;\n",nw->nw_Label);
	}
	FPuts(handle,"\n");

	SetProgress(35L);

/* Windows definitions */
	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		if(nw->nw_PageGadget==0)
		{
			FPrintf(handle,"\n/* %s Window tags */\n",nw->nw_Label);
			FPrintf(handle,"ULONG %sWTags[]=\n{\n",nw->nw_Label);
			FUNC_GenerateWindowTags(handle,wo);
			FPuts(handle,"TAG_DONE\n};\n");
		}
	}
	FPuts(handle,"\n");

	SetProgress(40L);

/* Gadget realeted structures (lists, tables, etc) */
	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		if(!IsListEmpty(List(&nw->nw_List_Gadget)))
		{
			FPrintf(handle,"/* %s Gadget structures */\n",nw->nw_Label);
			for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
				GADGET_GenerateStruct(handle,go);
		}
		FPuts(handle,"\n");
	}

	SetProgress(45L);

/* Gadget definitions */
	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		if(!IsListEmpty(List(&nw->nw_List_Gadget)))
		{
			FPrintf(handle,"/* %s Gadget tags */\n",nw->nw_Label);
			FPrintf(handle,"ULONG %sGTags[]=\n{\n",nw->nw_Label);
			for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
				GADGET_GenerateTags(handle,go);
			Seek(handle,-2,OFFSET_CURRENT);
			FPuts(handle,"\n};\n");
			FPuts(handle,"\n");
		}
	}

	SetProgress(50L);

/* Function definitions */
	FUNC_GenerateApplication(handle);
	FUNC_GenerateCatalogUpdate(handle);
	FUNC_GenerateScreen(handle);

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
		FUNC_GenerateWindow(handle,wo);
}

void GenerateImages(BPTR handle)
{
	piObject *io;
	struct Node_Image *ni;
	ULONG a,b,*data;
	UBYTE *datab;

	FPrintf(handle,"/*\n** %s_img.c Images File\n*/\n\n",BaseName);

	FPrintf(handle,"#include<proto/exec.h>\n#include<proto/graphics.h>\n#include\"%s.h\"\n\n",BaseName);

	if(CheckToolBarWindows())
	{
		FPuts(handle,"#define Bit32(x) ((ULONG)(x|(x<<8)|(x<<16)|(x<<24)))\n\n");
		FPrintf(handle,"struct SmartBitMapIRGB ToolBarPalette[]=" \
			"{\n" \
			"	{ 0,Bit32(189),Bit32(189),Bit32(189)},		// Background\n" \
			"	{-1,Bit32(  0),Bit32(  0),Bit32(  0)},		// Black\n" \
			"	{-1,Bit32(255),Bit32(255),Bit32(255)},		// White\n" \
			"	{-1,Bit32(123),Bit32(  0),Bit32(  0)},		// Red 50%\n" \
			"	{-1,Bit32(  0),Bit32(123),Bit32(  0)},		// Green 50%\n" \
			"	{-1,Bit32(  0),Bit32(  0),Bit32(123)},		// Blue 50%\n" \
			"	{-1,Bit32(123),Bit32(123),Bit32(  0)},		// Yellow 50%\n" \
			"	{-1,Bit32(  0),Bit32(123),Bit32(123)},		// Cyan 50%\n" \
			"	{-1,Bit32(123),Bit32(  0),Bit32(123)},		// Magenta 50%\n" \
			"	{-1,Bit32(255),Bit32(  0),Bit32(  0)},		// Red 100%\n" \
			"	{-1,Bit32(  0),Bit32(255),Bit32(  0)},		// Green 100%\n" \
			"	{-1,Bit32(  0),Bit32(  0),Bit32(255)},		// Blue 100%\n" \
			"	{-1,Bit32(255),Bit32(255),Bit32(  0)},		// Yellow 100%\n" \
			"	{-1,Bit32(  0),Bit32(255),Bit32(255)},		// Cyan 100%\n" \
			"	{-1,Bit32(255),Bit32(  0),Bit32(255)},		// Magenta 100%\n" \
			"	{-1,Bit32(123),Bit32(123),Bit32(123)}			// Dark Gray\n" \
			"};\n\n");
	}

/* Image definitions */
	for(io=(piObject *)GetHead(&GUI->gui_List_Image);GetSucc(io);io=(piObject *)GetSucc(io))
	{
		ni=(struct Node_Image *)OBJECT_DATA(io);
		switch(ni->ni_Type)
		{
			case IMTYPE_NORMAL:
				FPrintf(handle,"struct SmartBitMapIRGB %sPalette[]=\n{\n",ni->ni_Label);
				for(a=0;a<(2<<(ni->ni_Depth-1));a++)
					FPrintf(handle,"	{-1, Bit32(%3ld), Bit32(%3ld), Bit32(%3ld)},\n",Bit8(ni->ni_CT[a].cp_R),Bit8(ni->ni_CT[a].cp_G),Bit8(ni->ni_CT[a].cp_B));

				FPrintf(handle,"};\n\nUBYTE __chip %sPlanes[] = {\n",ni->ni_Label);
				for(a=0;a<ni->ni_Depth;a++)
				{
					datab=(UBYTE *)ni->ni_BitMap->Planes[a];
					for(b=0;b<(ni->ni_BitMap->BytesPerRow*ni->ni_BitMap->Rows);b++)
					{
						FPrintf(handle,"	0x%lx,",datab[b]);
						if(((b+1)%32)==0) FPuts(handle,"\n");
					}
					FPrintf(handle,"\n");
				}

				FPrintf(handle,"};\n\nObject *%sImg=NULL;\nstruct BitMap %sBitMap = { %ld, %ld, NULL, %ld, NULL, {",ni->ni_Label,ni->ni_Label,ni->ni_BitMap->BytesPerRow,ni->ni_BitMap->Rows,ni->ni_BitMap->Depth);
				for(a=0;a<ni->ni_Depth;a++)
				{
					FPrintf(handle,"(PLANEPTR)&%sPlanes[%ld],",ni->ni_Label,a*(ni->ni_BitMap->BytesPerRow*ni->ni_BitMap->Rows));
				}
				Seek(handle,-1,OFFSET_CURRENT);
				FPuts(handle,"} };\n\n");
				break;
			case IMTYPE_POINTER:
				FPrintf(handle,"ULONG __chip %sPlanes[] = {\n",ni->ni_Label);
				for(a=0;a<2;a++)
				{
					data=(ULONG *)ni->ni_BitMap->Planes[a];
					for(b=0;b<32;b+=8) FPrintf(handle,"	0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx, 0x%lx,\n",data[b],data[b+1],data[b+2],data[b+3],data[b+4],data[b+5],data[b+6],data[b+7]);
				}
				FPrintf(handle,"};\nObject *%sPtr=NULL;\nstruct BitMap %sBitMap = { 4, 32, NULL, 2, NULL, {(PLANEPTR)&%sPlanes[0],(PLANEPTR)&%sPlanes[32]} };\n\n",ni->ni_Label,ni->ni_Label,ni->ni_Label,ni->ni_Label);
				break;
			case IMTYPE_TOOLBAR:
				FPrintf(handle,"UBYTE __chip %sPlanes[] = {\n",ni->ni_Label);
				for(a=0;a<4;a++)
				{
					datab=(UBYTE *)ni->ni_BitMap->Planes[a];
					for(b=0;b<(32/8)*20;b++) FPrintf(handle,"	0x%lx,",datab[b]);
					FPrintf(handle,"\n");
				}
				FPrintf(handle,"};\nObject *%sTbr=NULL;\nstruct BitMap %sBitMap = { 4, 20, NULL, 4, NULL, {(PLANEPTR)&%sPlanes[0],(PLANEPTR)&%sPlanes[80],(PLANEPTR)&%sPlanes[160],(PLANEPTR)&%sPlanes[240]} };\n\n",ni->ni_Label,ni->ni_Label,ni->ni_Label,ni->ni_Label,ni->ni_Label,ni->ni_Label);
				break;
		}
	}
	FPuts(handle,"\n");
}

void GenerateHeader(BPTR handle)
{
	piObject *wo,*go,*mo,*io;
	struct Node_Window *nw;
	struct Node_Gadget *ng;
	struct Node_Menu *nm;
	struct Node_Screen *ns;
	struct Node_Version *nv;
	struct Node_Image *ni;
	ULONG cnt;

	SetProgress(55L);

	ns=(struct Node_Screen *)OBJECT_DATA(GUI->gui_Node_Screen);
	nv=(struct Node_Version *)OBJECT_DATA(GUI->gui_Node_Version);

	FPrintf(handle,"#ifndef %s_H\n",BaseName);
	FPrintf(handle,"#define %s_H\n",BaseName);
	FPrintf(handle,"/*\n** %s.h Header File\n*/\n\n",BaseName);

	if(!IsListEmpty(List(&GUI->gui_List_Image)))
		FPuts(handle,"#include<images/smartbitmap.h>\n\n");
	FPuts(handle,"#include<exec/types.h>\n#include<intuition/classusr.h>\n\n");

	FPuts(handle,"#define GetString( g )  ((( struct StringInfo * )g->SpecialInfo )->Buffer  )\n");
	FPuts(handle,"#define GetInteger( g ) ((( struct StringInfo * )g->SpecialInfo )->LongInt )\n");

	FPuts(handle,"#define TOOL_HORIZ 0\n" \
							 "#define TOOL_VERT 1\n\n");

	FPuts(handle,"#define DRAG_ID MAKE_ID('D','R','A','G')\n\n");

/*	if(CheckPlugIn(COLORWHEEL_ID)) FPuts(handle,"#define COLORWHEEL_KIND 10000\n");
	if(CheckPlugIn(DATATYPE_ID)) FPuts(handle,"#define DATATYPE_KIND 10001\n");
	if(CheckPlugIn(GRADIENTSLIDER_ID)) FPuts(handle,"#define GRADIENTSLIDER_KIND 10002\n");
	if(CheckPlugIn(PAGE_ID)) FPuts(handle,"#define PAGE_KIND 10003\n");
	if(CheckPlugIn(POPUPMENU_ID)) FPuts(handle,"#define POPUPMENU_KIND 10004\n");
	if(CheckPlugIn(PROGRESS_ID)) FPuts(handle,"#define PROGRESS_KIND 10005\n");
	if(CheckPlugIn(TAPEDECK_ID)) FPuts(handle,"#define TAPEDECK_KIND 10006\n");
	if(CheckPlugIn(TEXTFIELD_ID)) FPuts(handle,"#define TEXTFIELD_KIND 10007\n");
	if(CheckPlugIn(TREEVIEW_ID)) FPuts(handle,"#define TREEVIEW_KIND 10009\n");
*/
	FPuts(handle,"#define COLORWHEEL_KIND 10000\n");
	FPuts(handle,"#define DATATYPE_KIND 10001\n");
	FPuts(handle,"#define GRADIENTSLIDER_KIND 10002\n");
	FPuts(handle,"#define PAGE_KIND 10003\n");
	FPuts(handle,"#define POPUPMENU_KIND 10004\n");
	FPuts(handle,"#define PROGRESS_KIND 10005\n");
	FPuts(handle,"#define TAPEDECK_KIND 10006\n");
	FPuts(handle,"#define TEXTFIELD_KIND 10007\n");
	FPuts(handle,"#define TOOLIMAGE_KIND 10008\n\n");
	FPuts(handle,"#define TREEVIEW_KIND 10009\n");

	FPrintf(handle,"#define VER_PROGRAMNAME \"%s\"\n",nv->nv_ProgramName);
	FPrintf(handle,"#define VER_AUTHOR \"%s\"\n",nv->nv_Author);
	FPrintf(handle,"#define VER_VERSION %ld\n",nv->nv_Version);
	FPrintf(handle,"#define VER_REVISION %ld\n",nv->nv_Revision);
	FPrintf(handle,"#define VER_SVER \"\\0$VER: \"VER_PROGRAMNAME\" %ld.%ld \"__AMIGADATE__\"\"\n\n",nv->nv_Version,nv->nv_Revision);

	if(!IsListEmpty(List(&GUI->gui_List_Window)))
	{
		FPuts(handle,"#define WA_CenterX (WA_Dummy + 0x100)\n");
		FPuts(handle,"#define WA_CenterY (WA_Dummy + 0x101)\n");
		FPuts(handle,"#define WA_RelRight (WA_Dummy + 0x102)\n");
		FPuts(handle,"#define WA_RelBottom (WA_Dummy + 0x103)\n");
		FPuts(handle,"#define WA_RelWidth (WA_Dummy + 0x104)\n");
		FPuts(handle,"#define WA_RelHeight (WA_Dummy + 0x105)\n");
		FPuts(handle,"#define WA_ToolDragBar (WA_Dummy + 0x106)\n");
		FPuts(handle,"#define WA_FontAdaptive (WA_Dummy + 0x107)\n");
	}

	SetProgress(60L);

	FUNC_GenerateErrors(handle);

	SetProgress(64L);

	if(OPT_Screen)
	{
		FPuts(handle,"extern TEXT ScrTitle[];\n");
		if(ns->ns_Type!=TYPE_CUSTOMSCREEN) FPuts(handle,"extern TEXT ScrPubName[];\n");
		if(ns->ns_Type!=TYPE_WORKBENCH)
		{
			FPuts(handle,"extern UWORD ScrWidth,ScrHeight,ScrDepth;\n");
			FPuts(handle,"extern ULONG ScrDisplayID;\n");
		}
		FPuts(handle,"extern struct Screen *Scr;\n" \
					"extern struct DrawInfo *DrawInfo;\n" \
					"extern APTR VisualInfo;\n");
	}
	if(CheckAppWindow()) FPuts(handle,"extern struct MsgPort *AppMsgPort;\n");
	if(CheckProjectWindow()) FPuts(handle,"extern struct MsgPort *ProjectMsgPort;\n");
	if(OPT_GUIFuncs) FPuts(handle,"extern struct TextAttr *Font, Attr;\n");
	FPuts(handle,"\n");

	SetProgress(66L);

/* Windows releated definitions */
	if(!IsListEmpty(List(&GUI->gui_List_Window)))
	{
		for(cnt=0,wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo),cnt++)
		{
			nw=(struct Node_Window *)OBJECT_DATA(wo);

			if(nw->nw_PageGadget==0) FPrintf(handle,"extern struct Window *%sWnd;\n",nw->nw_Label);
			GetFlag(nw->nw_MyFlags,MYWA_Project) FPrintf(handle,"extern APTR %sWApp;\n",nw->nw_Label);
			if(!IsListEmpty(List(&nw->nw_List_Gadget))) FPrintf(handle,"extern struct Gadget *%sGList;\n",nw->nw_Label);
			if(nw->nw_PageGadget==0) FPrintf(handle,"extern ULONG %sWTags[];\n",nw->nw_Label);
			if(nw->nw_PageGadget==0) FPrintf(handle,"extern struct IntuiMessage %sMsg;\n",nw->nw_Label);
			if(!IsListEmpty(List(&nw->nw_List_Gadget))) FPrintf(handle,"extern struct Gadget *%sGadgets[];\n",nw->nw_Label);
			FPrintf(handle,"#define WND_%s %ld\n",nw->nw_Label,cnt);
		}
		FPuts(handle,"\n");
		FPrintf(handle,"#define CNTWND_WINDOWS %ld\n\n",cnt+1);
	}

	SetProgress(68L);

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		if(!IsListEmpty(List(&nw->nw_List_Gadget)))
			FPrintf(handle,"extern ULONG %sGTags[];\n",nw->nw_Label);
	}

	SetProgress(70L);

	FPuts(handle,"extern struct SmartBitMapIRGB ToolBarPalette[];");
	for(io=(piObject *)GetHead(&GUI->gui_List_Image);GetSucc(io);io=(piObject *)GetSucc(io))
	{
		ni=(struct Node_Image *)OBJECT_DATA(io);
		switch(ni->ni_Type)
		{
			case IMTYPE_NORMAL:
				FPrintf(handle,"extern Object *%sImg;\nextern struct BitMap %sBitMap;\nextern struct SmartBitMapIRGB %sPalette[];\n",ni->ni_Label,ni->ni_Label,ni->ni_Label);
				break;
			case IMTYPE_POINTER:
				FPrintf(handle,"extern Object *%sPtr;\nextern struct BitMap %sBitMap;\n",ni->ni_Label,ni->ni_Label);
				break;
			case IMTYPE_TOOLBAR:
				FPrintf(handle,"extern Object *%sTbr;\nextern struct BitMap %sBitMap;\n",ni->ni_Label,ni->ni_Label);
				break;
		}
	}
	FPuts(handle,"\n");

	SetProgress(72L);

//	if(!IsListEmpty(List(&GUI->gui_List_Menu)))
//	{
		for(cnt=0,mo=(piObject *)GetHead(List(&GUI->gui_List_Menu));GetSucc(mo);mo=(piObject *)GetSucc(mo),cnt++)
		{
			nm=(struct Node_Menu *)OBJECT_DATA(mo);
			FPrintf(handle,"extern struct Menu *%sMenus;\n",nm->nm_Label);
			FPrintf(handle,"extern struct NewMenu %sNewMenu[];\n",nm->nm_Label);
			FPrintf(handle,"#define MN_%s %ld\n",nm->nm_Label,cnt);
		}
		FPrintf(handle,"#define CNTMN_MENUS %ld\n\n",cnt+1);
//	}
	FPuts(handle,"\n");

	SetProgress(75L);

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		for(cnt=0,go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go),cnt++);
		if(cnt>0) FPrintf(handle,"#define CNT_%s %ld\n",nw->nw_Label,cnt);
		for(cnt=0,go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go),cnt++)
		{
			ng=(struct Node_Gadget *)OBJECT_DATA(go);
			FPrintf(handle,"#define GD_%s %ld\n",ng->ng_Label,cnt);
		}
		FPuts(handle,"\n");
	}

	SetProgress(78L);

	FPuts(handle,"/* Prototypes */\n");
	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
		{
			ng=(struct Node_Gadget *)OBJECT_DATA(go);
			GADGET_GenerateProtos(handle,go);
		}
		FPuts(handle,"\n");
	}

	SetProgress(80L);

	if(!IsListEmpty(List(&GUI->gui_List_Menu)))
	{
		for(mo=(piObject *)GetHead(List(&GUI->gui_List_Menu));GetSucc(mo);mo=(piObject *)GetSucc(mo))
		{
			nm=(struct Node_Menu *)OBJECT_DATA(mo);
			MENU_GenerateProtos(handle,mo);
		}
		FPuts(handle,"\n");
	}

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		if(nw->nw_PageGadget==0)
		{
			FPrintf(handle,"int HandleIDCMP_%s(void);\n",nw->nw_Label);
			FPrintf(handle,"void Render_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_MENUVERIFY)
				FPrintf(handle,"int FN_MenuVerify_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_SIZEVERIFY)
				FPrintf(handle,"int FN_SizeVerify_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_REQVERIFY)
				FPrintf(handle,"int FN_ReqVerify_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_REQSET)
				FPrintf(handle,"int FN_ReqSet_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_REQCLEAR)
				FPrintf(handle,"int FN_ReqClear_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_NEWSIZE)
				FPrintf(handle,"int FN_NewSize_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_NEWPREFS)
				FPrintf(handle,"int FN_NewPrefs_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_MENUHELP)
				FPrintf(handle,"int FN_MenuHelp_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_GADGETHELP)
				FPrintf(handle,"int FN_GadgetHelp)%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_MOUSEMOVE)
				FPrintf(handle,"int FN_MouseMove_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_MOUSEBUTTONS)
				FPrintf(handle,"int FN_MouseButtons_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_RAWKEY)
				FPrintf(handle,"int FN_RawKey_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_VANILLAKEY)
				FPrintf(handle,"int FN_VanillaKey_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_INTUITICKS)
				FPrintf(handle,"int FN_IntuiTicks_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_IDCMPUPDATE)
				FPrintf(handle,"int FN_IDCMPUpdate_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_CHANGEWINDOW)
				FPrintf(handle,"int FN_ChangeWindow_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_CLOSEWINDOW)
				FPrintf(handle,"int FN_CloseWindow_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_ACTIVEWINDOW)
				FPrintf(handle,"int FN_ActiveWindow_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_INACTIVEWINDOW)
				FPrintf(handle,"int FN_InActiveWindow_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_DISKREMOVED)
				FPrintf(handle,"int FN_DiskRemoved_%s(void);\n",nw->nw_Label);
			GetFlag(nw->nw_IDCMP,IDCMP_DISKINSERTED)
				FPrintf(handle,"int FN_DiskInserted_%s(void);\n",nw->nw_Label);
		}
		FPrintf(handle,"UWORD OpenWindow_%s(void);\n",nw->nw_Label);
		FPrintf(handle,"void CloseWindow_%s(void);\n",nw->nw_Label);
		FPuts(handle,"\n");
	}

	FPuts(handle,"UWORD SetupScreen(UBYTE);\nvoid CloseDownScreen(void);\n");
	if(OPT_UseLocale) FPuts(handle,"UWORD InitApplication(STRPTR);\n");
	else FPuts(handle,"UWORD InitApplication(void);\n");
	FPuts(handle,"void FreeApplication(void);\n\n");
	if(CheckProjectWindow()) FPuts(handle,"void CloseWindowSafely(struct Window *);\nvoid StripIntuiMessages(struct MsgPort *,struct Window *);\n");
	if(OPT_GUIFuncs)
	{
		FPuts(handle,"struct Gadget *CreateGadgetExA(UWORD,UWORD,UWORD,UWORD,struct Gadget *,struct TagItem *);\n");
		FPuts(handle,"void DeleteGadgetExA(struct Gadget *,struct TagItem *);\n");
		FPuts(handle,"struct Window *OpenWindowTagsEx(ULONG, ...);\n");
		FPuts(handle,"void CloseWindowEx(struct Window *);\n");
		FPuts(handle,"void CalcLeftTop(UWORD *,UWORD *,struct TagItem *);\n");
		FPuts(handle,"void CalcRightBottom(UWORD *,UWORD *,struct TagItem *);\n");
		FPuts(handle,"UWORD AdaptX(UWORD);\n");
		FPuts(handle,"UWORD AdaptY(UWORD);\n");
		FPuts(handle,"void CalcAdaptivity(UWORD,UWORD);\n");
	}
	FPuts(handle,"STRPTR GetCatString(APTR);\n");

	FPrintf(handle,"\n#endif /* %s_H */\n",BaseName);
}

void GenerateTemplate(BPTR handle)
{
	piObject *wo,*go,*mo;
	struct Node_Window *nw;

	FPrintf(handle,"/*\n** %s_temp.c Template File\n*/\n\n",BaseName);

	FPrintf(handle,"#include\"%s.h\"\n\n",BaseName);

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		FUNC_GenerateTemplate(handle,wo);
	}
	FPuts(handle,"\n");

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
		{
			GADGET_GenerateTemplate(handle,go);
		}
		FPuts(handle,"\n");
	}

	if(!IsListEmpty(List(&GUI->gui_List_Menu)))
	{
		for(mo=(piObject *)GetHead(List(&GUI->gui_List_Menu));GetSucc(mo);mo=(piObject *)GetSucc(mo))
		{
			MENU_GenerateTemplate(handle,mo);
		}
		FPuts(handle,"\n");
	}
}

void GenerateCatalogSource(BPTR handle)
{
	piObject *wo,*go,*mo;
	struct Node_Window *nw;
	struct Node_Version *nv;
	struct Node_String *ns;
	struct Node_StringData *nd;
	struct Node_Screen *nsc;
	LONG a;

	nv=(struct Node_Version *)OBJECT_DATA(GUI->gui_Node_Version);
	ns=(struct Node_String *)OBJECT_DATA(GUI->gui_Node_String);
	nsc=(struct Node_Screen *)OBJECT_DATA(GUI->gui_Node_Screen);

	FPrintf(handle,"/*\n** %s_cat.c Catalog File\n*/\n\n",BaseName);

	FPrintf(handle,"#include<proto/locale.h>\n");
	FPrintf(handle,"#include\"%s_cat.h\"\n\n",BaseName);

	a=0;

	for(mo=(piObject *)GetHead(List(&GUI->gui_List_Menu));GetSucc(mo);mo=(piObject *)GetSucc(mo))
		MENU_GenerateCatalogSource(handle,mo,&a);

	if(OPT_Screen)
	{
		FPrintf(handle,"const struct FC_Type _MSG_SCREENTITLE_SCR = { %ld, \"%s\" };\n",a,nsc->ns_Title);
		a++;
	}

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		if(nw->nw_PageGadget==0) if(!(nw->nw_MyFlags&MYWA_DragToolHoriz)) if(!(nw->nw_MyFlags&MYWA_DragToolVert))
		{
			FPrintf(handle,"const struct FC_Type _MSG_%s_WND = { %ld, \"%s\" };\n",nw->nw_Label,a,nw->nw_WindowTitle);
			a++;
		}
		if(nw->nw_PageGadget==0) if(nw->nw_ScreenTitle[0])
		{
			FPrintf(handle,"const struct FC_Type _MSG_%s_ST_WND = { %ld, \"%s\" };\n",nw->nw_Label,a,nw->nw_ScreenTitle);
			a++;
		}

		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
			GADGET_GenerateCatalogSource(handle,go,&a);
	}

	for(nd=(struct Node_StringData *)GetHead(&ns->ns_Strings);GetSucc(Node(nd));nd=(struct Node_StringData *)GetSucc(Node(nd)),a++)
	{
		FPrintf(handle,"const struct FC_Type _MSG_%s = { %ld, \"%s\" };\n",nd->ns_Label,a,nd->ns_String);
	}
	FPuts(handle,"\n\n");
}

void GenerateCatalogHeader(BPTR handle)
{
	piObject *wo,*go,*mo;
	struct Node_Window *nw;
	struct Node_Version *nv;
	struct Node_String *ns;
	struct Node_StringData *nd;
	struct Node_Screen *nsc;

	nv=(struct Node_Version *)OBJECT_DATA(GUI->gui_Node_Version);
	ns=(struct Node_String *)OBJECT_DATA(GUI->gui_Node_String);
	nsc=(struct Node_Screen *)OBJECT_DATA(GUI->gui_Node_Screen);

	FPrintf(handle,"#ifndef %s_cat_H\n",BaseName);
	FPrintf(handle,"#define %s_cat_H\n",BaseName);
	FPrintf(handle,"/*\n** %s_cat.h Catalog Header File\n*/\n\n",BaseName);

	FPuts(handle,"#include<exec/types.h>\n\n");

	FPuts(handle,"struct FC_Type\n{\n	LONG ID;\n	STRPTR Str;\n};\n\n");

	for(mo=(piObject *)GetHead(List(&GUI->gui_List_Menu));GetSucc(mo);mo=(piObject *)GetSucc(mo))
		MENU_GenerateCatalogHeader(handle,mo);

	if(OPT_Screen)
	{
		FPrintf(handle,"extern const struct FC_Type _MSG_SCREENTITLE_SCR;\n");
		FPrintf(handle,"#define MSG_SCREENTITLE_SCR ((APTR) &_MSG_SCREENTITLE_SCR)\n",nsc->ns_Title);
	}

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		if(nw->nw_PageGadget==0) if(!(nw->nw_MyFlags&MYWA_DragToolHoriz)) if(!(nw->nw_MyFlags&MYWA_DragToolVert))
		{
			FPrintf(handle,"extern const struct FC_Type _MSG_%s_WND;\n",nw->nw_Label);
			FPrintf(handle,"#define MSG_%s_WND ((APTR) &_MSG_%s_WND)\n",nw->nw_Label,nw->nw_Label);
		}
		if(nw->nw_PageGadget==0) if(nw->nw_ScreenTitle[0])
		{
			FPrintf(handle,"extern const struct FC_Type _MSG_%s_ST_WND;\n",nw->nw_Label);
			FPrintf(handle,"#define MSG_%s_ST_WND ((APTR) &_MSG_%s_ST_WND)\n",nw->nw_Label,nw->nw_Label);
		}

		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
		{
			GADGET_GenerateCatalogHeader(handle,go);
		}
	}

	for(nd=(struct Node_StringData *)GetHead(&ns->ns_Strings);GetSucc(Node(nd));nd=(struct Node_StringData *)GetSucc(Node(nd)))
	{
		FPrintf(handle,"extern const struct FC_Type _MSG_%s;\n",nd->ns_Label);
		FPrintf(handle,"#define MSG_%s ((APTR) &_MSG_%s)\n",nd->ns_Label,nd->ns_Label);
	}
	FPuts(handle,"\n");

	FPrintf(handle,"\n#endif /* %s_cat_H */\n",BaseName);
}

void GenerateCatalogDescription(BPTR handle)
{
	piObject *wo,*go,*mo;
	struct Node_Window *nw;
	struct Node_Version *nv;
	struct Node_String *ns;
	struct Node_StringData *nd;
	struct Node_Screen *nsc;

	nv=(struct Node_Version *)OBJECT_DATA(GUI->gui_Node_Version);
	ns=(struct Node_String *)OBJECT_DATA(GUI->gui_Node_String);
	nsc=(struct Node_Screen *)OBJECT_DATA(GUI->gui_Node_Screen);

	FPrintf(handle,";\n; %s locale description file\n;\n",nv->nv_ProgramName);

	for(mo=(piObject *)GetHead(List(&GUI->gui_List_Menu));GetSucc(mo);mo=(piObject *)GetSucc(mo))
		MENU_GenerateCatalogDescription(handle,mo);

	if(OPT_Screen) FPrintf(handle,"MSG_SCREENTITLE_SCR (//)\n%s\n;\n",nsc->ns_Title);

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		if(nw->nw_PageGadget==0) if(!(nw->nw_MyFlags&MYWA_DragToolHoriz)) if(!(nw->nw_MyFlags&MYWA_DragToolVert))
			FPrintf(handle,"MSG_%s_WND (//)\n%s\n;\n",nw->nw_Label,nw->nw_WindowTitle);
		if(nw->nw_PageGadget==0) if(nw->nw_ScreenTitle[0]) FPrintf(handle,"MSG_%s_ST_WND (//)\n%s\n;\n",nw->nw_Label,nw->nw_ScreenTitle);

		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
			GADGET_GenerateCatalogDescription(handle,go);
	}

	for(nd=(struct Node_StringData *)GetHead(&ns->ns_Strings);GetSucc(Node(nd));nd=(struct Node_StringData *)GetSucc(Node(nd)))
	{
		FPrintf(handle,"MSG_%s (//)\n%s\n;\n",nd->ns_Label,nd->ns_String);
	}
}

void GenerateCatalogTranslation(BPTR handle)
{
	piObject *wo,*go,*mo;
	struct Node_Window *nw;
	struct Node_Version *nv;
	struct Node_String *ns;
	struct Node_StringData *nd;
	struct Node_Screen *nsc;

	nv=(struct Node_Version *)OBJECT_DATA(GUI->gui_Node_Version);
	ns=(struct Node_String *)OBJECT_DATA(GUI->gui_Node_String);
	nsc=(struct Node_Screen *)OBJECT_DATA(GUI->gui_Node_Screen);

	FPrintf(handle,"## version $VER: %s.catalog %ld.%ld (xx.xx.xx)\n## language <enter language here>\n## codeset 0\n",nv->nv_ProgramName,nv->nv_Version,nv->nv_Revision);
	FPrintf(handle,";\n; %s locale translation file\n;\n",nv->nv_ProgramName);

	for(mo=(piObject *)GetHead(List(&GUI->gui_List_Menu));GetSucc(mo);mo=(piObject *)GetSucc(mo))
		MENU_GenerateCatalogTranslation(handle,mo);

	if(OPT_Screen) FPrintf(handle,"MSG_SCREENTITLE_SCR\n\n;%s\n;\n",nsc->ns_Title);

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		if(nw->nw_PageGadget==0) if(!(nw->nw_MyFlags&MYWA_DragToolHoriz)) if(!(nw->nw_MyFlags&MYWA_DragToolVert))
			FPrintf(handle,"MSG_%s_WND\n\n;%s\n;\n",nw->nw_Label,nw->nw_WindowTitle);
		if(nw->nw_PageGadget==0) if(nw->nw_ScreenTitle[0]) FPrintf(handle,"MSG_%s_ST_WND\n\n;%s\n;\n",nw->nw_Label,nw->nw_ScreenTitle);

		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
			GADGET_GenerateCatalogTranslation(handle,go);
	}

	for(nd=(struct Node_StringData *)GetHead(&ns->ns_Strings);GetSucc(Node(nd));nd=(struct Node_StringData *)GetSucc(Node(nd)))
	{
		FPrintf(handle,"MSG_%s\n\n;%s\n;\n",nd->ns_Label,nd->ns_String);
	}
}


int FN_TEMPLATE_Clicked(void)
{
	ULONG code=PrefsMsg.Code;

	DoFlag(SP.spr_Flags,MYPR_Template);
	return 1;
}

int FN_SCREEN_Clicked(void)
{
	ULONG code=PrefsMsg.Code;

	DoFlag(SP.spr_Flags,MYPR_Screen);
	return 1;
}

int FN_LOCALIZE_Clicked(void)
{
	ULONG code=PrefsMsg.Code;

	DoFlag(SP.spr_Flags,MYPR_Localize);
	return 1;
}

int FN_GUIFUNCS_Clicked(void)
{
	ULONG code=PrefsMsg.Code;

	DoFlag(SP.spr_Flags,MYPR_GUIFuncs);
	return 1;
}

int FN_SAVE_Clicked(void)
{
	SavePrefs("ENV:GadToolsBox/c.generator.prefs");
	SavePrefs("ENVARC:GadToolsBox/c.generator.prefs");
	return 0;
}

int FN_USE_Clicked(void)
{
	SavePrefs("ENV:GadToolsBox/c.generator.prefs");
	return 0;
}

int FN_CANCEL_Clicked(void)
{
	return 0;
}

int FN_CloseWindow_Prefs(void)
{
	return 0;
}

#define ID_CGEN MAKE_ID('C','G','E','N')

void SavePrefs(STRPTR name)
{
	struct IFFHandle *iffh;
	struct PrefHeader prefhdr={0,0,0};
	LONG error=0;

	if(iffh=AllocIFF())
	{
		if(iffh->iff_Stream=Open(name,MODE_NEWFILE))
		{
			InitIFFasDOS(iffh);
			unless(error=OpenIFF(iffh,IFFF_WRITE))
			{
				unless(PushChunk(iffh,ID_PREF,ID_FORM,IFFSIZE_UNKNOWN))
				{
					if(error=PushChunk(iffh,NULL,ID_PRHD,sizeof(struct PrefHeader))) IFF_Error(error);
					WriteChunkBytes(iffh,(APTR)&prefhdr,sizeof(struct PrefHeader));
					if(error=PopChunk(iffh)) IFF_Error(error);
					if(error=PushChunk(iffh,NULL,ID_CGEN,sizeof(struct SavePrefs))) IFF_Error(error);
					WriteChunkBytes(iffh,(APTR)&SP,sizeof(struct SavePrefs));
					if(error=PopChunk(iffh)) IFF_Error(error);
				}
skip:;
				CloseIFF(iffh);
			} else IFF_Error(error);
			Close(iffh->iff_Stream);
		}
		FreeIFF(iffh);
	}
}

void LoadPrefs(STRPTR name)
{
	struct IFFHandle *iffh;
	struct ContextNode *cn;
	LONG error=0;

	if(iffh=AllocIFF())
	{
		if(iffh->iff_Stream=Open(name,MODE_OLDFILE))
		{
			InitIFFasDOS(iffh);
			unless(error=OpenIFF(iffh,IFFF_READ))
			{
				if(error=StopChunk(iffh,ID_PREF,ID_CGEN)) IFF_Error(error);
				while(1)
				{
					error=ParseIFF(iffh,IFFPARSE_SCAN);
					if(error==IFFERR_EOC) continue;       /* enter next context */
					else if(error) break;
					if(cn=CurrentChunk(iffh))
					{
						if(cn->cn_Type==ID_PREF)
						{
							switch(cn->cn_ID)
							{
								case ID_CGEN:
									ReadChunkBytes(iffh,(APTR)&SP,sizeof(struct SavePrefs));
									break;
							}
						}
					}
				}
skip:;
				CloseIFF(iffh);
			} else IFF_Error(error);
			Close(iffh->iff_Stream);
		}
		else
		{
			SP.spr_Flags=NULL;
		}
		FreeIFF(iffh);
	}
}
