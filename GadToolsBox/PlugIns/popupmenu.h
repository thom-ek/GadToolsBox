#include"defines.h"

#define POPUPMENU_ID MAKE_ID('P','U','P','M')

ULONG POPUPMENU_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG POPUPMENU_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG POPUPMENU_DISPOSE_Gadget(piClass *,piObject *,Msg);
ULONG POPUPMENU_INIT_Gadget(piClass *,piObject *,Msg);
ULONG POPUPMENU_CREATE_Gadget(piClass *,piObject *,struct piWindow *);
ULONG POPUPMENU_DELETE_Gadget(piClass *,piObject *,Msg);
