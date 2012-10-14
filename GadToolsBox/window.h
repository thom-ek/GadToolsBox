#ifndef GTB_WINDOW_H
#define GTB_WINDOW_H
/*
** Window
*/

/*
** Window
*/

#define ID_WIND MAKE_ID('W','I','N','D')  // Window Chunk

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
	UBYTE nw_XPos,nw_YPos; // nw_RelX,nw_RelY,nw_RelWidth,nw_RelHeight;
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

#define WNOBJ_GadgetList (TAG_USER+40+1)
#define WNOBJ_FrameList (TAG_USER+40+2)
#define WNOBJ_RastPort (TAG_USER+40+3)

void LayoutAllWindows(void);
void DeleteAllWindows(void);
void DisposeAllWindows(void);
UWORD WinLeft(piObject *);
UWORD WinTop(piObject *);
UWORD WinWidth(piObject *);
UWORD WinHeight(piObject *);
void CloseWindowSafely(struct Window *);
void StripIntuiMessages(struct MsgPort *,struct Window *);
void SetWindowAttrWindow(piObject *);
void SetWindowIDCMPWindow(piObject *);

ULONG Dispatcher_Window(piClass *,piObject *,Msg);
ULONG NEW_Window(piClass *,piObject *,struct piSet *);
ULONG DISPOSE_Window(piClass *,piObject *,Msg );
ULONG RENDER_Window(piClass *,piObject *,Msg );
ULONG ERASE_Window(piClass *,piObject *,Msg );
ULONG OPEN_Window(piClass *,piObject *,Msg );
ULONG CLOSE_Window(piClass *,piObject *,Msg );
ULONG SET_Window(piClass *,piObject *,struct piSet *);
ULONG GET_Window(piClass *,piObject *,struct piGet *);
ULONG READ_Window(piClass *,piObject *,struct piIO *);
ULONG WRITE_Window(piClass *,piObject *,struct piIO *);
ULONG INIT_Window(piClass *,piObject *,Msg );
ULONG APPLY_Window(piClass *,piObject *,Msg );
ULONG CUT_Window(piClass *,piObject *,Msg );
ULONG COPY_Window(piClass *,piObject *,struct piCopy *);

#endif
