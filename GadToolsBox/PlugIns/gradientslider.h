#include"defines.h"

#define GRADIENTSLIDER_ID MAKE_ID('G','R','S','L')

ULONG GRADIENTSLIDER_Dispatcher_Gadget(piClass *,piObject *,Msg);
ULONG GRADIENTSLIDER_NEW_Gadget(piClass *,piObject *,struct piSet *);
ULONG GRADIENTSLIDER_INIT_Gadget(piClass *,piObject *,Msg);
ULONG GRADIENTSLIDER_APPLY_Gadget(piClass *,piObject *,Msg);
ULONG GRADIENTSLIDER_CREATE_Gadget(piClass *,piObject *,struct piWindow *);
ULONG GRADIENTSLIDER_DELETE_Gadget(piClass *,piObject *,Msg);
