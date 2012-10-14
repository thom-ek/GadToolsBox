/*
** ColorWheel PlugIn
*/

#include"includes.h"
#include"colorwheel.h"

/*
** OwnTag definitions
*/

/*
** Default Tags
*/

struct TagItem ColorWheelTags[]=
{
	GA_Disabled,FALSE,
	TAG_IGNORE,NULL, // WHEEL_Donation,NULL,
	WHEEL_MaxPens,1,
	GA_FollowMouse,FALSE,
	WHEEL_Abbrv,NULL,
	WHEEL_BevelBox,FALSE,
	WHEEL_Screen,NULL,
	TAG_DONE,0
};

UBYTE ColorWheelTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_TEXT,TAGTYPE_NUMBER,TAGTYPE_NONE,TAGTYPE_DONE,
};

struct TagItem ColorWheelOwnTags[]=
{
	TAG_DONE,0
};

UBYTE ColorWheelOwnTagsTypes[]=
{
	TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 7
#define GSIZE_NUMOWNTAGS 0

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGBO_General[];

struct Page PAGES_PROPERTIES_PG_CWHL_PAGES[]=
{
	&PAGES_PROPERTIES_PG_CWHL_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG COLORWHEEL_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_DEFAULT:
			retval=COLORWHEEL_DEFAULT_Gadget(cl,o,msg);
			break;
		case OM_INIT:
			retval=COLORWHEEL_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=COLORWHEEL_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_CWHL_Flags();
			PAGES_PROPERTIES_PG_CWHL_PAGES[0].FirstGadget=PGBO_GeneralGList;
			PAGES_PROPERTIES_PG_CWHL_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L0_GAD);
			PAGES_PROPERTIES_PG_CWHL_PAGES[1].FirstGadget=PG_CWHL_FlagsGList;
			PAGES_PROPERTIES_PG_CWHL_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L3_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_CWHL_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_CWHL_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_CWHL_Flags();
			break;
		case OM_NEW:
			retval=COLORWHEEL_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_DISPOSE:
			retval=COLORWHEEL_DISPOSE_Gadget(cl,o,msg);
			break;
		case OM_CREATE:
			retval=COLORWHEEL_CREATE_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_DELETE:
			retval=COLORWHEEL_DELETE_Gadget(cl,o,msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG COLORWHEEL_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);
	piDoSuperMethodA(cl,o,msg);

	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PG_CWHL_FlagsGadgets[GD_PG_CWHL_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_CWHL_FlagsGadgets[GD_PG_CWHL_FLAGS_FOLLOWMOUSE],GetTagData(GA_FollowMouse,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_CWHL_FlagsGadgets[GD_PG_CWHL_FLAGS_BEVELBOX],GetTagData(WHEEL_BevelBox,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_CWHL_FlagsGadgets[GD_PG_CWHL_FLAGS_MAXPENS],GetTagData(WHEEL_MaxPens,1,ng->ng_Tags));
	SetString(PropsWnd,PG_CWHL_FlagsGadgets[GD_PG_CWHL_FLAGS_ABBRV],GetTagData(WHEEL_Abbrv,0,ng->ng_Tags));
//	SetString(PropsWnd,PG_CWHL_FlagsGadgets[GD_PG_CWHL_FLAGS_GRADIENTSLIDER],GetTagData(OWNTAG_GradientSlider,0,ng->ng_OwnTags));
	return TRUE;
}

ULONG COLORWHEEL_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);

	piDoSuperMethodA(cl,o,msg);

//	strcpy((char *)FindTagItem(OWNTAG_GradientSlider,ng->ng_OwnTags)->ti_Data,GetString(PG_CWHL_FlagsGadgets[GD_PG_CWHL_FLAGS_GRADIENTSLIDER]));
	strcpy((char *)FindTagItem(WHEEL_Abbrv,ng->ng_Tags)->ti_Data,GetString(PG_CWHL_FlagsGadgets[GD_PG_CWHL_FLAGS_ABBRV]));
	FindTagItem(WHEEL_MaxPens,ng->ng_Tags)->ti_Data=GetInteger(PG_CWHL_FlagsGadgets[GD_PG_CWHL_FLAGS_MAXPENS]);
	return TRUE;
}

ULONG COLORWHEEL_DEFAULT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	
	ng->ng_Width=100;
	ng->ng_Height=100;
	return TRUE;
}

ULONG COLORWHEEL_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_BOOPSI,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)ColorWheelTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)ColorWheelOwnTagsTypes,
		GAOBJ_ID,COLORWHEEL_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_CWHL_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);

		CopyTagItems(ColorWheelTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(ColorWheelOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);
		FindTagItem(WHEEL_Abbrv,ng->ng_Tags)->ti_Data=(ULONG) MyAllocVec(7,MEMF_PUBLIC|MEMF_CLEAR);
		strcpy((char *)FindTagItem(WHEEL_Abbrv,ng->ng_Tags)->ti_Data,"GCBMRY");
		return (ULONG)obj;
	}
	return FALSE;
}

ULONG COLORWHEEL_DISPOSE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct TagItem *ti;

	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
	MyFreeVec((APTR)FindTagItem(WHEEL_Abbrv,ti)->ti_Data);
	return piDoSuperMethodA(cl,o,msg);
}

ULONG COLORWHEEL_CREATE_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
{
	struct TagItem *ti;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	WORD x,y,w,h;

	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
	piGetAttr(o,OBJ_Node,(ULONG *)&ng);

	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
	if(ng->ng_RelX) x=-(nw->nw_Window->Width-nw->nw_Window->BorderLeft-ng->ng_X-1);
	else x=piCX(o);
	if(ng->ng_RelY) y=-(nw->nw_Window->Height-nw->nw_Window->BorderTop-ng->ng_Y-1);
	else y=piCY(o);
	if(ng->ng_RelWidth) w=-(nw->nw_Window->BorderLeft+nw->nw_Window->BorderRight+(nw->nw_Window->Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight-ng->ng_Width)-1);
	else w=piCW(o)+1;
	if(ng->ng_RelHeight) h=-(nw->nw_Window->BorderTop+nw->nw_Window->BorderBottom+(nw->nw_Window->Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom-ng->ng_Height)-1);
	else h=piCH(o)+1;

	FindTagItem(WHEEL_Screen,ti)->ti_Data=(ULONG)Scr;
	return (ULONG) (ng->ng_Gadget=NewObject(NULL, "colorwheel.gadget",
									ng->ng_RelX?GA_RelRight:GA_Left,x,
									ng->ng_RelY?GA_RelBottom:GA_Top,y,
									ng->ng_RelWidth?GA_RelWidth:GA_Width,w,
									ng->ng_RelHeight?GA_RelHeight:GA_Height,h,
									(msg->Window) ? GA_Previous : TAG_IGNORE, msg->Window,
									GA_ID,0,
									TAG_MORE,ti));
}

ULONG COLORWHEEL_DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	if(ng->ng_Gadget && ng->ng_Window) RemoveGadget(((struct Node_Window *)OBJECT_DATA(ng->ng_Window))->nw_Window,ng->ng_Gadget);
	if(ng->ng_Gadget) DisposeObject(ng->ng_Gadget);
	ng->ng_Gadget=NULL;
	return piDoSuperMethodA(cl,o,msg);
}

int FN_PG_CWHL_FLAGS_ABBRV_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_CWHL_FLAGS_GSLIDER_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_CWHL_FLAGS_MAXPENS_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_CWHL_FLAGS_FOLLOWMOUSE_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_FollowMouse,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_CWHL_FLAGS_BEVELBOX_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(WHEEL_BevelBox,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_CWHL_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
