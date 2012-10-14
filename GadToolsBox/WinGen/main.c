/*
** Win32Res Source code generator
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
void GenerateMainScript(BPTR);
void GenerateHeader(BPTR);
void GenerateImages(BPTR);
BOOL CheckPlugIn(ULONG);
BOOL CheckBOOPSIWindow(piObject *);
BOOL CheckDataTypeWindow(piObject *);
BOOL CheckGadToolsWindow(piObject *);
BOOL CheckPlugInWindow(piObject *,ULONG);
BOOL CheckAppWindow(void);
BOOL CheckToolBarWindows(void);
BOOL CheckToolBarWindow(piObject *);
BOOL CheckProjectWindow(void);
STRPTR ConvertString(STRPTR,BOOL);

void GADGET_GenerateGadget(BPTR,piObject *);
void WIND_GenerateWindow(BPTR,piObject *);
void WIND_GenerateInit(BPTR,piObject *);
void MENU_GenerateMenu(BPTR,piObject *);
void FRAME_GenerateFrame(BPTR,piObject *);

#define SetProgress(prg) SetGadgetAttrs(GTBSem->gs_Progress,GTBSem->gs_Window,NULL,PROG_CurVal,prg,TAG_DONE)
#define IFF_Error(num)		\
		{											\
			printf("IFFError #%d",num);	\
			goto skip;	\
		}

struct Lang
{
	STRPTR str;
	STRPTR sub;
	UWORD codepage;
};

struct Lang languages[]=
{
	"ENGLISH","ENGLISH_UK",1252,
	"ENGLISH","ENGLISH_US",1252,
	"POLISH","DEFAULT",1250,
};

/*
** Variables
*/

struct GTBSemaphore *GTBSem;
struct GUI *GUI;
extern struct Screen *Scr;
STRPTR BaseName;
BOOL OPT_ConvUS,OPT_CurLang;
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

	LoadPrefs("ENV:GadToolsBox/win32resource.generator.prefs");

	if(setprefs)
	{
		if(!InitApplication(NULL))
		{
			if(!SetupScreen(0))
			{
				if(!OpenWindow_Prefs())
				{
					SetCheckMark(PrefsWnd,PrefsGadgets[GD_CONVUNDERSCORES],SP.spr_Flags&MYPR_ConvUS);
					SetCycle(PrefsWnd,PrefsGadgets[GD_LANGUAGE],SP.spr_CodePage);
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
	OPT_ConvUS=SP.spr_Flags&MYPR_ConvUS;
	OPT_CurLang=SP.spr_CodePage;
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

	sprintf(fname,"%s.rc",basename);
	if(file=Open(fname,MODE_NEWFILE))
	{
		GenerateMainScript(file);
		Close(file);
	}

	SetProgress(50L);

	sprintf(fname,"%s.h",basename);
	if(file=Open(fname,MODE_NEWFILE))
	{
		GenerateHeader(file);
		Close(file);
	}

	SetProgress(100L);

	return TRUE;
}

void GenerateMainScript(BPTR handle)
{
	piObject *wo,*go,*mo;
	struct Node_Window *nw;
	struct Node_Menu *nm;
	struct Node_Version *nv;
	struct Node_String *ns;
	struct Node_StringData *nd;

	nv=(struct Node_Version *)OBJECT_DATA(GUI->gui_Node_Version);

	SetProgress(0L);

/* Header */
	FPrintf(handle,"//" \
		"\n// Resource script generated by GadToolsBox %ld.%ld (c) by Tomasz Muszynski\n" \
		"// GUI designed by: %s\n" \
		"//\n\n",GUI->gui_Version,GUI->gui_Revision,nv->nv_Author);

	SetProgress(2L);

/* Includes */
	FPrintf(handle,"#include\"%s.h\"\n",BaseName);
	FPuts(handle,"\n");

	FPrintf(handle,"#define APSTUDIO_READONLY_SYMBOLS\n"\
		"/////////////////////////////////////////////////////////////////////////////\n"\
		"//\n"\
		"// Generated from the TEXTINCLUDE 2 resource.\n"\
		"//\n"\
		"#include \"afxres.h\"\n"\
		"\n"\
		"/////////////////////////////////////////////////////////////////////////////\n"\
		"#undef APSTUDIO_READONLY_SYMBOLS\n"\
		"\n"\
		"/////////////////////////////////////////////////////////////////////////////\n"\
		"// English resources\n"\
		"\n"\
		"#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_PLK)\n"\
		"#ifdef _WIN32\n"\
		"LANGUAGE LANG_%s, SUBLANG_%s\n"\
		"#pragma code_page(%ld)\n"\
		"#endif //_WIN32\n\n",languages[OPT_CurLang].str,languages[OPT_CurLang].sub,languages[OPT_CurLang].codepage);

	SetProgress(10L);

	SetProgress(20L);

/* Windows releated definitions */
	FPuts(handle,"\n/////////////////////////////////////////////////////////////////////////////\n"\
		"//\n"\
		"// Dialog\n"\
		"//\n");

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		FPuts(handle,"\n");
		WIND_GenerateWindow(handle,wo);
		if(!IsListEmpty(List(&nw->nw_List_Gadget)))
		{
			FPuts(handle,"BEGIN\n");
			for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
				GADGET_GenerateGadget(handle,go);
			for(go=(piObject *)GetHead(List(&nw->nw_List_Frame));GetSucc(go);go=(piObject *)GetSucc(go))
				FRAME_GenerateFrame(handle,go);
			FPuts(handle,"END\n");
		}
	}
	FPuts(handle,"\n");

	SetProgress(30L);

/* Menu definitions */
	FPuts(handle,"\n/////////////////////////////////////////////////////////////////////////////\n"\
		"//\n"\
		"// Menu\n"\
		"//\n");

	for(mo=(piObject *)GetHead(List(&GUI->gui_List_Menu));GetSucc(mo);mo=(piObject *)GetSucc(mo))
	{
		nm=(struct Node_Menu *)OBJECT_DATA(mo);
		MENU_GenerateMenu(handle,mo);
	}

	SetProgress(50L);

/* Windows releated definitions */
	FPuts(handle,"\n/////////////////////////////////////////////////////////////////////////////\n"\
		"//\n"\
		"// Dialog Info\n"\
		"//\n");

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		WIND_GenerateInit(handle,wo);
	}
	FPuts(handle,"\n");

/* Menu definitions */
	FPuts(handle,"\n#ifndef _MAC\n/////////////////////////////////////////////////////////////////////////////\n"\
		"//\n"\
		"// Version\n"\
		"//\n\n");
	FPuts(handle,"VS_VERSION_INFO VERSIONINFO\n");
	FPrintf(handle," FILEVERSION %ld,%ld\n",nv->nv_Version,nv->nv_Revision);
	FPrintf(handle," PRODUCTVERSION %ld,%ld\n",nv->nv_Version,nv->nv_Revision);
	FPuts(handle," FILEFLAGSMASK 0x3fL\n");
	FPuts(handle,"#ifdef _DEBUG\n");
	FPuts(handle," FILEFLAGS 0x1L\n");
	FPuts(handle,"#else\n");
	FPuts(handle," FILEFLAGS 0x0L\n");
	FPuts(handle,"#endif\n");
	FPuts(handle," FILEOS 0x40004L\n");
	FPuts(handle," FILETYPE 0x1L\n");
	FPuts(handle," FILESUBTYPE 0x0L\n");
	FPuts(handle,"BEGIN\n");
	FPuts(handle,"    BLOCK \"StringFileInfo\"\n");
	FPuts(handle,"    BEGIN\n");
	FPuts(handle,"        BLOCK \"041504b0\"\n");
	FPuts(handle,"        BEGIN\n");
//	FPuts(handle,"            VALUE \"Comments\", \"\\0\"\n");
//	FPuts(handle,"            VALUE \"CompanyName\", \"Union Systems\\0\"\n");
//	FPuts(handle,"            VALUE \"FileDescription\", \"To jest example skrypt\\0\"\n");
	FPrintf(handle,"            VALUE \"FileVersion\", \"%ld,%ld\\0\"\n",nv->nv_Version,nv->nv_Revision);
	FPrintf(handle,"            VALUE \"InternalName\", \"%s\\0\"\n",nv->nv_ProgramName);
	FPrintf(handle,"            VALUE \"LegalCopyright\", \"Copyright © 1998 by %s\\0\"\n",nv->nv_Author);
//	FPuts(handle,"            VALUE \"LegalTrademarks\", \"TM by Union\\0\"\n");
	FPrintf(handle,"            VALUE \"OriginalFilename\", \"%s.RC\\0\"\n",BaseName);
	FPrintf(handle,"            VALUE \"ProductName\", \"%s\\0\"\n",nv->nv_ProgramName);
	FPrintf(handle,"            VALUE \"ProductVersion\", \"%ld,%ld\\0\"\n",nv->nv_Version,nv->nv_Revision);
	FPuts(handle,"        END\n");
	FPuts(handle,"    END\n");
	FPuts(handle,"    BLOCK \"VarFileInfo\"\n");
	FPuts(handle,"    BEGIN\n");
	FPuts(handle,"        VALUE \"Translation\", 0x415, 1200\n");
	FPuts(handle,"    END\n");
	FPuts(handle,"END\n");
	FPuts(handle,"\n");
	FPuts(handle,"#endif    // !_MAC\n");

	FPrintf(handle,"#ifdef APSTUDIO_INVOKED\n"\
		"/////////////////////////////////////////////////////////////////////////////\n"\
		"//\n"\
		"// TEXTINCLUDE\n"\
		"//\n"\
		"\n"\
		"1 TEXTINCLUDE DISCARDABLE \n"\
		"BEGIN\n"\
		"    \"%s.h\\0\"\n"\
		"END\n"\
		"\n"\
		"2 TEXTINCLUDE DISCARDABLE \n"\
		"BEGIN\n"\
		"    \"#include \"\"afxres.h\"\"\\r\\n\"\n"\
		"    \"\\0\"\n"\
		"END\n"\
		"\n"\
		"3 TEXTINCLUDE DISCARDABLE \n"\
		"BEGIN\n"\
		"    \"\\r\\n\"\n"\
		"    \"\\0\"\n"\
		"END\n"\
		"\n"\
		"#endif    // APSTUDIO_INVOKED\n\n",BaseName);

	ns=(struct Node_String *)OBJECT_DATA(GUI->gui_Node_String);
	if(!IsListEmpty(List(&ns->ns_Strings)))
	{
		FPuts(handle,"/////////////////////////////////////////////////////////////////////////////\n"\
			"//\n"\
			"// String Table\n"\
			"//\n\nSTRINGTABLE DISCARDABLE \n"\
			"BEGIN\n");
		for(nd=(struct Node_StringData *)GetHead(&ns->ns_Strings);GetSucc(Node(nd));nd=(struct Node_StringData *)GetSucc(Node(nd)))
		{
			FPrintf(handle,"    MSG_%s \"%s\"\n",nd->ns_Label,nd->ns_String);
		}
		FPuts(handle,"END\n");
	}

	FPuts(handle,"#endif    // English resources\n"\
		"/////////////////////////////////////////////////////////////////////////////\n"\
		"\n"\
		"\n"\
		"\n"\
		"#ifndef APSTUDIO_INVOKED\n"\
		"/////////////////////////////////////////////////////////////////////////////\n"\
		"//\n"\
		"// Generated from the TEXTINCLUDE 3 resource.\n"\
		"//\n"\
		"\n"\
		"\n"\
		"/////////////////////////////////////////////////////////////////////////////\n"\
		"#endif    // not APSTUDIO_INVOKED\n"\
		"\n");
}

void GenerateHeader(BPTR handle)
{
	piObject *wo,*go,*mo;
	struct Node_Window *nw;
	struct Node_Gadget *ng;
	struct Node_Menu *nm;
	struct Node_Version *nv;
	struct Node_String *ns;
	struct Node_StringData *nd;
	struct TagItem *ti;
	struct Node *n;
	ULONG cnt,a;

	SetProgress(55L);

	nv=(struct Node_Version *)OBJECT_DATA(GUI->gui_Node_Version);

	FPrintf(handle,"//{{NO_DEPENDENCIES}}" \
		"\n// Include file generated by GadToolsBox %ld.%ld (c) by Tomasz Muszynski\n" \
		"// Used by %s\n//\n\n",GUI->gui_Version,GUI->gui_Revision,BaseName);

	FPrintf(handle,"#define VER_PROGRAMNAME \"%s\"\n",nv->nv_ProgramName);
	FPrintf(handle,"#define VER_AUTHOR \"%s\"\n",nv->nv_Author);
	FPrintf(handle,"#define VER_VERSION %ld\n",nv->nv_Version);
	FPrintf(handle,"#define VER_REVISION %ld\n",nv->nv_Revision);
	FPrintf(handle,"#define VER_SVER \"\\0$VER: \"VER_PROGRAMNAME\" %ld.%ld \"__AMIGADATE__\"\"\n\n",nv->nv_Version,nv->nv_Revision);

	cnt=0;

	SetProgress(60L);

	ns=(struct Node_String *)OBJECT_DATA(GUI->gui_Node_String);
	for(nd=(struct Node_StringData *)GetHead(&ns->ns_Strings);GetSucc(Node(nd));nd=(struct Node_StringData *)GetSucc(Node(nd)),cnt++)
	{
		FPrintf(handle,"#define MSG_%s %ld\n",nd->ns_Label,cnt);
	}

	SetProgress(65L);

/* Windows releated definitions */
	if(!IsListEmpty(List(&GUI->gui_List_Window)))
	{
		for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo),cnt++)
		{
			nw=(struct Node_Window *)OBJECT_DATA(wo);

			FPrintf(handle,"#define WND_%s %ld\n",nw->nw_Label,cnt);
		}
		FPuts(handle,"\n");
	}

	SetProgress(70L);

	for(mo=(piObject *)GetHead(List(&GUI->gui_List_Menu));GetSucc(mo);mo=(piObject *)GetSucc(mo),cnt++)
	{
		nm=(struct Node_Menu *)OBJECT_DATA(mo);
		FPrintf(handle,"#define MN_%s %ld\n",nm->nm_Label,cnt);
	}
	FPuts(handle,"\n");

	SetProgress(75L);

	for(wo=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo);wo=(piObject *)GetSucc(wo))
	{
		nw=(struct Node_Window *)OBJECT_DATA(wo);
		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go),cnt++)
		{
			ng=(struct Node_Gadget *)OBJECT_DATA(go);
			switch(ng->ng_PlugInID)
			{
				case MX_ID:
					if(ti=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
						for(n=GetHead(ti->ti_Data),a=0;GetSucc(n);n=GetSucc(n),a++,cnt++)
							FPrintf(handle,"#define GD_%s_%ld %ld\n",ng->ng_Label,a,cnt);
					break;
				default:
					FPrintf(handle,"#define GD_%s %ld\n",ng->ng_Label,cnt);
					break;
			}
		}
		FPuts(handle,"\n");
	}

	SetProgress(78L);

	FPrintf(handle,"// Next default values for new objects\n"\
		"// \n"\
		"#ifdef APSTUDIO_INVOKED\n"\
		"#ifndef APSTUDIO_READONLY_SYMBOLS\n"\
		"#define _APS_NEXT_RESOURCE_VALUE        %ld\n"\
		"#define _APS_NEXT_COMMAND_VALUE         %ld\n"\
		"#define _APS_NEXT_CONTROL_VALUE         1000\n"\
		"#define _APS_NEXT_SYMED_VALUE           101\n"\
		"#endif\n"\
		"#endif\n",0,0); //ResVal,GadVal);
}

int FN_CONVUNDERSCORES_Clicked(void)
{
	ULONG code=PrefsMsg.Code;

	DoFlag(SP.spr_Flags,MYPR_ConvUS);
	return 1;
}

int FN_LANGUAGE_Clicked(void)
{
	SP.spr_CodePage=PrefsMsg.Code;
	return 1;
}

int FN_SAVE_Clicked(void)
{
	SavePrefs("ENV:GadToolsBox/win32resource.generator.prefs");
	SavePrefs("ENVARC:GadToolsBox/win32resource.generator.prefs");
	return 0;
}

int FN_USE_Clicked(void)
{
	SavePrefs("ENV:GadToolsBox/win32resource.generator.prefs");
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

#define ID_W32R MAKE_ID('W','3','2','R')

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
					if(error=PushChunk(iffh,NULL,ID_W32R,sizeof(struct SavePrefs))) IFF_Error(error);
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
				if(error=StopChunk(iffh,ID_PREF,ID_W32R)) IFF_Error(error);
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
								case ID_W32R:
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

TEXT tmptext[1024];

STRPTR ConvertString(STRPTR str,BOOL us)
{
	int a;

	strcpy(tmptext,str);
	for(a=0;a<strlen(tmptext);a++)
	{
		switch(tmptext[a])
		{
			case '_': if(OPT_ConvUS && us) tmptext[a]='&'; break;
		}
		if(OPT_CurLang==2)	// Polish
		{
			switch(tmptext[a])
			{
				case 'ë': tmptext[a]='e'; break;
				case 'ó': tmptext[a]='o'; break;
				case 'â': tmptext[a]='a'; break;
				case 'ô': tmptext[a]='s'; break;
				case 'î': tmptext[a]='l'; break;
				case 'û': tmptext[a]='z'; break;
				case 'ú': tmptext[a]='z'; break;
				case 'ê': tmptext[a]='c'; break;
				case 'ï': tmptext[a]='n'; break;
				case 'Ë': tmptext[a]='E'; break;
				case 'Ó': tmptext[a]='O'; break;
				case 'Â': tmptext[a]='A'; break;
				case 'Ô': tmptext[a]='S'; break;
				case 'Î': tmptext[a]='L'; break;
				case 'Û': tmptext[a]='Z'; break;
				case 'Ú': tmptext[a]='Z'; break;
				case 'Ê': tmptext[a]='C'; break;
				case 'Ï': tmptext[a]='N'; break;
			}
		}
	}
	return tmptext;
}
