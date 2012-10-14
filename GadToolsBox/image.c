/*
** GadToolsBox v3 - Image Managment
*/

#include"includes.h"
#include<datatypes/pictureclass.h>

/*
** Defines
*/

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGPI_General[];

struct Page PAGES_PROPERTIES_PGPI_PAGES[]=
{
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

#define IM_WIDTH(o) ((struct Image *) o)->Width
#define IM_HEIGHT(o) ((struct Image *) o)->Height
#define Bit32(x) ((ULONG)(x|(x<<8)|(x<<16)|(x<<24)))
#define Bit8(x) ((UBYTE)((x&0xff000000)>>24))

/*
** Functions
*/

ULONG Dispatcher_Image(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_NEW:
			retval=NEW_Image(cl,o,(struct piSet *)msg);
			break;
		case OM_INIT:
			retval=INIT_Image(cl,o,msg);
			break;
		case OM_APPLY:
			retval=APPLY_Image(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PGPI_General();
			PAGES_PROPERTIES_PGPI_PAGES[0].FirstGadget=PGPI_GeneralGList;
			PAGES_PROPERTIES_PGPI_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L41_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PGPI_PAGES[0].FirstGadget=NULL;
			CloseWindow_PGPI_General();
			break;
		case OM_CREATE:
			retval=CREATE_Image(cl,o,(struct piSet *)msg);
			break;
		case OM_DELETE:
			retval=DELETE_Image(cl,o,(struct piSet *)msg);
			break;
		case OM_SET:
			retval=SET_Image(cl,o,(struct piSet *)msg);
			break;
		case OM_GET:
			retval=GET_Image(cl,o,(struct piGet *)msg);
			break;
		case OM_DISPOSE:
			retval=DISPOSE_Image(cl,o,msg);
			break;
		case OM_RENDER:
			retval=RENDER_Image(cl,o,msg);
			break;
		case OM_ADDTAIL:
			AddTail(List(&GUI.gui_List_Image),Node(o));
			break;
		case OM_REMOVE:
			Remove(Node(o));
			break;
		case OM_READ:
			retval=READ_Image(cl,o,(struct piIO *)msg);
			break;
		case OM_WRITE:
			retval=WRITE_Image(cl,o,(struct piIO *)msg);
			break;
		case OM_CUT:
			retval=CUT_Image(cl,o,msg);
			break;
		case OM_COPY:
			retval=COPY_Image(cl,o,(struct piCopy *)msg);
			break;
	}
	return retval;
}

ULONG CUT_Image(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Image *ni;

	piGetAttr(o,OBJ_Node,(ULONG *)&ni);

	piCopyObject(o,COPY_CLIP);
	piCopyObject(o,COPY_UNDO);

	piRemoveObject(o);
	piDisposeObject(o);

	return TRUE;
}

ULONG COPY_Image(piClass *cl,piObject *o,struct piCopy *msg)
{
	struct IFFHandle *iffh;
	ULONG error=0;

	if(!(ClipHandle=OpenClipboard(PRIMARY_CLIP))) return 1;
	if(iffh=AllocIFF())
	{
		iffh->iff_Stream=(ULONG)((msg->Copy==COPY_CLIP)?ClipHandle:UndoHandle);
		InitIFFasClip(iffh);
		if(!OpenIFF(iffh,IFFF_WRITE))
		{
			if(!PushChunk(iffh,ID_GTBX,ID_FORM,IFFSIZE_UNKNOWN))
			{
				piWriteObject(o,iffh);
				if(error=PopChunk(iffh)) goto skip;;
			}
skip:	CloseIFF(iffh);
		}
		FreeIFF(iffh);
	}
	if(ClipHandle) CloseClipboard(ClipHandle); ClipHandle=NULL;
	return TRUE;
}

ULONG INIT_Image(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Image *ni;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ni);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetString(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_LABEL],ni->ni_Label);
	SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_WIDTH],ni->ni_Width);
	SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_HEIGHT],ni->ni_Height);
	SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_DEPTH],ni->ni_Depth);
	SetCycle(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_TYPE],ni->ni_Type);
	switch(ni->ni_Type)
	{
		case IMTYPE_NORMAL:
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_WIDTH],PropsWnd,FALSE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_HEIGHT],PropsWnd,FALSE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_DEPTH],PropsWnd,FALSE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_PALETTE],PropsWnd,TRUE); // FALSE
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_SETPOINT],PropsWnd,TRUE);
			break;
		case IMTYPE_TOOLBAR:
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_WIDTH],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_HEIGHT],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_DEPTH],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_PALETTE],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_SETPOINT],PropsWnd,TRUE);
			break;
		case IMTYPE_POINTER:
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_WIDTH],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_HEIGHT],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_DEPTH],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_PALETTE],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_SETPOINT],PropsWnd,FALSE);
			break;
	}
	return TRUE;
}

ULONG APPLY_Image(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Image *ni;
	struct Window *PropsWnd=NULL;
	struct BitMap *newbm;
	ULONG active=0,v,t;
	UWORD ow,oh,od;

	piGetAttr(o,OBJ_Node,(ULONG *)&ni);
	strcpy(ni->ni_Label,GetString(PGPI_GeneralGadgets[GD_PGPI_LABEL]));

	ow=ni->ni_Width;
	oh=ni->ni_Height;
	od=ni->ni_Depth;

	ni->ni_Width=GetInteger(PGPI_GeneralGadgets[GD_PGPI_WIDTH]);
	ni->ni_Height=GetInteger(PGPI_GeneralGadgets[GD_PGPI_HEIGHT]);
	ni->ni_Depth=GetInteger(PGPI_GeneralGadgets[GD_PGPI_DEPTH]);

	if(ni->ni_Width!=ow || ni->ni_Height!=oh || ni->ni_Depth!=od)
	{
		if(newbm=AllocBitMap(ni->ni_Width,ni->ni_Height,ni->ni_Depth,BMF_CLEAR,NULL))
		{
			if(ni->ni_BitMap)
			{
				BltBitMap(ni->ni_BitMap,0,0,newbm,0,0,(ni->ni_Width<ow)?ni->ni_Width:ow,(ni->ni_Height<oh)?ni->ni_Height:oh,0xc0,0xff,NULL);
				FreeBitMap(ni->ni_BitMap);
			}
			ni->ni_BitMap=newbm;
			SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_BitMap,ni->ni_BitMap,TAG_DONE);

			GetAttr(DRAW_VisibleX,ni->ni_Draw,(ULONG *)&v);
			GetAttr(DRAW_Left,ni->ni_Draw,(ULONG *)&t);
			SetGadgetAttrs((struct Gadget *)ni->ni_HorizProp,ni->ni_Window,NULL, PGA_Top, t, PGA_Visible, v, TAG_DONE);

			GetAttr(DRAW_VisibleY,ni->ni_Draw,(ULONG *)&v);
			GetAttr(DRAW_Top,ni->ni_Draw,(ULONG *)&t);
			SetGadgetAttrs((struct Gadget *)ni->ni_VertProp,ni->ni_Window,NULL, PGA_Top, t, PGA_Visible, v, TAG_DONE);
		}
		else
		{
			ni->ni_Width=ow;
			ni->ni_Height=oh;
			ni->ni_Depth=od;
			GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);
			if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
			SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_WIDTH],ni->ni_Width);
			SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_HEIGHT],ni->ni_Height);
			SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_DEPTH],ni->ni_Depth);
		}
	}

	RefreshObjectList();
	if(ni->ni_Window) RefreshWindowFrame(ni->ni_Window);
	return TRUE;
}

ULONG RENDER_Image(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Image *ni;

	piGetAttr(o,OBJ_Node,(ULONG *)&ni);
	return TRUE;
}

ULONG NEW_Image(piClass *cl,piObject *o,struct piSet *msg)
{
	piObject *obj;
	struct Node_Image *ni;

	if(!CheckRegistration())
		if(!ListEmpty(&GUI.gui_List_Image)) return NULL;
	if(obj=MyAllocVec(sizeof(piObject)+cl->Size,MEMF_PUBLIC|MEMF_CLEAR))
	{
		ni=(struct Node_Image *)OBJECT_DATA(obj);
		ni->ni_ID=ID_IMGE;
		ni->ni_Pages=PAGES_PROPERTIES_PGPI_PAGES;
		unless(ni->ni_BitMap=AllocBitMap(32,32,2,BMF_CLEAR,NULL)) return NULL;
		ni->ni_Width=32;
		ni->ni_Height=32;
		ni->ni_Depth=2;
		ni->ni_Type=IMTYPE_NORMAL;
		ni->ni_CT=(struct ColorPalette *)MyAllocVec(sizeof(struct ColorPalette)*256,MEMF_ANY|MEMF_CLEAR);
		ni->ni_Pens=(UBYTE *)MyAllocVec(sizeof(UBYTE)*256,MEMF_ANY|MEMF_CLEAR);
		obj->ln_Name=ni->ni_Label;
		return (ULONG) obj;
	}
	return NULL;
}

ULONG DISPOSE_Image(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Image *ni;

	piGetAttr(o,OBJ_Node,(ULONG *)&ni);
	if(ni->ni_Pens) MyFreeVec(ni->ni_Pens);
	if(ni->ni_CT) MyFreeVec(ni->ni_CT);
	if(ni->ni_BitMap) FreeBitMap(ni->ni_BitMap);
	piDoMethod(o,OM_DELETE);
	MyFreeVec(o);
	return TRUE;
}

int FN_DrawUp_Image(void)
{
	struct Node_Image *ni;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	ni->ni_Top--;
	if(ni->ni_Top<0L) ni->ni_Top=0L;
	SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_Top,ni->ni_Top,TAG_DONE);
	SetGadgetAttrs((struct Gadget *)ni->ni_VertProp,ni->ni_Window,NULL,PGA_Top,ni->ni_Top,TAG_DONE);
	return 1;
}

int FN_DrawDown_Image(void)
{
	struct Node_Image *ni;
	ULONG v;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	GetAttr(DRAW_VisibleY,ni->ni_Draw,(ULONG *)&v);
	ni->ni_Top++;
	if(ni->ni_Top>(32-v)) ni->ni_Top=32-v;
	SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_Top,ni->ni_Top,TAG_DONE);
	SetGadgetAttrs((struct Gadget *)ni->ni_VertProp,ni->ni_Window,NULL,PGA_Top,ni->ni_Top,TAG_DONE);
	return 1;
}

int FN_DrawVert_Image(void)
{
	struct Node_Image *ni;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	GetAttr(PGA_Top,ni->ni_VertProp,(ULONG *)&ni->ni_Top);
	SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_Top,ni->ni_Top,TAG_DONE);
	return 1;
}

int FN_DrawLeft_Image(void)
{
	struct Node_Image *ni;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	ni->ni_Left--;
	if(ni->ni_Left<0L) ni->ni_Left=0L;
	SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_Left,ni->ni_Left,TAG_DONE);
	SetGadgetAttrs((struct Gadget *)ni->ni_HorizProp,ni->ni_Window,NULL,PGA_Top,ni->ni_Left,TAG_DONE);
	return 1;
}

int FN_DrawRight_Image(void)
{
	struct Node_Image *ni;
	ULONG v;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	GetAttr(DRAW_VisibleX,ni->ni_Draw,(ULONG *)&v);
	ni->ni_Left++;
	if(ni->ni_Left>(32-v)) ni->ni_Left=32-v;
	SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_Left,ni->ni_Left,TAG_DONE);
	SetGadgetAttrs((struct Gadget *)ni->ni_HorizProp,ni->ni_Window,NULL,PGA_Top,ni->ni_Left,TAG_DONE);
	return 1;
}

int FN_DrawHoriz_Image(void)
{
	struct Node_Image *ni;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	GetAttr(PGA_Top,ni->ni_HorizProp,(ULONG *)&ni->ni_Left);
	SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_Left,ni->ni_Left,TAG_DONE);
	return 1;
}

int FN_DrawDraw_Image(void)
{
	struct Node_Image *ni;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
//	RefreshGList((struct Gadget *)ni->ni_Preview,ni->ni_Window,NULL,1);
	return 1;
}

struct ColorPalette PointerPens[]=
{
	{ 0,0,0,0},	// Transparent
	{17,0,0,0},	// Black
	{18,0,0,0},	// Red
	{19,0,0,0}	// Pink
};

struct ColorPalette ToolbarPens[]=
{
	{ 0,189,189,189},		// Background
	{ 0,  0,  0,  0},		// Black
	{ 0,255,255,255},		// White
	{ 0,123,  0,  0},		// Red 50%
	{ 0,  0,123,  0},		// Green 50%
	{ 0,  0,  0,123},		// Blue 50%
	{ 0,123,123,  0},		// Yellow 50%
	{ 0,  0,123,123},		// Cyan 50%
	{ 0,123,  0,123},		// Magenta 50%
	{ 0,255,  0,  0},		// Red 100%
	{ 0,  0,255,  0},		// Green 100%
	{ 0,  0,  0,255},		// Blue 100%
	{ 0,255,255,  0},		// Yellow 100%
	{ 0,  0,255,255},		// Cyan 100%
	{ 0,255,  0,255},		// Magenta 100%
	{ 0,123,123,123}		// Dark Gray
};

ULONG CREATE_Image(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Image *ni;
	ULONG v,t,x,z;

	piGetAttr(o,OBJ_Node,(ULONG *)&ni);
	if(!ni->ni_Window)
	{
		switch(ni->ni_Type)
		{
			case IMTYPE_NORMAL:
				for(x=0;x<(2<<(ni->ni_Depth-1));x++)
					ni->ni_Pens[x]=ni->ni_CT[x].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ni->ni_CT[x].cp_R),Bit32(ni->ni_CT[x].cp_G),Bit32(ni->ni_CT[x].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				break;
			case IMTYPE_POINTER:
				GetRGB32(Scr->ViewPort.ColorMap,PointerPens[0].cp_I,1,&ni->ni_CT[0].cp_R); ni->ni_Pens[0]=ni->ni_CT[0].cp_I=PointerPens[0].cp_I;
				GetRGB32(Scr->ViewPort.ColorMap,PointerPens[1].cp_I,1,&ni->ni_CT[1].cp_R); ni->ni_Pens[1]=ni->ni_CT[1].cp_I=PointerPens[1].cp_I;
				GetRGB32(Scr->ViewPort.ColorMap,PointerPens[2].cp_I,1,&ni->ni_CT[2].cp_R); ni->ni_Pens[2]=ni->ni_CT[2].cp_I=PointerPens[2].cp_I;
				GetRGB32(Scr->ViewPort.ColorMap,PointerPens[3].cp_I,1,&ni->ni_CT[3].cp_R); ni->ni_Pens[3]=ni->ni_CT[3].cp_I=PointerPens[3].cp_I;
				break;
			case IMTYPE_TOOLBAR:
				ni->ni_Pens[0]=ni->ni_CT[0].cp_I=DrawInfo->dri_Pens[BACKGROUNDPEN];
				ni->ni_Pens[1]=ni->ni_CT[1].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[1].cp_R),Bit32(ToolbarPens[1].cp_G),Bit32(ToolbarPens[1].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[2]=ni->ni_CT[2].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[2].cp_R),Bit32(ToolbarPens[2].cp_G),Bit32(ToolbarPens[2].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[3]=ni->ni_CT[3].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[3].cp_R),Bit32(ToolbarPens[3].cp_G),Bit32(ToolbarPens[3].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[4]=ni->ni_CT[4].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[4].cp_R),Bit32(ToolbarPens[4].cp_G),Bit32(ToolbarPens[4].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[5]=ni->ni_CT[5].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[5].cp_R),Bit32(ToolbarPens[5].cp_G),Bit32(ToolbarPens[5].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[6]=ni->ni_CT[6].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[6].cp_R),Bit32(ToolbarPens[6].cp_G),Bit32(ToolbarPens[6].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[7]=ni->ni_CT[7].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[7].cp_R),Bit32(ToolbarPens[7].cp_G),Bit32(ToolbarPens[7].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[8]=ni->ni_CT[8].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[8].cp_R),Bit32(ToolbarPens[8].cp_G),Bit32(ToolbarPens[8].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[9]=ni->ni_CT[9].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[9].cp_R),Bit32(ToolbarPens[9].cp_G),Bit32(ToolbarPens[9].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[10]=ni->ni_CT[10].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[10].cp_R),Bit32(ToolbarPens[10].cp_G),Bit32(ToolbarPens[10].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[11]=ni->ni_CT[11].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[11].cp_R),Bit32(ToolbarPens[11].cp_G),Bit32(ToolbarPens[11].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[12]=ni->ni_CT[12].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[12].cp_R),Bit32(ToolbarPens[12].cp_G),Bit32(ToolbarPens[12].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[13]=ni->ni_CT[13].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[13].cp_R),Bit32(ToolbarPens[13].cp_G),Bit32(ToolbarPens[13].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[14]=ni->ni_CT[14].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[14].cp_R),Bit32(ToolbarPens[14].cp_G),Bit32(ToolbarPens[14].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				ni->ni_Pens[15]=ni->ni_CT[15].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[15].cp_R),Bit32(ToolbarPens[15].cp_G),Bit32(ToolbarPens[15].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
				break;
		}
		unless(ni->ni_DownButton=NewObject(NULL, BUTTONGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			GA_Image, gui_DownImage,
			GA_RelRight, 1-(IM_WIDTH(gui_DownImage)),
			GA_RelBottom, 1-(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_DownImage)),
			GA_RightBorder,TRUE,
			GA_UserData,FN_DrawDown_Image, 
			GA_ID, 2,
			TAG_DONE)) return 1;
		unless(ni->ni_UpButton=NewObject(NULL, BUTTONGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			GA_Image, gui_UpImage,
			GA_RelRight, 1-(IM_WIDTH(gui_UpImage)),
			GA_RelBottom, 1-(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_DownImage)+IM_HEIGHT(gui_UpImage)),
			GA_RightBorder,TRUE,
			GA_Previous,ni->ni_DownButton,
			GA_UserData, FN_DrawUp_Image,
			GA_ID, 3,
			TAG_DONE)) return 1;
		unless(ni->ni_VertProp=NewObject(NULL, PROPGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			PGA_Freedom, FREEVERT,
			PGA_NewLook, TRUE,
			PGA_Borderless, ((DrawInfo->dri_Flags & DRIF_NEWLOOK) && DrawInfo->dri_Depth != 1),
			GA_RelRight, -(IM_WIDTH(gui_SizeImage)-5),
			GA_Top, Scr->WBorTop+Scr->Font->ta_YSize+1+1,
			GA_Width, IM_WIDTH(gui_SizeImage)-2-2-4,
			GA_RelHeight, -(IM_HEIGHT(gui_SizeImage)+IM_HEIGHT(gui_UpImage)+IM_HEIGHT(gui_DownImage)+Scr->WBorTop+Scr->Font->ta_YSize+1+1+1),
			GA_RightBorder, TRUE,
			GA_Previous,ni->ni_UpButton,
			GA_UserData, FN_DrawVert_Image,
			GA_ID, 4,
			PGA_Top, 0,
			PGA_Total, 1,
			PGA_Visible, 1,
			TAG_DONE)) return 1;
		unless(ni->ni_LeftButton=NewObject(NULL, BUTTONGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			GA_Image, gui_LeftImage,
			GA_RelRight, 1-(IM_WIDTH(gui_LeftImage)+IM_WIDTH(gui_SizeImage)+IM_WIDTH(gui_RightImage)),
			GA_RelBottom, 1-(IM_HEIGHT(gui_LeftImage)),
			GA_BottomBorder,TRUE,
			GA_Previous,ni->ni_VertProp,
			GA_UserData, FN_DrawLeft_Image,
			GA_ID, 5,
			TAG_DONE)) return 1;
		unless(ni->ni_RightButton=NewObject(NULL, BUTTONGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			GA_Image, gui_RightImage,
			GA_RelRight, 1-(IM_WIDTH(gui_RightImage)+IM_WIDTH(gui_SizeImage)),
			GA_RelBottom, 1-(IM_HEIGHT(gui_RightImage)),
			GA_BottomBorder,TRUE,
			GA_Previous,ni->ni_LeftButton,
			GA_UserData, FN_DrawRight_Image,
			GA_ID, 6,
			TAG_DONE)) return 1;
		unless(ni->ni_HorizProp=NewObject(NULL, PROPGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			PGA_Freedom, FREEHORIZ,
			PGA_NewLook, TRUE,
			PGA_Borderless, ((DrawInfo->dri_Flags & DRIF_NEWLOOK) && DrawInfo->dri_Depth != 1),
			GA_Left, 3,
			GA_RelBottom, -(IM_HEIGHT(gui_SizeImage)-3),
			GA_RelWidth, -(4+IM_WIDTH(gui_LeftImage)+IM_WIDTH(gui_RightImage)+IM_WIDTH(gui_SizeImage)+1),
			GA_Height, IM_HEIGHT(gui_SizeImage)-4,
			GA_BottomBorder, TRUE,
			GA_Previous,ni->ni_RightButton,
			GA_UserData, FN_DrawHoriz_Image,
			GA_ID, 7,
			PGA_Top, 0,
			PGA_Total, 1,
			PGA_Visible, 1,
			TAG_DONE)) return 1;

		if(!(ni->ni_Window=OpenWindowTags(NULL,
				WA_Title, ni->ni_Label,
				WA_CustomScreen, Scr,
				TAG_MORE, EditImageWTags))) return GTBERR_OPENWINDOW;

		SetWindowPointer(ni->ni_Window,WA_Pointer,DrawPtr,TAG_DONE);

		GetFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_SIZEZOOM)
		{
			z=min(ni->ni_Window->Width-ni->ni_Window->BorderLeft-ni->ni_Window->BorderRight,ni->ni_Window->Height-ni->ni_Window->BorderTop-ni->ni_Window->BorderBottom);
			if(z==ni->ni_Window->Width-ni->ni_Window->BorderLeft-ni->ni_Window->BorderRight)
				z=z/ni->ni_Width;
			else
				z=z/ni->ni_Height;
		}
		else z=7;

		ni->ni_Draw=NewObject(DrawClass,NULL,
				ICA_TARGET, ICTARGET_IDCMP,
				GA_Left,ni->ni_Window->BorderLeft,
				GA_Top,ni->ni_Window->BorderTop,
				GA_RelWidth,-ni->ni_Window->BorderLeft-ni->ni_Window->BorderRight,
				GA_RelHeight,-ni->ni_Window->BorderTop-ni->ni_Window->BorderBottom,
				GA_Previous, ni->ni_HorizProp,
				GA_ID,8,
				GA_RelVerify,TRUE,
				GA_Immediate,TRUE,
				GA_UserData, FN_DrawDraw_Image,
				GA_FollowMouse,TRUE,
				DRAW_Zoom,z<2?1:z-(GUI.gui_GridShow?1:0),
				DRAW_Grid,z<2?FALSE:GUI.gui_GridShow,
				DRAW_BitMap,ni->ni_BitMap,
				DRAW_Palette,(ULONG)ni->ni_CT,
				DRAW_Width,ni->ni_Width,
				DRAW_Border,TRUE,
				DRAW_PointX,ni->ni_PointX,
				DRAW_PointY,ni->ni_PointY,
				DRAW_ShowPoint,(ni->ni_Type==IMTYPE_POINTER)?TRUE:FALSE,
				TAG_DONE);

/*		ni->ni_Preview=NewObject(DrawClass,NULL,
				GA_RelRight,-ni->ni_Window->BorderRight-33-4,
				GA_Top,ni->ni_Window->BorderTop+4,
				GA_Width,34,
				GA_Height,34,
				GA_Previous, ni->ni_Draw,
				GA_ID,9,
				DRAW_Zoom,1,
				DRAW_Grid,FALSE,
				DRAW_BitMap,ni->ni_BitMap,
				DRAW_Palette,(ULONG)ni->ni_CT,
				DRAW_Border,TRUE,
				DRAW_ReadOnly,TRUE,
				TAG_DONE);
*/

		AddGList(ni->ni_Window,(struct Gadget *)ni->ni_DownButton,-1,-1,NULL);
		RefreshGList((struct Gadget *)ni->ni_DownButton,ni->ni_Window,NULL,-1);

		GetAttr(DRAW_VisibleX,ni->ni_Draw,(ULONG *)&v);
		GetAttr(DRAW_Left,ni->ni_Draw,(ULONG *)&t);
		SetGadgetAttrs((struct Gadget *)ni->ni_HorizProp,ni->ni_Window,NULL, PGA_Total, ni->ni_Width, PGA_Top, t, PGA_Visible, v, TAG_DONE);

		GetAttr(DRAW_VisibleY,ni->ni_Draw,(ULONG *)&v);
		GetAttr(DRAW_Top,ni->ni_Draw,(ULONG *)&t);
		SetGadgetAttrs((struct Gadget *)ni->ni_VertProp,ni->ni_Window,NULL, PGA_Total, ni->ni_Height, PGA_Top, t, PGA_Visible, v, TAG_DONE);

		ni->ni_Window->UserPort=ProjectMsgPort;
		ni->ni_Window->UserData=(BYTE *)o;
		ModifyIDCMP(ni->ni_Window,IDCMP_IDCMPUPDATE|IDCMP_CLOSEWINDOW|IDCMP_MOUSEMOVE|IDCMP_GADGETUP|IDCMP_GADGETDOWN|IDCMP_MOUSEBUTTONS|IDCMP_MENUPICK|IDCMP_NEWSIZE|IDCMP_ACTIVEWINDOW|IDCMP_MENUVERIFY);
		SetMenuStrip(ni->ni_Window,MainMenus);
		piRenderObject(o);
	}
	else
		ActivateWindow(ni->ni_Window);
	return GTBERR_OK;
}

ULONG DELETE_Image(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Image *ni;
	ULONG x;

	piGetAttr(o,OBJ_Node,(ULONG *)&ni);

	if(ni->ni_Window)
	{
		switch(ni->ni_Type)
		{
			case IMTYPE_NORMAL:
				for(x=0;x<(2<<(ni->ni_Depth-1));x++)
					ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[x].cp_I);
				break;
			case IMTYPE_TOOLBAR:
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[1].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[2].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[3].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[4].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[5].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[6].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[7].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[8].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[9].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[10].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[11].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[12].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[13].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[14].cp_I);
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[15].cp_I);
				break;
		}
		ClearMenuStrip(ni->ni_Window);
		if(ni->ni_Draw)
		{
			GetAttr(DRAW_PointX,ni->ni_Draw,&x); ni->ni_PointX=(UWORD)x;
			GetAttr(DRAW_PointY,ni->ni_Draw,&x); ni->ni_PointY=(UWORD)x;
			RemoveGList(ni->ni_Window,(struct Gadget *)ni->ni_DownButton,-1);
			if(ni->ni_UpButton) DisposeObject(ni->ni_UpButton);
			if(ni->ni_DownButton) DisposeObject(ni->ni_DownButton);
			if(ni->ni_VertProp) DisposeObject(ni->ni_VertProp);
			if(ni->ni_LeftButton) DisposeObject(ni->ni_LeftButton);
			if(ni->ni_RightButton) DisposeObject(ni->ni_RightButton);
			if(ni->ni_HorizProp) DisposeObject(ni->ni_HorizProp);
			if(ni->ni_Draw) DisposeObject(ni->ni_Draw);
//			if(ni->ni_Preview) DisposeObject(ni->ni_Preview);
		}
		if(ni->ni_Window) CloseWindowSafely(ni->ni_Window); ni->ni_Window=NULL;
	}
	return TRUE;
}

ULONG SET_Image(piClass *cl,piObject *o,struct piSet *msg)
{
//	struct TagItem *t;
	struct Node_Image *ni;

	piGetAttr(o,OBJ_Node,(ULONG *)&ni);

//	if(t=FindTagItem(OBJ_Select,msg->Tags)) nf->nf_Selected=t->ti_Data;
	return TRUE;
}

ULONG GET_Image(piClass *cl,piObject *o,struct piGet *msg)
{
	struct Node_Image *ni=(struct Node_Image *)OBJECT_DATA(o);

	switch(msg->TagID)
	{
		case OBJ_Node: *(msg->TagData)=(ULONG)ni; return TRUE;
		case OBJ_Properties: *(msg->TagData)=(ULONG)ni->ni_Pages; return TRUE;
		default: return FALSE;
	}
}

ULONG READ_Image(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0,a;
	struct Node_Image *ni;

	piGetAttr(o,OBJ_Node,(ULONG *)&ni);
	ReadChunkBytes(msg->IFFh,(APTR)ni->ni_Label,SIZE(Node_Image,ni_Label));
	if(ni->ni_BitMap) FreeBitMap(ni->ni_BitMap);
	if(ni->ni_BitMap=AllocBitMap(ni->ni_Width,ni->ni_Height,ni->ni_Depth,BMF_CLEAR,NULL))
	{
		if(ni->ni_Depth<9) // planar bitmap
		{
			for(a=0;a<ni->ni_Depth;a++)
				ReadChunkBytes(msg->IFFh,(APTR)ni->ni_BitMap->Planes[a],GetBitMapAttr(ni->ni_BitMap,BMA_WIDTH)*ni->ni_Height/8);
		}
		else // chunky bitmap
		{
			// can't load
		}
	}
	if(ni->ni_Type==IMTYPE_NORMAL)
	{
		if(ni->ni_CT) MyFreeVec(ni->ni_CT);
		if(ni->ni_CT=MyAllocVec(sizeof(struct ColorPalette)*(2<<(ni->ni_Depth-1)),MEMF_ANY))
		{
			ReadChunkBytes(msg->IFFh,(APTR)ni->ni_CT,sizeof(struct ColorPalette)*(2<<(ni->ni_Depth-1)));
		}
		if(ni->ni_Pens) MyFreeVec(ni->ni_Pens);
		ni->ni_Pens=MyAllocVec(sizeof(UBYTE)*(2<<(ni->ni_Depth-1)),MEMF_ANY);
		for(a=0;a<(2<<(ni->ni_Depth-1));a++) ni->ni_Pens[a]=ni->ni_CT[a].cp_I=-1;
	}
	return (ULONG)error;
}

ULONG WRITE_Image(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0,a;
	struct Node_Image *ni;

	piGetAttr(o,OBJ_Node,(ULONG *)&ni);
	if(error=PushChunk(msg->IFFh,NULL,ID_IMGE,IFFSIZE_UNKNOWN)) return (ULONG)error;
	WriteChunkBytes(msg->IFFh,(APTR)ni->ni_Label,SIZE(Node_Image,ni_Label));
	if(ni->ni_Depth<9) // planar bitmap
	{
		for(a=0;a<ni->ni_Depth;a++)
			WriteChunkBytes(msg->IFFh,(APTR)ni->ni_BitMap->Planes[a],GetBitMapAttr(ni->ni_BitMap,BMA_WIDTH)*ni->ni_Height/8);
	}
	else // chunky bitmap
	{
			// can't save
	}
	if(ni->ni_Type==IMTYPE_NORMAL)
		WriteChunkBytes(msg->IFFh,(APTR)ni->ni_CT,sizeof(struct ColorPalette)*(2<<(ni->ni_Depth-1)));

	if(error=PopChunk(msg->IFFh)) return (ULONG)error;
	return (ULONG)error;
}

/****************************************************************************/

void DisposeAllImages()
{
	piObject *obj;	

	while(obj=(piObject *)RemTail(List(&GUI.gui_List_Image))) piDisposeObject(obj);
}

void DeleteAllImages()
{
	piObject *obj;	

	for(obj=(piObject *)GetHead(&GUI.gui_List_Image);GetSucc(obj);obj=(piObject *)GetSucc(obj)) piCloseWindow(obj);
}

/****************************************************************************/

int FN_MouseButtons_EditImage(void)
{
	int (*func)();
	BOOL running=TRUE;
	struct Node_Image *ni;

	switch(EditMsg.Code)
	{
		case MENUDOWN:
			piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
			func=(void *) PM_OpenPopupMenu(ni->ni_Window,PM_Menu,PM_ObjMenu,PM_Code, EditMsg.Code, TAG_DONE);
			if(func)
			{
				running = func();
				if(running > 1) return running;
			}
			break;
	}
	return 1;
}

int FN_MouseMove_EditImage(void)
{
	return 1;
}

int FN_MenuVerify_EditImage(void)
{
	return 1;
}

int FN_CloseWindow_EditImage(void)
{
	piCloseWindow(CurrentImage);
	PropertiesMode=PM_NONE;
	RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	return 0;
}

int FN_NewSize_EditImage(void)
{
	struct Node_Image *ni;
	ULONG v,t,z;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	GetFlag(GUI.gui_Prefs->pr_Flags,PRFLAG_SIZEZOOM)
	{
		z=min(ni->ni_Window->Width-ni->ni_Window->BorderLeft-ni->ni_Window->BorderRight,ni->ni_Window->Height-ni->ni_Window->BorderTop-ni->ni_Window->BorderBottom);
		if(z==ni->ni_Window->Width-ni->ni_Window->BorderLeft-ni->ni_Window->BorderRight)
			z=z/ni->ni_Width;
		else
			z=z/ni->ni_Height;
		if(z<2)
			SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_Left,0,DRAW_Top,0,DRAW_Zoom,z-(GUI.gui_GridShow?1:0),DRAW_Grid,GUI.gui_GridShow,TAG_DONE);
		else
			SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_Left,0,DRAW_Top,0,DRAW_Zoom,1,DRAW_Grid,FALSE, TAG_DONE);
	}
	GetAttr(DRAW_VisibleX,ni->ni_Draw,(ULONG *)&v);
	GetAttr(DRAW_Left,ni->ni_Draw,(ULONG *)&t);
	SetGadgetAttrs((struct Gadget *)ni->ni_HorizProp,ni->ni_Window,NULL, PGA_Top, t, PGA_Visible, v, TAG_DONE);

	GetAttr(DRAW_VisibleY,ni->ni_Draw,(ULONG *)&v);
	GetAttr(DRAW_Top,ni->ni_Draw,(ULONG *)&t);
	SetGadgetAttrs((struct Gadget *)ni->ni_VertProp,ni->ni_Window,NULL, PGA_Top, t, PGA_Visible, v, TAG_DONE);
	return 1;
}

int FN_IDCMPUpdate_EditImage(void)
{
	switch(GetTagData(GA_ID,0,(struct TagItem *)EditMsg.IAddress))
	{
		case 2: FN_DrawDown_Image(); break;
		case 3: FN_DrawUp_Image(); break;
		case 4: FN_DrawVert_Image(); break;
		case 5: FN_DrawLeft_Image(); break;
		case 6: FN_DrawRight_Image(); break;
		case 7: FN_DrawHoriz_Image(); break;
		case 8: FN_DrawDraw_Image(); break;
	}
	return 1;
}

int FN_PGPI_LABEL_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGPI_TYPE_Clicked(void)
{
	struct Node_Image *ni;
	struct Window *PropsWnd=NULL;
	struct BitMap *newbm;
	ULONG active=0,v,t,x;
	UWORD ow,oh,od,ot;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	ot=ni->ni_Type;
	ow=ni->ni_Width;
	oh=ni->ni_Height;
	od=ni->ni_Depth;

	switch(ni->ni_Type)
	{
		case IMTYPE_NORMAL:
			for(x=0;x<(2<<(ni->ni_Depth-1));x++)
				ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[x].cp_I);
			break;
		case IMTYPE_TOOLBAR:
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[1].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[2].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[3].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[4].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[5].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[6].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[7].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[8].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[9].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[10].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[11].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[12].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[13].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[14].cp_I);
			ReleasePen(Scr->ViewPort.ColorMap,ni->ni_CT[15].cp_I);
			break;
	}

	ni->ni_Type=PropertiesMsg.Code;

	switch(ni->ni_Type)
	{
		case IMTYPE_NORMAL:
			for(x=0;x<(2<<(ni->ni_Depth-1));x++)
				ni->ni_Pens[x]=ni->ni_CT[x].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ni->ni_CT[x].cp_R),Bit32(ni->ni_CT[x].cp_G),Bit32(ni->ni_CT[x].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			break;
		case IMTYPE_POINTER:
			GetRGB32(Scr->ViewPort.ColorMap,PointerPens[0].cp_I,1,&ni->ni_CT[0].cp_R); ni->ni_Pens[0]=ni->ni_CT[0].cp_I=PointerPens[0].cp_I;
			GetRGB32(Scr->ViewPort.ColorMap,PointerPens[1].cp_I,1,&ni->ni_CT[1].cp_R); ni->ni_Pens[1]=ni->ni_CT[1].cp_I=PointerPens[1].cp_I;
			GetRGB32(Scr->ViewPort.ColorMap,PointerPens[2].cp_I,1,&ni->ni_CT[2].cp_R); ni->ni_Pens[2]=ni->ni_CT[2].cp_I=PointerPens[2].cp_I;
			GetRGB32(Scr->ViewPort.ColorMap,PointerPens[3].cp_I,1,&ni->ni_CT[3].cp_R); ni->ni_Pens[3]=ni->ni_CT[3].cp_I=PointerPens[3].cp_I;
			break;
		case IMTYPE_TOOLBAR:
			ni->ni_Pens[0]=ni->ni_CT[0].cp_I=DrawInfo->dri_Pens[BACKGROUNDPEN];
			ni->ni_Pens[1]=ni->ni_CT[1].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[1].cp_R),Bit32(ToolbarPens[1].cp_G),Bit32(ToolbarPens[1].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[2]=ni->ni_CT[2].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[2].cp_R),Bit32(ToolbarPens[2].cp_G),Bit32(ToolbarPens[2].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[3]=ni->ni_CT[3].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[3].cp_R),Bit32(ToolbarPens[3].cp_G),Bit32(ToolbarPens[3].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[4]=ni->ni_CT[4].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[4].cp_R),Bit32(ToolbarPens[4].cp_G),Bit32(ToolbarPens[4].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[5]=ni->ni_CT[5].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[5].cp_R),Bit32(ToolbarPens[5].cp_G),Bit32(ToolbarPens[5].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[6]=ni->ni_CT[6].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[6].cp_R),Bit32(ToolbarPens[6].cp_G),Bit32(ToolbarPens[6].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[7]=ni->ni_CT[7].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[7].cp_R),Bit32(ToolbarPens[7].cp_G),Bit32(ToolbarPens[7].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[8]=ni->ni_CT[8].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[8].cp_R),Bit32(ToolbarPens[8].cp_G),Bit32(ToolbarPens[8].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[9]=ni->ni_CT[9].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[9].cp_R),Bit32(ToolbarPens[9].cp_G),Bit32(ToolbarPens[9].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[10]=ni->ni_CT[10].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[10].cp_R),Bit32(ToolbarPens[10].cp_G),Bit32(ToolbarPens[10].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[11]=ni->ni_CT[11].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[11].cp_R),Bit32(ToolbarPens[11].cp_G),Bit32(ToolbarPens[11].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[12]=ni->ni_CT[12].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[12].cp_R),Bit32(ToolbarPens[12].cp_G),Bit32(ToolbarPens[12].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[13]=ni->ni_CT[13].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[13].cp_R),Bit32(ToolbarPens[13].cp_G),Bit32(ToolbarPens[13].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[14]=ni->ni_CT[14].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[14].cp_R),Bit32(ToolbarPens[14].cp_G),Bit32(ToolbarPens[14].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			ni->ni_Pens[15]=ni->ni_CT[15].cp_I=ObtainBestPen(Scr->ViewPort.ColorMap,Bit32(ToolbarPens[15].cp_R),Bit32(ToolbarPens[15].cp_G),Bit32(ToolbarPens[15].cp_B),OBP_Precision,PRECISION_IMAGE,TAG_DONE);
			break;
	}

	if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	switch(ni->ni_Type)
	{
		case IMTYPE_NORMAL:
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_WIDTH],PropsWnd,FALSE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_HEIGHT],PropsWnd,FALSE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_DEPTH],PropsWnd,FALSE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_PALETTE],PropsWnd,FALSE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_SETPOINT],PropsWnd,TRUE);
			break;
		case IMTYPE_TOOLBAR:
			ni->ni_Width=20;
			ni->ni_Height=20;
			ni->ni_Depth=4;
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_WIDTH],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_HEIGHT],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_DEPTH],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_PALETTE],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_SETPOINT],PropsWnd,TRUE);
			break;
		case IMTYPE_POINTER:
			ni->ni_Width=32;
			ni->ni_Height=32;
			ni->ni_Depth=2;
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_WIDTH],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_HEIGHT],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_DEPTH],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_PALETTE],PropsWnd,TRUE);
			DisableGadget(PGPI_GeneralGadgets[GD_PGPI_SETPOINT],PropsWnd,FALSE);
			break;
	}
	if(newbm=AllocBitMap(ni->ni_Width,ni->ni_Height,ni->ni_Depth,BMF_CLEAR,NULL))
	{
		if(ni->ni_BitMap)
		{
			BltBitMap(ni->ni_BitMap,0,0,newbm,0,0,(ni->ni_Width<ow)?ni->ni_Width:ow,(ni->ni_Height<oh)?ni->ni_Height:oh,0xc0,0xff,NULL);
			FreeBitMap(ni->ni_BitMap);
		}
		ni->ni_BitMap=newbm;
		SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_BitMap,ni->ni_BitMap,DRAW_Palette,(ULONG)ni->ni_CT,DRAW_Width,ni->ni_Width,TAG_DONE);
		RefreshGList((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,1);

		GetAttr(DRAW_VisibleX,ni->ni_Draw,(ULONG *)&v);
		GetAttr(DRAW_Left,ni->ni_Draw,(ULONG *)&t);
		SetGadgetAttrs((struct Gadget *)ni->ni_HorizProp,ni->ni_Window,NULL, PGA_Total, ni->ni_Width, PGA_Top, t, PGA_Visible, v, TAG_DONE);

		GetAttr(DRAW_VisibleY,ni->ni_Draw,(ULONG *)&v);
		GetAttr(DRAW_Top,ni->ni_Draw,(ULONG *)&t);
		SetGadgetAttrs((struct Gadget *)ni->ni_VertProp,ni->ni_Window,NULL, PGA_Total, ni->ni_Height, PGA_Top, t, PGA_Visible, v, TAG_DONE);
	}
	else
	{
		ni->ni_Width=ow;
		ni->ni_Height=oh;
		ni->ni_Depth=od;
		ni->ni_Type=ot;
		SetCycle(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_TYPE],ni->ni_Type);
	}
	SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_WIDTH],ni->ni_Width);
	SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_HEIGHT],ni->ni_Height);
	SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_DEPTH],ni->ni_Depth);
	PropertiesChanged=TRUE;
	FN_ActiveWindow_EditImage();
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGPI_WIDTH_Clicked(void)
{
	return FN_PGPI_DEPTH_Clicked();
}

int FN_PGPI_HEIGHT_Clicked(void)
{
	return FN_PGPI_DEPTH_Clicked();
}

int FN_PGPI_DEPTH_Clicked(void)
{
	struct Node_Image *ni;
	struct Window *PropsWnd=NULL;
	struct BitMap *newbm;
	ULONG active=0,v,t;
	UWORD ow,oh,od;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);

	ow=ni->ni_Width;
	oh=ni->ni_Height;
	od=ni->ni_Depth;

	ni->ni_Width=GetInteger(PGPI_GeneralGadgets[GD_PGPI_WIDTH]);
	ni->ni_Height=GetInteger(PGPI_GeneralGadgets[GD_PGPI_HEIGHT]);
	ni->ni_Depth=GetInteger(PGPI_GeneralGadgets[GD_PGPI_DEPTH]);

	if(newbm=AllocBitMap(ni->ni_Width,ni->ni_Height,ni->ni_Depth,BMF_CLEAR,NULL))
	{
		if(ni->ni_BitMap)
		{
			BltBitMap(ni->ni_BitMap,0,0,newbm,0,0,(ni->ni_Width<ow)?ni->ni_Width:ow,(ni->ni_Height<oh)?ni->ni_Height:oh,0xc0,0xff,NULL);
			FreeBitMap(ni->ni_BitMap);
		}
		ni->ni_BitMap=newbm;
		SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_BitMap,ni->ni_BitMap,DRAW_Palette,(ULONG)ni->ni_CT,DRAW_Width,ni->ni_Width,TAG_DONE);
		RefreshGList((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,1);

		GetAttr(DRAW_VisibleX,ni->ni_Draw,(ULONG *)&v);
		GetAttr(DRAW_Left,ni->ni_Draw,(ULONG *)&t);
		SetGadgetAttrs((struct Gadget *)ni->ni_HorizProp,ni->ni_Window,NULL, PGA_Total, ni->ni_Width, PGA_Top, t, PGA_Visible, v, TAG_DONE);

		GetAttr(DRAW_VisibleY,ni->ni_Draw,(ULONG *)&v);
		GetAttr(DRAW_Top,ni->ni_Draw,(ULONG *)&t);
		SetGadgetAttrs((struct Gadget *)ni->ni_VertProp,ni->ni_Window,NULL, PGA_Total, ni->ni_Height, PGA_Top, t, PGA_Visible, v, TAG_DONE);
	}
	else
	{
		ni->ni_Width=ow;
		ni->ni_Height=oh;
		ni->ni_Depth=od;
		GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);
		if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
		SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_WIDTH],ni->ni_Width);
		SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_HEIGHT],ni->ni_Height);
		SetInteger(PropsWnd,PGPI_GeneralGadgets[GD_PGPI_DEPTH],ni->ni_Depth);
	}
	PropertiesChanged=TRUE;
	FN_ActiveWindow_EditImage();
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGPI_SETPOINT_Clicked(void)
{
	struct Node_Image *ni;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_SetPoint,TRUE,TAG_DONE);
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

UBYTE SelectedPen;
ULONG SelectedPenGuns[3];
struct Node_Image *CurrentImageNode;

int FN_PGPI_PALETTE_Clicked(void)
{
	struct Node_Image *ni;
	int run=1;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	if(!OpenWindow_EditPalette())
	{
		CurrentImageNode=ni;
		while(run)
		{
			Wait(1L<<EditPaletteWnd->UserPort->mp_SigBit);
			run=HandleIDCMP_EditPalette();
		}
		CloseWindow_EditPalette();
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PAL_PALETTE_Clicked(void)
{
	SelectedPen=EditPaletteMsg.Code;
	GetRGB32(Scr->ViewPort.ColorMap,SelectedPen,1,SelectedPenGuns);
	CurrentImageNode->ni_CT[SelectedPen].cp_I=SelectedPen;
	CurrentImageNode->ni_CT[SelectedPen].cp_R=SelectedPenGuns[0];
	CurrentImageNode->ni_CT[SelectedPen].cp_G=SelectedPenGuns[1];
	CurrentImageNode->ni_CT[SelectedPen].cp_B=SelectedPenGuns[2];
	GT_SetGadgetAttrs(EditPaletteGadgets[GD_PAL_RED],EditPaletteWnd,NULL,GTSL_Level,Bit8(CurrentImageNode->ni_CT[SelectedPen].cp_R));
	GT_SetGadgetAttrs(EditPaletteGadgets[GD_PAL_GREEN],EditPaletteWnd,NULL,GTSL_Level,Bit8(CurrentImageNode->ni_CT[SelectedPen].cp_G));
	GT_SetGadgetAttrs(EditPaletteGadgets[GD_PAL_BLUE],EditPaletteWnd,NULL,GTSL_Level,Bit8(CurrentImageNode->ni_CT[SelectedPen].cp_B));
	return 1;
}

int FN_PAL_RED_Clicked(void)
{
	SelectedPenGuns[0]=EditPaletteMsg.Code;
	SetRGB32(&Scr->ViewPort,SelectedPen,Bit32(SelectedPenGuns[0]),Bit32(SelectedPenGuns[1]),Bit32(SelectedPenGuns[2]));
	CurrentImageNode->ni_CT[SelectedPen].cp_I=SelectedPen;
	CurrentImageNode->ni_CT[SelectedPen].cp_R=SelectedPenGuns[0];
	return 1;
}

int FN_PAL_GREEN_Clicked(void)
{
	SelectedPenGuns[1]=EditPaletteMsg.Code;
	SetRGB32(&Scr->ViewPort,SelectedPen,Bit32(SelectedPenGuns[0]),Bit32(SelectedPenGuns[1]),Bit32(SelectedPenGuns[2]));
	CurrentImageNode->ni_CT[SelectedPen].cp_I=SelectedPen;
	CurrentImageNode->ni_CT[SelectedPen].cp_G=SelectedPenGuns[1];
	return 1;
}

int FN_PAL_BLUE_Clicked(void)
{
	SelectedPenGuns[2]=EditPaletteMsg.Code;
	SetRGB32(&Scr->ViewPort,SelectedPen,Bit32(SelectedPenGuns[0]),Bit32(SelectedPenGuns[1]),Bit32(SelectedPenGuns[2]));
	CurrentImageNode->ni_CT[SelectedPen].cp_I=SelectedPen;
	CurrentImageNode->ni_CT[SelectedPen].cp_B=SelectedPenGuns[2];
	return 1;
}

int FN_PAL_OK_Clicked(void)
{
	return 0;
}

int FN_PAL_CANCEL_Clicked(void)
{
	return 0;
}

int FN_PAL_GSLIDER_Clicked(void)
{
	return 1;
}

int FN_PAL_COLORWHEEL_Clicked(void)
{
	return 1;
}

ULONG ToolColors=0;

int FN_ActiveWindow_EditImage(void)
{
	struct Node_Image *ni;
	struct TagItem *ti;
	ULONG color;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	if(TB_WindowWnd) CloseWindow_TB_Window();
	if(ToolColors!=ni->ni_Depth)// || (!TB_PointerWnd))
	{
		if(TB_PointerWnd) CloseWindow_TB_Pointer();
		piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
		if(ti=FindTagItem(GTPA_Depth,(struct TagItem *)&TB_PointerGTags[2]))
			ti->ti_Data=ni->ni_Depth;
		if(!TB_PointerWnd) OpenWindow_TB_Pointer();
		ToolColors=ni->ni_Depth;
	}
	SetWndSignals();

	GetAttr(DRAW_ActualPen,ni->ni_Draw,(ULONG *)&color);
	GT_SetGadgetAttrs(TB_PointerGadgets[GD_TBP_PALETTE],TB_PointerWnd,NULL,GTPA_ColorOffset,0,GTPA_ColorTable,ni->ni_Pens,GTPA_Color,ni->ni_Pens[color],TAG_DONE);

	PropertiesMode=PM_IMAGE;
	RefreshProperties();
	return 1;
}

int FN_IDCMPUpdate_EditPalette(void)
{
	return 1;
}
