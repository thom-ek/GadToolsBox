#include"defines.h"

#define LISTVIEW_ID MAKE_ID('L','V','I','E')

ULONG LISTVIEW_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG LISTVIEW_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG LISTVIEW_INIT_Gadget(piClass *,piObject *,Msg);
ULONG LISTVIEW_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG LISTVIEW_DEFAULT_Gadget(piClass *,piObject *,Msg);
ULONG LISTVIEW_DISPOSE_Gadget(piClass *,piObject *,Msg);
ULONG LISTVIEW_READ_Gadget(piClass *,piObject *,struct piIO *);
