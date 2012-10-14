#include"defines.h"

#define INTEGER_ID MAKE_ID('I','N','T','R')

ULONG INTEGER_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG INTEGER_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG INTEGER_INIT_Gadget(piClass *,piObject *,Msg);
ULONG INTEGER_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG INTEGER_READ_Gadget(piClass *,piObject *,struct piIO *);
