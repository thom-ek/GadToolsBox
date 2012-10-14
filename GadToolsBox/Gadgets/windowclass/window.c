/*
** Window Class
** $VER: window.c 37.1 (15.4.98)
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
#define WINDOWCLASS "gtbwindowclass"
#define IM_WIDTH(o) ((struct Image *) o)->Width
#define IM_HEIGHT(o) ((struct Image *) o)->Height

/*
** Prototypes
*/

Class *CreateWindowClass(struct gtbApplication *);
void DisposeWindowClass(Class *);
ULONG ASMFUNC WIND_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG WIND_NEW(Class *,Object *,struct opSet *);
ULONG WIND_DISPOSE(Class *, Object *, Msg);
ULONG WIND_SET(Class *, Object *, struct opSet *);
ULONG WIND_GET(Class *,Object *,struct opGet *);
ULONG WIND_UPDATE(Class *,Object *,struct opUpdate *);
ULONG WIND_NOTIFY(Class *,Object *,struct opUpdate *);
ULONG WIND_RENDER(Class *,Object *,struct gpRender *);
ULONG WIND_GETMESSAGE(Class *,Object *,Msg);
ULONG WIND_HANDLEMESSAGE(Class *,Object *,struct gtbWaitMessage *);
ULONG WIND_WAITMESSAGE(Class *,Object *,struct gtbWaitMessage *);
ULONG WIND_ADDMEMBER(Class *,Object *,struct opMember *);
ULONG WIND_REMMEMBER(Class *,Object *,struct opMember *);

/*
** Variables
*/

#define WD_LEFT 0
#define WD_RIGHT 1
#define WD_CENTER 2
#define WD_NORMAL 0
#define WD_RELATIVE 1
#define WD_INNER 2

struct WINDData
{
	BOOL wd_ObjectOK;

	struct Window *wd_Window;
	struct Gadget *wd_DragBar;

	Object *wd_Scroll[6];
	UBYTE wd_VertScroll,wd_HorizScroll;
	ULONG wd_VertTop,wd_VertTotal,wd_VertVisible;
	ULONG wd_HorizTop,wd_HorizTotal,wd_HorizVisible;

	struct MinList wd_ListGadgets;
	Object *wd_LeftImage,*wd_RightImage,*wd_UpImage,*wd_DownImage,*wd_SizeImage;

	struct Requester wd_Req;
	BOOL wd_Locked;

	UBYTE wd_LeftEdgeType,wd_TopEdgeType;
	UBYTE wd_WidthType,wd_HeightType;
	UWORD wd_LeftEdge,wd_TopEdge;
	UWORD wd_Width,wd_Height;
	ULONG wd_IDCMP;
	ULONG wd_PointerDelay;

	struct Gadget *wd_Gadgets;
	struct Gadget *wd_FirstGadget;

	struct Menu *wd_MenuStrip;

	struct MsgPort *wd_AppMsgPort;
	APTR wd_AppWindow;
	ULONG wd_AppID;

	struct MsgPort *wd_MessagePort;
	ULONG (*wd_WindowHandler)(Object *,ULONG,APTR);

	struct Screen *wd_Screen,*wd_PubScreen,*wd_UsedScreen;
	Object *wd_ScreenObject;
	struct DrawInfo *wd_DrawInfo;
	UBYTE *wd_PubScreenName;
	UBYTE wd_ScrUse;

	struct IntuiMessage *wd_IntuiMessage;
};

/*
** Create Class
*/

Class *CreateWindowClass(struct gtbApplication *appl)
{
	Class *cl=NULL;

	if(cl = MakeClass (NULL, GADGETCLASS, NULL, sizeof(struct WINDData), 0L))
	{
		cl->cl_Dispatcher.h_Entry = (HookFunction)  WIND_Dispatcher;
		cl->cl_UserData = (ULONG) appl;
		return cl;
	}
	return(NULL);
}

void DisposeWindowClass(Class *cl)
{
	FreeClass(cl);
}

void StripIntuiMessages(struct MsgPort *mp,struct Window *win)
{
	struct IntuiMessage *msg;
	struct Node *succ;

	msg = (struct IntuiMessage *) mp->mp_MsgList.lh_Head;
	while( succ =  msg->ExecMessage.mn_Node.ln_Succ )
	{
		if( msg->IDCMPWindow ==  win )
		{
			Remove((struct Node *)msg);
			ReplyMsg((struct Message *)msg);
		}
		msg = (struct IntuiMessage *) succ;
	}
}

Object *NewImageObject(struct Screen *scr,struct DrawInfo *dri,ULONG which)
{
	return NewObject(NULL, SYSICLASS,
		SYSIA_DrawInfo, dri,
		SYSIA_Which, which,
		SYSIA_Size, (scr->Flags & SCREENHIRES ? SYSISIZE_MEDRES : SYSISIZE_LOWRES),
		TAG_DONE);
}

#define COPYTAG(w,o) ((ti=FindTagItem(w,ops->ops_AttrList))?o:TAG_IGNORE)

/*
** Dispatcher
*/

ULONG ASMFUNC WIND_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = WIND_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = WIND_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = WIND_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = WIND_GET(cl, o, (struct opGet *)msg );
			break;
		case OM_UPDATE:
			retval = WIND_UPDATE(cl, o, (struct opUpdate *)msg );
			break;
		case OM_NOTIFY:
			retval = WIND_NOTIFY(cl, o, (struct opUpdate *)msg );
			break;
		case GM_RENDER:
			retval = WIND_RENDER(cl, o, (struct gpRender *)msg );
			break;
		case GTB_GETMESSAGE:
			retval = WIND_GETMESSAGE(cl,o,msg);
			break;
		case GTB_HANDLEMESSAGE:
			retval = WIND_HANDLEMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
			break;
		case GTB_WAITMESSAGE:
			retval = WIND_WAITMESSAGE(cl,o,(struct gtbWaitMessage *)msg);
			break;
		case OM_ADDMEMBER:
			retval = WIND_ADDMEMBER(cl,o,(struct opMember *)msg);
			break;
		case OM_REMMEMBER:
			retval = WIND_REMMEMBER(cl,o,(struct opMember *)msg);
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

ULONG SAVEDS WIND_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct WINDData *WD;
	struct TagItem *ti,*tag,*tstate;
	ULONG le,te,w,h,size;

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		WD = INST_DATA( cl, object );

		memset(WD,0,sizeof(struct WINDData));
		NewList((struct List *)&WD->wd_ListGadgets);
		WD->wd_Locked=FALSE;
		WD->wd_PointerDelay=FALSE;
		WD->wd_MenuStrip=NULL;

		WD->wd_WindowHandler=(ULONG (*)(Object *,ULONG,APTR))GetTagData( WIND_Handler, NULL, ops->ops_AttrList );
		if(WD->wd_WindowHandler) WD->wd_WindowHandler(object,TYPE_INIT,NULL);

		if(WD->wd_Screen = (struct Screen *)GetTagData( WIND_Screen, NULL, ops->ops_AttrList )) WD->wd_UsedScreen=WD->wd_Screen;
		if(WD->wd_PubScreen = (struct Screen *)GetTagData( WIND_PubScreen, NULL, ops->ops_AttrList )) WD->wd_UsedScreen=WD->wd_PubScreen;
		if(WD->wd_ScreenObject = (Object *)GetTagData( WIND_ScreenObject, NULL, ops->ops_AttrList )) GetAttr(SCRN_Screen,WD->wd_ScreenObject,(ULONG *)&WD->wd_UsedScreen);
		WD->wd_PubScreenName = (UBYTE *)GetTagData( WIND_PubScreenName, NULL, ops->ops_AttrList );
		if(!WD->wd_UsedScreen) { WD->wd_UsedScreen=LockPubScreen(WD->wd_PubScreenName); WD->wd_ScrUse=TRUE; }
		if(!(WD->wd_DrawInfo=GetScreenDrawInfo(WD->wd_UsedScreen)))
		{
			CoerceMethod(cl,object,OM_DISPOSE);
			return FALSE;
		}

		if(ti=FindTagItem(WIND_Width,ops->ops_AttrList)) { WD->wd_WidthType=WD_NORMAL; WD->wd_Width=ti->ti_Data; w=ti->ti_Data; } else
		if(ti=FindTagItem(WIND_InnerWidth,ops->ops_AttrList)) { WD->wd_WidthType=WD_INNER; WD->wd_Width=ti->ti_Data; w=ti->ti_Data; } else
		if(ti=FindTagItem(WIND_RelWidth,ops->ops_AttrList)) { WD->wd_WidthType=WD_RELATIVE; WD->wd_Width=ti->ti_Data; w=WD->wd_UsedScreen->Width-ti->ti_Data; } else
		{ WD->wd_WidthType=WD_NORMAL; WD->wd_Width=WD->wd_UsedScreen->Width; w=100; }

		if(ti=FindTagItem(WIND_Height,ops->ops_AttrList)) { WD->wd_HeightType=WD_NORMAL; WD->wd_Height=ti->ti_Data; h=ti->ti_Data; } else
		if(ti=FindTagItem(WIND_InnerHeight,ops->ops_AttrList)) { WD->wd_HeightType=WD_INNER; WD->wd_Height=ti->ti_Data; h=ti->ti_Data; } else
		if(ti=FindTagItem(WIND_RelHeight,ops->ops_AttrList)) { WD->wd_HeightType=WD_RELATIVE; WD->wd_Height=ti->ti_Data; h=WD->wd_UsedScreen->Height-ti->ti_Data; } else
		{ WD->wd_HeightType=WD_NORMAL; WD->wd_Height=WD->wd_UsedScreen->Height; h=100; }		

		if(ti=FindTagItem(WIND_Left,ops->ops_AttrList)) { WD->wd_LeftEdgeType=WD_LEFT; WD->wd_LeftEdge=ti->ti_Data; le=ti->ti_Data; } else
		if(ti=FindTagItem(WIND_RelRight,ops->ops_AttrList)) { WD->wd_LeftEdgeType=WD_RIGHT; WD->wd_LeftEdge=ti->ti_Data; le=WD->wd_UsedScreen->Width-w-ti->ti_Data; } else
		if(ti=FindTagItem(WIND_CenterX,ops->ops_AttrList)) { WD->wd_LeftEdgeType=WD_CENTER; WD->wd_LeftEdge=ti->ti_Data; le=WD->wd_UsedScreen->Width/2-w/2; } else
		{ WD->wd_LeftEdgeType=WD_LEFT; WD->wd_LeftEdge=0; le=0; }

		if(ti=FindTagItem(WIND_Top,ops->ops_AttrList)) { WD->wd_TopEdgeType=WD_LEFT; WD->wd_TopEdge=ti->ti_Data; te=ti->ti_Data; } else
		if(ti=FindTagItem(WIND_RelBottom,ops->ops_AttrList)) { WD->wd_TopEdgeType=WD_RIGHT; WD->wd_TopEdge=ti->ti_Data; te=WD->wd_UsedScreen->Height-h-ti->ti_Data; } else
		if(ti=FindTagItem(WIND_CenterY,ops->ops_AttrList)) { WD->wd_TopEdgeType=WD_CENTER; WD->wd_TopEdge=ti->ti_Data; te=WD->wd_UsedScreen->Height/2-h/2; } else
		{ WD->wd_TopEdgeType=WD_LEFT; WD->wd_TopEdge=0; te=0; }

		size=GetTagData(WIND_SizeGadget,FALSE,ops->ops_AttrList);

		if(ti=FindTagItem(WIND_ToolDragBar,ops->ops_AttrList))
		{
			if(!(WD->wd_DragBar=NewObject(NULL,GADGETCLASS,
				GA_Left,0,
				GA_Top,0,
				GA_Width,(ti->ti_Data==TOOL_HORIZ)?w:8,
				GA_Height,(ti->ti_Data==TOOL_VERT)?h:8,
				GA_SysGadget,TRUE,
				GA_SysGType,GTYP_WDRAGGING,
				GA_Highlight,GFLG_GADGHNONE,
				GetTagData(WIND_GimmeZeroZero,FALSE,ops->ops_AttrList)?GA_GZZGadget:TAG_IGNORE,TRUE,
				(ti->ti_Data==TOOL_HORIZ)?GA_TopBorder:GA_LeftBorder,TRUE,
				TAG_DONE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
			WD->wd_FirstGadget=WD->wd_DragBar;
		}

		if(!(WD->wd_SizeImage=NewImageObject(WD->wd_UsedScreen,WD->wd_DrawInfo,SIZEIMAGE)))
		{
			CoerceMethod(cl,object,OM_DISPOSE);
			return FALSE;
		}

		if(WD->wd_VertScroll=GetTagData( WIND_VertScroll, NULL, ops->ops_AttrList ))
		{
			if(!(WD->wd_UpImage=NewImageObject(WD->wd_UsedScreen,WD->wd_DrawInfo,UPIMAGE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
			if(!(WD->wd_DownImage=NewImageObject(WD->wd_UsedScreen,WD->wd_DrawInfo,DOWNIMAGE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}

			if(!(WD->wd_Scroll[0]=NewObject(NULL, BUTTONGCLASS,
				ICA_TARGET, ICTARGET_IDCMP,
				GA_Image, WD->wd_DownImage,
				GA_RelRight, 1-(IM_WIDTH(WD->wd_DownImage)),
				GA_RelBottom, 1-((size?IM_HEIGHT(WD->wd_SizeImage):0)+IM_HEIGHT(WD->wd_DownImage)),
				GA_RightBorder,TRUE,
				(WD->wd_DragBar?GA_Previous:TAG_IGNORE),WD->wd_DragBar,
				GA_ID,(UWORD)-1,
				GetTagData(WIND_GimmeZeroZero,FALSE,ops->ops_AttrList)?GA_GZZGadget:TAG_IGNORE,TRUE,
				TAG_DONE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
//			DoMethod(o,OM_ADDMEMBER,(LONG)WD->wd_Scroll[0]);
			if(!(WD->wd_Scroll[1]=NewObject(NULL, BUTTONGCLASS,
				ICA_TARGET, ICTARGET_IDCMP,
				GA_Image, WD->wd_UpImage,
				GA_RelRight, 1-(IM_WIDTH(WD->wd_UpImage)),
				GA_RelBottom, 1-((size?IM_HEIGHT(WD->wd_SizeImage):0)+IM_HEIGHT(WD->wd_DownImage)+IM_HEIGHT(WD->wd_UpImage)),
				GA_RightBorder,TRUE,
				GA_Previous,WD->wd_Scroll[0],
				GA_ID,(UWORD)-2,
				GetTagData(WIND_GimmeZeroZero,FALSE,ops->ops_AttrList)?GA_GZZGadget:TAG_IGNORE,TRUE,
				TAG_DONE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
//			DoMethod(o,OM_ADDMEMBER,(LONG)WD->wd_Scroll[1]);
			if(!(WD->wd_Scroll[2]=NewObject(NULL, PROPGCLASS,
				ICA_TARGET, ICTARGET_IDCMP,
				PGA_Freedom, FREEVERT,
				PGA_NewLook, TRUE,
				PGA_Borderless, ((WD->wd_DrawInfo->dri_Flags & DRIF_NEWLOOK) && WD->wd_DrawInfo->dri_Depth != 1),
				GA_RelRight, -(IM_WIDTH(WD->wd_SizeImage)-5),
				GA_Top, WD->wd_UsedScreen->WBorTop+WD->wd_UsedScreen->Font->ta_YSize+1+1,
				GA_Width, IM_WIDTH(WD->wd_SizeImage)-2-2-4,
				GA_RelHeight, -((size?IM_HEIGHT(WD->wd_SizeImage):0)+IM_HEIGHT(WD->wd_UpImage)+IM_HEIGHT(WD->wd_DownImage)+WD->wd_UsedScreen->WBorTop+WD->wd_UsedScreen->Font->ta_YSize+1+1+1),
				GA_RightBorder, TRUE,
				GA_Previous,WD->wd_Scroll[1],
				PGA_Top, WD->wd_VertTop=GetTagData( WIND_VertTop, 0, ops->ops_AttrList ),
				PGA_Total, WD->wd_VertTotal=GetTagData( WIND_VertTotal, 1, ops->ops_AttrList ),
				PGA_Visible, WD->wd_VertVisible=GetTagData( WIND_VertVisible, 1, ops->ops_AttrList ),
				GA_ID,(UWORD)-3,
				GetTagData(WIND_GimmeZeroZero,FALSE,ops->ops_AttrList)?GA_GZZGadget:TAG_IGNORE,TRUE,
				TAG_DONE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}

//			DoMethod(o,OM_ADDMEMBER,(LONG)WD->wd_Scroll[2]);
			if(!WD->wd_DragBar) WD->wd_FirstGadget=(struct Gadget *)WD->wd_Scroll[0];
		}
		if(WD->wd_HorizScroll=GetTagData( WIND_HorizScroll, NULL, ops->ops_AttrList ))
		{
			if(!(WD->wd_LeftImage=NewImageObject(WD->wd_UsedScreen,WD->wd_DrawInfo,LEFTIMAGE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
			if(!(WD->wd_RightImage=NewImageObject(WD->wd_UsedScreen,WD->wd_DrawInfo,RIGHTIMAGE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
			if(!(WD->wd_Scroll[3]=NewObject(NULL, BUTTONGCLASS,
				ICA_TARGET, ICTARGET_IDCMP,
				GA_Image, WD->wd_LeftImage,
				GA_RelRight, 1-(IM_WIDTH(WD->wd_LeftImage)+IM_WIDTH(WD->wd_SizeImage)+IM_WIDTH(WD->wd_RightImage)),
				GA_RelBottom, 1-(IM_HEIGHT(WD->wd_LeftImage)),
				GA_BottomBorder,TRUE,
				(WD->wd_VertScroll?GA_Previous:(WD->wd_DragBar?GA_Previous:TAG_IGNORE)),WD->wd_Scroll[2],
				GA_ID,(UWORD)-4,
				GetTagData(WIND_GimmeZeroZero,FALSE,ops->ops_AttrList)?GA_GZZGadget:TAG_IGNORE,TRUE,
				TAG_DONE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
//			DoMethod(o,OM_ADDMEMBER,(LONG)WD->wd_Scroll[3]);
			if(!(WD->wd_Scroll[4]=NewObject(NULL, BUTTONGCLASS,
				ICA_TARGET, ICTARGET_IDCMP,
				GA_Image, WD->wd_RightImage,
				GA_RelRight, 1-(IM_WIDTH(WD->wd_RightImage)+IM_WIDTH(WD->wd_SizeImage)),
				GA_RelBottom, 1-(IM_HEIGHT(WD->wd_RightImage)),
				GA_BottomBorder,TRUE,
				GA_Previous,WD->wd_Scroll[3],
				GA_ID,(UWORD)-5,
				GetTagData(WIND_GimmeZeroZero,FALSE,ops->ops_AttrList)?GA_GZZGadget:TAG_IGNORE,TRUE,
				TAG_DONE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
//			DoMethod(o,OM_ADDMEMBER,(LONG)WD->wd_Scroll[4]);
			if(!(WD->wd_Scroll[5]=NewObject(NULL, PROPGCLASS,
				ICA_TARGET, ICTARGET_IDCMP,
				PGA_Freedom, FREEHORIZ,
				PGA_NewLook, TRUE,
				PGA_Borderless, ((WD->wd_DrawInfo->dri_Flags & DRIF_NEWLOOK) && WD->wd_DrawInfo->dri_Depth != 1),
				GA_Left, WD->wd_UsedScreen->WBorLeft,
				GA_RelBottom, -(IM_HEIGHT(WD->wd_SizeImage)-3),
				GA_RelWidth, -(4+IM_WIDTH(WD->wd_LeftImage)+IM_WIDTH(WD->wd_RightImage)+IM_WIDTH(WD->wd_SizeImage)+1),
				GA_Height, IM_HEIGHT(WD->wd_SizeImage)-4,
				GA_BottomBorder, TRUE,
				GA_Previous,WD->wd_Scroll[4],
				PGA_Top, WD->wd_HorizTop=GetTagData( WIND_HorizTop, 0, ops->ops_AttrList ),
				PGA_Total, WD->wd_HorizTotal=GetTagData( WIND_HorizTotal, 1, ops->ops_AttrList ),
				PGA_Visible, WD->wd_HorizVisible=GetTagData( WIND_HorizVisible, 1, ops->ops_AttrList ),
				GA_ID,(UWORD)-6,
				GetTagData(WIND_GimmeZeroZero,FALSE,ops->ops_AttrList)?GA_GZZGadget:TAG_IGNORE,TRUE,
				TAG_DONE)))
			{
				CoerceMethod(cl,object,OM_DISPOSE);
				return FALSE;
			}
//			DoMethod(o,OM_ADDMEMBER,(LONG)WD->wd_Scroll[5]);
			if(!WD->wd_DragBar && !WD->wd_VertScroll) WD->wd_FirstGadget=(struct Gadget *)WD->wd_Scroll[3];
		}

		WD->wd_IDCMP=GetTagData( WIND_IDCMP, NULL, ops->ops_AttrList );
		WD->wd_MessagePort=(struct MsgPort *)GetTagData( WIND_MessagePort, NULL, ops->ops_AttrList );
		WD->wd_AppMsgPort=(struct MsgPort *)GetTagData( WIND_AppMessagePort, NULL, ops->ops_AttrList );
		WD->wd_AppID=GetTagData( WIND_AppID, NULL, ops->ops_AttrList );

		WD->wd_PointerDelay=GetTagData( WIND_PointerDelay, FALSE, ops->ops_AttrList );

		WD->wd_Gadgets=(struct Gadget *)GetTagData( WIND_Gadgets, NULL, ops->ops_AttrList );
		if(!WD->wd_VertScroll && !WD->wd_HorizScroll && !WD->wd_DragBar) WD->wd_FirstGadget=WD->wd_Gadgets;
		if(!WD->wd_VertScroll && !WD->wd_HorizScroll && WD->wd_DragBar) WD->wd_DragBar->NextGadget=WD->wd_Gadgets;
		if(WD->wd_VertScroll && !WD->wd_HorizScroll) ((struct Gadget *)WD->wd_Scroll[2])->NextGadget=WD->wd_Gadgets;
		if(WD->wd_HorizScroll) ((struct Gadget *)WD->wd_Scroll[5])->NextGadget=WD->wd_Gadgets;

		if(WD->wd_HorizScroll || WD->wd_VertScroll) WD->wd_IDCMP|=IDCMP_IDCMPUPDATE;
		
		if(!(WD->wd_Window=OpenWindowTags(NULL,
				WA_Left,le,
				WA_Top,te,
				(WD->wd_WidthType==WD_INNER?WA_InnerWidth:WA_Width),w,
				(WD->wd_HeightType==WD_INNER?WA_InnerHeight:WA_Height),h,
				((WD->wd_IDCMP && (!WD->wd_MessagePort)) ? WA_IDCMP:TAG_IGNORE), WD->wd_IDCMP,
				(WD->wd_FirstGadget?WA_Gadgets:TAG_IGNORE), WD->wd_FirstGadget,
				WD->wd_Screen?WA_CustomScreen:TAG_IGNORE,WD->wd_Screen,
				WD->wd_PubScreen?WA_PubScreen:TAG_IGNORE,WD->wd_PubScreen,
				WD->wd_PubScreenName?WA_PubScreenName:TAG_IGNORE,WD->wd_PubScreenName,
				WD->wd_ScreenObject?WA_PubScreen:TAG_IGNORE,WD->wd_UsedScreen,
				COPYTAG(WIND_SizeBRight,WA_SizeBRight),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_SizeBBottom,WA_SizeBBottom),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_Title,WA_Title),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_SuperBitMap,WA_SuperBitMap),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_MinWidth,WA_MinWidth),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_MinHeight,WA_MinHeight),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_MaxWidth,WA_MaxWidth),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_MaxHeight,WA_MaxHeight),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_SizeGadget,WA_SizeGadget),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_DragBar,WA_DragBar),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_DepthGadget,WA_DepthGadget),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_CloseGadget,WA_CloseGadget),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_Backdrop,WA_Backdrop),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_ReportMouse,WA_ReportMouse),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_NoCareRefresh,WA_NoCareRefresh),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_Borderless,WA_Borderless),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_Activate,WA_Activate),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_RMBTrap,WA_RMBTrap),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_SimpleRefresh,WA_SimpleRefresh),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_SmartRefresh,WA_SmartRefresh),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_SizeBRight,WA_SizeBRight),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_SizeBBottom,WA_SizeBBottom),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_GimmeZeroZero,WA_GimmeZeroZero),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_NewLookMenus,WA_NewLookMenus),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_ScreenTitle,WA_ScreenTitle),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_AutoAdjust,WA_AutoAdjust),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_PubScreenFallBack,WA_PubScreenFallBack),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_Zoom,WA_Zoom),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_MouseQueue,WA_MouseQueue),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_RptQueue,WA_RptQueue),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_BackFill,WA_BackFill),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_MenuHelp,WA_MenuHelp),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_NotifyDepth,WA_NotifyDepth),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_Checkmark,WA_Checkmark),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_AmigaKey,WA_AmigaKey),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_Pointer,WA_Pointer),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_BusyPointer,WA_BusyPointer),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_PointerDelay,WA_PointerDelay),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_HelpGroup,WA_HelpGroup),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_HelpGroupWindow,WA_HelpGroupWindow),(ti?ti->ti_Data:NULL),
				COPYTAG(WIND_TabletMessages,WA_TabletMessages),(ti?ti->ti_Data:NULL),
				TAG_DONE)))
		{
			CoerceMethod(cl,object,OM_DISPOSE);
			return FALSE;
		}

		WD->wd_Window->UserData = (BYTE *)GetTagData( WIND_UserData, NULL, ops->ops_AttrList );
		if(WD->wd_MessagePort)
		{
			WD->wd_Window->UserPort=WD->wd_MessagePort;
			ModifyIDCMP(WD->wd_Window,WD->wd_IDCMP);
		}
		if(WD->wd_AppMsgPort) WD->wd_AppWindow=AddAppWindow(WD->wd_AppID,NULL,WD->wd_Window,WD->wd_AppMsgPort,TAG_DONE);
		tstate=ops->ops_AttrList;
		while(tag = NextTagItem( &tstate ))
		{
			switch(tag->ti_Tag)
			{
				case WIND_MenuStrip:
					if(WD->wd_MenuStrip) ClearMenuStrip(WD->wd_Window);
					WD->wd_MenuStrip=(struct Menu *)tag->ti_Data;
					if(WD->wd_MenuStrip) SetMenuStrip(WD->wd_Window,WD->wd_MenuStrip);
					break;
				case WIND_LockWindow:
					if(tag->ti_Data && !WD->wd_Locked)
					{
						InitRequester(&WD->wd_Req);
						Request(&WD->wd_Req,WD->wd_Window);
						WD->wd_Locked=TRUE;
					}
					if(!tag->ti_Data && WD->wd_Locked)
					{
						EndRequest(&WD->wd_Req,WD->wd_Window);
						WD->wd_Locked=FALSE;
					}
					break;
			}
		}
		GT_RefreshWindow(WD->wd_Window,NULL);
		DoMethod(object,OM_ADDTAIL,&((struct gtbApplication *)cl->cl_UserData)->ga_ListWindows);
		if(WD->wd_ScreenObject) DoMethod(WD->wd_ScreenObject,OM_ADDMEMBER,object);
		WD->wd_ObjectOK=TRUE;
	}
	return( (ULONG)object );
}

/*
** OM_DISPOSE
*/

ULONG SAVEDS WIND_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct WINDData *WD = INST_DATA( cl, o );
	int a;
//	Object *ostate,*member;

	if(WD->wd_WindowHandler) WD->wd_WindowHandler(o,TYPE_DELETE,NULL);
	if(WD->wd_ObjectOK) DoMethod(o,OM_REMOVE);

	if(WD->wd_ScreenObject) DoMethod(WD->wd_ScreenObject,OM_REMMEMBER,o);

/*	ostate = (Object *) WD->wd_ListGadgets.mlh_Head;
	while (member = NextObject (&ostate))
	{
		DoMethod (member, OM_REMMEMBER);
		DoMethod (member, (ULONG)msg);
	}
*/

	if(WD->wd_Locked)			EndRequest(&WD->wd_Req,WD->wd_Window);
	if(WD->wd_AppWindow)	RemoveAppWindow(WD->wd_AppWindow);
	if(WD->wd_MenuStrip)	ClearMenuStrip(WD->wd_Window);
	if(WD->wd_MessagePort)
	{
		Forbid();
		StripIntuiMessages( WD->wd_MessagePort, WD->wd_Window );
		WD->wd_Window->UserPort = NULL;
		ModifyIDCMP( WD->wd_Window, 0L );
		Permit();
	}
	RemoveGList(WD->wd_Window,WD->wd_FirstGadget,-1);
	if(WD->wd_Window)			CloseWindow(WD->wd_Window);

	if(WD->wd_DragBar)		DisposeObject(WD->wd_DragBar);
	for(a=0;a<6;a++)			if(WD->wd_Scroll[a]) DisposeObject(WD->wd_Scroll[a]);
	if(WD->wd_SizeImage)	DisposeObject(WD->wd_SizeImage);
	if(WD->wd_DownImage)	DisposeObject(WD->wd_DownImage);
	if(WD->wd_UpImage)		DisposeObject(WD->wd_UpImage);
	if(WD->wd_RightImage)	DisposeObject(WD->wd_RightImage);
	if(WD->wd_LeftImage)	DisposeObject(WD->wd_LeftImage);

	if(WD->wd_UsedScreen && WD->wd_ScrUse) UnlockPubScreen(NULL,WD->wd_UsedScreen);
	if(WD->wd_DrawInfo)		FreeScreenDrawInfo(WD->wd_UsedScreen,WD->wd_DrawInfo);

	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS WIND_SET(Class *cl, Object *o, struct opSet *ops)
{
	ULONG hr=FALSE,vr=FALSE,wr=FALSE,wl=FALSE,wleft,wtop,wwidth,wheight,minw,minh,maxw,maxh;
	struct WINDData *WD = INST_DATA( cl, o );
	struct TagItem *tag, *tstate, notify;
//	struct RastPort *rp;
	ULONG retval;

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	wleft=WD->wd_Window->LeftEdge;
	wtop=WD->wd_Window->TopEdge;
	wwidth=WD->wd_Window->Width;
	wheight=WD->wd_Window->Height;
	minw=WD->wd_Window->MinWidth;
	minh=WD->wd_Window->MinHeight;
	maxw=WD->wd_Window->MaxWidth;
	maxh=WD->wd_Window->MaxHeight;

	tstate=ops->ops_AttrList;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case WIND_HorizTop: WD->wd_HorizTop=tag->ti_Data; hr=TRUE; break;
			case WIND_HorizTotal: WD->wd_HorizTotal=tag->ti_Data; hr=TRUE; break;
			case WIND_HorizVisible: WD->wd_HorizVisible=tag->ti_Data; hr=TRUE; break;
			case WIND_VertTop: WD->wd_VertTop=tag->ti_Data; vr=TRUE; break;
			case WIND_VertTotal: WD->wd_VertTotal=tag->ti_Data; vr=TRUE; break;
			case WIND_VertVisible: WD->wd_VertVisible=tag->ti_Data; vr=TRUE; break;

			case WIND_Left: wleft=tag->ti_Data; wr=TRUE; break;
			case WIND_RelRight: wleft=WD->wd_UsedScreen->Width-WD->wd_Window->Width-tag->ti_Data; wr=TRUE; break;
			case WIND_CenterX: wleft=WD->wd_UsedScreen->Width/2-WD->wd_Window->Width/2; wr=TRUE; break;
			case WIND_Top: wtop=tag->ti_Data; wr=TRUE; break;
			case WIND_RelBottom: wtop=WD->wd_UsedScreen->Height-WD->wd_Window->Height-tag->ti_Data; wr=TRUE; break;
			case WIND_CenterY: wtop=WD->wd_UsedScreen->Height/2-WD->wd_Window->Height/2; wr=TRUE; break;
			case WIND_Width: wwidth=tag->ti_Data; wr=TRUE; break;
			case WIND_InnerWidth: wwidth=tag->ti_Data+WD->wd_Window->BorderLeft+WD->wd_Window->BorderRight; wr=TRUE; break;
			case WIND_RelWidth: wwidth=WD->wd_UsedScreen->Width-tag->ti_Data; wr=TRUE; break;
			case WIND_Height: wheight=tag->ti_Data; wr=TRUE; break;
			case WIND_InnerHeight: wheight=tag->ti_Data+WD->wd_Window->BorderTop+WD->wd_Window->BorderBottom; wr=TRUE; break;
			case WIND_RelHeight: wheight=WD->wd_UsedScreen->Height-tag->ti_Data; wr=TRUE; break;

			case WIND_MouseQueue: SetMouseQueue(WD->wd_Window,tag->ti_Data); break;
			case WIND_UserData: WD->wd_Window->UserData=(BYTE *)tag->ti_Data; break;
			case WIND_IDCMP: ModifyIDCMP(WD->wd_Window,tag->ti_Data); break;
			case WIND_MenuStrip:
				if(WD->wd_MenuStrip) ClearMenuStrip(WD->wd_Window);
				WD->wd_MenuStrip=(struct Menu *)tag->ti_Data;
				if(WD->wd_MenuStrip) SetMenuStrip(WD->wd_Window,WD->wd_MenuStrip);
				break;
			case WIND_Handler: WD->wd_WindowHandler=(ULONG (*)(Object *,ULONG,APTR))tag->ti_Data;

			case WIND_Title: SetWindowTitles(WD->wd_Window,(UBYTE *)tag->ti_Data,(UBYTE *)-1); break;
			case WIND_ScreenTitle: SetWindowTitles(WD->wd_Window,(UBYTE *)-1,(UBYTE *)tag->ti_Data); break;

			case WIND_Pointer: SetWindowPointer(WD->wd_Window,WA_Pointer,tag->ti_Data,WA_PointerDelay,WD->wd_PointerDelay,TAG_DONE); break;
			case WIND_BusyPointer: SetWindowPointer(WD->wd_Window,WA_BusyPointer,tag->ti_Data,WA_PointerDelay,WD->wd_PointerDelay,TAG_DONE); break;
			case WIND_PointerDelay: WD->wd_PointerDelay=tag->ti_Data; break;
			case WIND_LockWindow:
				if(tag->ti_Data && !WD->wd_Locked)
				{
					InitRequester(&WD->wd_Req);
					Request(&WD->wd_Req,WD->wd_Window);
					WD->wd_Locked=TRUE;
				}
				if(!tag->ti_Data && WD->wd_Locked)
				{
					EndRequest(&WD->wd_Req,WD->wd_Window);
					WD->wd_Locked=FALSE;
				}
				break;
			case WIND_MinWidth: minw=tag->ti_Data; wl=TRUE; break;
			case WIND_MinHeight: minh=tag->ti_Data; wl=TRUE; break;
			case WIND_MaxWidth: maxw=tag->ti_Data; wl=TRUE; break;
			case WIND_MaxHeight: maxh=tag->ti_Data; wl=TRUE; break;
		}
	}

	SetTagArg( notify, TAG_END, NULL );
	DoMethod( o, OM_NOTIFY, &notify, ops->ops_GInfo, 0 );

	if(hr) SetGadgetAttrs((struct Gadget *)WD->wd_Scroll[5],WD->wd_Window,NULL,PGA_Top,WD->wd_HorizTop,PGA_Total,WD->wd_HorizTotal,PGA_Visible,WD->wd_HorizVisible,TAG_DONE);
	if(vr) SetGadgetAttrs((struct Gadget *)WD->wd_Scroll[2],WD->wd_Window,NULL,PGA_Top,WD->wd_VertTop,PGA_Total,WD->wd_VertTotal,PGA_Visible,WD->wd_VertVisible,TAG_DONE);
	if(wr) ChangeWindowBox(WD->wd_Window,wleft,wtop,wwidth,wheight);
	if(wl) WindowLimits(WD->wd_Window,minw,minh,maxw,maxh);

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS WIND_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval=TRUE;
	struct WINDData *WD = INST_DATA( cl, o );

	switch( opg->opg_AttrID )
	{
		case WIND_Window:				*opg->opg_Storage = (ULONG)WD->wd_Window; break;
		case WIND_MessagePort:	*opg->opg_Storage = (ULONG)WD->wd_Window->UserPort; break;
		case WIND_SigBit:				*opg->opg_Storage = (ULONG)(1L<<WD->wd_Window->UserPort->mp_SigBit); break;
		case WIND_RastPort:			*opg->opg_Storage = (ULONG)WD->wd_Window->RPort; break;
		case WIND_Gadgets:			*opg->opg_Storage = (ULONG)WD->wd_Window->FirstGadget; break;
		case WIND_MenuStrip:		*opg->opg_Storage = (ULONG)WD->wd_MenuStrip; break;
		case WIND_IDCMP:				*opg->opg_Storage = (ULONG)WD->wd_Window->IDCMPFlags; break;
		case WIND_UserData:			*opg->opg_Storage = (ULONG)WD->wd_Window->UserData; break;

		case WIND_Title:				*opg->opg_Storage = (ULONG)WD->wd_Window->Title; break;
		case WIND_ScreenTitle:	*opg->opg_Storage = (ULONG)WD->wd_Window->ScreenTitle; break;

		case WIND_VertTop:			*opg->opg_Storage = (ULONG)WD->wd_VertTop; break;
		case WIND_VertTotal:		*opg->opg_Storage = (ULONG)WD->wd_VertTotal; break;
		case WIND_VertVisible:	*opg->opg_Storage = (ULONG)WD->wd_VertVisible; break;
		case WIND_HorizTop:			*opg->opg_Storage = (ULONG)WD->wd_HorizTop; break;
		case WIND_HorizTotal:		*opg->opg_Storage = (ULONG)WD->wd_HorizTotal; break;
		case WIND_HorizVisible:	*opg->opg_Storage = (ULONG)WD->wd_HorizVisible; break;

		case WIND_Width:				*opg->opg_Storage = (ULONG)WD->wd_Window->Width; break;
		case WIND_InnerWidth:		*opg->opg_Storage = (ULONG)WD->wd_Window->Width-WD->wd_Window->BorderLeft-WD->wd_Window->BorderRight; break;
		case WIND_RelWidth:			*opg->opg_Storage = (ULONG)WD->wd_UsedScreen->Width-WD->wd_Window->Width; break;
		case WIND_Height:				*opg->opg_Storage = (ULONG)WD->wd_Window->Height; break;
		case WIND_InnerHeight:	*opg->opg_Storage = (ULONG)WD->wd_Window->Height-WD->wd_Window->BorderTop-WD->wd_Window->BorderBottom; break;
		case WIND_RelHeight:		*opg->opg_Storage = (ULONG)WD->wd_UsedScreen->Height-WD->wd_Window->Height; break;

		case WIND_MinWidth:			*opg->opg_Storage = (ULONG)WD->wd_Window->MinWidth; break;
		case WIND_MinHeight:		*opg->opg_Storage = (ULONG)WD->wd_Window->MinHeight; break;
		case WIND_MaxWidth:			*opg->opg_Storage = (ULONG)WD->wd_Window->MaxWidth; break;
		case WIND_MaxHeight:		*opg->opg_Storage = (ULONG)WD->wd_Window->MaxHeight; break;

		case WIND_BorderLeft:		*opg->opg_Storage = (ULONG)WD->wd_Window->BorderLeft; break;
		case WIND_BorderTop:		*opg->opg_Storage = (ULONG)WD->wd_Window->BorderTop; break;
		case WIND_BorderRight:	*opg->opg_Storage = (ULONG)WD->wd_Window->BorderRight; break;
		case WIND_BorderBottom:	*opg->opg_Storage = (ULONG)WD->wd_Window->BorderBottom; break;

		default: retval = DoSuperMethodA(cl, o, (Msg)opg); break;
	}
	return( retval );
}

/*
** OM_UPDATE
*/

ULONG SAVEDS WIND_UPDATE(Class *cl,Object *o,struct opUpdate *opu )
{
	ULONG retval=0;
//	struct WINDData *WD = INST_DATA( cl, o );
//	struct TagItem *tag, notify;
//	struct RastPort *rp;

	retval = DoSuperMethodA(cl, o, (Msg) opu);

/*	if( opu->opu_AttrList )
	{
		if(tag = FindTagItem( WIND_CurVal, opu->opu_AttrList ))
		{
			if( tag->ti_Data != WD->wd_CurVal)
			{
				WD->wd_CurVal = tag->ti_Data;

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

ULONG SAVEDS WIND_NOTIFY(Class *cl,Object *o,struct opUpdate *opu )
{
	struct TagItem tags[3];
//	struct WINDData *WD = INST_DATA( cl, o );

/*	WIND_SetTagArg(tags[0], GA_ID, ((struct Gadget *)o)->GadgetID);
	WIND_SetTagArg(tags[1], WIND_CurVal, WD->wd_CurVal);

	if( opu->opu_AttrList == NULL )
	{
		WIND_SetTagArg(tags[2], TAG_END, NULL);
	}
	else WIND_SetTagArg(tags[2], TAG_MORE, opu->opu_AttrList );
*/
	return( DoSuperMethod(cl, o, OM_NOTIFY, tags, opu->opu_GInfo, opu->opu_Flags) );
}

ULONG SAVEDS WIND_RENDER(Class *cl,Object *o,struct gpRender *gpr )
{
	ULONG retval;
//	struct RastPort *RP = gpr->gpr_RPort;
	struct WINDData *WD = INST_DATA( cl, o );

	retval = DoSuperMethodA(cl, o, (Msg) gpr);

	if(WD->wd_FirstGadget)
	{
		RefreshGList(WD->wd_FirstGadget,WD->wd_Window,NULL,-1);
		GT_RefreshWindow(WD->wd_Window,NULL);
	}
	return( retval );
}

ULONG SAVEDS WIND_GETMESSAGE(Class *cl,Object *o,Msg msg)
{
	struct IntuiMessage *m;
	struct WINDData *WD = INST_DATA( cl, o );

	if(m=GT_GetIMsg(WD->wd_Window->UserPort))
	{
		CopyMem((APTR)m,(APTR)&WD->wd_IntuiMessage,sizeof(struct IntuiMessage));
		GT_ReplyIMsg(m);
		return( (ULONG) &WD->wd_IntuiMessage );
	}
	else return NULL;
}

ULONG SAVEDS WIND_HANDLEMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct IntuiMessage *im;
	struct AppMessage *am;
	struct WINDData *WD = INST_DATA( cl, o );
	ULONG retval=1;
	ULONG (*wd_GadgetHandler)(Object *,ULONG,APTR);

	if((msg->Signals & (1L<<WD->wd_Window->UserPort->mp_SigBit)) && WD->wd_WindowHandler)
	{
		while(im=GT_GetIMsg(WD->wd_Window->UserPort))
		{
			if((im->Class==IDCMP_MENUVERIFY) || (im->Class==IDCMP_REQVERIFY) || (im->Class==IDCMP_SIZEVERIFY))
			{
				retval=WD->wd_WindowHandler(o,TYPE_INTUIMESSAGE,im);
				GT_ReplyIMsg(im);
			}
			else
			{
				GT_ReplyIMsg(im);
				switch(im->Class)
				{
					case IDCMP_IDCMPUPDATE:
						switch(GetTagData(GA_ID,0,im->IAddress))
						{
							case (UWORD)-1:
								if(WD->wd_VertTop<WD->wd_VertTotal-WD->wd_VertVisible)
								{
									WD->wd_VertTop++;
									SetGadgetAttrs((struct Gadget *)WD->wd_Scroll[2],WD->wd_Window,NULL,PGA_Top,WD->wd_VertTop,TAG_DONE);
									retval=WD->wd_WindowHandler(o,TYPE_VERTSCROLL,im);
								}
								goto end;
							case (UWORD)-2:
								if(WD->wd_VertTop>0)
								{
									WD->wd_VertTop--;
									SetGadgetAttrs((struct Gadget *)WD->wd_Scroll[2],WD->wd_Window,NULL,PGA_Top,WD->wd_VertTop,TAG_DONE);
									retval=WD->wd_WindowHandler(o,TYPE_VERTSCROLL,im);
								}
								goto end;
							case (UWORD)-3:
								GetAttr(PGA_Top,WD->wd_Scroll[2],&WD->wd_VertTop);
								retval=WD->wd_WindowHandler(o,TYPE_VERTSCROLL,im);
								goto end;
							case (UWORD)-4:
								if(WD->wd_HorizTop>0)
								{
									WD->wd_HorizTop--;
									SetGadgetAttrs((struct Gadget *)WD->wd_Scroll[5],WD->wd_Window,NULL,PGA_Top,WD->wd_HorizTop,TAG_DONE);
									retval=WD->wd_WindowHandler(o,TYPE_HORIZSCROLL,im);
								}
								goto end;
							case (UWORD)-5:
								if(WD->wd_HorizTop<WD->wd_HorizTotal-WD->wd_HorizVisible)
								{
									WD->wd_HorizTop++;
									SetGadgetAttrs((struct Gadget *)WD->wd_Scroll[5],WD->wd_Window,NULL,PGA_Top,WD->wd_HorizTop,TAG_DONE);
									retval=WD->wd_WindowHandler(o,TYPE_HORIZSCROLL,im);
								}
								goto end;
							case (UWORD)-6:
								GetAttr(PGA_Top,WD->wd_Scroll[5],&WD->wd_HorizTop);
								retval=WD->wd_WindowHandler(o,TYPE_HORIZSCROLL,im);
								goto end;
/*
							default:		// im->IAddress is taglist not gadget!!!
								gtbGetGadgetAttr(GT_Handler,(Object *)im->IAddress,(ULONG *)&wd_GadgetHandler);
								if(wd_GadgetHandler)
								{
									retval=wd_GadgetHandler((Object *)im->IAddress,((struct Gadget *)im->IAddress)->GadgetID,im);
									goto end;
								}
								break;
*/
						}
						break;
					case IDCMP_MOUSEMOVE:
					case IDCMP_GADGETDOWN:
					case IDCMP_GADGETUP:
						if(im->IAddress)
						{
							gtbGetGadgetAttr(GT_Handler,(Object *)im->IAddress,(ULONG *)&wd_GadgetHandler);
							if(wd_GadgetHandler)
							{
								retval=wd_GadgetHandler((Object *)im->IAddress,((struct Gadget *)im->IAddress)->GadgetID,im);
								goto end;
							}
						}
						break;
				}
				retval=WD->wd_WindowHandler(o,TYPE_INTUIMESSAGE,im);
end:;
			}
		}
	}
	if(WD->wd_AppMsgPort && WD->wd_WindowHandler)
	{
		if(msg->Signals & (1L<<WD->wd_AppMsgPort->mp_SigBit))
		{
			while(am=(struct AppMessage *)GetMsg(WD->wd_AppMsgPort))
			{
				retval=WD->wd_WindowHandler(o,TYPE_APPMESSAGE,am);
				ReplyMsg((struct Message *)am);
			}
		}
	}
	return retval;
}

ULONG SAVEDS WIND_WAITMESSAGE(Class *cl,Object *o,struct gtbWaitMessage *msg)
{
	struct WINDData *WD = INST_DATA( cl, o );

	return Wait((1L<<WD->wd_Window->UserPort->mp_SigBit)|(msg->Signals));
}

ULONG SAVEDS WIND_ADDMEMBER(Class *cl,Object *o,struct opMember *msg)
{
	struct WINDData *WD = INST_DATA( cl, o );

	AddGList(WD->wd_Window,(struct Gadget *)msg->opam_Object,-1,-1,NULL);
	return TRUE;
}

ULONG SAVEDS WIND_REMMEMBER(Class *cl,Object *o,struct opMember *msg)
{
	struct WINDData *WD = INST_DATA( cl, o );

	RemoveGadget(WD->wd_Window,(struct Gadget *)msg->opam_Object);
	return TRUE;
}
