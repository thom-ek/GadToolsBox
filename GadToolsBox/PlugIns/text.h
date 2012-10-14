#include"defines.h"

#define TEXT_ID MAKE_ID('T','E','X','T')

ULONG TEXT_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG TEXT_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG TEXT_INIT_Gadget(piClass *,piObject *,Msg);
ULONG TEXT_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG TEXT_READ_Gadget(piClass *,piObject *,struct piIO *);
