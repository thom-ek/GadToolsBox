#include"defines.h"

#define CHECKBOX_ID MAKE_ID('C','B','O','X')

ULONG CHECKBOX_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG CHECKBOX_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG CHECKBOX_INIT_Gadget(piClass *,piObject *,Msg);
ULONG CHECKBOX_DEFAULT_Gadget(piClass *,piObject *,Msg);
ULONG CHECKBOX_READ_Gadget(piClass *,piObject *,struct piIO *);
