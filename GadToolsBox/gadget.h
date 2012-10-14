#ifndef GTB_GADGET_H
#define GTB_GADGET_H
/*
** Gadget
*/

#include"plugins/button.h"
#include"plugins/checkbox.h"
#include"plugins/colorwheel.h"
#include"plugins/cycle.h"
#include"plugins/datatype.h"
#include"plugins/gradientslider.h"
#include"plugins/integer.h"
#include"plugins/listview.h"
#include"plugins/number.h"
#include"plugins/mx.h"
#include"plugins/palette.h"
#include"plugins/page.h"
#include"plugins/popupmenu.h"
#include"plugins/progress.h"
#include"plugins/scroller.h"
#include"plugins/slider.h"
#include"plugins/string.h"
#include"plugins/tapedeck.h"
#include"plugins/text.h"
#include"plugins/textfield.h"
#include"plugins/treeview.h"

struct EditList
{
	struct List **List;
	ULONG *Active;
};

//      XXXX_ID - Gadget specific data    // Gadget Data (see gadgets.h)

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

	WORD ng_X,ng_Y,ng_Width,ng_Height;
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

struct PlugIn
{
	STRPTR piName;																	// Name of plugin
	ULONG piID;																			// ID of plugin
	ULONG (*Dispatcher)(piClass*,piObject *,Msg);		// Object Dispatcher
	piClass *Class;
};

#define PLUGIN_GADGETS 21

extern struct PlugIn piGadgets[];

#define GadgetData(x) (((UBYTE *)x)+sizeof(struct Node_Gadget))	// Get Pointer to GadToolsData

#define GAOBJ_Type (TAG_USER+20+1)
#define GAOBJ_NumTags (TAG_USER+20+2)
#define GAOBJ_NumOwnTags (TAG_USER+20+3)
#define GAOBJ_Tags (TAG_USER+20+4)
#define GAOBJ_OwnTags (TAG_USER+20+5)
#define GAOBJ_TagTypes (TAG_USER+20+6)
#define GAOBJ_OwnTagTypes (TAG_USER+20+7)
#define GAOBJ_ID (TAG_USER+20+8)
#define GAOBJ_NodeData (TAG_USER+20+9)
#define GAOBJ_EditList (TAG_USER+20+10)
#define GAOBJLIST_List (TAG_USER+20+11)
#define GAOBJLIST_Active (TAG_USER+20+12)
#define GAOBJLIST_Tags (TAG_USER+20+13)

#define GTYPE_GADTOOLS 0
#define GTYPE_BOOPSI 1

struct Gadget *CreateAllGadgets(piObject *);
piObject *piFindGadget(piObject *,UWORD,UWORD);
ULONG GetGadgetPtr(ULONG);

ULONG Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG DEFAULT_Gadget(piClass *,piObject *,Msg);
ULONG INIT_Gadget(piClass *,piObject *,Msg);
ULONG APPLY_Gadget(piClass *,piObject *,Msg);
ULONG COMPUTE_Gadget(piClass *,piObject *,struct piCompute *);
ULONG HITTEST_Gadget(piClass *,piObject *,struct piHitTest *);
ULONG NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG DISPOSE_Gadget(piClass *,piObject *,Msg);
ULONG ADDTAIL_Gadget(piClass *,piObject *,struct piWindow *);
ULONG REMOVE_Gadget(piClass *,piObject *,Msg);
ULONG RENDER_Gadget(piClass *,piObject *,Msg);
ULONG ERASE_Gadget(piClass *,piObject *,Msg);
ULONG SET_Gadget(piClass *,piObject *,struct piSet *);
ULONG GET_Gadget(piClass *,piObject *,struct piGet *);
ULONG CREATE_Gadget(piClass *,piObject *,struct piWindow *);
ULONG DELETE_Gadget(piClass *,piObject *,Msg);
ULONG READ_Gadget(piClass *,piObject *,struct piIO *);
ULONG WRITE_Gadget(piClass *,piObject *,struct piIO *);

#endif
