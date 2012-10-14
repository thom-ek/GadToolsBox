#include"defines.h"

#define TREEVIEW_ID MAKE_ID('T','R','L','V')

ULONG TREEVIEW_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG TREEVIEW_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG TREEVIEW_INIT_Gadget(piClass *,piObject *,Msg);
ULONG TREEVIEW_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG TREEVIEW_DEFAULT_Gadget(piClass *,piObject *,Msg);
ULONG TREEVIEW_DISPOSE_Gadget(piClass *,piObject *,Msg);
ULONG TREEVIEW_CREATE_Gadget(piClass *,piObject *,struct piWindow *);
ULONG TREEVIEW_DELETE_Gadget(piClass *,piObject *,Msg);
