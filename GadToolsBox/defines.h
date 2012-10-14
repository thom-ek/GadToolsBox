#ifndef GTB_DEFINES_H
#define GTB_DEFINES_H
/*
** Defines
*/

#include<proto/intuition.h>

#define ID_GTBX MAKE_ID('G','T','B','X')  // GadToolsBox Chunk (GUI File)

/*
** Last Commands
*/

#define CMD_NONE 0
#define CMD_CUT 1
#define CMD_PASTE 2
#define CMD_CHANGE 3		// Size, Move

/*
** Gadget/Frame click position
*/

#define CP_OUTSIDE 0
#define CP_SIZE_LEFTUP 1
#define CP_SIZE_RIGHTUP 2
#define CP_SIZE_RIGHTDOWN 3
#define CP_SIZE_LEFTDOWN 4
#define CP_SIZE_LEFT 5
#define CP_SIZE_RIGHT 6
#define CP_SIZE_UP 7
#define CP_SIZE_DOWN 8
#define CP_MOVE 9

/*
** Gadget/Frame action
*/

#define GS_NOTHING 0
#define GS_CREATING 1
#define GS_MOVING 2
#define GS_SIZING_LEFTUP 3
#define GS_SIZING_RIGHTUP 4
#define GS_SIZING_RIGHTDOWN 5
#define GS_SIZING_LEFTDOWN 6
#define GS_SIZING_LEFT 7
#define GS_SIZING_RIGHT 8
#define GS_SIZING_UP 9
#define GS_SIZING_DOWN 10
#define GS_SELECTING 11

/*
** Properties Mode
*/

#define PM_NONE 0
#define PM_WINDOW 1
#define PM_GADGET 2
#define PM_FRAME 3
#define PM_IMAGE 4
#define PM_MENU 5
#define PM_TEXT 6
#define PM_SCREEN 7
#define PM_STRING 8

/*
** Edit Mode
*/

#define MODE_TEST 0
#define MODE_GADGET 1
#define MODE_FRAME 2

#define GD_DUMMY (10000)
#define GD_LEFTBUTTON (GD_DUMMY)
#define GD_RIGHTBUTTON (GD_DUMMY+1)
#define GD_HORIZBUTTON (GD_DUMMY+2)
#define GD_DOWNBUTTON (GD_DUMMY+3)
#define GD_UPBUTTON (GD_DUMMY+4)
#define GD_VERTBUTTON (GD_DUMMY+5)

/*
** Align types
*/

#define ALIGN_LEFT 0
#define ALIGN_HCENTER 1
#define ALIGN_RIGHT 2
#define ALIGN_TOP 3
#define ALIGN_VCENTER 4
#define ALIGN_BOTTOM 5
#define SPREAD_HORIZ 6
#define SPREAD_VERT 7
#define CLONE_WIDTH 8
#define CLONE_HEIGHT 9
#define CLONE_BOTH 10

/*
** Usefull macros
*/

#define unless(x) if(!(x))
#define ListEmpty(x) ((((struct List *)x)->lh_TailPred) == ((struct Node *)x))
#define WhileNode(l,n) for(n=(l)->lh_Head;n->ln_Succ!=0;n=n->ln_Succ)
#define SetFlag(x,f) x|=f
#define GetFlag(x,f) if(x&f)
#define ClearFlag(x,f) x&=(f^-1)
#define DoFlag(x,f) if(code) SetFlag(x,f); else ClearFlag(x,f);
#define Node(x) ((struct Node *)x)
#define List(x) ((struct List *)x)
#define DisableGadget(g,w,s) GT_SetGadgetAttrs(g,w,NULL,GA_Disabled,s,TAG_DONE)

#define SetCheckMark(w,g,a) GT_SetGadgetAttrs(g,w,NULL,GTCB_Checked,a,TAG_DONE)
#define SetCycle(w,g,a) GT_SetGadgetAttrs(g,w,NULL,GTCY_Active,a,TAG_DONE)
#define SetString(w,g,a) GT_SetGadgetAttrs(g,w,NULL,GTST_String,a,TAG_DONE)
#define SetInteger(w,g,a) GT_SetGadgetAttrs(g,w,NULL,GTIN_Number,a,TAG_DONE)
#define SetNumber(w,g,a) GT_SetGadgetAttrs(g,w,NULL,GTNM_Number,a,TAG_DONE)
#define SetListView(w,g,a) GT_SetGadgetAttrs(g,w,NULL,GTLV_Labels,a,TAG_DONE)
#define SetText(w,g,a) GT_SetGadgetAttrs(g,w,NULL,GTTX_Text,a,GTTX_Clipped,TRUE,TAG_DONE)
#define SetMX(w,g,a) GT_SetGadgetAttrs(g,w,NULL,GTMX_Active,a,TAG_DONE)

#define GetCheckMark(w,g,a) GT_GetGadgetAttrs(g,w,NULL,GTCB_Checked,a,TAG_DONE)
#define GetCycle(w,g,a) GT_GetGadgetAttrs(g,w,NULL,GTCY_Active,a,TAG_DONE)
#define GetListView(w,g,a) GT_GetGadgetAttrs(g,w,NULL,GTLV_Selected,a,TAG_DONE)
#define GetMX(w,g,a) GT_GetGadgetAttrs(g,w,NULL,GTMX_Active,a,TAG_DONE)

/*
** Prefs
*/

#include<prefs/prefhdr.h>

#define ID_PREF MAKE_ID('P','R','E','F')

#define WIN_PROPERTIES 0
#define WIN_TOOLBAR_MAIN 1
#define WIN_TOOLBAR_GADGETS 2
#define WIN_STATUS 3
#define WIN_TOOLBAR_IMAGE 4
#define WIN_OBJECT 5
#define WIN_OBJECT_DIMS 6

#define PRFLAG_AUTOBACKUP 1<<0
#define PRFLAG_SIZEZOOM 1<<1
#define PRFLAG_FASTREDRAW 1<<2
#define PRFLAG_WRITEICONS 1<<3
#define PRFLAG_OBJECTSOPEN 1<<4
#define PRFLAG_STATUSOPEN 1<<5
#define PRFLAG_PROPSOPEN 1<<6

struct Prefs
{
	ULONG pr_Flags;
	ULONG pr_DisplayID;
	UWORD pr_Width,pr_Height,pr_Depth;
	TEXT pr_Generator[108];
	UWORD pr_Grid[2];
	BOOL pr_GridOn,pr_GridShow;
	TEXT pr_FontName[30];
	UWORD pr_FontSize;
	struct
	{
		UWORD X,Y;
	} pr_WinPos[10];
};

/*
** GUI
*/

#define ID_GGUI MAKE_ID('G','G','U','I')

struct GUI
{
	UBYTE gui_Version,gui_Revision;
	STRPTR gui_PubScreenName;

	UWORD gui_Grid[2];
	BOOL gui_GridOn,gui_GridShow;

	UBYTE gui_Mode;
	struct Prefs *gui_Prefs;

	piObject *gui_Node_Version; 					// $VER string
	piObject *gui_Node_Screen;						// Screen
	piObject *gui_Node_String;						// List of locale strings
	struct MinList gui_List_Window;				// List of windows
	struct MinList gui_List_Menu;					// List of menus
	struct MinList gui_List_Image;				// List of images/mouse pointers/toolbars
	struct MinList gui_List_Binary;				// List of binary data
};

/*
** GadToolsBox Public Semaphore
*/

struct GTBSemaphore
{
	struct SignalSemaphore gs_Semaphore;
	struct GUI *gs_GUI;
	struct Window *gs_Window;
	struct Gadget *gs_Progress;
	struct TextFont *gs_FontTF;
};

typedef struct
{
	ULONG kf_KeyID;						// 4
	UWORD kf_KeyVersion;      // 6
	ULONG kf_ProgramID;       // 10
	UWORD kf_Version;         // 12
	UWORD kf_Revision;        // 14
	TEXT kf_Serial[9];				// 23
	ULONG kf_SerialNum;				// 27
	ULONG kf_Checksum;        // 31
//	STRPTR kf_UserName;     //
//	STRPTR kf_Address;      //
//	STRPTR kf_EMail;  	    //
//	ULONG kf_Checksum2;     //
} KEYFILE;

#endif
