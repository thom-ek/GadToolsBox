#include"defines.h"

#define STRING_ID MAKE_ID('S','T','R','G')

ULONG STRING_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG STRING_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG STRING_INIT_Gadget(piClass *,piObject *,Msg);
ULONG STRING_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG STRING_READ_Gadget(piClass *,piObject *,struct piIO *);
