#include"defines.h"

#define PALETTE_ID MAKE_ID('P','A','L','T')

ULONG PALETTE_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG PALETTE_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG PALETTE_INIT_Gadget(piClass *,piObject *,Msg);
ULONG PALETTE_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG PALETTE_READ_Gadget(piClass *,piObject *,struct piIO *);
