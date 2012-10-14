#include"defines.h"

#define SCROLLER_ID MAKE_ID('S','C','R','L')

ULONG SCROLLER_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG SCROLLER_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG SCROLLER_INIT_Gadget(piClass *,piObject *,Msg);
ULONG SCROLLER_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG SCROLLER_READ_Gadget(piClass *,piObject *,struct piIO *);
