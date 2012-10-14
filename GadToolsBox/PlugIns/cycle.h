#include"defines.h"

#define CYCLE_ID MAKE_ID('C','Y','C','E')

ULONG CYCLE_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG CYCLE_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG CYCLE_INIT_Gadget(piClass *,piObject *,Msg);
ULONG CYCLE_DEFAULT_Gadget(piClass *,piObject *,Msg);
ULONG CYCLE_DISPOSE_Gadget(piClass *,piObject *,Msg);
ULONG CYCLE_CREATE_Gadget(piClass *,piObject *,Msg);
ULONG CYCLE_DELETE_Gadget(piClass *,piObject *,Msg);
ULONG CYCLE_READ_Gadget(piClass *,piObject *,struct piIO *);
