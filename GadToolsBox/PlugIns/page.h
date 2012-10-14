#include"defines.h"

#define PAGE_ID MAKE_ID('P','A','G','E')

ULONG PAGE_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG PAGE_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG PAGE_DISPOSE_Gadget(piClass *,piObject *,Msg);
ULONG PAGE_INIT_Gadget(piClass *,piObject *,Msg);
ULONG PAGE_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG PAGE_DEFAULT_Gadget(piClass *,piObject *,Msg);
ULONG PAGE_CREATE_Gadget(piClass *,piObject *,struct piWindow *);
ULONG PAGE_DELETE_Gadget(piClass *,piObject *,Msg);
ULONG PAGE_ERASE_Gadget(piClass *,piObject *,Msg);
