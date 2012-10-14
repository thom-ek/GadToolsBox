/*
** AmigaGuide Class
** $VER: amigaguide.c 37.1 (2.6.98)
*/

#include<dos.h>
#include<stdio.h>
#include<string.h>
#include<clib/macros.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<proto/intuition.h>
#include<proto/amigaguide.h>
#include<exec/memory.h>
#include<intuition/gadgetclass.h>
#include<intuition/intuitionbase.h>
#include<intuition/icclass.h>
#include"gadtoolsbox.h"

/*
** Defines
*/

#define unless(x) if(!(x))
#define SetTagArg(tag,id,data)	{ tag.ti_Tag = (ULONG)(id); tag.ti_Data = (ULONG)(data); }
typedef ULONG (*HookFunction)(void);
#define SAVEDS 
#define ASMFUNC __saveds __asm
#define REG(x) register __## x
#define AMIGAGUIDECLASS "gtbamigaguideclass"

/*
** Prototypes
*/

Class *CreateAmigaGuideClass(struct gtbApplication *);
void DisposeAmigaGuideClass(Class *);
ULONG ASMFUNC AGUI_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG AGUI_NEW(Class *,Object *,struct opSet *);
ULONG AGUI_DISPOSE(Class *, Object *, Msg);
ULONG AGUI_SET(Class *, Object *, struct opSet *);
ULONG AGUI_GET(Class *,Object *,struct opGet *);
ULONG AGUI_GETMESSAGE(Class *,Object *,Msg);
ULONG AGUI_HANDLEMESSAGE(Class *,Object *,struct gtbWaitMessage *);
ULONG AGUI_WAITMESSAGE(Class *,Object *,struct gtbWaitMessage *);

/*
** Variables
*/
struct AGUIData
{
	AMIGAGUIDECONTEXT ag_AGC;
	struct NewAmigaGuide ag_NAG;
	BOOL ag_ObjectOK;
	Object *ag_ScreenObj;
	ULONG ag_SigBit;
	ULONG ag_Signal;
	ULONG ag_HelpGroup;
	BOOL ag_Activate;
	ULONG (*ag_MesgHandler)(Object *,ULONG,struct AmigaGuideMsg *);

	Object *ag_ARexxObj;
	struct MsgPort *ag_ARexxPort;
	STRPTR ag_ARexxPortName;
};

/*
** Create Class
*/

Class *CreateAmigaGuideClass(struct gtbApplication *appl)
{
	Class *cl=NULL;

	if(cl = MakeClass (NULL, ROOTCLASS, NULL, sizeof(struct AGUIData), 0L))
	{
		cl->cl_Dispatcher.h_Entry = (HookFunction)  AGUI_Dispatcher;
		cl->cl_UserData = (ULONG) appl;
		return cl;
	}
	return(NULL);
}

void DisposeAmigaGuideClass(Class *cl)
{
	FreeClass(cl);
}

/*
** Dispatcher
*/

ULONG ASMFUNC AGUI_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = AGUI_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = AGUI_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = AGUI_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = AGUI_GET(cl, o, (struct opGet *)msg );
			break;
		case GTB_GETMESSAGE:
			retval = AGUI_GETMESSAGE(cl,o,msg);
			break;
		case GTB_HANDLEMESSAGE:
			retval = AGUI_HANDLEMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
			break;
		case GTB_WAITMESSAGE:
			retval = AGUI_WAITMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
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

ULONG SAVEDS AGUI_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct AGUIData *AG;

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		AG = INST_DATA( cl, object );

		memset(AG,0,sizeof(struct AGUIData));

		AG->ag_MesgHandler=(ULONG (*)(Object *,ULONG,APTR))GetTagData( AGUI_Handler, NULL, ops->ops_AttrList );

		AG->ag_Activate=(BOOL)GetTagData( AGUI_Activate, FALSE, ops->ops_AttrList );

		AG->ag_ARexxPort=(struct MsgPort *)GetTagData( AGUI_ARexxPort, NULL, ops->ops_AttrList );
		AG->ag_ARexxPortName=(STRPTR)GetTagData( AGUI_ARexxPortName, NULL, ops->ops_AttrList );
		if(AG->ag_ARexxObj=(Object *)GetTagData( AGUI_ARexxObject, NULL, ops->ops_AttrList ))
			GetAttr(REXX_MessagePort,AG->ag_ARexxObj,(ULONG *)&AG->ag_ARexxPort);

		if(!(AG->ag_NAG.nag_Screen=(struct Screen *)GetTagData( AGUI_Screen, NULL, ops->ops_AttrList )))
		{
			if(AG->ag_ScreenObj = (Object *)GetTagData( AGUI_ScreenObject, NULL, ops->ops_AttrList ))
				GetAttr(SCRN_Screen,AG->ag_ScreenObj,(ULONG *)&AG->ag_NAG.nag_Screen);
		}
		AG->ag_NAG.nag_Lock=NULL;
		AG->ag_NAG.nag_Name=(STRPTR)GetTagData( AGUI_Name, NULL, ops->ops_AttrList );
		AG->ag_NAG.nag_BaseName=(STRPTR)GetTagData( AGUI_BaseName, NULL, ops->ops_AttrList );
		AG->ag_NAG.nag_PubScreen=(STRPTR)GetTagData( AGUI_PubScreenName, NULL, ops->ops_AttrList );
		AG->ag_NAG.nag_HostPort=NULL; //AG->ag_HostPort;
		AG->ag_NAG.nag_ClientPort=AG->ag_ARexxPortName;
		AG->ag_NAG.nag_Flags=AG->ag_Activate?NULL:HTF_NOACTIVATE;
		AG->ag_NAG.nag_Context=(STRPTR *)GetTagData( AGUI_Context, NULL, ops->ops_AttrList );
		AG->ag_NAG.nag_Extens=NULL;
		AG->ag_NAG.nag_Client=NULL;

		AG->ag_HelpGroup=GetTagData( AGUI_HelpGroup, NULL, ops->ops_AttrList );

		if(!(AG->ag_AGC=OpenAmigaGuideAsync(&AG->ag_NAG,
			AGA_HelpGroup,AG->ag_HelpGroup,
			AG->ag_ARexxPort?AGA_ARexxPort:TAG_IGNORE, AG->ag_ARexxPort,
			AG->ag_ARexxPortName?AGA_ARexxPortName:TAG_IGNORE, AG->ag_ARexxPortName,
			TAG_DONE)))
		{
			CoerceMethod(cl,object,OM_DISPOSE);
			return FALSE;
		}

		AG->ag_Signal=AmigaGuideSignal(AG->ag_AGC);
		AG->ag_SigBit=1L<<AG->ag_Signal;

		DoMethod(object,OM_ADDTAIL,&((struct gtbApplication *)cl->cl_UserData)->ga_ListAmigaGuide);
		AG->ag_ObjectOK=TRUE;
	}
	return( (ULONG)object );
}

/*
** OM_DISPOSE
*/

ULONG SAVEDS AGUI_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct AGUIData *AG = INST_DATA( cl, o );

	if(AG->ag_ObjectOK) DoMethod(o,OM_REMOVE);
	if(AG->ag_AGC) CloseAmigaGuide(AG->ag_AGC);
	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS AGUI_SET(Class *cl, Object *o, struct opSet *ops)
{
	struct AGUIData *AG = INST_DATA( cl, o );
	ULONG retval;
	struct TagItem *tag, *tstate;

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	tstate=ops->ops_AttrList;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case AGUI_Handler:
				AG->ag_MesgHandler=(ULONG (*)(Object *,ULONG,APTR)) tag->ti_Data;
				break;
		}
	}

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS AGUI_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval=TRUE;
	struct AGUIData *AG = INST_DATA( cl, o );

	switch( opg->opg_AttrID )
	{
		case AGUI_SigBit:
			*opg->opg_Storage = AG->ag_SigBit;
			break;
		case AGUI_Signal:
			*opg->opg_Storage = (ULONG)AG->ag_Signal;
			break;
		case AGUI_Context:
			*opg->opg_Storage = (ULONG)AG->ag_AGC;
			break;
		default: retval = DoSuperMethodA(cl, o, (Msg)opg); break;
	}
	return( retval );
}

ULONG SAVEDS AGUI_GETMESSAGE(Class *cl,Object *o,Msg msg)
{
	struct AmigaGuideMsg *mesg;
	struct AGUIData *AG = INST_DATA( cl, o );

	if(mesg=GetAmigaGuideMsg(AG->ag_AGC))
	{
		ReplyAmigaGuideMsg(mesg);
		return( (ULONG) mesg );
	}
	else return NULL;
}

ULONG SAVEDS AGUI_HANDLEMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct AGUIData *AG = INST_DATA( cl, o );
	struct AmigaGuideMsg *mesg;
	ULONG retval=1;

	if((msg->Signals & (1L<<AG->ag_Signal)))
	{
		if(mesg=GetAmigaGuideMsg(AG->ag_AGC))
		{
			if(AG->ag_MesgHandler) retval=AG->ag_MesgHandler(o,mesg->agm_Type,mesg);
			ReplyAmigaGuideMsg(mesg);
		}
	}
	return retval;
}

ULONG SAVEDS AGUI_WAITMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct AGUIData *AG = INST_DATA( cl, o );

	return Wait((AG->ag_SigBit)|(msg->Signals));
}
