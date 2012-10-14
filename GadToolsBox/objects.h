#ifndef GTB_OBJECTS_H
#define GTB_OBJECTS_H
/*
** Objects
*/

#define OBJECT_DATA(o) (((UBYTE *)o)+sizeof(piObject))
#define GetHead(x) (((struct List *)x)->lh_Head)
#define GetSucc(x) (x->ln_Succ) 
#define GetPred(x) (x->ln_Pred)

// ClassIDs

#define CLASS_GUI 0				// struct GUI
#define CLASS_GADGET 1		// struct Node_Gadget
#define CLASS_WINDOW 2		// struct Node_Window
#define CLASS_FRAME 3			// struct Node_Frame
#define CLASS_MENU 4			// struct Node_Menu
#define CLASS_POINTER 5		// struct Node_Pointer
#define CLASS_BINARY 6		// struct Node_Binary
#define CLASS_VERSION 7		// struct Node_Version
#define CLASS_STRING 8		// struct Node_String
#define CLASS_TEXT 9			// struct Node_Text
#define CLASS_SCREEN 10		// struct Node_Screen
#define CLASS_TOOLBAR 11	// struct Node_ToolBar
#define CLASS_IMAGE 12		// struct Node_Image

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

// OM_NEW, OM_SET
struct piSet
{
	ULONG MethodID;
	struct TagItem *Tags;
};

// OM_GET
struct piGet
{
	ULONG MethodID;
	ULONG TagID;
	ULONG *TagData;
};

// OM_ADDTAIL, OM_RENDER
struct piWindow
{
	ULONG MethodID;
	piObject *Window;
};

// OM_HITTEST
struct piHitTest
{
	ULONG MethodID;
	ULONG X;
	ULONG Y;
};

// OM_COMPUTE
struct piCompute
{
	ULONG MethodID;
	ULONG Type;
};

// OM_READ, OM_WRITE
struct piIO
{
	ULONG MethodID;
	struct IFFHandle *IFFh;
};

// OM_COPY
struct piCopy
{
	ULONG MethodID;
	ULONG Copy;
};

#define CT_LEFT 0
#define CT_TOP 1
#define CT_WIDTH 2
#define CT_HEIGHT 3

#define OM_SETUP 201		// Not used
#define OM_CREATE 202		// Add object to window
#define OM_DELETE 203		// Remove object from window
#define OM_COMPUTE 204	// Compute positions
#define OM_ERASE 205		// Erase object on window
#define OM_READ 206			// Read object from file
#define OM_WRITE 207		// Write object to file
#define OM_RENDER 208		// Render object on window
#define OM_HITTEST 209	// Check object for hit
#define OM_OPEN 210			// Create properties gadgets
#define OM_CLOSE 211		// Free properties gadgets
#define OM_INIT 212			// Init properties gadgets
#define OM_APPLY 213		// Apply changes to object
#define OM_DEFAULT 214	// Reset gadget to default values (only for new gadgets)
#define OM_CUT 215			// Cut object to clipboard
#define OM_COPY 216			// Copy object to clipboard
#define OM_PASTE 217		// Paste object from clipboard

#define OBJ_Select (TAG_USER+1)
#define OBJ_Window (TAG_USER+2)
#define OBJ_Node (TAG_USER+3)
#define OBJ_Properties (TAG_USER+4)

#define COPY_CLIP 0
#define COPY_UNDO 1

// prototypes

piClass *piMakeClass(ULONG, piClass *, ULONG);
void piFreeClass(piClass *);
ULONG piOpenClass(piClass *);
void piCloseClass(piClass *);
ULONG piDoMethod(piObject *, ULONG, ...);
ULONG piDoSuperMethod(piClass *, piObject *, ULONG, ...);
ULONG piDoMethodA(piObject *, Msg);
ULONG piDoSuperMethodA(piClass *, piObject *, Msg);
piObject *piNewObject(piClass *, ULONG, ...);
void piDisposeObject(piObject *);
void piResetToDefaults(piObject *);
void piInitObjectGUI(piObject *);
void piApplyObject(piObject *);
void piSetAttrs(piObject *,ULONG, ...);
ULONG piGetAttr(piObject *, ULONG, ULONG *);
void piRenderObject(piObject *);
void piEraseObject(piObject *);
ULONG piOpenWindow(piObject *);
void piCloseWindow(piObject *);
UWORD piCX(piObject *);
UWORD piCY(piObject *);
UWORD piCW(piObject *);
UWORD piCH(piObject *);
int piCutObject(piObject *);
int piCopyObject(piObject *,ULONG);
void piPasteObject(piObject *);
int piSetupObject(piObject *);
LONG piWriteObject(piObject *,struct IFFHandle *);
LONG piReadObject(piObject *,struct IFFHandle *);
void piAddObject(piObject *,piObject *);
void piRemoveObject(piObject *);
LONG piReadObject(piObject *,struct IFFHandle *);
LONG piWriteObject(piObject *,struct IFFHandle *);

UBYTE CheckObjectClickPosition(piObject *,UWORD,UWORD);
int UnSelectAllObjects(piObject *);
void SetObjectDims(piObject *,UWORD,UWORD,UWORD,UWORD);
void piShowGrid(piObject *);
//UWORD IGX(UWORD);
//UWORD IGY(UWORD);
//UWORD IGW(UWORD);
//UWORD IGH(UWORD);
void SelectObjects(piObject *,UWORD,UWORD,UWORD,UWORD);
void UpdateObjectList(void);
void RefreshObjectList(void);
int OpenObjectList(void);
void CloseObjectList(void);

#endif /* GTB_OBJECTS_H */
