#include"defines.h"

#define SLIDER_ID MAKE_ID('S','L','I','R')

ULONG SLIDER_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG SLIDER_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG SLIDER_INIT_Gadget(piClass *,piObject *,Msg);
ULONG SLIDER_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG SLIDER_READ_Gadget(piClass *,piObject *,struct piIO *);
