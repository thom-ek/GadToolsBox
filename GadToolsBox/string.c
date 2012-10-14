/*
** GadToolsBox v3 - String Management
*/

#include"includes.h"

/*
** Defines
*/

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGSTR_General[];

struct Page PAGES_PROPERTIES_PGSTR_PAGES[]=
{
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

LONG Top=0,Visible=0,Total=0;

#define IM_WIDTH(o) ((struct Image *) o)->Width
#define IM_HEIGHT(o) ((struct Image *) o)->Height
Object *DownButton,*UpButton,*VertProp;

/*
** Functions
*/

ULONG Dispatcher_String(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval=NULL;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=INIT_String(cl,o,msg);
			break;
		case OM_APPLY:
			retval=APPLY_String(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PGSTR_General();
			PAGES_PROPERTIES_PGSTR_PAGES[0].FirstGadget=PGSTR_GeneralGList;
			PAGES_PROPERTIES_PGSTR_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L39_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PGSTR_PAGES[0].FirstGadget=NULL;
			CloseWindow_PGSTR_General();
			break;
		case OM_NEW:
			retval=NEW_String(cl,o,(struct piSet *)msg);
			break;
		case OM_GET:
			retval=GET_String(cl,o,(struct piGet *)msg);
			break;
		case OM_DISPOSE:
			retval=DISPOSE_String(cl,o,(struct piSet *)msg);
			break;
		case OM_CREATE:
			retval=CREATE_String(cl,o,msg);
			break;
		case OM_DELETE:
			retval=DELETE_String(cl,o,msg);
			break;
		case OM_READ:
			retval=READ_String(cl,o,(struct piIO *)msg);
			break;
		case OM_WRITE:
			retval=WRITE_String(cl,o,(struct piIO *)msg);
			break;
		case OM_CUT:
			retval=CUT_String(cl,o,msg);
			break;
		case OM_COPY:
			retval=COPY_String(cl,o,(struct piCopy *)msg);
			break;
	}
	return retval;
}

ULONG CUT_String(piClass *cl,piObject *o,Msg msg)
{
	struct Node_String *ns;

	piCopyObject(o,COPY_CLIP);
	piCopyObject(o,COPY_UNDO);

	piGetAttr(o,OBJ_Node,(ULONG *)&ns);
	if(ns->ns_Selected)
	{
		Tree_Remove(ns->ns_Selected);
		if(ns->ns_Selected->ns_Label) MyFreeVec(ns->ns_Selected->ns_Label);
		if(ns->ns_Selected->ns_String) MyFreeVec(ns->ns_Selected->ns_String);
		if(ns->ns_Selected->ns_GadString) MyFreeVec(ns->ns_Selected->ns_GadString);
		MyFreeVec(ns->ns_Selected);
		ns->ns_Selected=NULL;
		Total--;
		SetGadgetAttrs((struct Gadget *)VertProp,EditStringWnd,NULL,
			PGA_Top, Top,
			PGA_Total, Total,
			PGA_Visible, Visible,
			TAG_DONE);
		RefreshGList(EditStringGadgets[GD_PGSTR_STRINGS],EditStringWnd,NULL,1);
	
		PropertiesMode=PM_NONE;
		RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
	}
	return TRUE;
}

ULONG COPY_String(piClass *cl,piObject *o,struct piCopy *msg)
{
	struct IFFHandle *iffh;
	LONG error=0,len;
	struct Node_String *ns;
	struct Node_StringData *nd;

	piGetAttr(o,OBJ_Node,(ULONG *)&ns);
	if(!(ClipHandle=OpenClipboard(PRIMARY_CLIP))) return 1;
	if(nd=ns->ns_Selected)
	{
		if(iffh=AllocIFF())
		{
			iffh->iff_Stream=(ULONG)((msg->Copy==COPY_CLIP)?ClipHandle:UndoHandle);
			InitIFFasClip(iffh);
			if(!OpenIFF(iffh,IFFF_WRITE))
			{
				if(!PushChunk(iffh,ID_GTBX,ID_FORM,IFFSIZE_UNKNOWN))
				{
					if(error=PushChunk(iffh,NULL,ID_LSTR,strlen(nd->ns_Label)+strlen(nd->ns_String)+10)) goto skip;
					len=strlen(nd->ns_Label)+1;
					WriteChunkBytes(iffh,(APTR)&len,sizeof(LONG));
					WriteChunkBytes(iffh,(APTR)nd->ns_Label,len);
					len=strlen(nd->ns_String)+1;
					WriteChunkBytes(iffh,(APTR)&len,sizeof(LONG));
					WriteChunkBytes(iffh,(APTR)nd->ns_String,len);
					if(error=PopChunk(iffh)) return (ULONG) error;
					if(error=PopChunk(iffh)) return (ULONG) error;
				}
skip:		CloseIFF(iffh);
			}
			FreeIFF(iffh);
		}
	}
	if(ClipHandle) CloseClipboard(ClipHandle); ClipHandle=NULL;
	return TRUE;
}

ULONG NEW_String(piClass *cl,piObject *o,struct piSet *msg)
{
	piObject *obj;
	struct Node_String *ns;

	if(obj=MyAllocVec(sizeof(piObject)+cl->Size,MEMF_PUBLIC|MEMF_CLEAR))
	{
		ns=(struct Node_String *)OBJECT_DATA(obj);
		ns->ns_Pages=PAGES_PROPERTIES_PGSTR_PAGES,
		obj->ln_Name="Locale strings";
		Tree_NewList(&ns->ns_Strings);
	}

	Total=0;

	ns->ns_Selected=NULL;
	return (ULONG) obj;
}

ULONG DISPOSE_String(piClass *cl,piObject *o,Msg msg)
{
	struct Node_String *ns;
	struct Node_StringData *nd;

	piDoMethod(o,OM_DELETE);
	piGetAttr(o,OBJ_Node,(ULONG *)&ns);
	while(nd=(struct Node_StringData *)Tree_RemTail(&ns->ns_Strings))
	{
		if(nd->ns_Label) MyFreeVec(nd->ns_Label);
		if(nd->ns_String) MyFreeVec(nd->ns_String);
		if(nd->ns_GadString) MyFreeVec(nd->ns_GadString);
		MyFreeVec(nd);
	}
	MyFreeVec(o);
	return TRUE;
}

int FN_Up(void)
{
	Top--;
	if(Top<0L) Top=0L;
	SetGadgetAttrs(EditStringGadgets[GD_PGSTR_STRINGS],EditStringWnd,NULL,LIST_Top,Top,TAG_DONE);
	SetGadgetAttrs((struct Gadget *)VertProp,EditStringWnd,NULL,PGA_Top,Top,TAG_DONE);
	return 1;
}

int FN_Down(void)
{
	Top++;
	if(Visible<Total) if(Top>Total-Visible) Top=Total-Visible;
	SetGadgetAttrs(EditStringGadgets[GD_PGSTR_STRINGS],EditStringWnd,NULL,LIST_Top,Top,TAG_DONE);
	SetGadgetAttrs((struct Gadget *)VertProp,EditStringWnd,NULL,PGA_Top,Top,TAG_DONE);
	return 1;
}

int FN_Vert(void)
{
	GetAttr(PGA_Top,VertProp,(ULONG *)&Top);
	SetGadgetAttrs(EditStringGadgets[GD_PGSTR_STRINGS],EditStringWnd,NULL,LIST_Top,Top,TAG_DONE);
	return 1;
}

ULONG CREATE_String(piClass *cl,piObject *o,Msg msg)
{
	struct Node_String *ns;

	piGetAttr(o,OBJ_Node,(ULONG *)&ns);
	if(!EditStringWnd)
	{
		ns->ns_Selected=NULL;
		unless(DownButton=NewObject(NULL, BUTTONGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			GA_Image, gui_DownImage,
			GA_RelRight, 1-(IM_WIDTH(gui_DownImage)),
			GA_RelBottom, 1-(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_DownImage)),
			GA_RightBorder,TRUE,
			GA_UserData,FN_Down, 
			GA_ID, 2,
			TAG_DONE)) return 1;
		unless(UpButton=NewObject(NULL, BUTTONGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			GA_Image, gui_UpImage,
			GA_RelRight, 1-(IM_WIDTH(gui_UpImage)),
			GA_RelBottom, 1-(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_DownImage)+IM_HEIGHT(gui_UpImage)),
			GA_RightBorder,TRUE,
			GA_Previous,DownButton,
			GA_UserData, FN_Up,
			GA_ID, 3,
			TAG_DONE)) return 1;
		unless(VertProp=NewObject(NULL, PROPGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			PGA_Freedom, FREEVERT,
			PGA_NewLook, TRUE,
			PGA_Borderless, ((DrawInfo->dri_Flags & DRIF_NEWLOOK) && DrawInfo->dri_Depth != 1),
			GA_RelRight, -(IM_WIDTH(gui_SizeImage)-5),
			GA_Top, Scr->WBorTop+Scr->Font->ta_YSize+1+1,
			GA_Width, IM_WIDTH(gui_SizeImage)-2-2-4,
			GA_RelHeight, -(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_UpImage)+IM_HEIGHT(gui_DownImage)+Scr->WBorTop+Scr->Font->ta_YSize+1+1+1),
			GA_RightBorder, TRUE,
			GA_Previous,UpButton,
			GA_UserData, FN_Vert,
			GA_ID, 4,
			PGA_Top, Top,
			PGA_Total, Total,
			PGA_Visible, Visible,
			TAG_DONE)) return 1;

		OpenWindow_EditString();
		SetGadgetAttrs(EditStringGadgets[GD_PGSTR_STRINGS],EditStringWnd,NULL,LIST_Labels,&ns->ns_Strings,TAG_DONE);

		GetAttr(LIST_Visible,EditStringGadgets[GD_PGSTR_STRINGS],(ULONG *)&Visible);
		GetAttr(LIST_Top,EditStringGadgets[GD_PGSTR_STRINGS],(ULONG *)&Top);
		SetGadgetAttrs((struct Gadget *)VertProp,EditStringWnd,NULL,
			PGA_Top, Top,
			PGA_Total, Total,
			PGA_Visible, Visible,
			TAG_DONE);
			
		AddGList(EditStringWnd,(struct Gadget *)DownButton,-1,-1,NULL);
		RefreshGList((struct Gadget *)DownButton,EditStringWnd,NULL,-1);
		piInitObjectGUI(o);
		SetWndSignals();
	}
	else
		ActivateWindow(EditStringWnd);
	return TRUE;
}

ULONG DELETE_String(piClass *cl,piObject *o,Msg msg)
{
	if(EditStringWnd)
	{
		piApplyObject(o);
		RemoveGList(EditStringWnd,(struct Gadget *)DownButton,-1);
		DisposeObject(VertProp);
		DisposeObject(DownButton);
		DisposeObject(UpButton);
		CloseWindow_EditString();
		SetWndSignals();
	}
	return TRUE;
}

ULONG GET_String(piClass *cl,piObject *o,struct piGet *msg)
{
	struct Node_String *ns=(struct Node_String *)OBJECT_DATA(o);

	switch(msg->TagID)
	{
		case OBJ_Node: *(msg->TagData)=(ULONG)ns; return TRUE;
		case OBJ_Properties: *(msg->TagData)=(ULONG)ns->ns_Pages; return TRUE;
		default: *(msg->TagData)=NULL; return FALSE;
	}
}

ULONG READ_String(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0,len;
	struct Node_String *ns;
	struct Node_StringData *nd;

	piGetAttr(o,OBJ_Node,(ULONG *)&ns);
	
	if(nd=(struct Node_StringData *)MyAllocVec(sizeof(struct Node_StringData),MEMF_ANY|MEMF_CLEAR))
	{
		ReadChunkBytes(msg->IFFh,&len,sizeof(LONG));
		if(nd->ns_Label=MyAllocVec(len,MEMF_ANY))
			ReadChunkBytes(msg->IFFh,nd->ns_Label,len);

		ReadChunkBytes(msg->IFFh,&len,sizeof(LONG));
		if(nd->ns_String=MyAllocVec(len,MEMF_ANY))
			ReadChunkBytes(msg->IFFh,nd->ns_String,len);

		if(nd->ns_GadString=MyAllocVec(strlen(nd->ns_String)+strlen(nd->ns_Label)+strlen(GetCatString(MSG_EDITSTR_STRING))+1,MEMF_ANY))
			sprintf(nd->ns_GadString,GetCatString(MSG_EDITSTR_STRING),nd->ns_Label,nd->ns_String);
		nd->ns_Node.ln_Name=nd->ns_GadString;
		Tree_AddTail(&ns->ns_Strings,(struct LVNode *)nd);
		Total++;
	}
	return (ULONG)error;
}

ULONG WRITE_String(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0,len;
	struct Node_String *ns;
	struct Node_StringData *nd;
	
	piGetAttr(o,OBJ_Node,(ULONG *)&ns);
	if(!Tree_IsListEmpty(&ns->ns_Strings))
	{
		for(nd=(struct Node_StringData *)GetHead(&ns->ns_Strings);GetSucc(Node(nd));nd=(struct Node_StringData *)GetSucc(Node(nd)))
		{
			if(error=PushChunk(msg->IFFh,NULL,ID_LSTR,strlen(nd->ns_Label)+strlen(nd->ns_String)+10)) return (ULONG)error;
			len=strlen(nd->ns_Label)+1;
			WriteChunkBytes(msg->IFFh,(APTR)&len,sizeof(LONG));
			WriteChunkBytes(msg->IFFh,(APTR)nd->ns_Label,len);
			len=strlen(nd->ns_String)+1;
			WriteChunkBytes(msg->IFFh,(APTR)&len,sizeof(LONG));
			WriteChunkBytes(msg->IFFh,(APTR)nd->ns_String,len);
			if(error=PopChunk(msg->IFFh)) return (ULONG) error;
		}
	}
	return (ULONG)error;
}

ULONG INIT_String(piClass *cl,piObject *o,Msg msg)
{
	struct Node_String *ns;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ns);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

// General
	if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	if(ns->ns_Selected)
	{
		SetString(PropsWnd,PGSTR_GeneralGadgets[GD_PGSTR_LABEL],ns->ns_Selected->ns_Label);
		SetString(PropsWnd,PGSTR_GeneralGadgets[GD_PGSTR_STRING],ns->ns_Selected->ns_String);
	}
	return TRUE;
}

ULONG APPLY_String(piClass *cl,piObject *o,Msg msg)
{
	struct Node_String *ns;
	struct Node_StringData *nd;

	piGetAttr(o,OBJ_Node,(ULONG *)&ns);

	nd=ns->ns_Selected;
	if(nd)
	{
		if(nd->ns_Label) MyFreeVec(nd->ns_Label); nd->ns_Label=NULL;
		if(nd->ns_Label=MyAllocVec(strlen(GetString(PGSTR_GeneralGadgets[GD_PGSTR_LABEL]))+1,MEMF_PUBLIC))
			strcpy(nd->ns_Label,GetString(PGSTR_GeneralGadgets[GD_PGSTR_LABEL]));
		if(nd->ns_String) MyFreeVec(nd->ns_String); nd->ns_String=NULL;
		if(nd->ns_String=MyAllocVec(strlen(GetString(PGSTR_GeneralGadgets[GD_PGSTR_STRING]))+1,MEMF_PUBLIC))
			strcpy(nd->ns_String,GetString(PGSTR_GeneralGadgets[GD_PGSTR_STRING]));
		if(nd->ns_GadString) MyFreeVec(nd->ns_GadString); nd->ns_GadString=NULL;
		if(nd->ns_GadString=MyAllocVec(strlen(nd->ns_String)+strlen(nd->ns_Label)+strlen(GetCatString(MSG_EDITSTR_STRING))+1,MEMF_ANY))
			sprintf(nd->ns_GadString,GetCatString(MSG_EDITSTR_STRING),nd->ns_Label,nd->ns_String);
		nd->ns_Node.ln_Name=nd->ns_GadString;
	}
	if(EditStringWnd) RefreshGList(EditStringGadgets[GD_PGSTR_STRINGS],EditStringWnd,NULL,1);
	return TRUE;
}

/******************************************************************************/

int FN_PGSTR_STRINGS_Clicked(void)
{
	struct Node_String *ns;

	piGetAttr(CurrentString,OBJ_Node,(ULONG *)&ns);

	GetAttr(LIST_Selected,EditStringGadgets[GD_PGSTR_STRINGS],(ULONG *)&ns->ns_Selected);
	if(PropertiesMode!=PM_STRING)
	{
		PropertiesMode=PM_STRING;
		if(!PropertiesWnd) OpenWindow_Props();
	}
	if(PropertiesWnd) piInitObjectGUI(CurrentString);
	return 1;
}

int FN_PGSTR_ADD_Clicked(void)
{
	struct Node_String *ns;
	struct Node_StringData *nd;

	piGetAttr(CurrentString,OBJ_Node,(ULONG *)&ns);

	if(nd=(struct Node_StringData *)MyAllocVec(sizeof(struct Node_StringData),MEMF_ANY|MEMF_CLEAR))
	{
		if(nd->ns_Label=MyAllocVec(strlen(GetCatString(MSG_UNNAMED))+1,MEMF_ANY)) strcpy(nd->ns_Label,GetCatString(MSG_UNNAMED));
		if(nd->ns_String=MyAllocVec(strlen(GetCatString(MSG_EMPTY))+1,MEMF_ANY)) strcpy(nd->ns_String,GetCatString(MSG_EMPTY));
		if(nd->ns_GadString=MyAllocVec(strlen(nd->ns_String)+strlen(nd->ns_Label)+strlen(GetCatString(MSG_EDITSTR_STRING))+1,MEMF_ANY))
			sprintf(nd->ns_GadString,GetCatString(MSG_EDITSTR_STRING),nd->ns_Label,nd->ns_String);
		nd->ns_Node.ln_Name=nd->ns_GadString;
		ns->ns_Selected=nd;
		Tree_AddTail(&ns->ns_Strings,(struct LVNode *)nd);
		SetGadgetAttrs(EditStringGadgets[GD_PGSTR_STRINGS],EditStringWnd,NULL,LIST_Selected,(ULONG)nd,TAG_DONE);
		Total++;
		SetGadgetAttrs((struct Gadget *)VertProp,EditStringWnd,NULL,
			PGA_Top, Top,
			PGA_Total, Total,
			PGA_Visible, Visible,
			TAG_DONE);
		RefreshGList(EditStringGadgets[GD_PGSTR_STRINGS],EditStringWnd,NULL,1);
		if(PropertiesWnd) piInitObjectGUI(CurrentString);
	}
	return 1;
}

int FN_PGSTR_DELETE_Clicked(void)
{
	piCutObject(CurrentString);
	return 1;
}

int FN_PGSTR_EDIT_Clicked(void)
{
	PropertiesMode=PM_STRING;
	OpenWindow_Props();
	piInitObjectGUI(CurrentString);
	return 1;
}

int FN_PGSTR_LABEL_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGSTR_STRING_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_CloseWindow_EditString(void)
{
	piCloseWindow(CurrentString);
	PropertiesMode=PM_NONE;
	RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
	return 2;
}

int FN_ChangeWindow_EditString(void)
{
	GetAttr(LIST_Visible,EditStringGadgets[GD_PGSTR_STRINGS],(ULONG *)&Visible);
	GetAttr(LIST_Top,EditStringGadgets[GD_PGSTR_STRINGS],(ULONG *)&Top);
	SetGadgetAttrs((struct Gadget *)VertProp,EditStringWnd,NULL,
		PGA_Top, Top,
		PGA_Total, Total,
		PGA_Visible, Visible,
		TAG_DONE);
	return 1;
}

int FN_IDCMPUpdate_EditString(void)
{
	switch(GetTagData(GA_ID,0,(struct TagItem *)EditStringMsg.IAddress))
	{
		case 2: FN_Down(); break;
		case 3: FN_Up(); break;
		case 4: FN_Vert(); break;
		default: FN_PGSTR_STRINGS_Clicked(); break;
	}
	return 1;
}

int FN_MouseButtons_EditString(void)
{
	switch(EditStringMsg.Code)
	{
		case SELECTUP:
			break;
	}
	return 1;
}

int FN_ActiveWindow_EditString( void )
{
	ActiveObject=ID_LSTR;
	PropertiesMode=PM_STRING;
	RefreshPropertiesWindow(GetCatString(MSG_STRINGOBJECT));
	if(TB_WindowWnd) CloseWindow_TB_Window();
	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	return 1;
}
