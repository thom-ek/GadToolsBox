#include"defines.h"

#define COLORWHEEL_ID MAKE_ID('C','W','H','L')

ULONG COLORWHEEL_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG COLORWHEEL_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG COLORWHEEL_DISPOSE_Gadget(piClass *,piObject *,Msg);
ULONG COLORWHEEL_INIT_Gadget(piClass *,piObject *,Msg);
ULONG COLORWHEEL_DEFAULT_Gadget(piClass *,piObject *,Msg);
ULONG COLORWHEEL_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG COLORWHEEL_CREATE_Gadget(piClass *,piObject *,struct piWindow *);
ULONG COLORWHEEL_DELETE_Gadget(piClass *,piObject *,Msg);
