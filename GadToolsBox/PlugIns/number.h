#include"defines.h"

#define NUMBER_ID MAKE_ID('N','U','M','B')

ULONG NUMBER_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG NUMBER_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG NUMBER_INIT_Gadget(piClass *,piObject *,Msg);
ULONG NUMBER_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG NUMBER_READ_Gadget(piClass *,piObject *,struct piIO *);
