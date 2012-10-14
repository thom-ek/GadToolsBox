/*
** GadToolsBox v3 - Window management
*/

#include"includes.h"
#include"defines.h"

/*
** Defines
*/

/*
** Functions
*/

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGWN_Style[];
extern struct BevelBox BBOX_PROPERTIES_PAGES_PGWN_Flags3[];

struct Page PAGES_PROPERTIES_PGWN_PAGES[]=
{
	&PAGES_PROPERTIES_PGWN_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PGWN_PAGES[2], NULL, FS_NORMAL, NULL, NULL, BBOX_PROPERTIES_PAGES_PGWN_Style, NULL, NULL,
	&PAGES_PROPERTIES_PGWN_PAGES[3], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PGWN_PAGES[4], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PGWN_PAGES[5], NULL, FS_NORMAL, NULL, NULL, BBOX_PROPERTIES_PAGES_PGWN_Flags3, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

ULONG Dispatcher_Window(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval=NULL;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=INIT_Window(cl,o,msg);
			break;
		case OM_APPLY:
			retval=APPLY_Window(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PGWN_General();
			OpenWindow_PGWN_Style();
			OpenWindow_PGWN_Flags1();
			OpenWindow_PGWN_Flags2();
			OpenWindow_PGWN_Flags3();
			OpenWindow_PGWN_TabOrder();
			PAGES_PROPERTIES_PGWN_PAGES[0].FirstGadget=PGWN_GeneralGList;
			PAGES_PROPERTIES_PGWN_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L30_GAD);
			PAGES_PROPERTIES_PGWN_PAGES[1].FirstGadget=PGWN_StyleGList;
			PAGES_PROPERTIES_PGWN_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L32_GAD);
			PAGES_PROPERTIES_PGWN_PAGES[2].FirstGadget=PGWN_Flags1GList;
			PAGES_PROPERTIES_PGWN_PAGES[2].Name=GetCatString(MSG_PROPERTIES_PAGES_L31_GAD);
			PAGES_PROPERTIES_PGWN_PAGES[3].FirstGadget=PGWN_Flags2GList;
			PAGES_PROPERTIES_PGWN_PAGES[3].Name=GetCatString(MSG_PROPERTIES_PAGES_L33_GAD);
			PAGES_PROPERTIES_PGWN_PAGES[4].FirstGadget=PGWN_Flags3GList;
			PAGES_PROPERTIES_PGWN_PAGES[4].Name=GetCatString(MSG_PROPERTIES_PAGES_L34_GAD);
			PAGES_PROPERTIES_PGWN_PAGES[5].FirstGadget=PGWN_TabOrderGList;
			PAGES_PROPERTIES_PGWN_PAGES[5].Name=GetCatString(MSG_PROPERTIES_PAGES_L42_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PGWN_PAGES[5].FirstGadget=NULL;
			PAGES_PROPERTIES_PGWN_PAGES[4].FirstGadget=NULL;
			PAGES_PROPERTIES_PGWN_PAGES[3].FirstGadget=NULL;
			PAGES_PROPERTIES_PGWN_PAGES[2].FirstGadget=NULL;
			PAGES_PROPERTIES_PGWN_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PGWN_PAGES[0].FirstGadget=NULL;
			CloseWindow_PGWN_TabOrder();
			CloseWindow_PGWN_Flags3();
			CloseWindow_PGWN_Flags2();
			CloseWindow_PGWN_Flags1();
			CloseWindow_PGWN_Style();
			CloseWindow_PGWN_General();
			break;
		case OM_NEW:
			retval=NEW_Window(cl,o,(struct piSet *)msg);
			break;
		case OM_SET:
			retval=SET_Window(cl,o,(struct piSet *)msg);
			break;
		case OM_GET:
			retval=GET_Window(cl,o,(struct piGet *)msg);
			break;
		case OM_DISPOSE:
			retval=DISPOSE_Window(cl,o,(struct piSet *)msg);
			break;
		case OM_RENDER:
			retval=RENDER_Window(cl,o,msg);
			break;
		case OM_ERASE:
			retval=ERASE_Window(cl,o,msg);
			break;
		case OM_ADDTAIL:
			AddTail(List(&GUI.gui_List_Window),Node(o));
			break;
		case OM_REMOVE:
			Remove((struct Node *)o);
			break;
		case OM_CREATE:
			retval=OPEN_Window(cl,o,msg);
			break;
		case OM_DELETE:
			retval=CLOSE_Window(cl,o,msg);
			break;
		case OM_READ:
			retval=READ_Window(cl,o,(struct piIO *)msg);
			break;
		case OM_WRITE:
			retval=WRITE_Window(cl,o,(struct piIO *)msg);
			break;
		case OM_CUT:
			retval=CUT_Window(cl,o,msg);
			break;
		case OM_COPY:
			retval=COPY_Window(cl,o,(struct piCopy *)msg);
			break;
	}
	return retval;
}

ULONG CUT_Window(piClass *cl,piObject *o,Msg msg)
{
	piObject *go,*fo;
	struct Node_Gadget *ng;
	struct Node_Frame *nf;
	struct Node_Window *nw;
	BOOL s=FALSE;

	piCopyObject(o,COPY_CLIP);
	piCopyObject(o,COPY_UNDO);

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
			{
				piGetAttr(go,OBJ_Node,(ULONG *)&ng);
				if(ng->ng_Selected) { s=TRUE; break; }
			}
			if(s)
			{
repg:		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
				{
					piGetAttr(go,OBJ_Node,(ULONG *)&ng);
					if(ng->ng_Selected)
					{
						piRemoveObject(go);
						piDisposeObject(go);
						goto repg;
					}
				}
				piEraseObject(o);
				piRenderObject(o);
				PropertiesMode=PM_NONE;
				RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
			}
			break;
		case MODE_FRAME:
			for(fo=(piObject *)GetHead(List(&nw->nw_List_Frame));GetSucc(fo);fo=(piObject *)GetSucc(fo))
			{
				piGetAttr(fo,OBJ_Node,(ULONG *)&nf);
				if(nf->nf_Selected) { s=TRUE; break; }
			}
			if(s)
			{
repf:		for(fo=(piObject *)GetHead(List(&nw->nw_List_Frame));GetSucc(fo);fo=(piObject *)GetSucc(fo))
				{
					piGetAttr(fo,OBJ_Node,(ULONG *)&nf);
					if(nf->nf_Selected)
					{
						piRemoveObject(fo);
						piDisposeObject(fo);
						goto repf;
					}
				}
				piEraseObject(o);
				piRenderObject(o);
				PropertiesMode=PM_NONE;
				RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
			}
			break;
	}
	if((!s) && (GUI.gui_Mode!=MODE_TEST))
	{
		piRemoveObject(o);
		piDisposeObject(o);
//		PropertiesMode=PM_NONE;
//		RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
		return 2;
	}
	return TRUE;
}

ULONG COPY_Window(piClass *cl,piObject *o,struct piCopy *msg)
{
	struct IFFHandle *iffh;
	ULONG error=0;
	piObject *go,*fo;
	struct Node_Gadget *ng;
	struct Node_Frame *nf;
	struct Node_Window *nw;
	BOOL sg=FALSE,sf=FALSE;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	if(!(ClipHandle=OpenClipboard(PRIMARY_CLIP))) return 1;
	switch(GUI.gui_Mode)
	{
		case MODE_GADGET:
			for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
			{
				piGetAttr(go,OBJ_Node,(ULONG *)&ng);
				if(ng->ng_Selected) { sg=TRUE; break; }
			}
			break;
		case MODE_FRAME:
			for(fo=(piObject *)GetHead(List(&nw->nw_List_Frame));GetSucc(fo);fo=(piObject *)GetSucc(fo))
			{
				piGetAttr(fo,OBJ_Node,(ULONG *)&nf);
				if(nf->nf_Selected) { sf=TRUE; break; }
			}
			break;
		case MODE_TEST:
			return TRUE;
	}
	if(iffh=AllocIFF())
	{
		iffh->iff_Stream=(ULONG)((msg->Copy==COPY_CLIP)?ClipHandle:UndoHandle);
		InitIFFasClip(iffh);
		if(!OpenIFF(iffh,IFFF_WRITE))
		{
			if(sg)		// Copy Selected Gadgets
			{
				if(!PushChunk(iffh,ID_GTBX,ID_FORM,IFFSIZE_UNKNOWN))
				{
					for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
					{
						piGetAttr(go,OBJ_Node,(ULONG *)&ng);
						if(ng->ng_Selected) piWriteObject(go,iffh);
					}
					if(error=PopChunk(iffh)) goto skip;
				}
			}
			if(sf)		// Copy Selected Frames
			{
				if(!PushChunk(iffh,ID_GTBX,ID_FORM,IFFSIZE_UNKNOWN))
				{
					for(fo=(piObject *)GetHead(List(&nw->nw_List_Frame));GetSucc(fo);fo=(piObject *)GetSucc(fo))
					{
						piGetAttr(fo,OBJ_Node,(ULONG *)&nf);
						if(nf->nf_Selected) piWriteObject(fo,iffh);
					}
					if(error=PopChunk(iffh)) goto skip;
				}
			}
			if((!sf) && (!sg))		// Copy Window
			{
				if(!PushChunk(iffh,ID_GTBX,ID_FORM,IFFSIZE_UNKNOWN))
				{
					piWriteObject(o,iffh);
					if(error=PopChunk(iffh)) goto skip;
				}
			}
skip:	CloseIFF(iffh);
		}
		FreeIFF(iffh);
	}
	if(ClipHandle) CloseClipboard(ClipHandle); ClipHandle=NULL;
	return TRUE;
}

ULONG NEW_Window(piClass *cl,piObject *o,struct piSet *msg)
{
	piObject *obj;
	struct Node_Window *nw;

	if(!CheckRegistration())
		if(!ListEmpty(&GUI.gui_List_Window)) return NULL;
	if(obj=MyAllocVec(sizeof(piObject)+cl->Size,MEMF_PUBLIC|MEMF_CLEAR))
	{
		nw=(struct Node_Window *)OBJECT_DATA(obj);
		nw->nw_ID=ID_WIND;
		obj->ln_Name=nw->nw_Label;
		NewList(List(&nw->nw_List_Gadget));
		NewList(List(&nw->nw_List_Frame));
		nw->nw_Flags=WFLG_SIZEGADGET|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_ACTIVATE|WFLG_NEWLOOKMENUS|WFLG_SIMPLE_REFRESH;
		nw->nw_MyFlags=MYWA_FontAdaptive;
		nw->nw_IDCMP=IDCMP_CLOSEWINDOW;
		nw->nw_XPos=WPOS_REL_LEFT_TOP;
		nw->nw_YPos=WPOS_REL_LEFT_TOP;
		nw->nw_X=10;
		nw->nw_Y=15;
		nw->nw_Width=400;
		nw->nw_Height=100;
		nw->nw_Dimensions[0]=2;		// min width
		nw->nw_Dimensions[1]=2;		// min height
		nw->nw_Dimensions[2]=65535;	// max width
		nw->nw_Dimensions[3]=65535;	// max hright
		nw->nw_PageGadget=0;			// 0 - no lock
		nw->nw_Window=NULL;
	}
	return (ULONG) obj;
}

ULONG DISPOSE_Window(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Window *nw;
	piObject *obj;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	piDoMethod(o,OM_DELETE);
	while(obj=(piObject *)RemTail(List(&nw->nw_List_Frame))) piDoMethod(obj,OM_DISPOSE);
	while(obj=(piObject *)RemTail(List(&nw->nw_List_Gadget))) piDoMethod(obj,OM_DISPOSE);

	MyFreeVec(o);
	return TRUE;
}

ULONG RENDER_Window(piClass *cl,piObject *o,Msg msg)
{
	piObject *obj;
	struct Node_Window *nw;
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	if(nw->nw_Window)
	{
		ShowGrid(o);
		for(obj=(piObject *)GetHead(List(&nw->nw_List_Frame));GetSucc(obj);obj=(piObject *)GetSucc(obj)) piDoMethod(obj,OM_RENDER);
		if(GUI.gui_Mode!=MODE_TEST)
		{
			for(obj=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(obj);obj=(piObject *)GetSucc(obj)) piDoMethod(obj,OM_RENDER);
		}
		else
		{
			for(obj=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(obj);obj=(piObject *)GetSucc(obj))
			{
				piGetAttr(obj,OBJ_Node,(ULONG *)&ng);
				if(ng->ng_PlugInID==DATATYPE_ID)
				{
					RefreshDTObject((Object *)ng->ng_Gadget,nw->nw_Window,NULL,TAG_DONE);
					RefreshGadgets(ng->ng_Gadget,nw->nw_Window,NULL);
				}
			}
			GT_RefreshWindow(nw->nw_Window,NULL);
		}
	}
	return TRUE;
}

ULONG ERASE_Window(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Window *nw;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	if(nw->nw_Window)
	{
		SetAPen(nw->nw_Window->RPort,DrawInfo->dri_Pens[BACKGROUNDPEN]);
		SetDrMd(nw->nw_Window->RPort,JAM1);
		RectFill(nw->nw_Window->RPort,nw->nw_Window->BorderLeft,nw->nw_Window->BorderTop,nw->nw_Window->Width-nw->nw_Window->BorderRight-1,nw->nw_Window->Height-nw->nw_Window->BorderBottom-1);
		ShowGrid(o);
	}
	return TRUE;
}

ULONG OPEN_Window(piClass *cl,piObject *o,Msg msg)
{
	struct Gadget *gads,*drag=NULL;
	struct Node_Window *nw;
	struct List *l;
	piObject *so;
	ULONG flags;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	if(nw)
	{
		if(nw->nw_Window)
		{
			WindowToFront(nw->nw_Window);
			ActivateWindow(nw->nw_Window);
			return TRUE;
		}
		else
		{
			piGetAttr(o,WNOBJ_GadgetList,(ULONG *)&l);
			for(so=(piObject *)GetHead(l);GetSucc(so);so=(piObject *)GetSucc(so)) piSetAttrs(so,OBJ_Select,FALSE,TAG_DONE);
			piGetAttr(o,WNOBJ_FrameList,(ULONG *)&l);
			for(so=(piObject *)GetHead(l);GetSucc(so);so=(piObject *)GetSucc(so)) piSetAttrs(so,OBJ_Select,FALSE,TAG_DONE);

			nw->nw_Zoom[0]=WinLeft(o);
			nw->nw_Zoom[1]=WinTop(o);
			if(nw->nw_WindowTitle[0]) nw->nw_Zoom[2]=TextLength(&Scr->RastPort,(UBYTE *)nw->nw_WindowTitle,strlen((char *)nw->nw_WindowTitle))+80;
			else nw->nw_Zoom[2]=80L;
			nw->nw_Zoom[3]=Scr->WBorTop+Scr->RastPort.TxHeight+1;

			if((nw->nw_MyFlags & MYWA_DragToolHoriz) || (nw->nw_MyFlags & MYWA_DragToolVert))
			{
				if(drag=MyAllocVec(sizeof(struct Gadget),MEMF_PUBLIC|MEMF_CLEAR))
				{
					if((nw->nw_MyFlags & MYWA_DragToolVert))
					{	drag->Width=8; drag->Height=nw->nw_Height; }
					if((nw->nw_MyFlags & MYWA_DragToolHoriz))
					{	drag->Width=nw->nw_Width; drag->Height=8; }
					drag->Flags=GFLG_GADGHNONE;
					drag->Activation=((nw->nw_MyFlags & MYWA_DragToolHoriz)?GACT_TOPBORDER:GACT_LEFTBORDER)|GACT_BORDERSNIFF;
					drag->GadgetType=GTYP_WDRAGGING;
					drag->NextGadget=NULL;
					drag->UserData=(APTR)DRAG_ID;
				}
			}

			flags=nw->nw_Flags;
			ClearFlag(flags,WFLG_SIMPLE_REFRESH);
			ClearFlag(flags,WFLG_SMART_REFRESH);
			ClearFlag(flags,WFLG_SUPER_BITMAP);
			if(nw->nw_Window=OpenWindowTags(NULL,
				WA_Left,WinLeft(o),
				WA_Top,WinTop(o),
				WA_Flags,flags,
				WA_CustomScreen,Scr,
				(nw->nw_ScreenTitle[0]) ? WA_ScreenTitle : TAG_IGNORE,nw->nw_ScreenTitle,
				(nw->nw_WindowTitle[0]) ? WA_Title : TAG_IGNORE,nw->nw_WindowTitle,
				(nw->nw_MyFlags&MYWA_InnerWidth) ? WA_InnerWidth : WA_Width,WinWidth(o),
				(nw->nw_MyFlags&MYWA_InnerHeight) ? WA_InnerHeight : WA_Height,WinHeight(o),
				(nw->nw_MyFlags&MYWA_MouseQueue) ? WA_MouseQueue : TAG_IGNORE,nw->nw_MouseQueue,
				(nw->nw_MyFlags&MYWA_RptQueue) ? WA_RptQueue : TAG_IGNORE,nw->nw_RptQueue,
				(nw->nw_MyFlags&MYWA_AutoAdjust) ? WA_AutoAdjust : TAG_IGNORE,TRUE,
				(nw->nw_MyFlags&MYWA_PubScreenFallBack) ? WA_PubScreenFallBack : TAG_IGNORE,TRUE,
				(nw->nw_MyFlags&MYWA_MenuHelp) ? WA_MenuHelp : TAG_IGNORE,TRUE,
				(nw->nw_MyFlags&MYWA_NotifyDepth) ? WA_NotifyDepth : TAG_IGNORE,TRUE,
				(nw->nw_MyFlags&MYWA_PointerDelay) ? WA_PointerDelay : TAG_IGNORE,TRUE,
				(nw->nw_Flags&WFLG_SIZEGADGET) ? WA_MinWidth : TAG_IGNORE,nw->nw_Dimensions[0],
				(nw->nw_Flags&WFLG_SIZEGADGET) ? WA_MinHeight : TAG_IGNORE,nw->nw_Dimensions[1],
				(nw->nw_Flags&WFLG_SIZEGADGET) ? WA_MaxWidth : TAG_IGNORE,nw->nw_Dimensions[2],
				(nw->nw_Flags&WFLG_SIZEGADGET) ? WA_MaxHeight : TAG_IGNORE,nw->nw_Dimensions[3],
				(nw->nw_MyFlags&MYWA_PubScreenFallBack) ? WA_PubScreenFallBack : TAG_IGNORE, TRUE,
				(nw->nw_MyFlags&MYWA_Zoom) ? WA_Zoom : TAG_IGNORE, nw->nw_Zoom,
				WA_ReportMouse,TRUE,
				WA_SmartRefresh,TRUE,
				drag ? WA_Gadgets : TAG_IGNORE, drag,
				TAG_DONE))
			{
				SetFont(nw->nw_Window->RPort,EditFontTF);

				if(gads=CreateAllGadgets(o))
				{
					AddGList(nw->nw_Window,gads,-1,-1,NULL);
					RefreshGList(gads,nw->nw_Window,NULL,-1);
				}

				nw->nw_Window->UserData=(BYTE *)o;
				nw->nw_Window->UserPort=ProjectMsgPort;
				ModifyIDCMP(nw->nw_Window,IDCMP_CLOSEWINDOW|IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS|IDCMP_MENUPICK|IDCMP_CHANGEWINDOW|IDCMP_NEWSIZE|IDCMP_GADGETDOWN|IDCMP_GADGETUP|IDCMP_RAWKEY|IDCMP_MENUVERIFY|IDCMP_ACTIVEWINDOW);
				SetMenuStrip(nw->nw_Window,MainMenus);
				piRenderObject(o);
				return TRUE;
			}
		}
	}
	return FALSE;
}

ULONG CLOSE_Window(piClass *cl,piObject *o,Msg msg)
{
	struct Gadget *gadg=NULL,*drag=NULL;
	struct Node_Window *nw;
	piObject *obj;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	if(GUI.gui_Mode==MODE_TEST)
		for(obj=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(obj);obj=(piObject *)GetSucc(obj))
			piDoMethod(obj,OM_DELETE);
	if(nw->nw_Window)
	{
		ClearMenuStrip(nw->nw_Window);
		gadg=nw->nw_Window->FirstGadget;
		while(gadg)
		{
			if(((ULONG)gadg->UserData)==DRAG_ID) { drag=gadg; break; }
			gadg=gadg->NextGadget;
		}
		if(drag)
		{
			RemoveGadget(nw->nw_Window,drag);
			MyFreeVec(drag);
		}
		CloseWindowSafely(nw->nw_Window); nw->nw_Window=NULL;
	}
	if(nw->nw_Gadgets) FreeGadgets(nw->nw_Gadgets); nw->nw_Gadgets=NULL;
	return TRUE;
}

ULONG SET_Window(piClass *cl,piObject *o,struct piSet *msg)
{
//	struct TagItem *t;
//	struct Node_Window *nw=(struct Node_Window *)OBJECT_DATA(o);

	return TRUE;
}

ULONG GET_Window(piClass *cl,piObject *o,struct piGet *msg)
{
	struct Node_Window *nw=(struct Node_Window *)OBJECT_DATA(o);

	switch(msg->TagID)
	{
		case WNOBJ_GadgetList: *(msg->TagData)=(ULONG)&nw->nw_List_Gadget; return TRUE;
		case WNOBJ_FrameList: *(msg->TagData)=(ULONG)&nw->nw_List_Frame; return TRUE;
		case WNOBJ_RastPort: *(msg->TagData)=(ULONG)nw->nw_Window->RPort; return TRUE;
		case OBJ_Node: *(msg->TagData)=(ULONG)nw; return TRUE;
		case OBJ_Properties: *(msg->TagData)=(ULONG)PAGES_PROPERTIES_PGWN_PAGES; return TRUE;
		case GA_Left: *(msg->TagData)=(ULONG)nw->nw_X; return TRUE;
		case GA_Top: *(msg->TagData)=(ULONG)nw->nw_Y; return TRUE;
		case GA_Width: *(msg->TagData)=(ULONG)nw->nw_Width; return TRUE;
		case GA_Height: *(msg->TagData)=(ULONG)nw->nw_Height; return TRUE;
		default: *(msg->TagData)=NULL; return FALSE;
	}
}

ULONG READ_Window(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0;
	struct Node_Window *nw;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	ReadChunkBytes(msg->IFFh,(APTR)&nw->nw_X,SIZE(Node_Window,nw_X));
	return (ULONG)error;
}

ULONG WRITE_Window(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0;
	struct Node_Window *nw;
	piObject *to;
	
	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	if(error=PushChunk(msg->IFFh,NULL,ID_WIND,SIZE(Node_Window,nw_X))) return (ULONG)error;
	WriteChunkBytes(msg->IFFh,(APTR)&nw->nw_X,SIZE(Node_Window,nw_X));
	if(error=PopChunk(msg->IFFh)) return (ULONG) error;

	if(!IsListEmpty(List(&nw->nw_List_Gadget)))
	{
		for(to=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(to);to=(piObject *)GetSucc(to)) piWriteObject(to,msg->IFFh);
	}
	if(!IsListEmpty(List(&nw->nw_List_Frame)))
	{
		for(to=(piObject *)GetHead(List(&nw->nw_List_Frame));GetSucc(to);to=(piObject *)GetSucc(to)) piWriteObject(to,msg->IFFh);
	}
	return (ULONG)error;
}

ULONG INIT_Window(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Window *nw;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

// General
	if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_INNERWIDTH],nw->nw_MyFlags&MYWA_InnerWidth);
	SetCheckMark(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_INNERHEIGHT],nw->nw_MyFlags&MYWA_InnerHeight);
	SetCycle(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_REL_X],nw->nw_XPos);
	SetCycle(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_REL_Y],nw->nw_YPos);
	SetString(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_LABEL],nw->nw_Label);
	SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_X],nw->nw_X);
	SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_Y],nw->nw_Y);
	SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_WIDTH],nw->nw_Width);
	SetInteger(PropsWnd,PGWN_GeneralGadgets[GD_PGWN_HEIGHT],nw->nw_Height);
	if(nw->nw_XPos==WPOS_CENTER) DisableGadget(PGWN_GeneralGadgets[GD_PGWN_X],PropsWnd,TRUE);
	else DisableGadget(PGWN_GeneralGadgets[GD_PGWN_X],PropsWnd,FALSE);
	if(nw->nw_YPos==WPOS_CENTER) DisableGadget(PGWN_GeneralGadgets[GD_PGWN_Y],PropsWnd,TRUE);
	else DisableGadget(PGWN_GeneralGadgets[GD_PGWN_Y],PropsWnd,FALSE);

// Style
	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCycle(PropsWnd,PGWN_StyleGadgets[GD_PGWN_DRAGGADGET],0);
	GetFlag(nw->nw_Flags,WFLG_DRAGBAR) SetCycle(PropsWnd,PGWN_StyleGadgets[GD_PGWN_DRAGGADGET],1);
	GetFlag(nw->nw_MyFlags,MYWA_DragToolHoriz) SetCycle(PropsWnd,PGWN_StyleGadgets[GD_PGWN_DRAGGADGET],2);
	GetFlag(nw->nw_MyFlags,MYWA_DragToolVert) SetCycle(PropsWnd,PGWN_StyleGadgets[GD_PGWN_DRAGGADGET],3);
	SetCheckMark(PropsWnd,PGWN_StyleGadgets[GD_PGWN_SIZEGADGET],nw->nw_Flags&WFLG_SIZEGADGET);
	SetCheckMark(PropsWnd,PGWN_StyleGadgets[GD_PGWN_CLOSEGADGET],nw->nw_Flags&WFLG_CLOSEGADGET);
	SetCheckMark(PropsWnd,PGWN_StyleGadgets[GD_PGWN_DEPTHGADGET],nw->nw_Flags&WFLG_DEPTHGADGET);
	SetCheckMark(PropsWnd,PGWN_StyleGadgets[GD_PGWN_SIZEBRIGHT],nw->nw_Flags&WFLG_SIZEBRIGHT);
	SetCheckMark(PropsWnd,PGWN_StyleGadgets[GD_PGWN_SIZEBBOTTOM],nw->nw_Flags&WFLG_SIZEBBOTTOM);
	SetCheckMark(PropsWnd,PGWN_StyleGadgets[GD_PGWN_BACKDROP],nw->nw_Flags&WFLG_BACKDROP);
	SetCheckMark(PropsWnd,PGWN_StyleGadgets[GD_PGWN_BORDERLESS],nw->nw_Flags&WFLG_BORDERLESS);

// Flags1
	if(active==2) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PGWN_Flags1Gadgets[GD_PGWN_PROJECT],nw->nw_MyFlags&MYWA_Project);
	SetCheckMark(PropsWnd,PGWN_Flags1Gadgets[GD_PGWN_FONTADAPTIVE],nw->nw_MyFlags&MYWA_FontAdaptive);
	SetString(PropsWnd,PGWN_Flags1Gadgets[GD_PGWN_WINDOW_TITLE],nw->nw_WindowTitle);
	SetString(PropsWnd,PGWN_Flags1Gadgets[GD_PGWN_SCREEN_TITLE],nw->nw_ScreenTitle);
	SetText(PropsWnd,PGWN_Flags1Gadgets[GD_PGWN_MENU],nw->nw_MenuTitle);

// Flags2
	if(active==3) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_GZZ],nw->nw_Flags&WFLG_GIMMEZEROZERO);
	SetCheckMark(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_NEWLOOKMENUS],nw->nw_Flags&WFLG_NEWLOOKMENUS);
	SetCheckMark(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_NOCAREREFRESH],nw->nw_Flags&WFLG_NOCAREREFRESH);
	SetCheckMark(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_REPORTMOUSE],nw->nw_Flags&WFLG_REPORTMOUSE);
	SetCheckMark(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_RMBTRAP],nw->nw_Flags&WFLG_RMBTRAP);
	SetCheckMark(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_ACTIVATE],nw->nw_Flags&WFLG_ACTIVATE);
	SetCheckMark(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_FALLBACK],nw->nw_MyFlags&MYWA_PubScreenFallBack);
	SetCheckMark(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_ZOOM],nw->nw_MyFlags&MYWA_Zoom);
	SetCheckMark(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_AUTOADJUST],nw->nw_MyFlags&MYWA_AutoAdjust);
	GetFlag(nw->nw_Flags,WFLG_SIMPLE_REFRESH) SetCycle(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_REFRESH],0);
	GetFlag(nw->nw_Flags,WFLG_SMART_REFRESH) SetCycle(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_REFRESH],1);
	GetFlag(nw->nw_Flags,WFLG_SUPER_BITMAP) SetCycle(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_REFRESH],2);
//	GetFlag(nw->nw_Flags,WFLG_OTHER_REFRESH) SetCycle(PropsWnd,PGWN_Flags2Gadgets[GD_PGWN_REFRESH],3);

// Flags3
	if(active==4) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PGWN_Flags3Gadgets[GD_PGWN_MENUHELP],nw->nw_MyFlags&MYWA_MenuHelp);
	SetCheckMark(PropsWnd,PGWN_Flags3Gadgets[GD_PGWN_MOUSEQUEUE_SET],nw->nw_MyFlags&MYWA_MouseQueue);
	SetCheckMark(PropsWnd,PGWN_Flags3Gadgets[GD_PGWN_REPEATQUEUE_SET],nw->nw_MyFlags&MYWA_RptQueue);
	SetCheckMark(PropsWnd,PGWN_Flags3Gadgets[GD_PGWN_POINTERDELAY],nw->nw_MyFlags&MYWA_PointerDelay);
	SetCheckMark(PropsWnd,PGWN_Flags3Gadgets[GD_PGWN_NOTIFYDEPTH],nw->nw_MyFlags&MYWA_NotifyDepth);
	SetInteger(PropsWnd,PGWN_Flags3Gadgets[GD_PGWN_MOUSEQUEUE],nw->nw_MouseQueue);
	SetInteger(PropsWnd,PGWN_Flags3Gadgets[GD_PGWN_REPEATQUEUE],nw->nw_RptQueue);

// TabOrder
	if(active==5) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetListView(PropsWnd,PGWN_TabOrderGadgets[GD_PGWN_TABORDER_LIST],&nw->nw_List_Gadget);
	return TRUE;
}

ULONG APPLY_Window(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Window *nw;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);

	strcpy(nw->nw_Label,GetString(PGWN_GeneralGadgets[GD_PGWN_LABEL]));
	RefreshObjectList();
	nw->nw_X=GetInteger(PGWN_GeneralGadgets[GD_PGWN_X]);
	nw->nw_Y=GetInteger(PGWN_GeneralGadgets[GD_PGWN_Y]);
	nw->nw_Width=GetInteger(PGWN_GeneralGadgets[GD_PGWN_WIDTH]);
	nw->nw_Height=GetInteger(PGWN_GeneralGadgets[GD_PGWN_HEIGHT]);

	strcpy(nw->nw_WindowTitle,GetString(PGWN_Flags1Gadgets[GD_PGWN_WINDOW_TITLE]));
	strcpy(nw->nw_ScreenTitle,GetString(PGWN_Flags1Gadgets[GD_PGWN_SCREEN_TITLE]));

	nw->nw_MouseQueue=GetInteger(PGWN_Flags3Gadgets[GD_PGWN_MOUSEQUEUE]);
	nw->nw_RptQueue=GetInteger(PGWN_Flags3Gadgets[GD_PGWN_REPEATQUEUE]);

	return TRUE;
}

/****************************************************************************/

void DeleteAllWindows()
{
	piObject *obj;	

	for(obj=(piObject *)GetHead(&GUI.gui_List_Window);GetSucc(obj);obj=(piObject *)GetSucc(obj)) piCloseWindow(obj);
}

void DisposeAllWindows()
{
	piObject *obj;	

	while(obj=(piObject *)RemTail(List(&GUI.gui_List_Window))) piDisposeObject(obj);
}

void LayoutAllWindows()
{
	piObject *obj;	

	for(obj=(piObject *)GetHead(&GUI.gui_List_Window);GetSucc(obj);obj=(piObject *)GetSucc(obj)) piRenderObject(obj);
}

UWORD WinLeft(piObject *o)
{
	struct Node_Window *nw;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	switch(nw->nw_XPos)
	{
		case WPOS_CENTER: return(UWORD)(Scr->Width/2-WinWidth(o)/2);
		case WPOS_REL_LEFT_TOP: return(UWORD)(nw->nw_X);
		case WPOS_REL_RIGHT_BOTTOM: return(UWORD)(Scr->Width-WinWidth(o)-nw->nw_X);
	}
}

UWORD WinTop(piObject *o)
{
	struct Node_Window *nw;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	switch(nw->nw_YPos)
	{
		case WPOS_CENTER: return(UWORD)(Scr->Height/2-WinHeight(o)/2);
		case WPOS_REL_LEFT_TOP: return(UWORD)(nw->nw_Y);
		case WPOS_REL_RIGHT_BOTTOM: return(UWORD)( Scr->Height-WinHeight(o)-nw->nw_Y);
	}
}

UWORD WinWidth(piObject *o)
{
	struct Node_Window *nw;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	return (UWORD) nw->nw_Width;
//	return(UWORD) ( nw->nw_MyFlags & MYWA_FontAdaptive ? ComputeX(nw->nw_Width) : nw->nw_Width);
}

UWORD WinHeight(piObject *o)
{
	struct Node_Window *nw;

	piGetAttr(o,OBJ_Node,(ULONG *)&nw);
	return (UWORD) nw->nw_Height;
//	return(UWORD)( nw->nw_MyFlags & MYWA_FontAdaptive ? (UWORD)ComputeY(nw->nw_Height) : (UWORD)nw->nw_Height);
}

/****************************************************************************/

int FN_PGWN_INNERWIDTH_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_InnerWidth);

	if(code) nw->nw_Width=nw->nw_Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight;
	else nw->nw_Width=nw->nw_Width+nw->nw_Window->BorderLeft+nw->nw_Window->BorderRight;

	SetInteger(PropertiesWnd,PGWN_GeneralGadgets[GD_PGWN_WIDTH],nw->nw_Width);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_INNERHEIGHT_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_InnerHeight);

	if(code) nw->nw_Height=nw->nw_Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom;
	else nw->nw_Height=nw->nw_Height+nw->nw_Window->BorderTop+nw->nw_Window->BorderBottom;

	SetInteger(PropertiesWnd,PGWN_GeneralGadgets[GD_PGWN_HEIGHT],nw->nw_Height);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_REL_X_Clicked(void)
{
	struct Node_Window *nw;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	nw->nw_XPos=PropertiesMsg.Code;

	if(nw->nw_XPos==WPOS_CENTER) DisableGadget(PGWN_GeneralGadgets[GD_PGWN_X],PropertiesWnd,TRUE);
	else DisableGadget(PGWN_GeneralGadgets[GD_PGWN_X],PropertiesWnd,FALSE);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_REL_Y_Clicked(void)
{
	struct Node_Window *nw;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	nw->nw_YPos=PropertiesMsg.Code;

	if(nw->nw_YPos==WPOS_CENTER) DisableGadget(PGWN_GeneralGadgets[GD_PGWN_Y],PropertiesWnd,TRUE);
	else DisableGadget(PGWN_GeneralGadgets[GD_PGWN_Y],PropertiesWnd,FALSE);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_REL_W_Clicked(void)
{
	struct Node_Window *nw;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_REL_H_Clicked(void)
{
	struct Node_Window *nw;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_LABEL_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_X_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_Y_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_WIDTH_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_HEIGHT_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_SIZEGADGET_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_SIZEGADGET);

	if(code)
	{
		DisableGadget(PGWN_StyleGadgets[GD_PGWN_SIZEBBOTTOM],PropertiesWnd,FALSE);
		DisableGadget(PGWN_StyleGadgets[GD_PGWN_SIZEBRIGHT],PropertiesWnd,FALSE);
	}
	else
	{
		DisableGadget(PGWN_StyleGadgets[GD_PGWN_SIZEBBOTTOM],PropertiesWnd,TRUE);
		DisableGadget(PGWN_StyleGadgets[GD_PGWN_SIZEBRIGHT],PropertiesWnd,TRUE);
	}

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_CLOSEGADGET_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_CLOSEGADGET);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_DEPTHGADGET_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_DEPTHGADGET);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_DRAGGADGET_Clicked(void)
{
	struct Node_Window *nw;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	ClearFlag(nw->nw_Flags,WFLG_DRAGBAR);
	ClearFlag(nw->nw_MyFlags,MYWA_DragToolHoriz);
	ClearFlag(nw->nw_MyFlags,MYWA_DragToolVert);
	switch(PropertiesMsg.Code)
	{
		case 0: break;
		case 1: SetFlag(nw->nw_Flags,WFLG_DRAGBAR); break;
		case 2: SetFlag(nw->nw_MyFlags,MYWA_DragToolHoriz); break;
		case 3: SetFlag(nw->nw_MyFlags,MYWA_DragToolVert); break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_SIZEBRIGHT_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_SIZEBRIGHT);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_SIZEBBOTTOM_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_SIZEBBOTTOM);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_BACKDROP_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_BACKDROP);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_BORDERLESS_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_BORDERLESS);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_PROJECT_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_Project);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_FONTADAPTIVE_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_FontAdaptive);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_WINDOW_TITLE_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_SCREEN_TITLE_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

extern ULONG ListSelected;

int FN_PGWN_MENU_GET_Clicked(void)
{
	piObject *mo;
	int run=1;
	struct Node_Window *nw;

	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	if(!OpenWindow_List())
	{
		SetListView(ListWnd,ListGadgets[GD_LIST],&GUI.gui_List_Menu);
		while(run==1)
		{
			Wait(1L<<ListWnd->UserPort->mp_SigBit);
			run=HandleIDCMP_List();
		}
		CloseWindow_List();
		if(run==2)
		{
			mo=(piObject *)FindNodeNum(List(&GUI.gui_List_Menu),ListSelected);
			strcpy(nw->nw_MenuTitle,mo->ln_Name);
			SetText(PropertiesWnd,PGWN_Flags1Gadgets[GD_PGWN_MENU],nw->nw_MenuTitle);
		}
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_PAGED_Clicked(void)
{
	struct Node_Window *nw;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	if(PropertiesMsg.Code) nw->nw_PageGadget++;
	else nw->nw_PageGadget--;

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_APPWINDOW_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_AppWindow);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_GZZ_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_GIMMEZEROZERO);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_NEWLOOKMENUS_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_NEWLOOKMENUS);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_NOCAREREFRESH_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_NOCAREREFRESH);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_REPORTMOUSE_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_REPORTMOUSE);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_RMBTRAP_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_RMBTrap);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_ACTIVATE_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_Flags,WFLG_ACTIVATE);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_FALLBACK_Clicked(void)
{
	struct Node_Window *nw;
//	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

//	code=PropertiesMsg.Code;
//	DoFlag(nw->nw_Flags,WFLG_FALLBACK);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_ZOOM_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_Zoom);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_AUTOADJUST_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_AutoAdjust);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_REFRESH_Clicked(void)
{
	struct Node_Window *nw;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	ClearFlag(nw->nw_Flags,WFLG_SIMPLE_REFRESH);
	ClearFlag(nw->nw_Flags,WFLG_SMART_REFRESH);
	ClearFlag(nw->nw_Flags,WFLG_SUPER_BITMAP);
//	ClearFlag(nw->nw_Flags,WFLG_OTHER_REFRESH);
	switch(PropertiesMsg.Code)
	{
		case 0: SetFlag(nw->nw_Flags,WFLG_SIMPLE_REFRESH); break;
		case 1: SetFlag(nw->nw_Flags,WFLG_SMART_REFRESH); break;
		case 2: SetFlag(nw->nw_Flags,WFLG_SUPER_BITMAP); break;
//		case 3: SetFlag(nw->nw_Flags,WFLG_OTHER_REFRESH); break;
	}

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_MENUHELP_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_MenuHelp);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_MOUSEQUEUE_SET_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_MouseQueue);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_REPEATQUEUE_SET_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_RptQueue);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_POINTERDELAY_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_PointerDelay);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_NOTIFYDEPTH_Clicked(void)
{
	struct Node_Window *nw;
	ULONG code;
	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);

	code=PropertiesMsg.Code;
	DoFlag(nw->nw_MyFlags,MYWA_NotifyDepth);

	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_MOUSEQUEUE_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_REPEATQUEUE_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

ULONG PGWNListSelectedLabel=-1;

int FN_PGWN_TABORDER_LIST_Clicked( void )
{
	struct Node_Window *nw;
	BOOL du,dd;
	struct piObject *go;
	struct Node_Gadget *ng;

	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);
	PGWNListSelectedLabel=PropertiesMsg.Code;
	if(!GetSucc(GetSucc(FindNodeNum(List(&nw->nw_List_Gadget),PGWNListSelectedLabel))))
		dd=TRUE;
	else
		dd=FALSE;
	if(PGWNListSelectedLabel==0) du=TRUE;
	else du=FALSE;

	if(!du)
	{
		if(go=(piObject *)FindNodeNum(List(&nw->nw_List_Gadget),PGWNListSelectedLabel))
		{
			piGetAttr(go,OBJ_Node,(ULONG *)&ng);
			if(ng->ng_PlugInID==PAGE_ID)
			{
				DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_UP],PropertiesWnd,TRUE);
				DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_DOWN],PropertiesWnd,TRUE);
				return 1;
			}
			if(GetSucc(GetSucc(go)))
			{
				if(go=GetSucc(go))
				{
					piGetAttr(go,OBJ_Node,(ULONG *)&ng);
					if(ng->ng_PlugInID==PAGE_ID)
					{
						DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_UP],PropertiesWnd,du);
						DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_DOWN],PropertiesWnd,TRUE);
						return 1;
					}
				}
			}
		}
	}
	DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_UP],PropertiesWnd,du);
	DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_DOWN],PropertiesWnd,dd);
	return 1;
}

int FN_PGWN_TABORDER_UP_Clicked( void )
{
	struct Node_Window *nw;
	struct piObject *go;
	struct Node_Gadget *ng;

	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);
	if(PGWNListSelectedLabel!=-1)
	{
		if(go=(piObject *)FindNodeNum(List(&nw->nw_List_Gadget),PGWNListSelectedLabel))
		{
			piGetAttr(go,OBJ_Node,(ULONG *)&ng);
			if(ng->ng_PlugInID==PAGE_ID) return 1;
		}
		SetListView(PropertiesWnd,PGWN_TabOrderGadgets[GD_PGWN_TABORDER_LIST],NULL);
		MoveNodeUp(List(&nw->nw_List_Gadget),FindNodeNum(List(&nw->nw_List_Gadget),PGWNListSelectedLabel));
		if(PGWNListSelectedLabel>0) PGWNListSelectedLabel--;
		SetListView(PropertiesWnd,PGWN_TabOrderGadgets[GD_PGWN_TABORDER_LIST],&nw->nw_List_Gadget);
		if(!GetSucc(GetSucc(FindNodeNum(List(&nw->nw_List_Gadget),PGWNListSelectedLabel))))
			DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_DOWN],PropertiesWnd,TRUE);
		else
			DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_DOWN],PropertiesWnd,FALSE);
		if(PGWNListSelectedLabel==0) DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_UP],PropertiesWnd,TRUE);
		else DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_UP],PropertiesWnd,FALSE);
		GT_SetGadgetAttrs(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_LIST],PropertiesWnd,NULL,GTLV_Selected,PGWNListSelectedLabel,GTLV_MakeVisible,PGWNListSelectedLabel,TAG_DONE);
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGWN_TABORDER_DOWN_Clicked( void )
{
	struct Node_Window *nw;
	struct piObject *go;
	struct Node_Gadget *ng;

	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);
	if(PGWNListSelectedLabel!=-1)
	{
		if(go=(piObject *)FindNodeNum(List(&nw->nw_List_Gadget),PGWNListSelectedLabel))
		{
			piGetAttr(go,OBJ_Node,(ULONG *)&ng);
			if(ng->ng_PlugInID==PAGE_ID) return 1;
			if(GetSucc(GetSucc(go)))
				if(go=GetSucc(go))
				{
					piGetAttr(go,OBJ_Node,(ULONG *)&ng);
					if(ng->ng_PlugInID==PAGE_ID) return 1;
				}
		}
		SetListView(PropertiesWnd,PGWN_TabOrderGadgets[GD_PGWN_TABORDER_LIST],NULL);
		MoveNodeDown(List(&nw->nw_List_Gadget),FindNodeNum(List(&nw->nw_List_Gadget),PGWNListSelectedLabel));
		if(FindNodeNum(List(&nw->nw_List_Gadget),PGWNListSelectedLabel+1)) PGWNListSelectedLabel++;
		SetListView(PropertiesWnd,PGWN_TabOrderGadgets[GD_PGWN_TABORDER_LIST],&nw->nw_List_Gadget);
		if(!GetSucc(GetSucc(FindNodeNum(List(&nw->nw_List_Gadget),PGWNListSelectedLabel))))
			DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_DOWN],PropertiesWnd,TRUE);
		else
			DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_DOWN],PropertiesWnd,FALSE);
		if(PGWNListSelectedLabel==0) DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_UP],PropertiesWnd,TRUE);
		else DisableGadget(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_UP],PropertiesWnd,FALSE);
		GT_SetGadgetAttrs(PGWN_TabOrderGadgets[GD_PGWN_TABORDER_LIST],PropertiesWnd,NULL,GTLV_Selected,PGWNListSelectedLabel,GTLV_MakeVisible,PGWNListSelectedLabel,TAG_DONE);
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

ULONG IDCMPs;

int FN_PGWN_IDCMP_Clicked(void)
{
	struct Node_Window *nw;
	int run=1;

	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);
	IDCMPs=nw->nw_IDCMP;
	if(!OpenWindow_WN_IDCMPFlags())
	{
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_MENUVERIFY],IDCMPs&IDCMP_MENUVERIFY);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_SIZEVERIFY],IDCMPs&IDCMP_SIZEVERIFY);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_REQVERIFY],IDCMPs&IDCMP_REQVERIFY);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_REQSET],IDCMPs&IDCMP_REQSET);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_REQCLEAR],IDCMPs&IDCMP_REQCLEAR);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_NEWSIZE],IDCMPs&IDCMP_NEWSIZE);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_NEWPREFS],IDCMPs&IDCMP_NEWPREFS);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_REFRESHWINDOW],IDCMPs&IDCMP_REFRESHWINDOW);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_CHANGEWINDOW],IDCMPs&IDCMP_CHANGEWINDOW);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_CLOSEWINDOW],IDCMPs&IDCMP_CLOSEWINDOW);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_IDCMPUPDATE],IDCMPs&IDCMP_IDCMPUPDATE);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_MENUHELP],IDCMPs&IDCMP_MENUHELP);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_GADGETHELP],IDCMPs&IDCMP_GADGETHELP);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_MOUSEMOVE],IDCMPs&IDCMP_MOUSEMOVE);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_DELTAMOVE],IDCMPs&IDCMP_DELTAMOVE);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_MOUSEBUTTONS],IDCMPs&IDCMP_MOUSEBUTTONS);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_GADGETUP],IDCMPs&IDCMP_GADGETUP);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_GADGETDOWN],IDCMPs&IDCMP_GADGETDOWN);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_RAWKEY],IDCMPs&IDCMP_RAWKEY);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_VANILLAKEY],IDCMPs&IDCMP_VANILLAKEY);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_INTUITICKS],IDCMPs&IDCMP_INTUITICKS);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_ACTIVEWINDOW],IDCMPs&IDCMP_ACTIVEWINDOW);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_INACTIVEWINDOW],IDCMPs&IDCMP_INACTIVEWINDOW);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_DISKREMOVED],IDCMPs&IDCMP_DISKREMOVED);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_DISKINSERTED],IDCMPs&IDCMP_DISKINSERTED);
		SetCheckMark(WN_IDCMPFlagsWnd,WN_IDCMPFlagsGadgets[GD_IDCMP_MENUPICK],IDCMPs&IDCMP_MENUPICK);
		while(run)
		{
			Wait(1L<<WN_IDCMPFlagsWnd->UserPort->mp_SigBit);
			run=HandleIDCMP_WN_IDCMPFlags();
		}
		CloseWindow_WN_IDCMPFlags();
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_IDCMP_OK_Clicked(void)
{
	struct Node_Window *nw;

	piGetAttr(CurrentWindow,OBJ_Node,(ULONG *)&nw);
	nw->nw_IDCMP=IDCMPs;
	return 0;
}

int FN_IDCMP_CANCEL_Clicked(void)
{
	return 0;
}

int FN_IDCMP_MENUVERIFY_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_MENUVERIFY);
	return 1;
}

int FN_IDCMP_SIZEVERIFY_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_SIZEVERIFY);
	return 1;
}

int FN_IDCMP_REQVERIFY_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_REQVERIFY);
	return 1;
}

int FN_IDCMP_REQSET_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_REQSET);
	return 1;
}

int FN_IDCMP_REQCLEAR_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_REQCLEAR);
	return 1;
}

int FN_IDCMP_NEWSIZE_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_NEWSIZE);
	return 1;
}

int FN_IDCMP_NEWPREFS_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_NEWPREFS);
	return 1;
}

int FN_IDCMP_REFRESHWINDOW_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_REFRESHWINDOW);
	return 1;
}

int FN_IDCMP_CHANGEWINDOW_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_CHANGEWINDOW);
	return 1;
}

int FN_IDCMP_CLOSEWINDOW_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_CLOSEWINDOW);
	return 1;
}

int FN_IDCMP_IDCMPUPDATE_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_IDCMPUPDATE);
	return 1;
}

int FN_IDCMP_MENUHELP_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_MENUHELP);
	return 1;
}

int FN_IDCMP_GADGETHELP_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_GADGETHELP);
	return 1;
}

int FN_IDCMP_MOUSEMOVE_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_MOUSEMOVE);
	return 1;
}

int FN_IDCMP_DELTAMOVE_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_DELTAMOVE);
	return 1;
}

int FN_IDCMP_MOUSEBUTTONS_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_MOUSEBUTTONS);
	return 1;
}

int FN_IDCMP_GADGETUP_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_GADGETUP);
	return 1;
}

int FN_IDCMP_GADGETDOWN_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_GADGETDOWN);
	return 1;
}

int FN_IDCMP_RAWKEY_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_RAWKEY);
	return 1;
}

int FN_IDCMP_VANILLAKEY_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_VANILLAKEY);
	return 1;
}

int FN_IDCMP_INTUITICKS_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_INTUITICKS);
	return 1;
}

int FN_IDCMP_ACTIVEWINDOW_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_ACTIVEWINDOW);
	return 1;
}

int FN_IDCMP_INACTIVEWINDOW_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_INACTIVEWINDOW);
	return 1;
}

int FN_IDCMP_DISKREMOVED_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_DISKREMOVED);
	return 1;
}

int FN_IDCMP_DISKINSERTED_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_DISKINSERTED);
	return 1;
}

int FN_IDCMP_MENUPICK_Clicked(void)
{
	ULONG code=WN_IDCMPFlagsMsg.Code;
	DoFlag(IDCMPs,IDCMP_MENUPICK);
	return 1;
}

int FN_CloseWindow_WN_IDCMPFlags(void)
{
	return 0;
}
