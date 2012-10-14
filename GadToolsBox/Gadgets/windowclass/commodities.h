#include<proto/utility.h>
#include<exec/types.h>
#include<gadtoolsbox/gadtoolsbox.h>

struct CxMessage
{
	LONG cx_ID;
	ULONG cx_Type;
	APTR cx_Data;
};

#define COMM_Dummy (TAG_USER)
#define COMM_Name (COMM_Dummy+0)
#define COMM_Title (COMM_Dummy+1)
#define COMM_Description (COMM_Dummy+2)
#define COMM_Unique (COMM_Dummy+3)
#define COMM_Notify (COMM_Dummy+4)
#define COMM_ShowHide (COMM_Dummy+5)
#define COMM_Priority (COMM_Dummy+6)
#define COMM_MessagePort (COMM_Dummy+7)
#define COMM_AddHotKeyStr (COMM_Dummy+8)
#define COMM_AddHotKeyID (COMM_Dummy+9)
#define COMM_Activate (COMM_Dummy+10)
#define COMM_Handler (COMM_Dummy+31)

#define COMM_Error (COMM_Dummy+100)
#define COMM_Broker (COMM_Dummy+101)
#define COMM_SigBit (COMM_Dummy+106)
