/*
** ARexx Class
** $VER: arexx.c 37.1 (15.4.98)
*/

#include<dos.h>
#include<stdio.h>
#include<string.h>
#include<clib/macros.h>
#include<proto/dos.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<proto/intuition.h>
#include<proto/commodities.h>
#include<proto/rexxsyslib.h>
#include<exec/memory.h>
#include<intuition/gadgetclass.h>
#include<intuition/intuitionbase.h>
#include<intuition/icclass.h>
#include<rexx/errors.h>
#include"gadtoolsbox.h"

//TEXT version[]=VERSTAG;

/*
** Defines
*/

#define unless(x) if(!(x))
#define SetTagArg(tag,id,data)	{ tag.ti_Tag = (ULONG)(id); tag.ti_Data = (ULONG)(data); }
typedef ULONG (*HookFunction)(void);
#define SAVEDS 
#define ASMFUNC __saveds __asm
#define REG(x) register __## x
#define MESSAGECLASS "gtbarexxclass"

/*
** Prototypes
*/

Class *CreateARexxClass(struct gtbApplication *);
void DisposeARexxClass(Class *);
ULONG ASMFUNC REXX_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG REXX_NEW(Class *,Object *,struct opSet *);
ULONG REXX_DISPOSE(Class *, Object *, Msg);
ULONG REXX_SET(Class *, Object *, struct opSet *);
ULONG REXX_GET(Class *,Object *,struct opGet *);
ULONG REXX_GETMESSAGE(Class *,Object *,Msg);
ULONG REXX_HANDLEMESSAGE(Class *,Object *,struct gtbWaitMessage *);
ULONG REXX_WAITMESSAGE(Class *,Object *,struct gtbWaitMessage *);
ULONG REXX_SENDMSG(Class *,Object *,struct gtbRexxSendMsg *);

/*
** Variables
*/
struct REXXData
{
	BOOL ad_ObjectOK;
	struct MsgPort *ad_MsgPort;
	STRPTR ad_Name;
	STRPTR ad_Extension;
	BYTE ad_Pri;
	LONG ad_MessagesSent;
	struct gtbRexxCommand *ad_CommandTable;
	ULONG (*ad_ARexxHandler)(Object *,struct gtbRexxCommand *,struct gtbRexxMessage *);
	struct RDArgs *ad_RDArgs;
};

/*
** Create Class
*/

Class *CreateARexxClass(struct gtbApplication *appl)
{
	Class *cl=NULL;

	if(cl = MakeClass (NULL, ROOTCLASS, NULL, sizeof(struct REXXData), 0L))
	{
		cl->cl_Dispatcher.h_Entry = (HookFunction)  REXX_Dispatcher;
		cl->cl_UserData = (ULONG) appl;
		return cl;
	}
	return(NULL);
}

void DisposeARexxClass(Class *cl)
{
	FreeClass(cl);
}

/*
** Dispatcher
*/

ULONG ASMFUNC REXX_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = REXX_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = REXX_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = REXX_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = REXX_GET(cl, o, (struct opGet *)msg );
			break;
		case GTB_GETMESSAGE:
			retval = REXX_GETMESSAGE(cl,o,msg);
			break;
		case GTB_HANDLEMESSAGE:
			retval = REXX_HANDLEMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
			break;
		case GTB_WAITMESSAGE:
			retval = REXX_WAITMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
			break;
		case GTB_REXX_SENDMSG:
			retval = REXX_SENDMSG(cl,o,(struct gtbRexxSendMsg *)msg);
			break;
		default:
			retval = DoSuperMethodA(cl, o, msg);
			break;
	}
	return(retval);
}

/*
** OM_NEW
*/

ULONG SAVEDS REXX_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct REXXData *AD;

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		AD = INST_DATA( cl, object );

		memset(AD,0,sizeof(struct REXXData));

		AD->ad_MsgPort=NULL;
		AD->ad_MessagesSent=0;
		AD->ad_Name = (STRPTR)GetTagData( REXX_Name, NULL, ops->ops_AttrList );
		AD->ad_Extension = (STRPTR)GetTagData( REXX_Extension, NULL, ops->ops_AttrList );
		AD->ad_Pri = (BYTE)GetTagData( REXX_Priority, 0, ops->ops_AttrList );
		AD->ad_CommandTable = (struct gtbRexxCommand *) GetTagData( REXX_CommandTable, NULL, ops->ops_AttrList );
		AD->ad_ARexxHandler=(ULONG (*)(Object *,struct gtbRexxCommand *,struct gtbRexxMessage *))GetTagData( REXX_Handler, NULL, ops->ops_AttrList );

		if(!AD->ad_Name)
		{
			CoerceMethod(cl,object,OM_DISPOSE);
			return FALSE;
		}

		Forbid();
		if(FindPort(AD->ad_Name))
		{
			Permit();
			CoerceMethod(cl,object,OM_DISPOSE);
			return FALSE;
		}
		Permit();

		if(!(AD->ad_MsgPort=CreatePort(AD->ad_Name,AD->ad_Pri)))
		{
			CoerceMethod(cl,object,OM_DISPOSE);
			return FALSE;
		}

		if(!(AD->ad_RDArgs=AllocDosObject(DOS_RDARGS,TAG_DONE)))
		{
			CoerceMethod(cl,object,OM_DISPOSE);
			return FALSE;
		}

		DoMethod(object,OM_ADDTAIL,&((struct gtbApplication *)cl->cl_UserData)->ga_ListARexx);
		AD->ad_ObjectOK=TRUE;
	}
	return( (ULONG)object );
}

/*
** OM_DISPOSE
*/

ULONG SAVEDS REXX_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct REXXData *AD = INST_DATA( cl, o );

	if(AD->ad_ObjectOK) DoMethod(o,OM_REMOVE);
	if(AD->ad_RDArgs) FreeDosObject(DOS_RDARGS,AD->ad_RDArgs);
	if(AD->ad_MsgPort) DeletePort(AD->ad_MsgPort); AD->ad_MsgPort=NULL;
	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS REXX_SET(Class *cl, Object *o, struct opSet *ops)
{
	struct REXXData *AD = INST_DATA( cl, o );
	ULONG retval;
	struct TagItem *tag, *tstate;

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	tstate=ops->ops_AttrList;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case REXX_Handler:
				AD->ad_ARexxHandler=(ULONG (*)(Object *,struct gtbRexxCommand *,struct gtbRexxMessage *)) tag->ti_Data;
				break;
		}
	}

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS REXX_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval=TRUE;
	struct REXXData *AD = INST_DATA( cl, o );

	switch( opg->opg_AttrID )
	{
		case REXX_MessagePort: *opg->opg_Storage = (ULONG)AD->ad_MsgPort; break;
		case REXX_SigBit: *opg->opg_Storage = (ULONG)(1L<<AD->ad_MsgPort->mp_SigBit); break;
		case REXX_Signal: *opg->opg_Storage = (ULONG)(AD->ad_MsgPort->mp_SigBit); break;
		default: retval = DoSuperMethodA(cl, o, (Msg)opg); break;
	}
	return( retval );
}

ULONG SAVEDS REXX_GETMESSAGE(Class *cl,Object *o,Msg msg)
{
	struct Message *mesg;
	struct REXXData *AD = INST_DATA( cl, o );

	if(mesg=GetMsg(AD->ad_MsgPort))
	{
		ReplyMsg((struct Message *)mesg);
		return( (ULONG) mesg );
	}
	else return NULL;
}

struct RexxMsg *sendRexxMsg(struct REXXData *AD,STRPTR s, struct RexxMsg *m,LONG flags)
{
	struct MsgPort *ARexxPort;
	struct RexxMsg *RexxMsg=NULL;

	if(!AD->ad_MsgPort) return(NULL);

	if(!(RexxMsg=CreateRexxMsg(AD->ad_MsgPort,AD->ad_Extension,AD->ad_MsgPort->mp_Node.ln_Name))) return NULL;
	if(RexxMsg->rm_Args[0]=CreateArgstring(s,(LONG)strlen(s)))
	{
		RexxMsg->rm_Action=RXCOMM|flags;
		RexxMsg->rm_Args[15]=(STRPTR)m;
		Forbid();
		if(ARexxPort=FindPort("REXX")) PutMsg(ARexxPort,(struct Message *) RexxMsg);
		Permit();
		if(ARexxPort)
		{
			AD->ad_MessagesSent++;
			return(RexxMsg);
		}
	}
	if(RexxMsg->rm_Args[0])
	{
		DeleteArgstring(RexxMsg->rm_Args[0]);
		DeleteRexxMsg(RexxMsg);
		return(NULL);
	}
}

ULONG REXX_SENDMSG(Class *cl,Object *o,struct gtbRexxSendMsg *msg)
{
	struct REXXData *AD = INST_DATA( cl, o );

	return (ULONG)sendRexxMsg(AD,msg->Command,NULL,msg->Flags);
}

void replyRexxCmd(struct RexxMsg *msg, LONG rc, STRPTR s)
{
	msg->rm_Result1=rc;
	if((msg->rm_Action & (1L<<RXFB_RESULT)) && s) msg->rm_Result2=(LONG)CreateArgstring((UBYTE *)s,strlen((char *)s));
	else msg->rm_Result2=NULL;
}

ULONG SAVEDS REXX_HANDLEMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct REXXData *AD = INST_DATA( cl, o );
	struct RexxMsg *mesg;
	struct gtbRexxMessage rm;
	struct RDArgs *rda;
	STRPTR p;
	ULONG retval=1,a;
	LONG *(params[100]);

	if(AD->ad_MsgPort)
	{
		if((msg->Signals & (1L<<AD->ad_MsgPort->mp_SigBit)) && AD->ad_ARexxHandler)
		{
			while(mesg=(struct RexxMsg *)GetMsg(AD->ad_MsgPort))
			{
				if(mesg->rm_Node.mn_Node.ln_Type==NT_REPLYMSG)
				{
					if(mesg->rm_Args[15]) ReplyMsg((struct Message *)mesg->rm_Args[15]);
					DeleteArgstring(mesg->rm_Args[0]);
					DeleteRexxMsg(mesg);
					AD->ad_MessagesSent--;
				}
				else
				{
					p=(STRPTR)mesg->rm_Args[0];
					while(*p>0 && *p<=' ') p++;
					mesg->rm_Result1=0;
					mesg->rm_Result2=0;

					for(a=0;AD->ad_CommandTable[a].rc_Function;a++)
					{
						if(strnicmp((char *)AD->ad_CommandTable[a].rc_Function,p,strlen((char *)AD->ad_CommandTable[a].rc_Function))==0)
						{
							rm.rm_RexxMsg=mesg;
							rm.rm_Command=(STRPTR)p;
							rm.rm_Object=o;
							rm.rm_RC=RC_OK;
							rm.rm_Result=NULL;
							rm.rm_ArgList=(LONG **)params;
							rda=AD->ad_RDArgs;
							rda->RDA_Source.CS_Length=strlen(p);
							for(rda->RDA_Source.CS_CurChr=0;(p[rda->RDA_Source.CS_CurChr]!=' ') && (p[rda->RDA_Source.CS_CurChr]!='\0');rda->RDA_Source.CS_CurChr++);
							if(rda->RDA_Source.CS_Buffer=AllocVec(rda->RDA_Source.CS_Length+1,MEMF_ANY|MEMF_CLEAR))
							{
								strcpy(rda->RDA_Source.CS_Buffer,p);
								rda->RDA_Source.CS_Buffer[strlen(rda->RDA_Source.CS_Buffer)]='\n';
								ReadArgs(AD->ad_CommandTable[a].rc_Template,(long *)rm.rm_ArgList,AD->ad_RDArgs);
								retval=AD->ad_ARexxHandler(o,&AD->ad_CommandTable[a],&rm);
//								else
//									retval=AD->ad_ARexxHandler(o,NULL,&rm);
								FreeVec(rda->RDA_Source.CS_Buffer);
								FreeArgs(AD->ad_RDArgs);
								replyRexxCmd(mesg,rm.rm_RC,rm.rm_Result);
							}
							break;
						}
					}
					if(AD->ad_CommandTable[a].rc_Function==NULL)
					{
						if(!sendRexxMsg(AD,mesg->rm_Args[0],mesg,0))
						{
							replyRexxCmd(mesg,RC_FATAL,NULL);
							ReplyMsg((struct Message *)mesg);
						}
					}
					else
						ReplyMsg((struct Message *)mesg);
				}
			}
		}
	}
	return retval;
}

ULONG SAVEDS REXX_WAITMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct REXXData *AD = INST_DATA( cl, o );

	return Wait((1L<<AD->ad_MsgPort->mp_SigBit)|(msg->Signals));
}
