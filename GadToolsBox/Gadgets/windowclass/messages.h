#include<proto/utility.h>
#include<exec/types.h>
#include<gadtoolsbox/gadtoolsbox.h>

#define MESG_Dummy (TAG_USER)
#define MESG_Name (MESG_Dummy+0)
#define MESG_Signal (MESG_Dummy+1)
#define MESG_Priority (MESG_Dummy+6)
#define MESG_MessagePort (MESG_Dummy+7)
#define MESG_Handler (MESG_Dummy+31)

#define MESG_SigBit (MESG_Dummy+106)
