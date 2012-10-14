#include<exec/types.h>
#include<proto/iffparse.h>
#include<proto/listview.h>

#define GadgetData(x) (((UBYTE *)x)+sizeof(struct Node_Gadget))	// Get Pointer to GadToolsData
#define OBJECT_DATA(o) (((UBYTE *)o)+sizeof(piObject))
#define GetHead(x) (((struct List *)x)->lh_Head)
#define GetSucc(x) (x->ln_Succ) 
#define GetPred(x) (x->ln_Pred)
#define Node(x) ((struct Node *)x)
#define List(x) ((struct List *)x)
#define SetFlag(x,f) x|=f
#define GetFlag(x,f) if(x&f)
#define ClearFlag(x,f) x&=(f^-1)
#define DoFlag(x,f) if(code) SetFlag(x,f); else ClearFlag(x,f);
#define ListEmpty(x) ((((struct List *)x)->lh_TailPred) == ((struct Node *)x))
#define unless(x) if(!(x))
#define SetCheckMark(w,g,a) GT_SetGadgetAttrs(g,w,NULL,GTCB_Checked,a,TAG_DONE)
#define SetCycle(w,g,a) GT_SetGadgetAttrs(g,w,NULL,GTCY_Active,a,TAG_DONE)
#define Bit8(x) ((UBYTE)((x&0xff000000)>>24))

typedef struct piClass piClass;
typedef struct piObject piObject;

struct piClass
{
	ULONG (*Dispatcher)(piClass *s,piObject *o,Msg m);
	piClass *Super;
	ULONG Size;
	ULONG Type;
};

struct piObject
{
	piObject *ln_Succ;
	piObject *ln_Pred;
	UWORD ln_pad;
	STRPTR ln_Name;
	piClass *piClass;
};

struct EditList
{
	struct List **List;
	ULONG *Active;
};

struct ColorPalette
{
	ULONG cp_I,cp_R,cp_G,cp_B;
};

#define IMTYPE_NORMAL 0
#define IMTYPE_TOOLBAR 1
#define IMTYPE_POINTER 2

struct Node_Image
{
	ULONG ni_ID;
	struct Window *ni_Window;
	struct Page *ni_Pages;
	Object *ni_Draw,*ni_Preview,*ni_UpButton,*ni_DownButton,*ni_VertProp,*ni_LeftButton,*ni_RightButton,*ni_HorizProp;
	LONG ni_Top,ni_Left;
	struct BitMap *ni_BitMap;
	struct ColorPalette *ni_CT;
	UBYTE *ni_Pens;

	UBYTE ni_Label[40];
	UBYTE ni_Type,ni_Depth;
	UWORD ni_Width,ni_Height;
	UWORD ni_PointX,ni_PointY;
	ULONG ni_Flags;
};

struct Node_Gadget
{
	struct Gadget *ng_Gadget;
	piObject *ng_Parent;	// Parent Object (NULL or page.gadget)
	piObject *ng_Window;	// Parent Window
	struct TagItem *ng_Tags;
	struct TagItem *ng_OwnTags;
	UBYTE *ng_TagTypes;
	UBYTE *ng_OwnTagTypes;
	struct Page *ng_Pages;
	struct EditList ng_EditList;

	UWORD ng_X,ng_Y,ng_Width,ng_Height;
	UBYTE ng_RelX,ng_RelY,ng_RelWidth,ng_RelHeight;
	ULONG ng_PlugInID;
	UBYTE ng_Selected;
	UBYTE ng_Type;
	TEXT ng_Label[40];
	UWORD ng_NumTags;
	UWORD ng_NumOwnTags;
};

struct GadToolsData				// GadTools has additional datas
{
	TEXT g_Text[256];
	ULONG g_Flags;
	ULONG g_Kind;
};

#define MYFR_Recessed (1<<0)

struct Node_Frame
{
	piObject *nf_Window;	// Parent Window
	struct Page *nf_Pages;

	UWORD nf_X,nf_Y,nf_Width,nf_Height;
	UBYTE nf_RelX,nf_RelY,nf_RelWidth,nf_RelHeight;
	UBYTE nf_FrameType;
	UBYTE nf_Selected;
	ULONG nf_Flags;
};

#define FTYPE_BUTTON 0
#define FTYPE_ICONDROPBOX 1
#define FTYPE_RIDGE 2
#define FTYPE_HORIZONTALLINE 3
#define FTYPE_VERTICALLINE 4

#define WPOS_REL_LEFT_TOP 0
#define WPOS_REL_RIGHT_BOTTOM 1
#define WPOS_CENTER 2

#define MYWA_FontAdaptive (1<<0)
#define MYWA_BRightSlider (1<<1)
#define MYWA_BBottomSlider (1<<2)
#define MYWA_InnerWidth (1<<3)
#define MYWA_InnerHeight (1<<4)
#define MYWA_MouseQueue (1<<5)
#define MYWA_RptQueue (1<<6)
#define MYWA_AutoAdjust (1<<7)
#define MYWA_PubScreenFallBack (1<<8)
#define MYWA_MenuHelp (1<<9)
#define MYWA_NotifyDepth (1<<10)
#define MYWA_PointerDelay (1<<11)
#define MYWA_RMBTrap (1<<12)
#define MYWA_Project (1<<13)
#define MYWA_Zoom (1<<14)
#define MYWA_AppWindow (1<<15)
#define MYWA_DragToolHoriz (1<<16)
#define MYWA_DragToolVert (1<<17)

struct Node_Window
{
	ULONG nw_ID;
	struct Window *nw_Window;
	struct Gadget *nw_Gadgets;
	Object *(nw_Object[6]);
	struct MinList nw_List_Gadget;
	struct MinList nw_List_Frame;
	
	UWORD nw_X,nw_Y,nw_Width,nw_Height;  // saveable datas follows
	UBYTE nw_XPos,nw_YPos;
	UWORD nw_Zoom[4];
	TEXT nw_WindowTitle[256];
	TEXT nw_ScreenTitle[256];
	TEXT nw_Label[40];
	TEXT nw_MenuTitle[128];
	TEXT nw_FontName[30];
	UWORD nw_Dimensions[4];					// Mins, Maxs
	UWORD nw_FontSize;
	ULONG nw_IDCMP;									// Standard IDCMP Flags
  ULONG nw_MyFlags;								// My Window Flags
	ULONG nw_Flags;									// Standard Window Flags
	ULONG nw_MouseQueue;
	ULONG nw_RptQueue;
	ULONG nw_PageGadget;						// LockCounter for PageGadget
};

#define MYMN_Toggle (1<<0)
#define MYMN_NewLookMenus (1<<1)
#define MYMN_Disabled (1<<2)
#define MYMN_Checked (1<<3)
#define MYMN_BarLabel (1<<4)
#define MYMN_PopUp (1<<5)

struct Node_Menu
{
	ULONG nm_ID;
	struct MinList nm_MenuItem;
	struct Menu *nm_Menu;
	struct NewMenu *nm_NewMenu;
	struct Window *nm_Window;
	struct Page *nm_Pages;
	LONG nm_Top,nm_Visible,nm_Total;
	Object *nm_LeftButton,*nm_RightButton,*nm_HorizProp;

	TEXT nm_Label[128];
	ULONG nm_Flags;
};

struct Node_MenuItem
{
	struct Node nm_Node;
	UBYTE nm_Selected;

	TEXT nm_Label[128];
	TEXT nm_Title[128];
	UBYTE nm_Type;
	ULONG nm_Flags;
	ULONG nm_MutEx;
	TEXT nm_Key[2];
};

#define TYPE_WORKBENCH 0
#define TYPE_CUSTOMSCREEN 1
#define TYPE_PUBLICSCREEN 2

#define MYSC_MakePublic (1<<0)
#define MYSC_ShowTitle (1<<3)
#define MYSC_Behind (1<<4)
#define MYSC_Quiet (1<<5)
#define MYSC_AutoScroll (1<<6)
#define MYSC_FullPalette (1<<7)
#define MYSC_Draggable (1<<8)
#define MYSC_Exclusive (1<<9)
#define MYSC_SharePens (1<<10)
#define MYSC_Interleaved (1<<11)
#define MYSC_LikeWB (1<<12)
#define MYSC_MinimizeISG (1<<13)
#define MYSC_DefaultID (1<<14)

struct Node_Screen
{
	UWORD ns_Width,ns_Height,ns_Depth;
	ULONG ns_DisplayID;
	UWORD ns_Type;
	TEXT ns_Title[256];
	TEXT ns_Public[30];
	TEXT ns_FontName[30];
	UWORD ns_FontSize;
	ULONG ns_MyFlags;
};

struct Node_Version
{
	ULONG nv_Version;
	ULONG nv_Revision;
	TEXT nv_ProgramName[256];
	TEXT nv_Author[256];
};

struct Node_String
{
	struct LVList ns_Strings;
	struct Page *ns_Pages;
	struct Node_StringData *ns_Selected;
};

struct Node_StringData
{
	struct LVNode ns_Node;
	STRPTR ns_GadString;
	STRPTR ns_Label;
	STRPTR ns_String;
};

#define MYPR_ConvUS (1<<0)

struct Prefs
{
	ULONG pr_Flags;
	ULONG pr_DisplayID;
	UWORD pr_Width,pr_Height,pr_Depth;
	TEXT pr_Generator[108];
	UWORD pr_Grid[2];
	BOOL pr_GridOn,pr_GridShow;
	struct
	{
		UWORD X,Y;
	} pr_WinPos[10];
};

struct SavePrefs
{
	ULONG spr_Flags;
	ULONG spr_CodePage;
};

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
	struct MinList gui_List_Image;				// List of mouse pointers
	struct MinList gui_List_Binary;				// List of binary data
};

struct GTBSemaphore
{
	struct SignalSemaphore gs_Semaphore;
	struct GUI *gs_GUI;
	struct Window *gs_Window;
	struct Gadget *gs_Progress;
	struct TextFont *gs_FontTF;
};

#define BUTTON_ID MAKE_ID('B','U','T','T')
#define CHECKBOX_ID MAKE_ID('C','B','O','X')
#define COLORWHEEL_ID MAKE_ID('C','W','H','L')
#define CYCLE_ID MAKE_ID('C','Y','C','E')
#define DATATYPE_ID MAKE_ID('D','T','Y','P')
#define GRADIENTSLIDER_ID MAKE_ID('G','R','S','L')
#define INTEGER_ID MAKE_ID('I','N','T','R')
#define LISTVIEW_ID MAKE_ID('L','V','I','E')
#define MX_ID MAKE_ID('M','X',' ',' ')
#define NUMBER_ID MAKE_ID('N','U','M','B')
#define PAGE_ID MAKE_ID('P','A','G','E')
#define PALETTE_ID MAKE_ID('P','A','L','T')
#define POPUPMENU_ID MAKE_ID('P','U','P','M')
#define PROGRESS_ID MAKE_ID('P','R','G','R')
#define SCROLLER_ID MAKE_ID('S','C','R','L')
#define SLIDER_ID MAKE_ID('S','L','I','R')
#define STRING_ID MAKE_ID('S','T','R','G')
#define TAPEDECK_ID MAKE_ID('T','D','E','K')
#define TEXT_ID MAKE_ID('T','E','X','T')
#define TEXTFIELD_ID MAKE_ID('T','F','L','D')
#define TREEVIEW_ID MAKE_ID('T','R','L','V')

