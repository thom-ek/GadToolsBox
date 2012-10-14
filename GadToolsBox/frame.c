/*
** GadToolsBox v3 - Frame Managment
*/

#include"includes.h"

/*
** Defines
*/

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGFR_General[];

struct Page PAGES_PROPERTIES_PGFR_PAGES[]=
{
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Functions
*/

ULONG Dispatcher_Frame(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_DEFAULT:
			retval=DEFAULT_Frame(cl,o,msg);
			break;
		case OM_INIT:
			retval=INIT_Frame(cl,o,msg);
			break;
		case OM_APPLY:
			retval=APPLY_Frame(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PGFR_General();
			PAGES_PROPERTIES_PGFR_PAGES[0].FirstGadget=PGFR_GeneralGList;
			PAGES_PROPERTIES_PGFR_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L36_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PGFR_PAGES[0].FirstGadget=NULL;
			CloseWindow_PGFR_General();
			break;
		case OM_NEW:
			retval=NEW_Frame(cl,o,(struct piSet *)msg);
			break;
		case OM_SET:
			retval=SET_Frame(cl,o,(struct piSet *)msg);
			break;
		case OM_GET:
			retval=GET_Frame(cl,o,(struct piGet *)msg);
			break;
		case OM_DISPOSE:
			retval=DISPOSE_Frame(cl,o,msg);
			break;
		case OM_ADDTAIL:
			retval=ADDTAIL_Frame(cl,o,(struct piWindow *)msg);
			break;
		case OM_REMOVE:
			Remove(Node(o));
			break;
		case OM_RENDER:
			retval=RENDER_Frame(cl,o,msg);
			break;
		case OM_ERASE:
			retval=ERASE_Frame(cl,o,msg);
			break;
		case OM_HITTEST:
			retval=HITTEST_Frame(cl,o,(struct piHitTest *)msg);
			break;
		case OM_COMPUTE:
			retval=COMPUTE_Frame(cl,o,(struct piCompute *)msg);
			break;
		case OM_READ:
			retval=READ_Frame(cl,o,(struct piIO *)msg);
			break;
		case OM_WRITE:
			retval=WRITE_Frame(cl,o,(struct piIO *)msg);
			break;
	}
	return retval;
}

ULONG DEFAULT_Frame(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Frame *nf;

	piGetAttr(o,OBJ_Node,(ULONG *)&nf);

	nf->nf_Width=30;
	nf->nf_Height=30;
	return TRUE;
}

ULONG INIT_Frame(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Frame *nf;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&nf);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetInteger(PropsWnd,PGFR_GeneralGadgets[GD_PGFR_X],nf->nf_X);
	SetInteger(PropsWnd,PGFR_GeneralGadgets[GD_PGFR_Y],nf->nf_Y);
	SetInteger(PropsWnd,PGFR_GeneralGadgets[GD_PGFR_WIDTH],nf->nf_Width);
	SetInteger(PropsWnd,PGFR_GeneralGadgets[GD_PGFR_HEIGHT],nf->nf_Height);
	SetCycle(PropsWnd,PGFR_GeneralGadgets[GD_PGFR_FRAME],nf->nf_FrameType);
	SetCheckMark(PropsWnd,PGFR_GeneralGadgets[GD_PGFR_RECESSED],(nf->nf_Flags&MYFR_Recessed)?TRUE:FALSE);
	return TRUE;
}

ULONG APPLY_Frame(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Frame *nf;

	piGetAttr(o,OBJ_Node,(ULONG *)&nf);

	nf->nf_X=GetInteger(PGFR_GeneralGadgets[GD_PGFR_X]);
	nf->nf_Y=GetInteger(PGFR_GeneralGadgets[GD_PGFR_Y]);
	nf->nf_Width=GetInteger(PGFR_GeneralGadgets[GD_PGFR_WIDTH]);
	nf->nf_Height=GetInteger(PGFR_GeneralGadgets[GD_PGFR_HEIGHT]);
	return TRUE;
}

ULONG COMPUTE_Frame(piClass *cl,piObject *o,struct piCompute *msg)
{
	struct Node_Frame *nf;
	struct Node_Window *nw;

	piGetAttr(o,OBJ_Node,(ULONG *)&nf);
	piGetAttr(nf->nf_Window,OBJ_Node,(ULONG *)&nw);
	switch(msg->Type)
	{
		case CT_LEFT:
			if(nw->nw_MyFlags&MYWA_DragToolVert)
				return((ULONG)(nf->nf_X+8));
			else
				return((ULONG)(nf->nf_X+Scr->WBorLeft));
		case CT_TOP:
			if(nw->nw_MyFlags&MYWA_DragToolVert)
				return((ULONG)(nf->nf_Y+Scr->WBorTop));
			else
				if(nw->nw_MyFlags&MYWA_DragToolHoriz)
					return((ULONG)(nf->nf_Y+8));
				else
					return((ULONG)(nf->nf_Y+Scr->RastPort.TxHeight+Scr->WBorTop+1));
		case CT_WIDTH:
			return(nf->nf_Width);
		case CT_HEIGHT:		
			return(nf->nf_Height);
	}
}

ULONG HITTEST_Frame(piClass *cl,piObject *o,struct piHitTest *msg)
{
	UWORD x,y,w,h;

	x=piCX(o);
	y=piCY(o);
	w=piCW(o);
	h=piCH(o);

	if((msg->X>=x) && (msg->X<=x+w) && (msg->Y>=y) && (msg->Y<=y+h)) return TRUE;
	else return FALSE;
}

ULONG NEW_Frame(piClass *cl,piObject *o,struct piSet *msg)
{
	piObject *obj;
	struct Node_Frame *nf;

	if(obj=MyAllocVec(sizeof(piObject)+cl->Size,MEMF_PUBLIC|MEMF_CLEAR))
	{
		nf=(struct Node_Frame *)OBJECT_DATA(obj);
		nf->nf_Pages=PAGES_PROPERTIES_PGFR_PAGES,
		obj->ln_Name=NULL;
		nf->nf_FrameType=FTYPE_BUTTON;
		ClearFlag(nf->nf_Flags,MYFR_Recessed);
		return (ULONG) obj;
	}
	return NULL;
}

ULONG DISPOSE_Frame(piClass *cl,piObject *o,Msg msg)
{
	piDoMethod(o,OM_ERASE);
	piDoMethod(o,OM_DELETE);
	MyFreeVec(o);
	return TRUE;
}

ULONG ADDTAIL_Frame(piClass *cl,piObject *o,struct piWindow *msg)
{
	struct Node_Frame *nf;
	struct List *l;

	piGetAttr(o,OBJ_Node,(ULONG *)&nf);
	piGetAttr(msg->Window,WNOBJ_FrameList,(ULONG *)&l);
	nf->nf_Window=msg->Window;
	AddTail(l,Node(o));
	return TRUE;
}

extern DrawRect(struct RastPort *,UWORD,UWORD,BOOL);

ULONG RENDER_Frame(piClass *cl,piObject *o,Msg msg)
{
	UWORD x,y,w,h;
	struct Node_Frame *nf;
	struct Node_Window *nw;
	BOOL fill=FALSE;

	piGetAttr(o,OBJ_Node,(ULONG *)&nf);
	piGetAttr(nf->nf_Window,OBJ_Node,(ULONG *)&nw);

	x=piCX(o);
	y=piCY(o);
	w=piCW(o);
	h=piCH(o);

	SetAPen(nw->nw_Window->RPort,DrawInfo->dri_Pens[BACKGROUNDPEN]);
	SetDrMd(nw->nw_Window->RPort,JAM1);
	RectFill(nw->nw_Window->RPort,x,y,x+w,y+h);

	switch(nf->nf_FrameType)
	{
		case FTYPE_BUTTON:
			DrawBevelBox(nw->nw_Window->RPort,x,y,w+1,h+1,
				GTBB_FrameType, BBFT_BUTTON,
				(nf->nf_Flags&MYFR_Recessed) ? GTBB_Recessed : TAG_IGNORE, TRUE,
				GT_VisualInfo,VisualInfo,
				TAG_DONE);
			break;
		case FTYPE_ICONDROPBOX:
			DrawBevelBox(nw->nw_Window->RPort,x,y,w+1,h+1,
				GTBB_FrameType, BBFT_ICONDROPBOX,
				(nf->nf_Flags&MYFR_Recessed) ? GTBB_Recessed : TAG_IGNORE, TRUE,
				GT_VisualInfo,VisualInfo,
				TAG_DONE);
			break;
		case FTYPE_RIDGE:
			DrawBevelBox(nw->nw_Window->RPort,x,y,w+1,h+1,
				GTBB_FrameType, BBFT_RIDGE,
				(nf->nf_Flags&MYFR_Recessed) ? GTBB_Recessed : TAG_IGNORE, TRUE,
				GT_VisualInfo,VisualInfo,
				TAG_DONE);
			break;
		case FTYPE_HORIZONTALLINE:
			DrawBevelBox(nw->nw_Window->RPort,x,y,w+1,2,
				GTBB_FrameType, BBFT_BUTTON,
				(nf->nf_Flags&MYFR_Recessed) ? GTBB_Recessed : TAG_IGNORE, TRUE,
				GT_VisualInfo,VisualInfo,
				TAG_DONE);
			nf->nf_Height=2;
			break;
		case FTYPE_VERTICALLINE:
			DrawBevelBox(nw->nw_Window->RPort,x,y,2,h+1,
				GTBB_FrameType, BBFT_BUTTON,
				(nf->nf_Flags&MYFR_Recessed) ? GTBB_Recessed : TAG_IGNORE, TRUE,
				GT_VisualInfo,VisualInfo,
				TAG_DONE);
			nf->nf_Width=2;
			break;
	}

	if(nf->nf_Selected && GUI.gui_Mode==MODE_FRAME)
	{
		SetAPen(nw->nw_Window->RPort,1);
		SetDrMd(nw->nw_Window->RPort,COMPLEMENT);
		DrawRect(nw->nw_Window->RPort,x,y,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x+(w/2)-2,y,fill);
		DrawRect(nw->nw_Window->RPort,x+(w/2)-2,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x,y+(h/2)-2,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y+(h/2)-2,fill);
/*
		RectFill(nw->nw_Window->RPort,x,        y,        x+6,      y+3);
		RectFill(nw->nw_Window->RPort,x+w-6,    y,        x+w,      y+3);
		RectFill(nw->nw_Window->RPort,x+w-6,    y+h-3,    x+w,      y+h);
		RectFill(nw->nw_Window->RPort,x,        y+h-3,    x+6,      y+h);
		RectFill(nw->nw_Window->RPort,x+(w/2)-3,y,        x+(w/2)+3,y+3);
		RectFill(nw->nw_Window->RPort,x+(w/2)-3,y+h-3,    x+(w/2)+3,y+h);
		RectFill(nw->nw_Window->RPort,x,        y+(h/2)-2,x+6,      y+(h/2)+2);
		RectFill(nw->nw_Window->RPort,x+w-6,    y+(h/2)-2,x+w,      y+(h/2)+2);
*/
	}
	return TRUE;
}

ULONG ERASE_Frame(piClass *cl,piObject *o,Msg msg)
{
	UWORD x,y,w,h;
	piObject *wo;
	struct Node_Window *nw;

	piGetAttr(o,OBJ_Window,(ULONG *)&wo);
	if(wo)
	{
		piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
		if(nw) if(nw->nw_Window)
		{
			x=piCX(o);
			y=piCY(o);
			w=piCW(o);
			h=piCH(o);

			SetAPen(nw->nw_Window->RPort,DrawInfo->dri_Pens[BACKGROUNDPEN]);
			SetDrMd(nw->nw_Window->RPort,JAM1);
			RectFill(nw->nw_Window->RPort,x,y,x+w,y+h);
			piShowGrid(o);
		}
	}
	return TRUE;
}

ULONG SET_Frame(piClass *cl,piObject *o,struct piSet *msg)
{
	struct TagItem *t;
	struct Node_Frame *nf;

	piGetAttr(o,OBJ_Node,(ULONG *)&nf);

	if(t=FindTagItem(GA_Left,msg->Tags)) nf->nf_X=t->ti_Data;
	if(t=FindTagItem(GA_Top,msg->Tags)) nf->nf_Y=t->ti_Data;
	if(t=FindTagItem(GA_Width,msg->Tags)) nf->nf_Width=t->ti_Data;
	if(t=FindTagItem(GA_Height,msg->Tags)) nf->nf_Height=t->ti_Data;
	if(t=FindTagItem(FROBJ_Type,msg->Tags)) nf->nf_FrameType=t->ti_Data;
	if(t=FindTagItem(OBJ_Select,msg->Tags)) nf->nf_Selected=t->ti_Data;
	return TRUE;
}

ULONG GET_Frame(piClass *cl,piObject *o,struct piGet *msg)
{
	struct Node_Frame *nf=(struct Node_Frame *)OBJECT_DATA(o);

	switch(msg->TagID)
	{
		case GA_Left: *(msg->TagData)=nf->nf_X; return TRUE;
		case GA_Top: *(msg->TagData)=nf->nf_Y; return TRUE;
		case GA_Width: *(msg->TagData)=nf->nf_Width; return TRUE;
		case GA_Height: *(msg->TagData)=nf->nf_Height; return TRUE;
		case OBJ_Select: *(msg->TagData)=nf->nf_Selected; return TRUE;
		case OBJ_Node: *(msg->TagData)=(ULONG)nf; return TRUE;
		case OBJ_Window: *(msg->TagData)=(ULONG)nf->nf_Window; return TRUE;
		case OBJ_Properties: *(msg->TagData)=(ULONG)nf->nf_Pages; return TRUE;
		case FROBJ_Type: *(msg->TagData)=nf->nf_FrameType; return TRUE;
		default: return FALSE;
	}
}

ULONG READ_Frame(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0;
	struct Node_Frame *nf;

	piGetAttr(o,OBJ_Node,(ULONG *)&nf);
	ReadChunkBytes(msg->IFFh,(APTR)&nf->nf_X,SIZE(Node_Frame,nf_X));
	return (ULONG)error;
}

ULONG WRITE_Frame(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0;
	struct Node_Frame *nf;

	piGetAttr(o,OBJ_Node,(ULONG *)&nf);
	if(error=PushChunk(msg->IFFh,NULL,ID_FRME,IFFSIZE_UNKNOWN)) return (ULONG)error;
	WriteChunkBytes(msg->IFFh,(APTR)&nf->nf_X,SIZE(Node_Frame,nf_X));
	if(error=PopChunk(msg->IFFh)) return (ULONG)error;
	return (ULONG)error;
}

/****************************************************************************/

piObject *piFindFrame(piObject *w,UWORD x,UWORD y)
{
	piObject *o;
	struct List *l;

	piGetAttr(w,WNOBJ_FrameList,(ULONG *)&l);

	for(o=(piObject *)GetHead(l);GetSucc(o);o=(piObject *)GetSucc(o))
		if(piDoMethod(o,OM_HITTEST,(ULONG)x,(ULONG)y)) return o;
	return NULL;
}

/****************************************************************************/

int FN_PGFR_X_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGFR_Y_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGFR_WIDTH_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGFR_HEIGHT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return TRUE;
}

int FN_PGFR_FRAME_Clicked( void )
{
	/* routine when gadget "" is clicked. */
	struct Node_Frame *nf;

	piGetAttr(CurrentFrame,OBJ_Node,(ULONG *)&nf);
	nf->nf_FrameType=PropertiesMsg.Code;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGFR_RECESSED_Clicked( void )
{
	/* routine when gadget "Recessed" is clicked. */
	struct Node_Frame *nf;
	ULONG code;

	piGetAttr(CurrentFrame,OBJ_Node,(ULONG *)&nf);
	code=PropertiesMsg.Code;
	DoFlag(nf->nf_Flags,MYFR_Recessed);
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
