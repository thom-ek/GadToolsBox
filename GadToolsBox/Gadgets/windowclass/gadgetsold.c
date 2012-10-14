/*
** Gadget Class
** $VER: gadget.c 37.1 (15.4.98)
*/

#include<dos.h>
#include<stdio.h>
#include<string.h>
#include<clib/macros.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<proto/intuition.h>
#include<proto/gadtools.h>
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
#define GADGETGADGET "gtbgadtoolsclass"

/*
** Prototypes
*/

Class *CreateGadgetClass(struct gtbApplication *);
void DisposeGadgetClass(Class *);
ULONG ASMFUNC GADG_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG GADG_NEW(Class *,Object *,struct opSet *);
ULONG GADG_DISPOSE(Class *, Object *, Msg);
ULONG GADG_SET(Class *, Object *, struct opSet *);
ULONG GADG_GET(Class *,Object *,struct opGet *);
ULONG GADG_HANDLEMESSAGE(Class *,Object *,struct gtbWaitMessage *);

/*
** Variables
*/
struct GADGData
{
	BOOL gd_ObjectOK;
	struct NewGadget gd_NewGadget;
	struct Gadget *gd_Gadget;
	ULONG (*gd_GadgHandler)(Object *,ULONG,APTR);
};

/*
** Create Class
*/

Class *CreateGadgetClass(struct gtbApplication *appl)
{
	Class *cl=NULL;

	if(cl = MakeClass (NULL, GROUPGCLASS, NULL, sizeof(struct GADGData), 0L))
	{
		cl->cl_Dispatcher.h_Entry = (HookFunction)  GADG_Dispatcher;
//		cl->cl_UserData = (ULONG) appl;
		return cl;
	}
	return(NULL);
}

void DisposeGadgetClass(Class *cl)
{
	FreeClass(cl);
}

/*
** Dispatcher
*/

ULONG ASMFUNC GADG_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = GADG_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = GADG_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = GADG_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = GADG_GET(cl, o, (struct opGet *)msg );
			break;
		case GTB_HANDLEMESSAGE:
			retval = GADG_HANDLEMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
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

ULONG SAVEDS GADG_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct GADGData *GD;

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		GD = INST_DATA( cl, object );

		memset(GD,0,sizeof(struct GADGData));

		GD->gd_NewGadget.ng_LeftEdge=GetTagData( GA_Left, 0, ops->ops_AttrList );
		GD->gd_NewGadget.ng_TopEdge=GetTagData( GA_Top, 0, ops->ops_AttrList );
		GD->gd_NewGadget.ng_Width=GetTagData( GA_Width, 10, ops->ops_AttrList );
		GD->gd_NewGadget.ng_Height=GetTagData( GA_Height, 10, ops->ops_AttrList );
		GD->gd_NewGadget.ng_GadgetText=(STRPTR)GetTagData( GA_Text, NULL, ops->ops_AttrList );
		GD->gd_NewGadget.ng_GadgetID=GetTagData( GA_ID, 0, ops->ops_AttrList );
		GD->gd_NewGadget.ng_Flags=PLACETEXT_IN; //GetTagData( GA_Height, 10, ops->ops_AttrList );
		GD->gd_NewGadget.ng_VisualInfo=(APTR)GetVisualInfo(GetTagData( GADG_Screen, NULL, ops->ops_AttrList ),TAG_DONE);
		GD->gd_NewGadget.ng_UserData=(APTR)GetTagData( GA_UserData, 0, ops->ops_AttrList );

		GD->gd_Gadget=CreateGadgetA(GetTagData( GADG_Kind, BUTTON_KIND, ops->ops_AttrList ),GetTagData( GA_Previous, NULL, ops->ops_AttrList ),&GD->gd_NewGadget,ops->ops_AttrList);

		DoMethod(o,OM_ADDMEMBER,GD->gd_Gadget);

		GD->gd_GadgHandler=(ULONG (*)(Object *,ULONG,APTR))GetTagData( GADG_Handler, NULL, ops->ops_AttrList );
		GD->gd_ObjectOK=TRUE;
	}
	return( (ULONG)object );
}

/*
** OM_DISPOSE
*/

ULONG SAVEDS GADG_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct GADGData *GD = INST_DATA( cl, o );

	DoMethod(o,OM_REMMEMBER,GD->gd_Gadget);
	FreeVisualInfo(GD->gd_NewGadget.ng_VisualInfo);
	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS GADG_SET(Class *cl, Object *o, struct opSet *ops)
{
	struct GADGData *GD = INST_DATA( cl, o );
	ULONG retval;
	struct TagItem *tag, *tstate;

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	tstate=ops->ops_AttrList;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case GADG_Handler:
				GD->gd_GadgHandler=(ULONG (*)(Object *,ULONG,APTR)) tag->ti_Data;
				break;
		}
	}

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS GADG_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval=TRUE;
	struct GADGData *GD = INST_DATA( cl, o );

	switch( opg->opg_AttrID )
	{
		default: retval = DoSuperMethodA(cl, o, (Msg)opg); break;
	}
	return( retval );
}

ULONG SAVEDS GADG_HANDLEMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct GADGData *GD = INST_DATA( cl, o );

	return GD->gd_GadgHandler(o,NULL,NULL);
}
