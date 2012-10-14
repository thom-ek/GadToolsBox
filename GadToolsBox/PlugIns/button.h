#include"gadtools.h"
#include"defines.h"
#include"includes.h"

#define BUTTON_ID MAKE_ID('B','U','T','T')

ULONG BUTTON_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG BUTTON_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG BUTTON_INIT_Gadget(piClass *,piObject *,Msg);
ULONG BUTTON_CREATE_Gadget(piClass *,piObject *,struct piWindow *);
ULONG BUTTON_DELETE_Gadget(piClass *,piObject *,Msg);
ULONG BUTTON_READ_Gadget(piClass *,piObject *,struct piIO *);
