/*
** Commodities Class
** $VER: commodities.c 37.1 (15.4.98)
*/

#include<dos.h>
#include<stdio.h>
#include<string.h>
#include<clib/macros.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<proto/intuition.h>
#include<proto/commodities.h>
#include<exec/memory.h>
#include<intuition/gadgetclass.h>
#include<intuition/intuitionbase.h>
#include<intuition/icclass.h>
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
#define CXCLASS "gtbcommoditiesclass"

/*
** Prototypes
*/

Class *CreateCommoditiesClass(struct gtbApplication *);
void DisposeCommoditiesClass(Class *);
ULONG ASMFUNC COMM_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG COMM_NEW(Class *,Object *,struct opSet *);
ULONG COMM_DISPOSE(Class *, Object *, Msg);
ULONG COMM_SET(Class *, Object *, struct opSet *);
ULONG COMM_GET(Class *,Object *,struct opGet *);
ULONG COMM_GETMESSAGE(Class *,Object *,Msg);
ULONG COMM_HANDLEMESSAGE(Class *,Object *,struct gtbWaitMessage *);
ULONG COMM_WAITMESSAGE(Class *,Object *,struct gtbWaitMessage *);

/*
** Variables
*/
struct COMMData
{
	BOOL cd_ObjectOK;
	struct NewBroker cd_NewBroker;
	struct MsgPort *cd_MsgPort;
	CxObj *cd_Broker;
	LONG cd_BrokerError;
	struct CxMessage cd_CxMessage;
	BOOL cd_Active;
	ULONG (*cd_CommHandler)(Object *,ULONG,APTR);
};

/*
** Create Class
*/

Class *CreateCommoditiesClass(struct gtbApplication *appl)
{
	Class *cl=NULL;

	if(cl = MakeClass (NULL, ROOTCLASS, NULL, sizeof(struct COMMData), 0L))
	{
		cl->cl_Dispatcher.h_Entry = (HookFunction)  COMM_Dispatcher;
		cl->cl_UserData = (ULONG) appl;
		return cl;
	}
	return(NULL);
}

void DisposeCommoditiesClass(Class *cl)
{
	FreeClass(cl);
}

/*
** Dispatcher
*/

ULONG ASMFUNC COMM_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = COMM_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = COMM_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = COMM_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = COMM_GET(cl, o, (struct opGet *)msg );
			break;
		case GTB_GETMESSAGE:
			retval = COMM_GETMESSAGE(cl,o,msg);
			break;
		case GTB_HANDLEMESSAGE:
			retval = COMM_HANDLEMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
			break;
		case GTB_WAITMESSAGE:
			retval = COMM_WAITMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
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

ULONG SAVEDS COMM_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct COMMData *CD;
	LONG *BrokerError;
	STRPTR PopKey;
	CxObj *PopKeyObj;

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		CD = INST_DATA( cl, object );

		memset(CD,0,sizeof(struct COMMData));

		CD->cd_Active=FALSE;
		CD->cd_MsgPort=NULL;
		CD->cd_NewBroker.nb_Unique=NULL;
		CD->cd_NewBroker.nb_Flags=NULL;
		CD->cd_NewBroker.nb_Version=NB_VERSION;
		CD->cd_NewBroker.nb_ReservedChannel=NULL;
		CD->cd_NewBroker.nb_Name = (STRPTR)GetTagData( COMM_Name, NULL, ops->ops_AttrList );
		CD->cd_NewBroker.nb_Title = (STRPTR)GetTagData( COMM_Title, NULL, ops->ops_AttrList );
		CD->cd_NewBroker.nb_Descr = (STRPTR)GetTagData( COMM_Description, NULL, ops->ops_AttrList );
		CD->cd_NewBroker.nb_Unique |= ((GetTagData( COMM_Unique, FALSE, ops->ops_AttrList )?NBU_UNIQUE:NULL));
		CD->cd_NewBroker.nb_Unique |= ((GetTagData( COMM_Notify, FALSE, ops->ops_AttrList )?NBU_NOTIFY:NULL));
		CD->cd_NewBroker.nb_Flags |= ((GetTagData( COMM_ShowHide, FALSE, ops->ops_AttrList )?COF_SHOW_HIDE:NULL));
		CD->cd_NewBroker.nb_Pri = (BYTE)GetTagData( COMM_Priority, 0, ops->ops_AttrList );
		CD->cd_NewBroker.nb_Port = (struct MsgPort *)GetTagData( COMM_MessagePort, NULL, ops->ops_AttrList );
		CD->cd_CommHandler=(ULONG (*)(Object *,ULONG,APTR))GetTagData( COMM_Handler, NULL, ops->ops_AttrList );
		BrokerError = (LONG *)GetTagData( COMM_Error, (LONG)&CD->cd_BrokerError, ops->ops_AttrList );
		if(!CD->cd_NewBroker.nb_Port) CD->cd_NewBroker.nb_Port=CD->cd_MsgPort=CreateMsgPort();
		CD->cd_Broker=CxBroker(&CD->cd_NewBroker,BrokerError);
		if((*BrokerError)!=CBERR_OK)
		{
			CoerceMethod(cl, object, OM_DISPOSE);
			object=NULL;
			return((ULONG)object);
		}

		PopKey = (STRPTR) GetTagData( COMM_AddHotKeyStr, NULL, ops->ops_AttrList );
		if(PopKey)
		{
			PopKeyObj=HotKey(PopKey,CD->cd_NewBroker.nb_Port,GetTagData( COMM_AddHotKeyID, 0, ops->ops_AttrList ));
			AttachCxObj(CD->cd_Broker,PopKeyObj);
		}
		ActivateCxObj(CD->cd_Broker,CD->cd_Active=GetTagData( COMM_Activate, 1L, ops->ops_AttrList ));
		DoMethod(object,OM_ADDTAIL,&((struct gtbApplication *)cl->cl_UserData)->ga_ListCommodities);
		CD->cd_ObjectOK=TRUE;
	}
	return( (ULONG)object );
}

/*
** OM_DISPOSE
*/

ULONG SAVEDS COMM_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct COMMData *CD = INST_DATA( cl, o );

	if(CD->cd_ObjectOK) DoMethod(o,OM_REMOVE);
	if(CD->cd_Broker) DeleteCxObjAll(CD->cd_Broker); CD->cd_Broker=NULL;
	if(CD->cd_MsgPort) DeleteMsgPort(CD->cd_MsgPort); CD->cd_MsgPort=NULL;
	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS COMM_SET(Class *cl, Object *o, struct opSet *ops)
{
	struct COMMData *CD = INST_DATA( cl, o );
	ULONG retval;
	LONG *BrokerError;
	STRPTR PopKey;
	CxObj *PopKeyObj;
	struct TagItem *tag, *tstate;

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	tstate=ops->ops_AttrList;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case COMM_AddHotKeyStr:
				if(PopKey=(STRPTR)tag->ti_Data)
				{
					PopKeyObj=HotKey(PopKey,CD->cd_NewBroker.nb_Port,GetTagData( COMM_AddHotKeyID, 0, ops->ops_AttrList ));
					AttachCxObj(CD->cd_Broker,PopKeyObj);
					BrokerError = (LONG *)GetTagData( COMM_Error, (LONG)&CD->cd_BrokerError, ops->ops_AttrList );
					*BrokerError=CxObjError(PopKeyObj);
				}
				break;
			case COMM_Activate:
				ActivateCxObj(CD->cd_Broker,tag->ti_Data);
				break;
			case COMM_Handler:
				CD->cd_CommHandler=(ULONG (*)(Object *,ULONG,APTR)) tag->ti_Data;
				break;
		}
	}

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS COMM_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval=TRUE;
	struct COMMData *CD = INST_DATA( cl, o );

	switch( opg->opg_AttrID )
	{
		case COMM_Activate:		*opg->opg_Storage = (ULONG)CD->cd_Active; break;
		case COMM_MessagePort:*opg->opg_Storage = (ULONG)CD->cd_NewBroker.nb_Port;
		case COMM_SigBit:			*opg->opg_Storage = (ULONG)(1L<<CD->cd_NewBroker.nb_Port->mp_SigBit); break;
		case COMM_Broker:			*opg->opg_Storage = (ULONG)CD->cd_Broker; break;
		default: retval = DoSuperMethodA(cl, o, (Msg)opg); break;
	}
	return( retval );
}

ULONG SAVEDS COMM_GETMESSAGE(Class *cl,Object *o,Msg msg)
{
	CxMsg *cxmsg;
	struct COMMData *CD = INST_DATA( cl, o );

	if(cxmsg=(CxMsg *)GetMsg(CD->cd_NewBroker.nb_Port))
	{
		CD->cd_CxMessage.cx_ID  = CxMsgID(cxmsg);
		CD->cd_CxMessage.cx_Type= CxMsgType(cxmsg);
		CD->cd_CxMessage.cx_Data= CxMsgData(cxmsg);
		ReplyMsg((struct Message *)cxmsg);
		return( (ULONG) &CD->cd_CxMessage );
	}
	else return NULL;
}

ULONG SAVEDS COMM_HANDLEMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct COMMData *CD = INST_DATA( cl, o );
	ULONG retval=1;

	if((msg->Signals & (1L<<CD->cd_NewBroker.nb_Port->mp_SigBit)) && CD->cd_CommHandler)
	{
		while(DoMethod(o,GTB_GETMESSAGE))
			retval=CD->cd_CommHandler(o,CD->cd_CxMessage.cx_Type,&CD->cd_CxMessage);
	}
	return retval;
}

ULONG SAVEDS COMM_WAITMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct COMMData *CD = INST_DATA( cl, o );

	return Wait((1L<<CD->cd_NewBroker.nb_Port->mp_SigBit)|(msg->Signals));
}
