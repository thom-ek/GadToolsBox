/*
** GadToolsBox v3 - Gadget Managment
*/

#include"includes.h"

/*
** Defines
*/

#define IM_WIDTH(o) (((struct Image *) o)->Width)
#define IM_HEIGHT(o) (((struct Image *) o)->Height)

struct PlugIn piGadgets[]=
{
	"Button",			BUTTON_ID,		BUTTON_Dispatcher_Gadget,			NULL,
	"CheckBox",		CHECKBOX_ID,	CHECKBOX_Dispatcher_Gadget,		NULL,
	"ColorWheel",	COLORWHEEL_ID,COLORWHEEL_Dispatcher_Gadget,	NULL,
	"Cycle",			CYCLE_ID,			CYCLE_Dispatcher_Gadget,			NULL,
	"Datatype",		DATATYPE_ID,	DATATYPE_Dispatcher_Gadget,		NULL,
	"GradientSlider",GRADIENTSLIDER_ID,GRADIENTSLIDER_Dispatcher_Gadget,NULL,
	"Integer",		INTEGER_ID,		INTEGER_Dispatcher_Gadget,		NULL,
	"Listview",		LISTVIEW_ID,	LISTVIEW_Dispatcher_Gadget,		NULL,
	"MX",					MX_ID,				MX_Dispatcher_Gadget,					NULL,
	"Number",			NUMBER_ID,		NUMBER_Dispatcher_Gadget,			NULL,
	"Page",				PAGE_ID,			PAGE_Dispatcher_Gadget,				NULL,
	"Palette",		PALETTE_ID,		PALETTE_Dispatcher_Gadget,		NULL,
	"PopUpMenu",	POPUPMENU_ID,	POPUPMENU_Dispatcher_Gadget,	NULL,
	"Progress",		PROGRESS_ID,	PROGRESS_Dispatcher_Gadget,		NULL,
	"Scroller",		SCROLLER_ID,	SCROLLER_Dispatcher_Gadget,		NULL,
	"Slider",			SLIDER_ID,		SLIDER_Dispatcher_Gadget,			NULL,
	"String",			STRING_ID,		STRING_Dispatcher_Gadget,			NULL,
	"TapeDeck",		TAPEDECK_ID,	TAPEDECK_Dispatcher_Gadget,		NULL,
	"Text",				TEXT_ID,			TEXT_Dispatcher_Gadget,				NULL,
	"TextField",	TEXTFIELD_ID,	TEXTFIELD_Dispatcher_Gadget,	NULL,
	"TreeView",		TREEVIEW_ID,	TREEVIEW_Dispatcher_Gadget,		NULL,
	NULL,					NULL,					NULL,													NULL
};

extern ULONG ActiveGadget;

ULONG GetGadgetPtr(ULONG id)
{
	ULONG a;

	for(a=0;a<PLUGIN_GADGETS;a++) if(piGadgets[a].piID==id) return a;
	return -1;
}

/*
** Functions
*/

ULONG Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval=0;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PGBO_General();
			OpenWindow_PGGT_General();
			OpenWindow_PG_Lists();
			break;
		case OM_CLOSE:
			CloseWindow_PG_Lists();
			CloseWindow_PGGT_General();
			CloseWindow_PGBO_General();
			break;
		case OM_DEFAULT:
			retval=DEFAULT_Gadget(cl,o,msg);
			break;
		case OM_NEW:
			retval=NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_SET:
			retval=SET_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_GET:
			retval=GET_Gadget(cl,o,(struct piGet *)msg);
			break;
		case OM_DISPOSE:
			retval=DISPOSE_Gadget(cl,o,msg);
			break;
		case OM_ADDTAIL:
			retval=ADDTAIL_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_REMOVE:
			retval=REMOVE_Gadget(cl,o,msg);
			break;
		case OM_RENDER:
			retval=RENDER_Gadget(cl,o,msg);
			break;
		case OM_ERASE:
			retval=ERASE_Gadget(cl,o,msg);
			break;
		case OM_CREATE:
			retval=CREATE_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_DELETE:
			retval=DELETE_Gadget(cl,o,msg);
			break;
		case OM_HITTEST:
			retval=HITTEST_Gadget(cl,o,(struct piHitTest *)msg);
			break;
		case OM_COMPUTE:
			retval=COMPUTE_Gadget(cl,o,(struct piCompute *)msg);
			break;
		case OM_READ:
			retval=READ_Gadget(cl,o,(struct piIO *)msg);
			break;
		case OM_WRITE:
			retval=WRITE_Gadget(cl,o,(struct piIO *)msg);
			break;
	}
	return retval;
}

ULONG DEFAULT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);

	ng->ng_Width=80;
	ng->ng_Height=18;
	return TRUE;
}

ULONG INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	if(ng->ng_Type==GTYPE_GADTOOLS)
	{
		piGetAttr(o,GAOBJ_NodeData,(ULONG *)&gtd);

		SetString(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_LABEL],ng->ng_Label);
		SetString(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_TITLE],gtd->g_Text);
		SetInteger(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_X],ng->ng_X);
		SetInteger(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_Y],ng->ng_Y);
		SetInteger(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_WIDTH],ng->ng_Width);
		SetInteger(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_HEIGHT],ng->ng_Height);
		SetCycle(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_REL_X],ng->ng_RelX);
		SetCycle(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_REL_Y],ng->ng_RelY);
		SetCycle(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_REL_W],ng->ng_RelWidth);
		SetCycle(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_REL_H],ng->ng_RelHeight);
		GetFlag(gtd->g_Flags,PLACETEXT_IN) SetCycle(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_PLACEMENT],0);
		GetFlag(gtd->g_Flags,PLACETEXT_LEFT) SetCycle(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_PLACEMENT],1);
		GetFlag(gtd->g_Flags,PLACETEXT_RIGHT) SetCycle(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_PLACEMENT],2);
		GetFlag(gtd->g_Flags,PLACETEXT_ABOVE) SetCycle(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_PLACEMENT],3);
		GetFlag(gtd->g_Flags,PLACETEXT_BELOW) SetCycle(PropsWnd,PGGT_GeneralGadgets[GD_PGGT_PLACEMENT],4);
	}
	else
	{
		SetString(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_LABEL],ng->ng_Label);
		SetInteger(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_X],ng->ng_X);
		SetInteger(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_Y],ng->ng_Y);
		SetInteger(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_WIDTH],ng->ng_Width);
		SetInteger(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_HEIGHT],ng->ng_Height);
		SetCycle(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_REL_X],ng->ng_RelX);
		SetCycle(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_REL_Y],ng->ng_RelY);
		SetCycle(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_REL_W],ng->ng_RelWidth);
		SetCycle(PropsWnd,PGBO_GeneralGadgets[GD_PGBO_REL_H],ng->ng_RelHeight);
	}
	return TRUE;
}

ULONG APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(o,GAOBJ_NodeData,(ULONG *)&gtd);

	if(ng->ng_Type==GTYPE_GADTOOLS)
	{
		piGetAttr(o,GAOBJ_NodeData,(ULONG *)&gtd);
		strcpy(gtd->g_Text,GetString(PGGT_GeneralGadgets[GD_PGGT_TITLE]));
		strcpy(ng->ng_Label,GetString(PGGT_GeneralGadgets[GD_PGGT_LABEL]));
		ng->ng_X=GetInteger(PGGT_GeneralGadgets[GD_PGGT_X]);
		ng->ng_Y=GetInteger(PGGT_GeneralGadgets[GD_PGGT_Y]);
		ng->ng_Width=GetInteger(PGGT_GeneralGadgets[GD_PGGT_WIDTH]);
		ng->ng_Height=GetInteger(PGGT_GeneralGadgets[GD_PGGT_HEIGHT]);
	}
	else
	{
		strcpy(ng->ng_Label,GetString(PGBO_GeneralGadgets[GD_PGBO_LABEL]));
		ng->ng_X=GetInteger(PGBO_GeneralGadgets[GD_PGBO_X]);
		ng->ng_Y=GetInteger(PGBO_GeneralGadgets[GD_PGBO_Y]);
		ng->ng_Width=GetInteger(PGBO_GeneralGadgets[GD_PGBO_WIDTH]);
		ng->ng_Height=GetInteger(PGBO_GeneralGadgets[GD_PGBO_HEIGHT]);
	}
	if(ng->ng_X<0) ng->ng_X=0;
	if(ng->ng_Y<0) ng->ng_Y=0;
	return TRUE;
}

ULONG REMOVE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	Remove(Node(o));
	ng->ng_Window=NULL;
	return TRUE;
}

ULONG COMPUTE_Gadget(piClass *cl,piObject *o,struct piCompute *msg)
{
	struct Node_Gadget *ng;
	struct Node_Gadget *npg=NULL;
	struct Node_Window *nw;
	ULONG pgw=0;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
	if(ng->ng_Parent) piGetAttr(ng->ng_Parent,OBJ_Node,(ULONG *)&npg);
	switch(msg->Type)
	{
		case CT_LEFT:
			if(nw->nw_MyFlags&MYWA_DragToolVert)
				return((ULONG)(ng->ng_X+8)+(npg?npg->ng_X:0));
			else
				return((ULONG)(ng->ng_X+Scr->WBorLeft)+(npg?npg->ng_X:0));
		case CT_TOP:
			if(npg) pgw=piDoMethod(ng->ng_Parent,OM_COMPUTE,(ULONG)CT_HEIGHT);
			if(nw->nw_MyFlags&MYWA_DragToolVert)
				return((ULONG)(ng->ng_Y+Scr->WBorTop)+(ng->ng_Parent?npg->ng_Y:0)+(npg?pgw:0));
			else
			{
				if(nw->nw_MyFlags&MYWA_DragToolHoriz)
					return((ULONG)(ng->ng_Y+8)+(ng->ng_Parent?npg->ng_Y:0)+(npg?pgw:0));
				else
					return((ULONG)(ng->ng_Y+Scr->RastPort.TxHeight+Scr->WBorTop+1)+(ng->ng_Parent?npg->ng_Y:0)+(npg?pgw:0));
			}
		case CT_WIDTH:
//			if(ng->ng_RelWidth)
//				return((ULONG)(ng->ng_Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight-ng->ng_X));
//			else
				return(ng->ng_Width);
		case CT_HEIGHT:		
//			if(ng->ng_RelHeight)
//				return((ULONG)(ng->ng_Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom-ng->ng_Y));
//			else
				return(ng->ng_Height);
	}
}

ULONG HITTEST_Gadget(piClass *cl,piObject *o,struct piHitTest *msg)
{
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	UWORD x,y,w,h;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
//	if(!ng->ng_RelX) 
	x=piCX(o);// else x=nw->nw_Width-ng->ng_X;
//	if(!ng->ng_RelY)
	y=piCY(o);// else y=nw->nw_Height-ng->ng_Y;
//	if(!ng->ng_RelWidth)
	w=piCW(o);// else w=nw->nw_Width-nw->nw_Window->BorderRight+ng->ng_Width-x;
//	if(!ng->ng_RelHeight)
	h=piCH(o);//else h=nw->nw_Height-nw->nw_Window->BorderBottom+ng->ng_Height-y;

	if((msg->X>=x) && (msg->X<=x+w) && (msg->Y>=y) && (msg->Y<=y+h)) return TRUE;
	else return FALSE;
}

ULONG NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	piObject *obj;
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	ULONG size=0;

	if(GetTagData(GAOBJ_Type,GTYPE_GADTOOLS,msg->Tags)==GTYPE_GADTOOLS) size=sizeof(struct GadToolsData);

	if(obj=MyAllocVec(sizeof(piObject)+cl->Size+size,MEMF_PUBLIC|MEMF_CLEAR))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);
		obj->ln_Name=ng->ng_Label;
		ng->ng_NumTags=GetTagData(GAOBJ_NumTags,1,msg->Tags);
		ng->ng_NumOwnTags=GetTagData(GAOBJ_NumOwnTags,1,msg->Tags);
		if(ng->ng_Tags=AllocTagItems(ng->ng_NumTags))
		{
			if(ng->ng_OwnTags=AllocTagItems(ng->ng_NumOwnTags))
			{
				switch(GetTagData(GAOBJLIST_Tags,2,msg->Tags))
				{
					case 0: ng->ng_EditList.List=(struct List **)&(ng->ng_Tags[GetTagData(GAOBJLIST_List,0,msg->Tags)].ti_Data); break;
					case 1: ng->ng_EditList.List=(struct List **)&(ng->ng_OwnTags[GetTagData(GAOBJLIST_List,0,msg->Tags)].ti_Data); break;
					case 2: ng->ng_EditList.List=NULL; break;
				}
				ng->ng_EditList.Active=(ULONG *)&(ng->ng_Tags[GetTagData(GAOBJLIST_Active,0,msg->Tags)].ti_Data);
				ng->ng_TagTypes=(UBYTE *)GetTagData(GAOBJ_TagTypes,0,msg->Tags);
				ng->ng_OwnTagTypes=(UBYTE *)GetTagData(GAOBJ_OwnTagTypes,0,msg->Tags);
				ng->ng_PlugInID=GetTagData(GAOBJ_ID,0,msg->Tags);
				ng->ng_Type=GetTagData(GAOBJ_Type,GTYPE_GADTOOLS,msg->Tags);
				ng->ng_Pages=(struct Page *)GetTagData(OBJ_Properties,NULL,msg->Tags);
				ng->ng_Gadget=NULL;
				if(ng->ng_Type==GTYPE_GADTOOLS)
				{
					gtd=(struct GadToolsData *)GadgetData(ng);
					gtd->g_Kind=BUTTON_KIND;
					gtd->g_Flags=PLACETEXT_LEFT;
				}
				return (ULONG) obj;
//				FreeTagItems(ng->ng_OwnTags);
			}
			FreeTagItems(ng->ng_Tags);
		}
		MyFreeVec(obj);
	}
	return NULL;
}

ULONG DISPOSE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoMethod(o,OM_DELETE);
	MyFreeTagItems(ng->ng_Tags);
	MyFreeTagItems(ng->ng_OwnTags);
	MyFreeVec(o);
	return TRUE;
}

ULONG ADDTAIL_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
{
	piObject *on;
	struct Node_Gadget *ng,*ngn;
	struct Node_Window *nw;
	struct List *l;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(msg->Window,OBJ_Node,(ULONG *)&nw);
	piGetAttr(msg->Window,WNOBJ_GadgetList,(ULONG *)&l);

	ng->ng_Window=msg->Window;
	if(ng->ng_PlugInID==PAGE_ID) AddTail(l,Node(o));
	else
	{
		if(IsListEmpty(l))
		{
			AddTail(l,Node(o));
		}
		else
		{
			on=(piObject *)l->lh_TailPred;
			while(on->ln_Pred)
			{
				piGetAttr(on,OBJ_Node,(ULONG *)&ngn);
				if(ngn->ng_PlugInID==PAGE_ID)
					on=(piObject *)on->ln_Pred;
				else
				{
					Insert(l,Node(o),Node(on->ln_Succ));
					break;
				}
			}
		}
	}
	return TRUE;
}

void DrawRect(struct RastPort *rp,UWORD x,UWORD y,BOOL fill)
{
	if(fill)
		RectFill(rp,x,y,x+5,y+5);
	else
	{
		Move(rp,x,y);
		Draw(rp,x+5,y);
		Draw(rp,x+5,y+5);
		Draw(rp,x,y+5);
		Draw(rp,x,y);
	}
}

ULONG RENDER_Gadget(piClass *cl,piObject *o,Msg msg)
{
	UWORD x,y,w,h;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	struct Gadget *g,*ga;
	BOOL fill=FALSE;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);

	x=piCX(o);
	y=piCY(o);
	w=piCW(o);
	h=piCH(o);

	SetAPen(nw->nw_Window->RPort,DrawInfo->dri_Pens[BACKGROUNDPEN]);
	SetDrMd(nw->nw_Window->RPort,JAM1);
	RectFill(nw->nw_Window->RPort,x,y,x+w,y+h);

	g=CreateContext(&g);
	if(ga=(struct Gadget *)piDoMethod(o,OM_CREATE,g))
	{
		RefreshGList(g,nw->nw_Window,NULL,-1);
	}
	piDoMethod(o,OM_DELETE);
	if(ng->ng_Type==GTYPE_BOOPSI) g->NextGadget=NULL;
	FreeGadgets(g);

	if(ng->ng_Selected && GUI.gui_Mode==MODE_GADGET)
	{
		SetAPen(nw->nw_Window->RPort,1);
		SetDrMd(nw->nw_Window->RPort,COMPLEMENT);
		if(CurrentGadget==o) fill=TRUE;
		DrawRect(nw->nw_Window->RPort,x,y,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x+(w/2)-2,y,fill);
		DrawRect(nw->nw_Window->RPort,x+(w/2)-2,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x,y+(h/2)-2,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y+(h/2)-2,fill);
	}
	return TRUE;
}

ULONG ERASE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	UWORD x,y,w,h;
	piObject *wo;
	struct Node_Window *nw;
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(o,OBJ_Window,(ULONG *)&wo);
	if(wo)
	{
		piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
		if(nw) if(nw->nw_Window)
		{
//			if(!ng->ng_RelX)
 x=piCX(o);// else x=nw->nw_Width-ng->ng_X;
//			if(!ng->ng_RelY)
 y=piCY(o);// else y=nw->nw_Height-ng->ng_Y;
//			if(!ng->ng_RelWidth)
 w=piCW(o);// else w=nw->nw_Width-nw->nw_Window->BorderRight+ng->ng_Width-x;
//			if(!ng->ng_RelHeight)
 h=piCH(o);// else h=nw->nw_Height-nw->nw_Window->BorderBottom+ng->ng_Height-y;

			SetAPen(nw->nw_Window->RPort,DrawInfo->dri_Pens[BACKGROUNDPEN]);
			SetDrMd(nw->nw_Window->RPort,JAM1);
			RectFill(nw->nw_Window->RPort,x,y,x+w,y+h);
			piShowGrid(o);
		}
	}
	return TRUE;
}

ULONG SET_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct TagItem *tag,*tstate;
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);

	tstate=msg->Tags;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case GA_Left: ng->ng_X=tag->ti_Data; break;
			case GA_Top: ng->ng_Y=tag->ti_Data; break;
			case GA_Width: ng->ng_Width=tag->ti_Data; break;
			case GA_Height: ng->ng_Height=tag->ti_Data; break;
			case GAOBJ_Type: ng->ng_Type=tag->ti_Data; break;
			case OBJ_Select: ng->ng_Selected=tag->ti_Data; break;
		}
	}
	return TRUE;
}

ULONG GET_Gadget(piClass *cl,piObject *o,struct piGet *msg)
{
	struct Node_Gadget *ng=(struct Node_Gadget *)OBJECT_DATA(o);
//	struct Node_Window *nw;

	switch(msg->TagID)
	{
		case GA_Left: *(msg->TagData)=ng->ng_X; return TRUE;
		case GA_Top: *(msg->TagData)=ng->ng_Y; return TRUE;
		case GA_Width:
//			if(ng->ng_RelWidth)
//			{
//				piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
//				*(msg->TagData)=(nw->nw_Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight)+ng->ng_Width-ng->ng_X;
//			}
//			else
				*(msg->TagData)=ng->ng_Width;
			return TRUE;
		case GA_Height:
//			if(ng->ng_RelHeight)
//			{
//				piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
//				*(msg->TagData)=(nw->nw_Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom)+ng->ng_Height-ng->ng_Y;
//			}
//			else
				*(msg->TagData)=ng->ng_Height;
			return TRUE;
		case OBJ_Select: *(msg->TagData)=ng->ng_Selected; return TRUE;
		case OBJ_Node: *(msg->TagData)=(ULONG)ng; return TRUE;
		case OBJ_Window: *(msg->TagData)=(ULONG)ng->ng_Window; return TRUE;
		case OBJ_Properties: *(msg->TagData)=(ULONG)ng->ng_Pages; return TRUE;
		case GAOBJ_Type: *(msg->TagData)=ng->ng_Type; return TRUE;
		case GAOBJ_NumTags: *(msg->TagData)=ng->ng_NumTags; return TRUE;
		case GAOBJ_NumOwnTags: *(msg->TagData)=ng->ng_NumOwnTags; return TRUE;
		case GAOBJ_Tags: *(msg->TagData)=(ULONG)ng->ng_Tags; return TRUE;
		case GAOBJ_OwnTags: *(msg->TagData)=(ULONG)ng->ng_OwnTags; return TRUE;
		case GAOBJ_TagTypes: *(msg->TagData)=(ULONG)ng->ng_TagTypes; return TRUE;
		case GAOBJ_OwnTagTypes: *(msg->TagData)=(ULONG)ng->ng_OwnTagTypes; return TRUE;
		case GAOBJ_NodeData: *(msg->TagData)=(ULONG)GadgetData(ng); return TRUE;
		case GAOBJ_EditList: *(msg->TagData)=(ULONG)&ng->ng_EditList; return TRUE;
		default: return FALSE;
	}
}

ULONG CREATE_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
{
	WORD x,y,w,h;
	struct TagItem *ti;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	struct GadToolsData *gtd;
	struct NewGadget newg;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	if(ng->ng_Type==GTYPE_GADTOOLS)
	{
		piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
		piGetAttr(o,GAOBJ_NodeData,(ULONG *)&gtd);
		piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
//		gtd=(struct GadToolsData *) GadgetData(ng);

		if(ng->ng_RelX) x=-(nw->nw_Window->Width-nw->nw_Window->BorderLeft-ng->ng_X-1);
		else x=piCX(o);
		if(ng->ng_RelY) y=-(nw->nw_Window->Height-nw->nw_Window->BorderTop-ng->ng_Y-1);
		else y=piCY(o);
		if(ng->ng_RelWidth) w=-(nw->nw_Window->BorderLeft+nw->nw_Window->BorderRight+ng->ng_X+(nw->nw_Window->Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight-ng->ng_Width)-1);
		else w=piCW(o)+1;
		if(ng->ng_RelHeight) h=-(nw->nw_Window->BorderTop+nw->nw_Window->BorderBottom+ng->ng_Y+(nw->nw_Window->Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom-ng->ng_Height)-1);
		else h=piCH(o)+1;

		newg.ng_LeftEdge=x;
		newg.ng_TopEdge=y;
		newg.ng_Width=w;
		newg.ng_Height=h;
		newg.ng_GadgetText=gtd->g_Text;
		newg.ng_TextAttr=&EditFontTA;
		newg.ng_GadgetID=0;
		newg.ng_Flags=gtd->g_Flags;
		newg.ng_VisualInfo=VisualInfo;
		newg.ng_UserData=NULL;

		ng->ng_Gadget=CreateGadgetA(gtd->g_Kind,(struct Gadget *)msg->Window,&newg,ti);
		if(ng->ng_RelX) ng->ng_Gadget->Flags|=GFLG_RELRIGHT;
		if(ng->ng_RelY) ng->ng_Gadget->Flags|=GFLG_RELBOTTOM;
		if(ng->ng_RelWidth) ng->ng_Gadget->Flags|=GFLG_RELWIDTH;
		if(ng->ng_RelHeight) ng->ng_Gadget->Flags|=GFLG_RELHEIGHT;
		return (ULONG) (ng->ng_Gadget);
	}
	return NULL;
}

ULONG DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	return TRUE;
}

ULONG READ_Gadget(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0,a;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	struct GadToolsData *gtd;
	UWORD OrgNumTags,OrgNumOwnTags;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
	piGetAttr(o,GAOBJ_NodeData,(ULONG *)&gtd);

	OrgNumTags=ng->ng_NumTags;
	OrgNumOwnTags=ng->ng_NumOwnTags;
	ReadChunkBytes(msg->IFFh,(APTR)&ng->ng_X,SIZE(Node_Gadget,ng_X));
	for(a=0;a<ng->ng_NumTags;a++) ReadTagItem(msg->IFFh,&ng->ng_Tags[a]);
	for(a=0;a<ng->ng_NumOwnTags;a++) ReadTagItem(msg->IFFh,&ng->ng_OwnTags[a]);
	if(ng->ng_Type==GTYPE_GADTOOLS) ReadChunkBytes(msg->IFFh,(APTR)gtd,sizeof(struct GadToolsData));
	ng->ng_NumTags=OrgNumTags;
	ng->ng_NumOwnTags=OrgNumOwnTags;
	ng->ng_Tags[ng->ng_NumTags].ti_Tag=TAG_DONE;
	ng->ng_OwnTags[ng->ng_NumOwnTags].ti_Tag=TAG_DONE;
	return (ULONG)error;
}

ULONG WRITE_Gadget(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0,a;
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(o,GAOBJ_NodeData,(ULONG *)&gtd);
	if(error=PushChunk(msg->IFFh,NULL,ng->ng_PlugInID,IFFSIZE_UNKNOWN)) return (ULONG)error;
	WriteChunkBytes(msg->IFFh,(APTR)&ng->ng_X,SIZE(Node_Gadget,ng_X));
	for(a=0;a<ng->ng_NumTags;a++) WriteTagItem(msg->IFFh,&ng->ng_Tags[a],ng->ng_TagTypes[a]);
	for(a=0;a<ng->ng_NumOwnTags;a++) WriteTagItem(msg->IFFh,&ng->ng_OwnTags[a],ng->ng_OwnTagTypes[a]);
	if(ng->ng_Type==GTYPE_GADTOOLS) WriteChunkBytes(msg->IFFh,(APTR)gtd,sizeof(struct GadToolsData));
	if(error=PopChunk(msg->IFFh)) return (ULONG)error;
	return (ULONG)error;
}

/****************************************************************************/

piObject *piFindGadget(piObject *w,UWORD x,UWORD y)
{
	piObject *o;
	struct Node_Window *nw;

	piGetAttr(w,OBJ_Node,(ULONG *)&nw);

	for(o=(piObject *)GetHead(&nw->nw_List_Gadget);GetSucc(o);o=(piObject *)GetSucc(o))
		if(piDoMethod(o,OM_HITTEST,(ULONG)x,(ULONG)y)) return o;
	return NULL;
}

struct Gadget *CreateAllGadgets(piObject *o)
{
	struct Gadget *g=NULL;
	piObject *go;
	struct Node_Window *nw;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);

	nw->nw_Gadgets=NULL;
	if(nw->nw_PageGadget==0 && GUI.gui_Mode==MODE_TEST)
	{
		if(nw->nw_MyFlags&MYWA_BBottomSlider)
		{
			g=CreateContext(&nw->nw_Gadgets);
			unless(nw->nw_Object[0]=NewObject(NULL, BUTTONGCLASS,
//				ICA_TARGET, ICTARGET_IDCMP,
				GA_RelVerify,TRUE,
				GA_Image, gui_LeftImage,
				GA_RelRight, 1-(IM_WIDTH(gui_LeftImage)+IM_WIDTH(gui_SizeImage)+IM_WIDTH(gui_RightImage)),
				GA_RelBottom, 1-(IM_HEIGHT(gui_LeftImage)),
				GA_BottomBorder,TRUE,
				GA_Previous,(ULONG *)g,
				(nw->nw_Flags&WFLG_GIMMEZEROZERO) ? GA_GZZGadget : TAG_IGNORE,TRUE,
				GA_ID, GD_LEFTBUTTON,
				TAG_DONE)) return NULL;
			g=(struct Gadget *)nw->nw_Object[0];
			unless(nw->nw_Object[1]=NewObject(NULL, BUTTONGCLASS,
//				ICA_TARGET, ICTARGET_IDCMP,
				GA_RelVerify,TRUE,
				GA_Image, gui_RightImage,
				GA_RelRight, 1-(IM_WIDTH(gui_RightImage)+IM_WIDTH(gui_SizeImage)),
				GA_RelBottom, 1-(IM_HEIGHT(gui_RightImage)),
				GA_BottomBorder,TRUE,
				GA_Previous,(ULONG *)g,
				(nw->nw_Flags&WFLG_GIMMEZEROZERO) ? GA_GZZGadget : TAG_IGNORE,TRUE,
				GA_ID, GD_RIGHTBUTTON,
				TAG_DONE)) return NULL;
			g=(struct Gadget *)nw->nw_Object[1];
			unless(nw->nw_Object[2]=NewObject(NULL, PROPGCLASS,
//				ICA_TARGET, ICTARGET_IDCMP,
				GA_RelVerify,TRUE,
				PGA_Freedom, FREEHORIZ,
				PGA_NewLook, TRUE,
				PGA_Borderless, ((DrawInfo->dri_Flags & DRIF_NEWLOOK) && DrawInfo->dri_Depth != 1),
				GA_Left, 3,
				GA_RelBottom, -(IM_HEIGHT(gui_SizeImage)-3),
				GA_RelWidth, -(4+IM_WIDTH(gui_LeftImage)+IM_WIDTH(gui_RightImage)+IM_WIDTH(gui_SizeImage)+1),
				GA_Height, IM_HEIGHT(gui_SizeImage)-4,
				GA_BottomBorder, TRUE,
				GA_Previous,(ULONG *)g,
				(nw->nw_Flags&WFLG_GIMMEZEROZERO) ? GA_GZZGadget : TAG_IGNORE,TRUE,
				GA_ID, GD_HORIZBUTTON,
				PGA_Total, 1,
				PGA_Visible, 1,
				TAG_DONE)) return NULL;
			g=(struct Gadget *)nw->nw_Object[2];
		}
		else
		{
			nw->nw_Object[0]=NULL;
			nw->nw_Object[1]=NULL;
			nw->nw_Object[2]=NULL;
		}
		if(nw->nw_MyFlags&MYWA_BRightSlider)
		{
			if(!g) g=CreateContext(&nw->nw_Gadgets);
			unless(nw->nw_Object[3]=NewObject(NULL, BUTTONGCLASS,
//				ICA_TARGET, ICTARGET_IDCMP,
				GA_RelVerify,TRUE,
				GA_Image, gui_DownImage,
				GA_RelRight, 1-(IM_WIDTH(gui_DownImage)),
				GA_RelBottom, 1-(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_DownImage)),
				GA_RightBorder,TRUE,
				GA_Previous,(ULONG *)g,
				(nw->nw_Flags&WFLG_GIMMEZEROZERO) ? GA_GZZGadget : TAG_IGNORE,TRUE,
				GA_ID, GD_DOWNBUTTON,
				TAG_DONE)) return NULL;
			g=(struct Gadget *)nw->nw_Object[3];
			unless(nw->nw_Object[4]=NewObject(NULL, BUTTONGCLASS,
//				ICA_TARGET, ICTARGET_IDCMP,
				GA_RelVerify,TRUE,
				GA_Image, gui_UpImage,
				GA_RelRight, 1-(IM_WIDTH(gui_UpImage)),
				GA_RelBottom, 1-(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_DownImage)+IM_HEIGHT(gui_UpImage)),
				GA_RightBorder,TRUE,
				GA_Previous,(ULONG *)g,
				(nw->nw_Flags&WFLG_GIMMEZEROZERO) ? GA_GZZGadget : TAG_IGNORE,TRUE,
				GA_ID, GD_UPBUTTON,
				TAG_DONE)) return NULL;
			g=(struct Gadget *)nw->nw_Object[4];
			unless(nw->nw_Object[5]=NewObject(NULL, PROPGCLASS,
//				ICA_TARGET, ICTARGET_IDCMP,
				GA_RelVerify,TRUE,
				PGA_Freedom, FREEVERT,
				PGA_NewLook, TRUE,
				PGA_Borderless, ((DrawInfo->dri_Flags & DRIF_NEWLOOK) && DrawInfo->dri_Depth != 1),
				GA_RelRight, -(IM_WIDTH(gui_SizeImage)-5),
				GA_Top, Scr->WBorTop+Scr->Font->ta_YSize+1+1,
				GA_Width, IM_WIDTH(gui_SizeImage)-2-2-4,
				GA_RelHeight, -(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_UpImage)+IM_HEIGHT(gui_DownImage)+Scr->WBorTop+Scr->Font->ta_YSize+1+1+1),
				GA_RightBorder, TRUE,
				GA_Previous,(ULONG *)g,
				(nw->nw_Flags&WFLG_GIMMEZEROZERO) ? GA_GZZGadget : TAG_IGNORE,TRUE,
				GA_ID, GD_VERTBUTTON,
				PGA_Total, 1,
				PGA_Visible, 1,
				TAG_DONE)) return NULL;
			g=(struct Gadget *)nw->nw_Object[5];
		}
		else
		{
			nw->nw_Object[3]=NULL;
			nw->nw_Object[4]=NULL;
			nw->nw_Object[5]=NULL;
		}
		if(!IsListEmpty(List(&nw->nw_List_Gadget)))
		{
			if(!g) g=CreateContext(&nw->nw_Gadgets);
			for(go=(piObject *)GetHead(&nw->nw_List_Gadget);GetSucc(go);go=(piObject *)GetSucc(go))
				g=(struct Gadget *)piDoMethod(go,OM_CREATE,(ULONG)g);
		}
	}
	return nw->nw_Gadgets;
}

/********** General GadTools ***********************************************/

int FN_PGGT_X_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGGT_Y_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGGT_WIDTH_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGGT_HEIGHT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGGT_REL_X_Clicked( void )
{
	struct Node_Gadget *ng;
	piGetAttr(CurrentGadget,OBJ_Node,(ULONG *)&ng);
	ng->ng_RelX=PropertiesMsg.Code;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGGT_REL_Y_Clicked( void )
{
	struct Node_Gadget *ng;
	piGetAttr(CurrentGadget,OBJ_Node,(ULONG *)&ng);
	ng->ng_RelY=PropertiesMsg.Code;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGGT_REL_W_Clicked( void )
{
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	piGetAttr(CurrentGadget,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
	ng->ng_RelWidth=PropertiesMsg.Code;
//	if(ng->ng_RelWidth)
//	{
//		ng->ng_Width=-(nw->nw_Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight)+ng->ng_X+ng->ng_Width;
//		SetInteger(PropertiesWnd,PGGT_GeneralGadgets[GD_PGGT_WIDTH],ng->ng_Width);
//	}
//	else
//	{
//		ng->ng_Width=(nw->nw_Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight)+ng->ng_Width-ng->ng_X;
//		SetInteger(PropertiesWnd,PGGT_GeneralGadgets[GD_PGGT_WIDTH],ng->ng_Width);
//	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGGT_REL_H_Clicked( void )
{
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	piGetAttr(CurrentGadget,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
	ng->ng_RelHeight=PropertiesMsg.Code;
//	if(ng->ng_RelHeight)
//	{
//		ng->ng_Height=-(nw->nw_Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom)+ng->ng_Y+ng->ng_Height;
//		SetInteger(PropertiesWnd,PGGT_GeneralGadgets[GD_PGGT_HEIGHT],ng->ng_Height);
//	}
//	else
//	{
//		ng->ng_Height=(nw->nw_Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom)+ng->ng_Height-ng->ng_Y;
//		SetInteger(PropertiesWnd,PGGT_GeneralGadgets[GD_PGGT_HEIGHT],ng->ng_Height);
//	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGGT_TITLE_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGGT_LABEL_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGGT_PLACEMENT_Clicked( void )
{
	ULONG code;
	struct GadToolsData *gtd;

	piGetAttr(CurrentGadget,GAOBJ_NodeData,(ULONG *)&gtd);
	code=PropertiesMsg.Code;
	ClearFlag(gtd->g_Flags,PLACETEXT_LEFT);
	ClearFlag(gtd->g_Flags,PLACETEXT_RIGHT);
	ClearFlag(gtd->g_Flags,PLACETEXT_ABOVE);
	ClearFlag(gtd->g_Flags,PLACETEXT_BELOW);
	ClearFlag(gtd->g_Flags,PLACETEXT_IN);
	switch(code)
	{
		case 0: SetFlag(gtd->g_Flags,PLACETEXT_IN); break;
		case 1: SetFlag(gtd->g_Flags,PLACETEXT_LEFT); break;
		case 2: SetFlag(gtd->g_Flags,PLACETEXT_RIGHT); break;
		case 3: SetFlag(gtd->g_Flags,PLACETEXT_ABOVE); break;
		case 4: SetFlag(gtd->g_Flags,PLACETEXT_BELOW); break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

/********** General BOOPSI *************************************************/

int FN_PGBO_X_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGBO_Y_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGBO_WIDTH_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGBO_HEIGHT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGBO_REL_X_Clicked( void )
{
	struct Node_Gadget *ng;
	piGetAttr(CurrentGadget,OBJ_Node,(ULONG *)&ng);
	ng->ng_RelX=PropertiesMsg.Code;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGBO_REL_Y_Clicked( void )
{
	struct Node_Gadget *ng;
	piGetAttr(CurrentGadget,OBJ_Node,(ULONG *)&ng);
	ng->ng_RelY=PropertiesMsg.Code;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGBO_REL_W_Clicked( void )
{
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	piGetAttr(CurrentGadget,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
	ng->ng_RelWidth=PropertiesMsg.Code;
//	if(ng->ng_RelWidth)
//	{
//		ng->ng_Width=-(nw->nw_Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight)+ng->ng_X+ng->ng_Width;
//		SetInteger(PropertiesWnd,PGBO_GeneralGadgets[GD_PGBO_WIDTH],ng->ng_Width);
//	}
//	else
//	{
//		ng->ng_Width=(nw->nw_Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight)+ng->ng_Width-ng->ng_X;
//		SetInteger(PropertiesWnd,PGBO_GeneralGadgets[GD_PGBO_WIDTH],ng->ng_Width);
//	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGBO_REL_H_Clicked( void )
{
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	piGetAttr(CurrentGadget,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
	ng->ng_RelHeight=PropertiesMsg.Code;
//	if(ng->ng_RelHeight)
//	{
//		ng->ng_Height=-(nw->nw_Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom)+ng->ng_Y+ng->ng_Height;
//		SetInteger(PropertiesWnd,PGBO_GeneralGadgets[GD_PGBO_HEIGHT],ng->ng_Height);
//	}
//	else
//	{
//		ng->ng_Height=(nw->nw_Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom)+ng->ng_Height-ng->ng_Y;
//		SetInteger(PropertiesWnd,PGBO_GeneralGadgets[GD_PGBO_HEIGHT],ng->ng_Height);
//	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGBO_LABEL_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

/********** Lists **********************************************************/

ULONG ListSelectedLabel=-1;

int FN_PG_LISTS_LIST_Clicked( void )
{
	struct EditList *el;

	piGetAttr(CurrentGadget,GAOBJ_EditList,(ULONG *)&el);
	ListSelectedLabel=PropertiesMsg.Code;
	if(!GetSucc(GetSucc(FindNodeNum(*(el->List),ListSelectedLabel))))
		DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DOWN],PropertiesWnd,TRUE);
	else
		DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DOWN],PropertiesWnd,FALSE);
	if(ListSelectedLabel==0) DisableGadget(PG_ListsGadgets[GD_PG_LISTS_UP],PropertiesWnd,TRUE);
		else DisableGadget(PG_ListsGadgets[GD_PG_LISTS_UP],PropertiesWnd,FALSE);
	DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DEL],PropertiesWnd,FALSE);
	DisableGadget(PG_ListsGadgets[GD_PG_LISTS_EDIT],PropertiesWnd,FALSE);
	SetString(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_EDIT],FindNodeNum(*(el->List),ListSelectedLabel)->ln_Name);
	return 1;
}

int FN_PG_LISTS_EDIT_Clicked( void )
{
	struct Node *n;
	struct EditList *el;

	piGetAttr(CurrentGadget,GAOBJ_EditList,(ULONG *)&el);
	if(ListSelectedLabel!=-1)
	{
		if(n=FindNodeNum(*(el->List),ListSelectedLabel))
		{
			SetListView(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],NULL);
			MyFreeVec(n->ln_Name);
			n->ln_Name=MyAllocVec(strlen(GetString(PG_ListsGadgets[GD_PG_LISTS_EDIT]))+1,MEMF_ANY);
			strcpy(n->ln_Name,GetString(PG_ListsGadgets[GD_PG_LISTS_EDIT]));
			SetListView(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],*(el->List));
			GT_SetGadgetAttrs(PG_ListsGadgets[GD_PG_LISTS_LIST],PropertiesWnd,NULL,GTLV_Selected,ListSelectedLabel,GTLV_MakeVisible,ListSelectedLabel,TAG_DONE);
		}
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LISTS_ADD_Clicked( void )
{
	struct Node *n;
	struct EditList *el;

	piGetAttr(CurrentGadget,GAOBJ_EditList,(ULONG *)&el);
	SetListView(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],NULL);
	if(n=MyAllocVec(sizeof(struct Node),MEMF_ANY))
	{
		n->ln_Name=MyAllocVec(strlen(GetCatString(MSG_NEW))+1,MEMF_ANY);
		strcpy(n->ln_Name,GetCatString(MSG_NEW));
		DisableGadget(PG_ListsGadgets[GD_PG_LISTS_EDIT],PropertiesWnd,FALSE);
		AddTail(*(el->List),n);
		SetString(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_EDIT],n->ln_Name);
		SetListView(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],*(el->List));
		ListSelectedLabel=GetNumNode(*(el->List),n);
		GT_SetGadgetAttrs(PG_ListsGadgets[GD_PG_LISTS_LIST],PropertiesWnd,NULL,GTLV_Selected,ListSelectedLabel,GTLV_MakeVisible,ListSelectedLabel,TAG_DONE);
		ActivateGadget(PG_ListsGadgets[GD_PG_LISTS_EDIT],PropertiesWnd,NULL);
		if(!GetSucc(GetSucc(FindNodeNum(*(el->List),ListSelectedLabel))))
			DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DOWN],PropertiesWnd,TRUE);
		else
			DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DOWN],PropertiesWnd,FALSE);
		if(ListSelectedLabel==0) DisableGadget(PG_ListsGadgets[GD_PG_LISTS_UP],PropertiesWnd,TRUE);
		else DisableGadget(PG_ListsGadgets[GD_PG_LISTS_UP],PropertiesWnd,FALSE);
		DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DEL],PropertiesWnd,FALSE);
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LISTS_DEL_Clicked( void )
{
	struct Node *n;
	struct EditList *el;

	piGetAttr(CurrentGadget,GAOBJ_EditList,(ULONG *)&el);
	if(ListSelectedLabel!=-1)
	{
		if(n=FindNodeNum(*(el->List),ListSelectedLabel))
		{
			SetListView(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],NULL);
			MyFreeVec(n->ln_Name);
			Remove(n);
			MyFreeVec(n);
			SetListView(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],*(el->List));
			ListSelectedLabel=-1;
			DisableGadget(PG_ListsGadgets[GD_PG_LISTS_UP],PropertiesWnd,TRUE);
			DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DOWN],PropertiesWnd,TRUE);
			DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DEL],PropertiesWnd,TRUE);
			DisableGadget(PG_ListsGadgets[GD_PG_LISTS_EDIT],PropertiesWnd,TRUE);
		}
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LISTS_UP_Clicked( void )
{
	struct EditList *el;

	piGetAttr(CurrentGadget,GAOBJ_EditList,(ULONG *)&el);
	if(ListSelectedLabel!=-1)
	{
		if(*(el->List))
		{
			SetListView(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],NULL);
			MoveNodeUp(*(el->List),FindNodeNum(*(el->List),ListSelectedLabel));
			if(ListSelectedLabel>0) ListSelectedLabel--;
			SetListView(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],*(el->List));
			if(!GetSucc(GetSucc(FindNodeNum(*(el->List),ListSelectedLabel))))
				DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DOWN],PropertiesWnd,TRUE);
			else
				DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DOWN],PropertiesWnd,FALSE);
			if(ListSelectedLabel==0) DisableGadget(PG_ListsGadgets[GD_PG_LISTS_UP],PropertiesWnd,TRUE);
			else DisableGadget(PG_ListsGadgets[GD_PG_LISTS_UP],PropertiesWnd,FALSE);
			GT_SetGadgetAttrs(PG_ListsGadgets[GD_PG_LISTS_LIST],PropertiesWnd,NULL,GTLV_Selected,ListSelectedLabel,GTLV_MakeVisible,ListSelectedLabel,TAG_DONE);
		}
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LISTS_DOWN_Clicked( void )
{
	struct EditList *el;

	piGetAttr(CurrentGadget,GAOBJ_EditList,(ULONG *)&el);
	if(ListSelectedLabel!=-1)
	{
		if(*(el->List))
		{
			SetListView(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],NULL);
			MoveNodeDown(*(el->List),FindNodeNum(*(el->List),ListSelectedLabel));
			if(FindNodeNum(*(el->List),ListSelectedLabel+1)) ListSelectedLabel++;
			SetListView(PropertiesWnd,PG_ListsGadgets[GD_PG_LISTS_LIST],*(el->List));
			if(!GetSucc(GetSucc(FindNodeNum(*(el->List),ListSelectedLabel))))
				DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DOWN],PropertiesWnd,TRUE);
			else
				DisableGadget(PG_ListsGadgets[GD_PG_LISTS_DOWN],PropertiesWnd,FALSE);
			if(ListSelectedLabel==0) DisableGadget(PG_ListsGadgets[GD_PG_LISTS_UP],PropertiesWnd,TRUE);
			else DisableGadget(PG_ListsGadgets[GD_PG_LISTS_UP],PropertiesWnd,FALSE);
			GT_SetGadgetAttrs(PG_ListsGadgets[GD_PG_LISTS_LIST],PropertiesWnd,NULL,GTLV_Selected,ListSelectedLabel,GTLV_MakeVisible,ListSelectedLabel,TAG_DONE);
		}
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_LISTS_ACTIVE_Clicked( void )
{
	struct EditList *el;

	piGetAttr(CurrentGadget,GAOBJ_EditList,(ULONG *)&el);
	*(el->Active)=GetInteger(PG_ListsGadgets[GD_PG_LISTS_ACTIVE]);
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
