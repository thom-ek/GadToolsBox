#include"defines.h"

#define TEXTFIELD_ID MAKE_ID('T','F','L','D')

ULONG TEXTFIELD_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG TEXTFIELD_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG TEXTFIELD_INIT_Gadget(piClass *,piObject *,Msg);
ULONG TEXTFIELD_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG TEXTFIELD_CREATE_Gadget(piClass *,piObject *,struct piWindow *);
ULONG TEXTFIELD_DELETE_Gadget(piClass *,piObject *,Msg);
