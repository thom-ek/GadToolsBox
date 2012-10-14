#include"defines.h"

#define PROGRESS_ID MAKE_ID('P','R','G','R')

ULONG PROGRESS_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG PROGRESS_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG PROGRESS_INIT_Gadget(piClass *,piObject *,Msg);
ULONG PROGRESS_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG PROGRESS_CREATE_Gadget(piClass *,piObject *,struct piWindow *);
ULONG PROGRESS_DELETE_Gadget(piClass *,piObject *,Msg);
