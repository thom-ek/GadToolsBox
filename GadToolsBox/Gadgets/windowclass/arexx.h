#include<proto/utility.h>
#include<exec/types.h>
#include<gadtoolsbox/gadtoolsbox.h>

struct gtbRexxCommand
{
	STRPTR rc_Function;
	ULONG rc_ID;
	STRPTR rc_Template;
};

struct gtbRexxMessage
{
	struct RexxMsg *rm_RexxMsg;
	STRPTR rm_Command;
	Object *rm_Object;
	LONG **rm_ArgList;
	LONG rm_RC;
	STRPTR rm_Result;
};

struct gtbRexxSendMsg
{
	ULONG MethodID;
	STRPTR Command;
	ULONG Flags;
};

#define REXX_Dummy (TAG_USER)
#define REXX_Name (MESG_Dummy+0)
#define REXX_Signal (MESG_Dummy+1)
#define REXX_Extension (MESG_Dummy+2)
#define REXX_CommandTable (MESG_Dummy+3)
#define REXX_Priority (MESG_Dummy+6)
#define REXX_MessagePort (MESG_Dummy+7)
#define REXX_Handler (MESG_Dummy+31)

#define REXX_SigBit (MESG_Dummy+106)

#define GTB_REXX_SENDMSG 1000
#define GTB_REXX_REPLYMSG 1001
