/*
** Progress PlugIn
*/

#include"includes.h"
#include"progress.h"

/*
** Defines
*/

/*
** OwnTag definitions
*/

/*
** Default Tags
*/

struct TagItem ProgressTags[]=
{
	GA_Disabled,FALSE,
	PROG_MaxVal,100,
	PROG_CurVal,0,
	PROG_ConvertToPercents,FALSE,	
	PROG_Format,NULL,
	PROG_Border,FALSE,
	PROG_Placement,PLACETEXT_IN,
	TAG_IGNORE,NULL, //PROG_TextFont,NULL,
	TAG_DONE,0
};

UBYTE ProgressTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_TEXT,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem ProgressOwnTags[]=
{
	TAG_DONE,0
};

UBYTE ProgressOwnTagsTypes[]=
{
	TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 7
#define GSIZE_NUMOWNTAGS 9

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGBO_General[];

struct Page PAGES_PROPERTIES_PG_PRGR_PAGES[]=
{
	&PAGES_PROPERTIES_PG_PRGR_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG PROGRESS_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=PROGRESS_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=PROGRESS_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_PRGR_Flags();
			PAGES_PROPERTIES_PG_PRGR_PAGES[0].FirstGadget=PGBO_GeneralGList;
			PAGES_PROPERTIES_PG_PRGR_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L0_GAD);
			PAGES_PROPERTIES_PG_PRGR_PAGES[1].FirstGadget=PG_PRGR_FlagsGList;
			PAGES_PROPERTIES_PG_PRGR_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L18_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_PRGR_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_PRGR_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_PRGR_Flags();
			break;
		case OM_NEW:
			retval=PROGRESS_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_CREATE:
			retval=PROGRESS_CREATE_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_DELETE:
			retval=PROGRESS_DELETE_Gadget(cl,o,msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG PROGRESS_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	SetCheckMark(PropsWnd,PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_PERCENTS],GetTagData(PROG_ConvertToPercents,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_BORDER],GetTagData(PROG_Border,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_MAX],GetTagData(PROG_MaxVal,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_CURRENT],GetTagData(PROG_CurVal,0,ng->ng_Tags));
	SetString(PropsWnd,PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_FORMAT],GetTagData(PROG_Format,0,ng->ng_Tags));
	switch(GetTagData(PROG_Placement,0,ng->ng_Tags))
	{
		case PLACETEXT_IN: SetCycle(PropsWnd,PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_PLACEMENT],0); break;
		case PLACETEXT_LEFT: SetCycle(PropsWnd,PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_PLACEMENT],1); break;
		case PLACETEXT_RIGHT: SetCycle(PropsWnd,PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_PLACEMENT],2); break;
		case PLACETEXT_ABOVE: SetCycle(PropsWnd,PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_PLACEMENT],3); break;
		case PLACETEXT_BELOW: SetCycle(PropsWnd,PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_PLACEMENT],4); break;
	}
	return TRUE;
}

ULONG PROGRESS_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	FindTagItem(PROG_MaxVal,ng->ng_Tags)->ti_Data=GetInteger(PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_MAX]);
	FindTagItem(PROG_CurVal,ng->ng_Tags)->ti_Data=GetInteger(PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_CURRENT]);
	if(FindTagItem(PROG_Format,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(PROG_Format,ng->ng_Tags)->ti_Data);
	FindTagItem(PROG_Format,ng->ng_Tags)->ti_Data=NULL;
	if(FindTagItem(PROG_Format,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(strlen(GetString(PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_FORMAT]))+1,MEMF_PUBLIC))
		strcpy((char *)((struct TagItem *)FindTagItem(PROG_Format,ng->ng_Tags)->ti_Data),GetString(PG_PRGR_FlagsGadgets[GD_PG_PRGR_FLAGS_FORMAT]));

	return TRUE;
}

ULONG PROGRESS_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_BOOPSI,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)ProgressTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)ProgressOwnTagsTypes,
		GAOBJ_ID,PROGRESS_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_PRGR_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);

		CopyTagItems(ProgressTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(ProgressOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);

		return (ULONG)obj;
	}
	return FALSE;
}

ULONG PROGRESS_CREATE_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
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

	return (ULONG) (ng->ng_Gadget=NewObject(ProgressClass, NULL,
									ng->ng_RelX?GA_RelRight:GA_Left,x,
									ng->ng_RelY?GA_RelBottom:GA_Top,y,
									ng->ng_RelWidth?GA_RelWidth:GA_Width,w,
									ng->ng_RelHeight?GA_RelHeight:GA_Height,h,
									(msg->Window) ? GA_Previous : TAG_IGNORE, msg->Window,
									GA_ID,0,
									TAG_MORE,ti));
}

ULONG PROGRESS_DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	if(ng->ng_Gadget && ng->ng_Window) RemoveGadget(((struct Node_Window *)OBJECT_DATA(ng->ng_Window))->nw_Window,ng->ng_Gadget);
	if(ng->ng_Gadget) DisposeObject(ng->ng_Gadget);
	ng->ng_Gadget=NULL;
	return piDoSuperMethodA(cl,o,msg);
}

int FN_PG_PRGR_FLAGS_FORMAT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PRGR_FLAGS_MAX_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PRGR_FLAGS_CURRENT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PRGR_FLAGS_PERCENTS_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(PROG_ConvertToPercents,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PRGR_FLAGS_BORDER_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(PROG_Border,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PRGR_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PRGR_FLAGS_PLACEMENT_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	switch(PropertiesMsg.Code)
	{
		case 0: FindTagItem(PROG_Placement,ti)->ti_Data=PLACETEXT_IN; break;
		case 1: FindTagItem(PROG_Placement,ti)->ti_Data=PLACETEXT_LEFT; break;
		case 2: FindTagItem(PROG_Placement,ti)->ti_Data=PLACETEXT_RIGHT; break;
		case 3: FindTagItem(PROG_Placement,ti)->ti_Data=PLACETEXT_ABOVE; break;
		case 4: FindTagItem(PROG_Placement,ti)->ti_Data=PLACETEXT_BELOW; break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
