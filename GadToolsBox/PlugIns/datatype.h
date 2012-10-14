#include"defines.h"

#define DATATYPE_ID MAKE_ID('D','T','Y','P')

ULONG DATATYPE_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG DATATYPE_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG DATATYPE_INIT_Gadget(piClass *,piObject *,Msg);
ULONG DATATYPE_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG DATATYPE_CREATE_Gadget(piClass *,piObject *,struct piWindow *);
ULONG DATATYPE_DELETE_Gadget(piClass *,piObject *,Msg);
ULONG DATATYPE_RENDER_Gadget(piClass *,piObject *,Msg);
