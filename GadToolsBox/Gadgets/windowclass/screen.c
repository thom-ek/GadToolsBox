/*
** Screen Class
** $VER: screen.c 37.1 (15.4.98)
*/

#include<dos.h>
#include<stdio.h>
#include<string.h>
#include<clib/macros.h>
#include<proto/wb.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<proto/gadtools.h>
#include<proto/graphics.h>
#include<proto/intuition.h>
#include<graphics/gfxmacros.h>
#include<exec/memory.h>
#include<libraries/gadtools.h>
#include<intuition/imageclass.h>
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
#define SCREENCLASS "gtbscreenclass"

/*
** Prototypes
*/

Class *CreateScreenClass(struct gtbApplication *);
void DisposeScreenClass(Class *);
ULONG ASMFUNC SCRN_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG SCRN_NEW(Class *,Object *,struct opSet *);
ULONG SCRN_DISPOSE(Class *, Object *, Msg);
ULONG SCRN_SET(Class *, Object *, struct opSet *);
ULONG SCRN_GET(Class *,Object *,struct opGet *);
ULONG SCRN_UPDATE(Class *,Object *,struct opUpdate *);
ULONG SCRN_NOTIFY(Class *,Object *,struct opUpdate *);
ULONG SCRN_RENDER(Class *,Object *,struct gpRender *);
ULONG SCRN_GETMESSAGE(Class *,Object *,Msg);
ULONG SCRN_HANDLEMESSAGE(Class *,Object *,struct gtbWaitMessage *);
ULONG SCRN_WAITMESSAGE(Class *,Object *,struct gtbWaitMessage *);
ULONG SCRN_ADDMEMBER(Class *,Object *,struct opMember *);
ULONG SCRN_REMMEMBER(Class *,Object *,struct opMember *);

/*
** Variables
*/

struct SCRNData
{
	BOOL sd_ObjectOK;

	struct Screen *sd_Screen;
	struct MinList sd_ListWindows;

	Object *sd_Message;
	ULONG sd_Signal;
	ULONG (*sd_ScreenHandler)(Object *,ULONG,APTR);

	struct DrawInfo *sd_DrawInfo;
	APTR sd_VisualInfo;
	BOOL sd_IsLock;
};

/*
** Create Class
*/

Class *CreateScreenClass(struct gtbApplication *appl)
{
	Class *cl=NULL;

	if(cl = MakeClass (NULL, GADGETCLASS, NULL, sizeof(struct SCRNData), 0L))
	{
		cl->cl_Dispatcher.h_Entry = (HookFunction)  SCRN_Dispatcher;
		cl->cl_UserData = (ULONG) appl;
		return cl;
	}
	return(NULL);
}

void DisposeScreenClass(Class *cl)
{
	FreeClass(cl);
}

#define COPYTAG(w,o) ((ti=FindTagItem(w,ops->ops_AttrList))?o:TAG_IGNORE)

/*
** Dispatcher
*/

ULONG ASMFUNC SCRN_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = SCRN_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = SCRN_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = SCRN_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = SCRN_GET(cl, o, (struct opGet *)msg );
			break;
		case OM_UPDATE:
			retval = SCRN_UPDATE(cl, o, (struct opUpdate *)msg );
			break;
		case OM_NOTIFY:
			retval = SCRN_NOTIFY(cl, o, (struct opUpdate *)msg );
			break;
		case GM_RENDER:
			retval = SCRN_RENDER(cl, o, (struct gpRender *)msg );
			break;
		case GTB_GETMESSAGE:
			retval = SCRN_GETMESSAGE(cl,o,msg);
			break;
		case GTB_HANDLEMESSAGE:
			retval = SCRN_HANDLEMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
			break;
		case GTB_WAITMESSAGE:
			retval = SCRN_WAITMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
			break;
		case OM_ADDMEMBER:
			retval = SCRN_ADDMEMBER(cl,o,(struct opMember *)msg);
			break;
		case OM_REMMEMBER:
			retval = SCRN_REMMEMBER(cl,o,(struct opMember *)msg);
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

ULONG SAVEDS SCRN_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct SCRNData *SD;
	struct TagItem *ti,*tag,*tstate;

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		SD = INST_DATA( cl, object );

		memset(SD,0,sizeof(struct SCRNData));
		SD->sd_IsLock=FALSE;

		NewList((struct List *)&SD->sd_ListWindows);

		if(ti=FindTagItem(SCRN_Handler,ops->ops_AttrList))
		{
			if(!(SD->sd_Message=NewObject(((struct gtbApplication *)cl->cl_UserData)->ga_MsgClass,NULL,
				(tag=FindTagItem(SCRN_PubSig,ops->ops_AttrList))?MESG_SigBit:MESG_Signal,tag?(1L<<tag->ti_Data):-1,
				MESG_Handler,ti->ti_Data,
				TAG_DONE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
			if(tag) SD->sd_Signal=tag->ti_Data;
			else GetAttr(MESG_Signal,SD->sd_Message,&SD->sd_Signal);
			SD->sd_ScreenHandler=(ULONG (*)(Object *,ULONG,APTR))ti->ti_Data;
		}

		if(ti=FindTagItem(SCRN_LockPubName,ops->ops_AttrList))
		{
			if(!(SD->sd_Screen=LockPubScreen((UBYTE *)ti->ti_Data)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
			SD->sd_IsLock=TRUE;
		}
		else
		{
			ti=NULL;
			if(!(SD->sd_Screen=OpenScreenTags(NULL,
				(FindTagItem(SCRN_Handler,ops->ops_AttrList)||(ti=FindTagItem(SCRN_PubTask,ops->ops_AttrList)))?SA_PubTask:TAG_IGNORE,ti?ti->ti_Data:(ULONG)FindTask(NULL),
				(FindTagItem(SCRN_Handler,ops->ops_AttrList)||FindTagItem(SCRN_PubSig,ops->ops_AttrList))?SA_PubSig:TAG_IGNORE,SD->sd_Signal,
				COPYTAG(SCRN_Left,SA_Left),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Top,SA_Top),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Width,SA_Width),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Height,SA_Height),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Depth,SA_Depth),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_DetailPen,SA_DetailPen),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_BlockPen,SA_BlockPen),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Title,SA_Title),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Font,SA_Font),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_BitMap,SA_BitMap),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_ShowTitle,SA_ShowTitle),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Behind,SA_Behind),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Quiet,SA_Quiet),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Type,SA_Type),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_DisplayID,SA_DisplayID),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Overscan,SA_Overscan),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_DClip,SA_DClip),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_AutoScroll,SA_AutoScroll),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_PubName,SA_PubName),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Pens,SA_Pens),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Colors,SA_Colors),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_FullPalette,SA_FullPalette),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_ErrorCode,SA_ErrorCode),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_SysFont,SA_SysFont),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Parent,SA_Parent),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_FrontChild,SA_FrontChild),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_BackChild,SA_BackChild),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_BackFill,SA_BackFill),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Draggable,SA_Draggable),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Exclusive,SA_Exclusive),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_SharePens,SA_SharePens),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Colors32,SA_Colors32),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_Interleaved,SA_Interleaved),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_VideoControl,SA_VideoControl),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_ColorMapEntries,SA_ColorMapEntries),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_LikeWorkbench,SA_LikeWorkbench),(ti?ti->ti_Data:NULL),
				COPYTAG(SCRN_MinimizeISG,SA_MinimizeISG),(ti?ti->ti_Data:NULL),
				TAG_DONE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
		}
		tstate=ops->ops_AttrList;
		while(tag = NextTagItem( &tstate ))
		{
			switch(tag->ti_Tag)
			{
				case SCRN_Private: PubScreenStatus(SD->sd_Screen,tag->ti_Data?PSNF_PRIVATE:0); break;
			}
		}
		if(!(SD->sd_DrawInfo=GetScreenDrawInfo(SD->sd_Screen)))
		{
			CoerceMethod(cl,object,OM_DISPOSE);
			return FALSE;
		}
		if(!(SD->sd_VisualInfo=GetVisualInfo(SD->sd_Screen,TAG_DONE)))
		{
			CoerceMethod(cl,object,OM_DISPOSE);
			return FALSE;
		}
		DoMethod(object,OM_ADDTAIL,&((struct gtbApplication *)cl->cl_UserData)->ga_ListScreens);
		SD->sd_ObjectOK=TRUE;
	}
	return( (ULONG)object );
}

/*
** OM_DISPOSE
*/

ULONG SAVEDS SCRN_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct SCRNData *SD = INST_DATA( cl, o );
	struct Node *node;

	if(SD->sd_ObjectOK) DoMethod(o,OM_REMOVE); SD->sd_ObjectOK=FALSE;

	while(node=RemTail((struct List *)&SD->sd_ListWindows))
	{
		DisposeObject((Object *)node->ln_Name);
		FreeVec(node);
	}

	if(SD->sd_VisualInfo) FreeVisualInfo(SD->sd_VisualInfo);
	if(SD->sd_DrawInfo)		FreeScreenDrawInfo(SD->sd_Screen,SD->sd_DrawInfo);
	if(SD->sd_IsLock)			UnlockPubScreen(NULL,SD->sd_Screen);
	else if(SD->sd_Screen)CloseScreen(SD->sd_Screen);
	if(SD->sd_Message)		DisposeObject(SD->sd_Message);

	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS SCRN_SET(Class *cl, Object *o, struct opSet *ops)
{
	ULONG sr=FALSE,sleft,stop;
	struct SCRNData *SD = INST_DATA( cl, o );
	struct TagItem *tag, *tstate, notify;
	ULONG retval;

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	sleft=SD->sd_Screen->LeftEdge;
	stop=SD->sd_Screen->TopEdge;

	tstate=ops->ops_AttrList;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case SCRN_Left: sleft=tag->ti_Data; sr=TRUE; break;
			case SCRN_Top: stop=tag->ti_Data; sr=TRUE; break;

			case SCRN_Handler: SD->sd_ScreenHandler=(ULONG (*)(Object *,ULONG,APTR))tag->ti_Data;

//			case SCRN_Title: SetWindowTitles(SD->sd_Window,(UBYTE *)tag->ti_Data,(UBYTE *)-1); break;
			case SCRN_Private: PubScreenStatus(SD->sd_Screen,tag->ti_Data?PSNF_PRIVATE:0); break;
			case SCRN_ShowTitle: ShowTitle(SD->sd_Screen,tag->ti_Data); break;
		}
	}

	SetTagArg( notify, TAG_END, NULL );
	DoMethod( o, OM_NOTIFY, &notify, ops->ops_GInfo, 0 );

	if(sr) ScreenPosition(SD->sd_Screen,SPOS_ABSOLUTE,sleft,stop,NULL,NULL);

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS SCRN_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval=TRUE;
	struct SCRNData *SD = INST_DATA( cl, o );

	switch( opg->opg_AttrID )
	{
		case SCRN_Screen:				*opg->opg_Storage = (ULONG)SD->sd_Screen; break;
		case SCRN_SigBit:				*opg->opg_Storage = (ULONG)GetAttr(MESG_SigBit,SD->sd_Message,opg->opg_Storage); break;//(1L<<SD->sd_Signal); break;

		case SCRN_Title:				*opg->opg_Storage = (ULONG)SD->sd_Screen->Title; break;
		case SCRN_DefaultTitle:	*opg->opg_Storage = (ULONG)SD->sd_Screen->DefaultTitle; break;

		case SCRN_DrawInfo:			*opg->opg_Storage = (ULONG)SD->sd_DrawInfo; break;
		case SCRN_DetailPen:		*opg->opg_Storage = (ULONG)SD->sd_Screen->DetailPen; break;
		case SCRN_BlockPen:			*opg->opg_Storage = (ULONG)SD->sd_Screen->BlockPen; break;
		case SCRN_VisualInfo:		*opg->opg_Storage = (ULONG)SD->sd_VisualInfo; break;

		case SCRN_Left:					*opg->opg_Storage = (ULONG)SD->sd_Screen->LeftEdge; break;
		case SCRN_Top:					*opg->opg_Storage = (ULONG)SD->sd_Screen->TopEdge; break;
		case SCRN_Width:				*opg->opg_Storage = (ULONG)SD->sd_Screen->Width; break;
		case SCRN_Height:				*opg->opg_Storage = (ULONG)SD->sd_Screen->Height; break;
		default: retval = DoSuperMethodA(cl, o, (Msg)opg); break;
	}
	return( retval );
}

/*
** OM_UPDATE
*/

ULONG SAVEDS SCRN_UPDATE(Class *cl,Object *o,struct opUpdate *opu )
{
	ULONG retval=0;
//	struct SCRNData *SD = INST_DATA( cl, o );
//	struct TagItem *tag, notify;
//	struct RastPort *rp;

	retval = DoSuperMethodA(cl, o, (Msg) opu);

/*	if( opu->opu_AttrList )
	{
		if(tag = FindTagItem( SCRN_CurVal, opu->opu_AttrList ))
		{
			if( tag->ti_Data != SD->sd_CurVal)
			{
				SD->sd_CurVal = tag->ti_Data;

				rp = ObtainGIRPort( opu->opu_GInfo );
				if( rp )
				{
					DoMethod( o, GM_RENDER, opu->opu_GInfo, rp, GREDRAW_UPDATE );
					ReleaseGIRPort( rp );
				}

				SetTagArg( notify, TAG_END, NULL );
				DoMethod( o, OM_NOTIFY, &notify, opu->opu_GInfo, 0 );
			}
		}
	}
*/
	return( retval );
}

ULONG SAVEDS SCRN_NOTIFY(Class *cl,Object *o,struct opUpdate *opu )
{
	struct TagItem tags[3];
//	struct SCRNData *SD = INST_DATA( cl, o );

/*	SCRN_SetTagArg(tags[0], GA_ID, ((struct Gadget *)o)->GadgetID);
	SCRN_SetTagArg(tags[1], SCRN_CurVal, SD->sd_CurVal);

	if( opu->opu_AttrList == NULL )
	{
		SCRN_SetTagArg(tags[2], TAG_END, NULL);
	}
	else SCRN_SetTagArg(tags[2], TAG_MORE, opu->opu_AttrList );
*/
	return( DoSuperMethod(cl, o, OM_NOTIFY, tags, opu->opu_GInfo, opu->opu_Flags) );
}

ULONG SAVEDS SCRN_RENDER(Class *cl,Object *o,struct gpRender *gpr )
{
	ULONG retval;
//	struct RastPort *RP = gpr->gpr_RPort;
//	struct SCRNData *SD = INST_DATA( cl, o );

	retval = DoSuperMethodA(cl, o, (Msg) gpr);
	return( retval );
}

ULONG SAVEDS SCRN_GETMESSAGE(Class *cl,Object *o,Msg msg)
{
	struct SCRNData *SD = INST_DATA( cl, o );
	return DoMethodA(SD->sd_Message,(Msg)msg);
}

ULONG SAVEDS SCRN_HANDLEMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct SCRNData *SD = INST_DATA( cl, o );
	printf("bleeeee\n");
	return DoMethodA(SD->sd_Message,(Msg)msg);
}

ULONG SAVEDS SCRN_WAITMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct SCRNData *SD = INST_DATA( cl, o );
	return DoMethodA(SD->sd_Message,(Msg)msg);
}

ULONG SAVEDS SCRN_ADDMEMBER(Class *cl,Object *o,struct opMember *msg)
{
	struct SCRNData *SD = INST_DATA( cl, o );
	struct Node *node;

	if(node=(struct Node *)AllocVec(sizeof(struct Node),MEMF_ANY))
	{
		node->ln_Name=(char *)msg->opam_Object;
		AddTail((struct List *)&SD->sd_ListWindows,node);
	}
	return TRUE;
}

ULONG SAVEDS SCRN_REMMEMBER(Class *cl,Object *o,struct opMember *msg)
{
	struct SCRNData *SD = INST_DATA( cl, o );
	struct Node *node;

	if(SD->sd_ObjectOK)
	{
		for(node=(struct Node *)SD->sd_ListWindows.mlh_Head;node->ln_Succ;node=node->ln_Succ)
		{
			if(node->ln_Name==(char *)msg->opam_Object)
			{
				Remove(node);
				FreeVec(node);
				break;
			}
		}
	}
	return TRUE;
}
