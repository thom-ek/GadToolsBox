/*
** Objects handling
*/

#include"includes.h"

piClass *piMakeClass(ULONG Type,piClass *Super,ULONG size)
{
	piClass *cl=NULL;

	if(cl=AllocVec(sizeof(piClass),MEMF_PUBLIC))
	{
		cl->Super=Super;
		cl->Type=Type;
		cl->Size=size;
		if(!Super)
			switch(Type)
			{
				case CLASS_GUI:
//					cl->Dispatcher=Dispatcher_GUI;
					break;
				case CLASS_GADGET:
					cl->Dispatcher=Dispatcher_Gadget;
					break;
				case CLASS_WINDOW:
					cl->Dispatcher=Dispatcher_Window;
					break;
				case CLASS_FRAME:
					cl->Dispatcher=Dispatcher_Frame;
					break;
				case CLASS_MENU:
					cl->Dispatcher=Dispatcher_Menu;
					break;
				case CLASS_IMAGE:
					cl->Dispatcher=Dispatcher_Image;
					break;
				case CLASS_BINARY:
//					cl->Dispatcher=Dispatcher_Binary;
					break;
				case CLASS_VERSION:
					cl->Dispatcher=Dispatcher_Version;
					break;
				case CLASS_STRING:
					cl->Dispatcher=Dispatcher_String;
					break;
				case CLASS_TEXT:
//					cl->Dispatcher=Dispatcher_Text;
					break;
				case CLASS_SCREEN:
					cl->Dispatcher=Dispatcher_Screen;
					break;
			}
	}
	return cl;
}

void piFreeClass(piClass *cl)
{
	MyFreeVec(cl);
}

ULONG piOpenClass(piClass *cl)
{
	ULONG msg;

	msg=OM_OPEN;
	if(cl)
		return cl->Dispatcher(cl,NULL,(Msg)&msg);
	else return FALSE;
}

void piCloseClass(piClass *cl)
{
	ULONG msg;

	msg=OM_CLOSE;
	if(cl)
		cl->Dispatcher(cl,NULL,(Msg)&msg);
}

ULONG piDoMethodA(piObject *o,Msg msg)
{
	if(o) if(o->piClass) return o->piClass->Dispatcher(o->piClass,o,msg);
	else return NULL;
}

ULONG piDoMethod(piObject *o,ULONG MethodID,...)
{
	return piDoMethodA(o,(Msg)&MethodID);
}

ULONG piDoSuperMethodA(piClass *cl,piObject *o,Msg msg)
{
	if(cl) if(cl->Super) return cl->Super->Dispatcher(cl,o,msg);
	else return NULL;
}

ULONG piDoSuperMethod(piClass *cl,piObject *o,ULONG MethodID,...)
{
	return piDoSuperMethodA(cl,o,(Msg) &MethodID);
}

piObject *piNewObject(piClass *cl,ULONG tags,...)
{
	piObject o,*obj;

	if(cl)
	{
		o.piClass=cl;
		obj=(piObject *)piDoMethod(&o,OM_NEW,(ULONG)&tags);
		obj->piClass=cl;
		return obj;
	}
	return NULL;
}

void piDisposeObject(piObject *o)
{
	if(o) piDoMethod(o,OM_DISPOSE);
}

void piResetToDefaults(piObject *o)
{
	piDoMethod(o,OM_DEFAULT);
}

void piInitObjectGUI(piObject *o)
{
	piDoMethod(o,OM_INIT);
}

void piApplyObject(piObject *o)
{
	piDoMethod(o,OM_APPLY);
}

void piRenderObject(piObject *o)
{
	piDoMethod(o,OM_RENDER);
}

void piEraseObject(piObject *o)
{
	piDoMethod(o,OM_ERASE);
}

void piAddObject(piObject *o,piObject *w)
{
	piDoMethod(o,OM_ADDTAIL,(ULONG)w);
}

void piRemoveObject(piObject *o)
{
	piDoMethod(o,OM_REMOVE);
}

void piSetAttrs(piObject *o,ULONG tags, ...)
{
	piDoMethod(o,OM_SET,(ULONG)&tags);
}

ULONG piGetAttr(piObject *o,ULONG tag,ULONG *storage)
{
	return piDoMethod(o,OM_GET,(ULONG)tag,(ULONG)storage);
}

ULONG piOpenWindow(piObject *o)
{
	return piDoMethod(o,OM_CREATE);
}

void piCloseWindow(piObject *o)
{
	piDoMethod(o,OM_DELETE);
}

int piSetupObject(piObject *o)
{
	return (int)piDoMethod(o,OM_SETUP);
}

LONG piReadObject(piObject *o,struct IFFHandle *iffh)
{
	return (LONG) piDoMethod(o,OM_READ,iffh);
}

LONG piWriteObject(piObject *o,struct IFFHandle *iffh)
{
	return (LONG) piDoMethod(o,OM_WRITE,iffh);
}

UWORD piCX(piObject *o)
{
	return (UWORD) piDoMethod(o,OM_COMPUTE,CT_LEFT);
}

UWORD piCY(piObject *o)
{
	return (UWORD) piDoMethod(o,OM_COMPUTE,CT_TOP);
}

UWORD piCW(piObject *o)
{
	return (UWORD) piDoMethod(o,OM_COMPUTE,CT_WIDTH);
}

UWORD piCH(piObject *o)
{
	return (UWORD) piDoMethod(o,OM_COMPUTE,CT_HEIGHT);
}

int piCutObject(piObject *o)
{
	return (int) piDoMethod(o,OM_CUT);
}

int piCopyObject(piObject *o,ULONG where)
{
	return (int) piDoMethod(o,OM_COPY,where);
}

void piPasteObject(piObject *o)
{
	piDoMethod(o,OM_PASTE);
}

/****************************************************************************/

UBYTE CheckObjectClickPosition(piObject *o,UWORD x,UWORD y)
{
	UWORD gx,gy,gw,gh;
	struct Node_Gadget *ng;
	struct Node_Window *nw;

	switch(o->piClass->Type)
	{
		case CLASS_GADGET:
			piGetAttr(o,OBJ_Node,(ULONG *)&ng);
			piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
			gx=piCX(o);
			gy=piCY(o);
//			if(!ng->ng_RelWidth)
 gw=piCW(o);// else gw=nw->nw_Width+ng->ng_Width+gx;
//			if(!ng->ng_RelHeight)
gh=piCH(o);// else gh=nw->nw_Height+ng->ng_Height+gy;
			break;
		default:
			gx=piCX(o);
			gy=piCY(o);
			gw=piCW(o);
			gh=piCH(o);
			break;
	}


	if(x<gx || x>gx+gw || y<gy || y>gy+gh) return CP_OUTSIDE;

	if(x>=gx    && x<=gx+5    && y>=gy    && y<=gy+5   ) return CP_SIZE_LEFTUP;
	if(x<=gx+gw && x>=gx+gw-5 && y>=gy    && y<=gy+5   ) return CP_SIZE_RIGHTUP;
	if(x<=gx+gw && x>=gx+gw-5 && y<=gy+gh && y>=gy+gh-5) return CP_SIZE_RIGHTDOWN;
	if(x>=gx    && x<=gx+5    && y<=gy+gh && y>=gy+gh-5) return CP_SIZE_LEFTDOWN;

	if(x<=gx+(gw)/2+3 && x>=gx+(gw)/2-3 && y>=gy          && y<=gy+5       ) return CP_SIZE_UP;
	if(x<=gx+(gw)/2+3 && x>=gx+(gw)/2-3 && y<=gy+gh       && y>=gy+gh-5    ) return CP_SIZE_DOWN;
	if(x>=gx          && x<=gx+5        && y<=gy+(gh)/2+3 && y>=gy+(gh)/2-3) return CP_SIZE_LEFT;
	if(x<=gx+gw       && x>=gx+gw-5     && y<=gy+(gh)/2+3 && y>=gy+(gh)/2-3) return CP_SIZE_RIGHT;

	return CP_MOVE;
}

int UnSelectAllObjects(piObject *w)
{
	piObject *o;
	struct List *l;
	ULONG sel,s=0;

	piGetAttr(w,WNOBJ_GadgetList,(ULONG *)&l);
	for(o=(piObject *)GetHead(l);GetSucc(o);o=(piObject *)GetSucc(o))
	{
		piGetAttr(o,OBJ_Select,(ULONG *)&sel);
		if(sel)
		{
			s=1;
			piSetAttrs(o,OBJ_Select,FALSE,TAG_DONE);
			piRenderObject(o);
		}
	}
	piGetAttr(w,WNOBJ_FrameList,(ULONG *)&l);
	for(o=(piObject *)GetHead(l);GetSucc(o);o=(piObject *)GetSucc(o))
	{
		piGetAttr(o,OBJ_Select,(ULONG *)&sel);
		if(sel)
		{
			s=1;
			piSetAttrs(o,OBJ_Select,FALSE,TAG_DONE);
			piRenderObject(o);
		}
	}
	return (int)s;
}

void SetObjectDims(piObject *o,UWORD sx,UWORD sy,UWORD ex,UWORD ey)
{
	ULONG OffX,OffY;
	struct Node_Window *nw;
	struct Node_Gadget *ng;
	struct Node_Frame *nf;
	BOOL rw=FALSE,rh=FALSE;

	switch(o->piClass->Type)
	{
		case CLASS_GADGET:
			piGetAttr(o,OBJ_Node,(ULONG *)&ng);
			piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
//			if(ng->ng_RelWidth) rw=TRUE;
//			if(ng->ng_RelHeight) rh=TRUE;
			break;
		case CLASS_FRAME:
			piGetAttr(o,OBJ_Node,(ULONG *)&nf);
			piGetAttr(nf->nf_Window,OBJ_Node,(ULONG *)&nw);
			break;
	}
	if(nw->nw_MyFlags&MYWA_DragToolVert)
		OffX=(8);
	else
		OffX=(Scr->WBorLeft);

	if(nw->nw_MyFlags&MYWA_DragToolVert)
		OffY=(Scr->WBorTop);
	else
	{
		if(nw->nw_MyFlags&MYWA_DragToolHoriz)
			OffY=(8);
		else
			OffY=(Scr->RastPort.TxHeight+Scr->WBorTop+1);
	}

	if(sx<ex)
	{
		piSetAttrs(o,GA_Left,sx-OffX,TAG_DONE);
		if(ex!=-1) piSetAttrs(o,GA_Width,rw?(nw->nw_Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight)-ng->ng_X-(ex-sx):(ex-sx),TAG_DONE);
	}
	else
	{
		piSetAttrs(o,GA_Left,ex-OffX,TAG_DONE);
		if(ex!=-1) piSetAttrs(o,GA_Width,rw?(nw->nw_Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight)-ng->ng_X-(sx-ex):(sx-ex),TAG_DONE);
	}
	if(sy<ey)
	{
		piSetAttrs(o,GA_Top,sy-OffY,TAG_DONE);
		if(ey!=-1) piSetAttrs(o,GA_Height,rh?(nw->nw_Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom)-ng->ng_Y-(ey-sy):(ey-sy),TAG_DONE);
	}
	else
	{
		piSetAttrs(o,GA_Top,ey-OffY,TAG_DONE);
		if(ey!=-1) piSetAttrs(o,GA_Height,rh?(nw->nw_Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom)-ng->ng_Y-(sy-ey):(sy-ey),TAG_DONE);
	}
}

void piShowGrid(piObject *o)
{
	ULONG x,y,xs,ys,xo,yo,wo,ho;
	struct Node_Window *nw;
	piObject *wobj;

	if(GUI.gui_GridShow)
	{
		piGetAttr(o,GA_Left,&xo);
		piGetAttr(o,GA_Top,&yo);
		piGetAttr(o,GA_Width,&wo);
		piGetAttr(o,GA_Height,&ho);
		piGetAttr(o,OBJ_Window,(ULONG *)&wobj);
		piGetAttr(wobj,OBJ_Node,(ULONG *)&nw);
		xs=xo/GUI.gui_Grid[0];
		ys=yo/GUI.gui_Grid[1];

		SetDrMd(nw->nw_Window->RPort,JAM1);
		SetAPen(nw->nw_Window->RPort,DrawInfo->dri_Pens[SHADOWPEN]);
		for(x=xs*GUI.gui_Grid[0];x<xo+wo+1;x+=GUI.gui_Grid[0])
		{
			for(y=ys*GUI.gui_Grid[1];y<yo+ho+1;y+=GUI.gui_Grid[1])
			{
				WritePixel(nw->nw_Window->RPort,nw->nw_Window->BorderLeft+x,nw->nw_Window->BorderTop+y);
			}
		}
	}
}

void SelectObjects(piObject *wo,UWORD x,UWORD y,UWORD w,UWORD h)
{
	struct List *l;
	piObject *o;
	ULONG x1,y1,w1,h1,f=TRUE;
	ULONG OffX,OffY;
	struct Node_Window *nw;

	piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
	if(nw->nw_MyFlags&MYWA_DragToolVert)
		OffX=(8);
	else
		OffX=(Scr->WBorLeft);

	if(nw->nw_MyFlags&MYWA_DragToolVert)
		OffY=(Scr->WBorTop);
	else
	{
		if(nw->nw_MyFlags&MYWA_DragToolHoriz)
			OffY=(8);
		else
			OffY=(Scr->RastPort.TxHeight+Scr->WBorTop+1);
	}

	x-=OffX; y-=OffY;
	w-=OffX; h-=OffY;
	if(x>w) { x1=x; x=w; w=x1; }
	if(y>h) { y1=y; y=h; h=y1; }

	piGetAttr(wo,WNOBJ_GadgetList,(ULONG *)&l);
	for(o=(piObject *)GetHead(l);GetSucc(o);o=(piObject *)GetSucc(o))
	{
		piGetAttr(o,GA_Left,&x1);
		piGetAttr(o,GA_Top,&y1);
		piGetAttr(o,GA_Width,&w1);
		piGetAttr(o,GA_Height,&h1);
		if((x1>=x) && (y1>=y) && (x1+w1<=w) && (y1+h1<=h))
		{
			if(f) CurrentGadget=o;
			f=FALSE;
			piSetAttrs(o,OBJ_Select,TRUE,TAG_DONE);
		}
	}
}

/****************************************************************************/

struct OLVNode
{
	struct LVNode lvnode;
	piObject *obj;
};

struct LVList ObjectList;
struct OLVNode ObjectNode[]=
{
	NULL,NULL,NULL,"Windows",NULL,NULL,0,(piObject *)0,
	NULL,NULL,NULL,"Menus",NULL,NULL,0,(piObject *)1,
	NULL,NULL,NULL,"Images",NULL,NULL,0,(piObject *)2,
	NULL,NULL,NULL,"Strings",NULL,NULL,0,(piObject *)3,
	NULL,NULL,NULL,"Screen",NULL,NULL,0,(piObject *)4,
	NULL,NULL,NULL,"Version",NULL,NULL,0,(piObject *)5
};

LONG ObjTop=0,ObjVisible=0,ObjTotal=0;

#define IM_WIDTH(o) ((struct Image *) o)->Width
#define IM_HEIGHT(o) ((struct Image *) o)->Height
Object *ObjDownButton,*ObjUpButton,*ObjVertProp;

int OBJFN_Up(void)
{
	ObjTop--;
	if(ObjTop<0L) ObjTop=0L;
	SetGadgetAttrs(ObjectGadgets[GD_OBJLIST],ObjectWnd,NULL,LIST_Top,ObjTop,TAG_DONE);
	SetGadgetAttrs((struct Gadget *)ObjVertProp,ObjectWnd,NULL,PGA_Top,ObjTop,TAG_DONE);
	return 1;
}

int OBJFN_Down(void)
{
	ObjTop++;
	if((ObjVisible<ObjTotal) || (ObjTop>(ObjTotal-ObjVisible))) ObjTop=ObjTotal-ObjVisible;
	SetGadgetAttrs(ObjectGadgets[GD_OBJLIST],ObjectWnd,NULL,LIST_Top,ObjTop,TAG_DONE);
	SetGadgetAttrs((struct Gadget *)ObjVertProp,ObjectWnd,NULL,PGA_Top,ObjTop,TAG_DONE);
	return 1;
}

int OBJFN_Vert(void)
{
	GetAttr(PGA_Top,ObjVertProp,(ULONG *)&ObjTop);
	SetGadgetAttrs(ObjectGadgets[GD_OBJLIST],ObjectWnd,NULL,LIST_Top,ObjTop,TAG_DONE);
	return 1;
}

int OpenObjectList()
{
	if(!ObjectWnd)
	{
		ObjectNode[0].lvnode.ln_Name=GetCatString(MSG_MainObjWindow_MITEM)+2;
		ObjectNode[1].lvnode.ln_Name=GetCatString(MSG_MainObjMenu_MITEM)+2;
		ObjectNode[2].lvnode.ln_Name=GetCatString(MSG_MainObjImage_MITEM)+2;
		ObjectNode[3].lvnode.ln_Name=GetCatString(MSG_MainObjStrings_MITEM)+2;
		ObjectNode[4].lvnode.ln_Name=GetCatString(MSG_MainObjScreen_MITEM)+2;
		ObjectNode[5].lvnode.ln_Name=GetCatString(MSG_MainObjVersion_MITEM)+2;
		unless(ObjDownButton=NewObject(NULL, BUTTONGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			GA_Image, gui_DownImage,
			GA_RelRight, 1-(IM_WIDTH(gui_DownImage)),
			GA_RelBottom, 1-(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_DownImage)),
			GA_RightBorder,TRUE,
			GA_UserData,OBJFN_Down, 
			GA_ID, 2,
			TAG_DONE)) return 1;
		unless(ObjUpButton=NewObject(NULL, BUTTONGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			GA_Image, gui_UpImage,
			GA_RelRight, 1-(IM_WIDTH(gui_UpImage)),
			GA_RelBottom, 1-(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_DownImage)+IM_HEIGHT(gui_UpImage)),
			GA_RightBorder,TRUE,
			GA_Previous,ObjDownButton,
			GA_UserData, OBJFN_Up,
			GA_ID, 3,
			TAG_DONE)) return 1;
		unless(ObjVertProp=NewObject(NULL, PROPGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			PGA_Freedom, FREEVERT,
			PGA_NewLook, TRUE,
			PGA_Borderless, ((DrawInfo->dri_Flags & DRIF_NEWLOOK) && DrawInfo->dri_Depth != 1),
			GA_RelRight, -(IM_WIDTH(gui_SizeImage)-5),
			GA_Top, Scr->WBorTop+Scr->Font->ta_YSize+1+1,
			GA_Width, IM_WIDTH(gui_SizeImage)-2-2-4,
			GA_RelHeight, -(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_UpImage)+IM_HEIGHT(gui_DownImage)+Scr->WBorTop+Scr->Font->ta_YSize+1+1+1),
			GA_RightBorder, TRUE,
			GA_Previous,ObjUpButton,
			GA_UserData, OBJFN_Vert,
			GA_ID, 4,
			PGA_Top, ObjTop,
			PGA_Total, ObjTotal,
			PGA_Visible, ObjVisible,
			TAG_DONE)) return 1;

		OpenWindow_Object();
		Tree_NewList(&ObjectList);
		UpdateObjectList();

		GetAttr(LIST_Visible,ObjectGadgets[GD_OBJLIST],(ULONG *)&ObjVisible);
		GetAttr(LIST_Top,ObjectGadgets[GD_OBJLIST],(ULONG *)&ObjTop);
		SetGadgetAttrs((struct Gadget *)ObjVertProp,ObjectWnd,NULL,
			PGA_Top, ObjTop,
			PGA_Total, ObjTotal,
			PGA_Visible, ObjVisible,
			TAG_DONE);
			
		AddGList(ObjectWnd,(struct Gadget *)ObjDownButton,-1,-1,NULL);
		RefreshGList((struct Gadget *)ObjDownButton,ObjectWnd,NULL,-1);
		SetWndSignals();
		MenuCheck(MainMenus,FULLMENUNUM(3,1,0),TRUE);
		SetFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_OBJECTSOPEN);
	}
	else
	{
		ActivateWindow(ObjectWnd);
		WindowToFront(ObjectWnd);
		UpdateObjectList();
	}
	return NULL;
}

void CloseObjectList()
{
	struct LVNode *ln,*sln;

	if(ObjectWnd)
	{
		while(ln=Tree_RemTail(&ObjectList))
			if(ln->ln_Sub) while(sln=Tree_RemSubTail(ln)) if(sln->ln_Parent) MyFreeVec(sln);
		Tree_NewList(&ObjectList);
		RemoveGList(ObjectWnd,(struct Gadget *)ObjDownButton,-1);
		if(ObjVertProp) DisposeObject(ObjVertProp);
		if(ObjDownButton) DisposeObject(ObjDownButton);
		if(ObjUpButton) DisposeObject(ObjUpButton);
		CloseWindow_Object();
		SetWndSignals();
		MenuCheck(MainMenus,FULLMENUNUM(3,1,0),FALSE);
		ClearFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_OBJECTSOPEN);
	}
}

void RefreshObjectList()
{
	if(ObjectWnd) SetGadgetAttrs(ObjectGadgets[GD_OBJLIST],ObjectWnd,NULL,LIST_Labels,&ObjectList,TAG_DONE);
}

void UpdateObjectList()
{
	piObject *o;
	struct LVNode *ln,*sln;

	if(ObjectWnd)
	{
		while(ln=Tree_RemTail(&ObjectList))
			if(ln->ln_Sub) while(sln=Tree_RemSubTail(ln)) if(sln->ln_Parent) MyFreeVec(sln);

		Tree_NewList(&ObjectList);
		Tree_AddTail(&ObjectList,&ObjectNode[0]);
		for(o=(piObject *)GetHead(List(&GUI.gui_List_Window));GetSucc(o);o=(piObject *)GetSucc(o))
		{
			if(ln=MyAllocVec(sizeof(struct LVNode)+4,MEMF_ANY|MEMF_CLEAR))
			{
				ln->ln_Name=o->ln_Name;
				((struct OLVNode *)ln)->obj=o;
				Tree_AddSubTail(&ObjectList,&ObjectNode[0],ln);
			}
		}
		Tree_AddTail(&ObjectList,&ObjectNode[1]);
		for(o=(piObject *)GetHead(List(&GUI.gui_List_Menu));GetSucc(o);o=(piObject *)GetSucc(o))
		{
			if(ln=MyAllocVec(sizeof(struct LVNode)+4,MEMF_ANY|MEMF_CLEAR))
			{
				ln->ln_Name=o->ln_Name;
				((struct OLVNode *)ln)->obj=o;
				Tree_AddSubTail(&ObjectList,&ObjectNode[1],ln);
			}
		}
		Tree_AddTail(&ObjectList,&ObjectNode[2]);
		for(o=(piObject *)GetHead(List(&GUI.gui_List_Image));GetSucc(o);o=(piObject *)GetSucc(o))
		{
			if(ln=MyAllocVec(sizeof(struct LVNode)+4,MEMF_ANY|MEMF_CLEAR))
			{
				ln->ln_Name=o->ln_Name;
				((struct OLVNode *)ln)->obj=o;
				Tree_AddSubTail(&ObjectList,&ObjectNode[2],ln);
			}
		}
		Tree_AddTail(&ObjectList,&ObjectNode[3]);
		Tree_AddTail(&ObjectList,&ObjectNode[4]);
		Tree_AddTail(&ObjectList,&ObjectNode[5]);
		SetGadgetAttrs(ObjectGadgets[GD_OBJLIST],ObjectWnd,NULL,LIST_Labels,&ObjectList,TAG_DONE);

		for(ln=ObjectList.lh_Head,ObjTotal=0;Tree_NextNode(ln);ln=Tree_NextNode(ln),ObjTotal++);
		GetAttr(LIST_Visible,ObjectGadgets[GD_OBJLIST],(ULONG *)&ObjVisible);
		GetAttr(LIST_Top,ObjectGadgets[GD_OBJLIST],(ULONG *)&ObjTop);
		SetGadgetAttrs((struct Gadget *)ObjVertProp,ObjectWnd,NULL,
			PGA_Top, ObjTop,
			PGA_Total, ObjTotal,
			PGA_Visible, ObjVisible,
			TAG_DONE);
	}
}

int FN_MouseMove_Object()
{
	return 1;
}

int FN_IDCMPUpdate_Object()
{
	switch(GetTagData(GA_ID,0,(struct TagItem *)ObjectMsg.IAddress))
	{
		case 2: OBJFN_Down(); break;
		case 3: OBJFN_Up(); break;
		case 4: OBJFN_Vert(); break;
		default: FN_OBJLIST_Clicked(); break;
	}
	return 1;
}

int FN_CloseWindow_Object()
{
	CloseObjectList();
	return 2;
}

int FN_NewSize_Object(void)
{
	struct LVNode *ln;

	for(ln=ObjectList.lh_Head,ObjTotal=0;Tree_NextNode(ln);ln=Tree_NextNode(ln),ObjTotal++);
	GetAttr(LIST_Visible,ObjectGadgets[GD_OBJLIST],(ULONG *)&ObjVisible);
	GetAttr(LIST_Top,ObjectGadgets[GD_OBJLIST],(ULONG *)&ObjTop);
	SetGadgetAttrs((struct Gadget *)ObjVertProp,ObjectWnd,NULL,
		PGA_Top, ObjTop,
		PGA_Total, ObjTotal,
		PGA_Visible, ObjVisible,
		TAG_DONE);
	GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].X=ObjectWnd->LeftEdge;
	GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].Y=ObjectWnd->TopEdge;
	GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].X=ObjectWnd->Width;
	GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].Y=ObjectWnd->Height;
	FindTagItem(WA_Left,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].X;
	FindTagItem(WA_Top,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].Y;
	FindTagItem(WA_Width,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].X;
	FindTagItem(WA_Height,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].Y;
	return 1;
}

int FN_ChangeWindow_Object(void)
{
	GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].X=ObjectWnd->LeftEdge;
	GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].Y=ObjectWnd->TopEdge;
	GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].X=ObjectWnd->Width;
	GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].Y=ObjectWnd->Height;
	FindTagItem(WA_Left,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].X;
	FindTagItem(WA_Top,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].Y;
	FindTagItem(WA_Width,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].X;
	FindTagItem(WA_Height,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].Y;
	return 1;
}

ULONG OSSec=0,OSMic=0,OESec=0,OEMic=0;
struct LVNode *prev=NULL;

int FN_OBJLIST_Clicked()
{
	struct LVNode *lvn;
	piObject *o,*po;

	CurrentTime(&OESec,&OEMic);
	GetAttr(LIST_Selected,ObjectGadgets[GD_OBJLIST],(ULONG *)&lvn);
	FN_NewSize_Object();
	if(DoubleClick(OSSec,OSMic,OESec,OEMic) && lvn==prev)
	{
		if(lvn->ln_Parent)
		{
			po=(((struct OLVNode *)lvn->ln_Parent)->obj);
			o=(((struct OLVNode *)lvn)->obj);
		}
		else
		{
			po=(((struct OLVNode *)lvn)->obj);
			o=NULL;
		}
			
		switch((ULONG)po)
		{
			case 0:
				if(o)
				{
					ActiveObject=ID_WIND;
					CurrentWindow=o;
					CurrentWindowNode=(struct Node_Window *)OBJECT_DATA(CurrentWindow);
					piOpenWindow(o);
					FN_ActiveWindow_Edit();
				}
				break;
			case 1:
				if(o)
				{
					ActiveObject=ID_MENU;
					CurrentMenu=o;
					piOpenWindow(o);
					FN_ActiveWindow_EditMenu();
				}
				break;
			case 2:
				if(o)
				{
					ActiveObject=ID_IMGE;
					CurrentImage=o;
					piOpenWindow(o);
					FN_ActiveWindow_EditImage();
				}
				break;
			case 3:
				FN_MainObjStringsOpen_Selected();
				break;
			case 4:
				FN_MainObjScreenOpen_Selected();
				break;
			case 5:
				FN_MainObjVersionOpen_Selected();
				break;
		}
		CurrentTime(&OSSec,&OSMic);
		return 1;
	}
	else
	{
		if(lvn->ln_Parent)
		{
			po=(((struct OLVNode *)lvn->ln_Parent)->obj);
			o=(((struct OLVNode *)lvn)->obj);
		}
		else
		{
			po=(((struct OLVNode *)lvn)->obj);
			o=NULL;
		}
		ActiveObject=NULL;
		switch((ULONG)po)
		{
			case 0:
				if(o)
				{
					ActiveObject=ID_WIND;
					CurrentWindow=o;
					CurrentWindowNode=(struct Node_Window *)OBJECT_DATA(CurrentWindow);
				}
				break;
			case 1:
				if(o)
				{
					ActiveObject=ID_MENU;
					CurrentMenu=o;
				}
				break;
			case 2:
				if(o)
				{
					ActiveObject=ID_IMGE;
					CurrentImage=o;
				}
				break;
			case 3:
				FN_MainObjStringsClose_Selected();
				break;
			case 4:
				FN_MainObjScreenClose_Selected();
				break;
			case 5:
				FN_MainObjVersionClose_Selected();
				break;
		}
		RefreshProperties();
	}
	prev=lvn;
	CurrentTime(&OSSec,&OSMic);
	return 1;
}

int FN_RawKey_Object()
{
	struct LVNode *lvn;
	piObject *o,*po;

	GetAttr(LIST_Selected,ObjectGadgets[GD_OBJLIST],(ULONG *)&lvn);
	switch(ObjectMsg.Code)
	{
		case 0x5f:	// Help
			if(AmigaGuideObj) gtbSendAmigaGuideCmd(AmigaGuideObj,"ALINK main");
			break;
		case 0x46:	// Del
			if(lvn->ln_Parent)
			{
				po=(((struct OLVNode *)lvn->ln_Parent)->obj);
				o=(((struct OLVNode *)lvn)->obj);
			}
			else
			{
				po=(((struct OLVNode *)lvn)->obj);
				o=NULL;
			}
			switch((ULONG)po)
			{
				case 0:
					if(o)
					{
						CurrentWindow=o;
						CurrentWindowNode=(struct Node_Window *)OBJECT_DATA(CurrentWindow);
						FN_MainObjWindowDelete_Selected();
					}
					break;
				case 1:
					if(o)
					{
						CurrentMenu=o;
						FN_MainObjMenuDelete_Selected();
					}
					break;
				case 2:
					if(o)
					{
						CurrentImage=o;
						FN_MainObjImageDelete_Selected();
					}
					break;
				case 3:
					FN_MainObjStringsClose_Selected();
					break;
				case 4:
					FN_MainObjScreenClose_Selected();
					break;
				case 5:
					FN_MainObjVersionClose_Selected();
					break;
			}
			ActiveObject=NULL;
			break;
	}
	UpdateObjectList();
	RefreshProperties();
	return 1;
}
