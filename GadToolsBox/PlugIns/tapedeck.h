#include"defines.h"

#define TAPEDECK_ID MAKE_ID('T','D','E','K')

ULONG TAPEDECK_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG TAPEDECK_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG TAPEDECK_INIT_Gadget(piClass *,piObject *,Msg);
ULONG TAPEDECK_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG TAPEDECK_DEFAULT_Gadget(piClass *,piObject *,Msg);
ULONG TAPEDECK_CREATE_Gadget(piClass *,piObject *,struct piWindow *);
ULONG TAPEDECK_DELETE_Gadget(piClass *,piObject *,Msg);
ULONG TAPEDECK_RENDER_Gadget(piClass *,piObject *,Msg);
