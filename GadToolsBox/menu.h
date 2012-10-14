#ifndef GTB_MENU_H
#define GTB_MENU_H
/*
** Menu
*/

#define ID_MENU MAKE_ID('M','E','N','U') // Menu Chunk

#define MYMN_Toggle (1<<0)
#define MYMN_NewLookMenus (1<<1)
#define MYMN_Disabled (1<<2)
#define MYMN_Checked (1<<3)
#define MYMN_BarLabel (1<<4)
#define MYMN_PopUp (1<<5)

struct Node_Menu
{
	ULONG nm_ID;
	struct MinList nm_MenuItem;			// List of menus 
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
	struct Node nm_Node;						// Next item in list
	UBYTE nm_Selected;

	TEXT nm_Label[128];
	TEXT nm_Title[128];
	UBYTE nm_Type;
	ULONG nm_Flags;
	ULONG nm_MutEx;
	TEXT nm_Key[2];
};

ULONG Dispatcher_Menu(piClass *,piObject *,Msg);
ULONG INIT_Menu(piClass *,piObject *,Msg);
ULONG APPLY_Menu(piClass *,piObject *,Msg);
ULONG CREATE_Menu(piClass *,piObject *,Msg);
ULONG DELETE_Menu(piClass *,piObject *,Msg);
ULONG NEW_Menu(piClass *,piObject *,struct piSet *);
ULONG DISPOSE_Menu(piClass *,piObject *,Msg);
ULONG RENDER_Menu(piClass *,piObject *,Msg);
ULONG SET_Menu(piClass *,piObject *,struct piSet *);
ULONG GET_Menu(piClass *,piObject *,struct piGet *);
ULONG READ_Menu(piClass *,piObject *,struct piIO *);
ULONG WRITE_Menu(piClass *,piObject *,struct piIO *);
ULONG CUT_Menu(piClass *,piObject *,Msg);
ULONG COPY_Menu(piClass *,piObject *,struct piCopy *);
void DisposeAllMenus(void);
void DeleteAllMenus(void);
int FN_MouseButtons_EditMenu(void);
int FN_IDCMPUpdate_EditMenu(void);

#endif
