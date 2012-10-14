/*
** Messages Class
** $VER: messages.c 37.1 (15.4.98)
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
#define MESSAGECLASS "gtbmessageclass"

/*
** Prototypes
*/

Class *CreateMessageClass(struct gtbApplication *);
void DisposeMessageClass(Class *);
ULONG ASMFUNC MESG_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG MESG_NEW(Class *,Object *,struct opSet *);
ULONG MESG_DISPOSE(Class *, Object *, Msg);
ULONG MESG_SET(Class *, Object *, struct opSet *);
ULONG MESG_GET(Class *,Object *,struct opGet *);
ULONG MESG_GETMESSAGE(Class *,Object *,Msg);
ULONG MESG_HANDLEMESSAGE(Class *,Object *,struct gtbWaitMessage *);
ULONG MESG_WAITMESSAGE(Class *,Object *,struct gtbWaitMessage *);

/*
** Variables
*/
struct MESGData
{
	BOOL md_ObjectOK;
	struct MsgPort *md_MsgPort;
	struct MsgPort *md_MyMsgPort;
	ULONG md_SigBit;
	BYTE md_Signal;
	STRPTR md_Name;
	BYTE md_Pri;
	ULONG (*md_MesgHandler)(Object *,ULONG,APTR);
};

/*
** Create Class
*/

Class *CreateMessageClass(struct gtbApplication *appl)
{
	Class *cl=NULL;

	if(cl = MakeClass (NULL, ROOTCLASS, NULL, sizeof(struct MESGData), 0L))
	{
		cl->cl_Dispatcher.h_Entry = (HookFunction)  MESG_Dispatcher;
		cl->cl_UserData = (ULONG) appl;
		return cl;
	}
	return(NULL);
}

void DisposeMessageClass(Class *cl)
{
	FreeClass(cl);
}

/*
** Dispatcher
*/

ULONG ASMFUNC MESG_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = MESG_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = MESG_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = MESG_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = MESG_GET(cl, o, (struct opGet *)msg );
			break;
		case GTB_GETMESSAGE:
			retval = MESG_GETMESSAGE(cl,o,msg);
			break;
		case GTB_HANDLEMESSAGE:
			retval = MESG_HANDLEMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
			break;
		case GTB_WAITMESSAGE:
			retval = MESG_WAITMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
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

ULONG SAVEDS MESG_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct MESGData *MD;
	struct TagItem *ti;

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		MD = INST_DATA( cl, object );

		memset(MD,0,sizeof(struct MESGData));

		MD->md_MyMsgPort=NULL;
		MD->md_Signal=-1;
		MD->md_Name = (STRPTR)GetTagData( MESG_Name, NULL, ops->ops_AttrList );
		MD->md_Pri = (BYTE)GetTagData( MESG_Priority, 0, ops->ops_AttrList );
		MD->md_MesgHandler=(ULONG (*)(Object *,ULONG,APTR))GetTagData( MESG_Handler, NULL, ops->ops_AttrList );
		MD->md_MsgPort = (struct MsgPort *)GetTagData( MESG_MessagePort, NULL, ops->ops_AttrList );
		MD->md_SigBit = GetTagData( MESG_SigBit, NULL, ops->ops_AttrList );
		if(ti=FindTagItem(MESG_Signal,ops->ops_AttrList))
		{
			MD->md_Signal=AllocSignal(ti->ti_Data);
			MD->md_SigBit=1L<<MD->md_Signal;
		}
		if((!FindTagItem(MESG_SigBit,ops->ops_AttrList)) && (!FindTagItem(MESG_Signal,ops->ops_AttrList)) && (!MD->md_MsgPort))
		{
			if(MD->md_MsgPort)
				MD->md_MsgPort=MD->md_MyMsgPort=CreatePort(MD->md_Name,MD->md_Pri);
			else
				MD->md_MsgPort=MD->md_MyMsgPort=CreateMsgPort();
		}

		DoMethod(object,OM_ADDTAIL,&((struct gtbApplication *)cl->cl_UserData)->ga_ListMessages);
		MD->md_ObjectOK=TRUE;
	}
	return( (ULONG)object );
}

/*
** OM_DISPOSE
*/

ULONG SAVEDS MESG_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct MESGData *MD = INST_DATA( cl, o );

	if(MD->md_ObjectOK) DoMethod(o,OM_REMOVE);
	if(MD->md_MyMsgPort) DeleteMsgPort(MD->md_MyMsgPort); MD->md_MyMsgPort=NULL;
	if(MD->md_Signal!=-1) FreeSignal(MD->md_Signal);
	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS MESG_SET(Class *cl, Object *o, struct opSet *ops)
{
	struct MESGData *MD = INST_DATA( cl, o );
	ULONG retval;
	struct TagItem *tag, *tstate;

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	tstate=ops->ops_AttrList;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case MESG_Handler:
				MD->md_MesgHandler=(ULONG (*)(Object *,ULONG,APTR)) tag->ti_Data;
				break;
			case MESG_Signal:
				if(MD->md_Signal!=-1) FreeSignal(MD->md_Signal);
				MD->md_Signal=AllocSignal(tag->ti_Data);
				MD->md_SigBit=1L<<MD->md_Signal;
				break;
		}
	}

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS MESG_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval=TRUE;
	struct MESGData *MD = INST_DATA( cl, o );

	switch( opg->opg_AttrID )
	{
		case MESG_MessagePort: *opg->opg_Storage = (ULONG)MD->md_MsgPort; break;
		case MESG_SigBit:
			if(MD->md_MsgPort)
				*opg->opg_Storage = (ULONG)(1L<<MD->md_MsgPort->mp_SigBit);
			else
				*opg->opg_Storage = MD->md_SigBit;
			break;
		case MESG_Signal:
			if(MD->md_MsgPort)
				*opg->opg_Storage = (ULONG)(MD->md_MsgPort->mp_SigBit);
			else
				*opg->opg_Storage = (ULONG)MD->md_Signal;
			break;
		default: retval = DoSuperMethodA(cl, o, (Msg)opg); break;
	}
	return( retval );
}

ULONG SAVEDS MESG_GETMESSAGE(Class *cl,Object *o,Msg msg)
{
	struct Message *mesg;
	struct MESGData *MD = INST_DATA( cl, o );

	if(mesg=GetMsg(MD->md_MsgPort))
	{
		ReplyMsg((struct Message *)mesg);
		return( (ULONG) mesg );
	}
	else return NULL;
}

ULONG SAVEDS MESG_HANDLEMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct MESGData *MD = INST_DATA( cl, o );
	struct Message *mesg;
	ULONG retval=1;

	if(MD->md_MsgPort)
	{
		if((msg->Signals & (1L<<MD->md_MsgPort->mp_SigBit)) && MD->md_MesgHandler)
		{
			while(mesg=GetMsg(MD->md_MsgPort))
			{
				retval=MD->md_MesgHandler(o,NULL,mesg);
				ReplyMsg((struct Message *)mesg);
			}
		}
	}
	else
	{
		if((msg->Signals & MD->md_SigBit) && MD->md_MesgHandler)
		{
			retval=MD->md_MesgHandler(o,NULL,NULL);
		}
	}
	return retval;
}

ULONG SAVEDS MESG_WAITMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct MESGData *MD = INST_DATA( cl, o );

	if(MD->md_MsgPort)
		return Wait((1L<<MD->md_MsgPort->mp_SigBit)|(msg->Signals));
	else
		return Wait((MD->md_SigBit)|(msg->Signals));
}
