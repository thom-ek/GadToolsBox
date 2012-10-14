#include"defines.h"

#define MX_ID MAKE_ID('M','X',' ',' ')

ULONG MX_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG MX_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG MX_INIT_Gadget(piClass *,piObject *,Msg);
ULONG MX_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG MX_DEFAULT_Gadget(piClass *,piObject *,Msg);
ULONG MX_DISPOSE_Gadget(piClass *,piObject *,Msg);
ULONG MX_CREATE_Gadget(piClass *,piObject *,Msg);
ULONG MX_DELETE_Gadget(piClass *,piObject *,Msg);
ULONG MX_READ_Gadget(piClass *,piObject *,struct piIO *);
