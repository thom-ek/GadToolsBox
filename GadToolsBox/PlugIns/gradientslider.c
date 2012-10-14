/*
** GradientSlider PlugIn
*/

#include"includes.h"
#include"gradientslider.h"

/*
** OwnTag definitions
*/

#define OWNTAG_Pens (TAG_USER+1)

/*
** Default Tags
*/

struct TagItem GradientSliderTags[]=
{
	GA_Disabled,FALSE,
	GRAD_MaxVal,0xffff,
	GRAD_CurVal,0,
	GRAD_SkipVal,0x1111,
	GRAD_KnobPixels,5,
	GRAD_PenArray,NULL,
	PGA_Freedom,LORIENT_HORIZ,
	TAG_DONE,0
};

UBYTE GradientSliderTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NONE,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem GradientSliderOwnTags[]=
{
	OWNTAG_Pens,0,
	TAG_DONE,0
};

UBYTE GradientSliderOwnTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 7
#define GSIZE_NUMOWNTAGS 1

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGBO_General[];

struct Page PAGES_PROPERTIES_PG_GRSL_PAGES[]=
{
	&PAGES_PROPERTIES_PG_GRSL_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG GRADIENTSLIDER_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=GRADIENTSLIDER_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=GRADIENTSLIDER_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_GRSL_Flags();
			PAGES_PROPERTIES_PG_GRSL_PAGES[0].FirstGadget=PGBO_GeneralGList;
			PAGES_PROPERTIES_PG_GRSL_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L0_GAD);
			PAGES_PROPERTIES_PG_GRSL_PAGES[1].FirstGadget=PG_GRSL_FlagsGList;
			PAGES_PROPERTIES_PG_GRSL_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L7_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_GRSL_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_GRSL_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_GRSL_Flags();
			break;
		case OM_NEW:
			retval=GRADIENTSLIDER_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_CREATE:
			retval=GRADIENTSLIDER_CREATE_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_DELETE:
			retval=GRADIENTSLIDER_DELETE_Gadget(cl,o,msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG GRADIENTSLIDER_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PG_GRSL_FlagsGadgets[GD_PG_GRSL_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_GRSL_FlagsGadgets[GD_PG_GRSL_FLAGS_MAXVAL],GetTagData(GRAD_MaxVal,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_GRSL_FlagsGadgets[GD_PG_GRSL_FLAGS_CURVAL],GetTagData(GRAD_CurVal,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_GRSL_FlagsGadgets[GD_PG_GRSL_FLAGS_SKIPVAL],GetTagData(GRAD_SkipVal,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_GRSL_FlagsGadgets[GD_PG_GRSL_FLAGS_KNOBPIXELS],GetTagData(GRAD_KnobPixels,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_GRSL_FlagsGadgets[GD_PG_GRSL_FLAGS_PENS],GetTagData(OWNTAG_Pens,0,ng->ng_OwnTags));
	return TRUE;
}

ULONG GRADIENTSLIDER_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);

	piDoSuperMethodA(cl,o,msg);

	FindTagItem(GRAD_CurVal,ng->ng_Tags)->ti_Data=GetInteger(PG_GRSL_FlagsGadgets[GD_PG_GRSL_FLAGS_CURVAL]);
	FindTagItem(GRAD_SkipVal,ng->ng_Tags)->ti_Data=GetInteger(PG_GRSL_FlagsGadgets[GD_PG_GRSL_FLAGS_SKIPVAL]);
	FindTagItem(GRAD_MaxVal,ng->ng_Tags)->ti_Data=GetInteger(PG_GRSL_FlagsGadgets[GD_PG_GRSL_FLAGS_MAXVAL]);
	FindTagItem(GRAD_KnobPixels,ng->ng_Tags)->ti_Data=GetInteger(PG_GRSL_FlagsGadgets[GD_PG_GRSL_FLAGS_KNOBPIXELS]);
	FindTagItem(OWNTAG_Pens,ng->ng_OwnTags)->ti_Data=GetInteger(PG_GRSL_FlagsGadgets[GD_PG_GRSL_FLAGS_PENS]);
	return TRUE;
}

ULONG GRADIENTSLIDER_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_BOOPSI,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)GradientSliderTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)GradientSliderOwnTagsTypes,
		GAOBJ_ID,GRADIENTSLIDER_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_GRSL_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);

		CopyTagItems(GradientSliderTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(GradientSliderOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);

		return (ULONG)obj;
	}
	return FALSE;
}

ULONG GRADIENTSLIDER_CREATE_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
{
	struct TagItem *ti,*oti;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	struct ColorWheelRGB rgb;
	struct ColorWheelHSB hsb;
	ULONG pens,a;
	WORD *penarray;
	WORD x,y,w,h;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
	piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&oti);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);

	rgb.cw_Red=0;
	rgb.cw_Green=0;
	rgb.cw_Blue=0;
	ConvertRGBToHSB(&rgb,&hsb);
	hsb.cw_Brightness = 0xffffffff;
	pens=GetTagData(OWNTAG_Pens,0,oti);
	if(pens>0 && pens<(2<<Scr->BitMap.Depth))
	{
		if(FindTagItem(GRAD_PenArray,ti)->ti_Data=(ULONG)MyAllocVec(pens*2+2,MEMF_ANY))
		{
			penarray=(WORD *)GetTagData(GRAD_PenArray,0,ti);
			for(a=0;a<pens;a++)
			{
				hsb.cw_Brightness=0xffffffff-((0xffffffff/pens)*a);
				ConvertHSBToRGB(&hsb,&rgb);
				penarray[a]=ObtainPen(Scr->ViewPort.ColorMap,-1,rgb.cw_Red,rgb.cw_Green,rgb.cw_Blue,PEN_EXCLUSIVE);
				if(penarray[a]==-1)
				{
					MyFreeVec(penarray);
					FindTagItem(GRAD_PenArray,ti)->ti_Data=NULL;
					penarray=NULL;
					break;
				}
			}
			if(penarray) penarray[pens]=~0;
		}
	}

	if(ng->ng_RelX) x=-(nw->nw_Window->Width-nw->nw_Window->BorderLeft-ng->ng_X-1);
	else x=piCX(o);
	if(ng->ng_RelY) y=-(nw->nw_Window->Height-nw->nw_Window->BorderTop-ng->ng_Y-1);
	else y=piCY(o);
	if(ng->ng_RelWidth) w=-(nw->nw_Window->BorderLeft+nw->nw_Window->BorderRight+(nw->nw_Window->Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight-ng->ng_Width)-1);
	else w=piCW(o)+1;
	if(ng->ng_RelHeight) h=-(nw->nw_Window->BorderTop+nw->nw_Window->BorderBottom+(nw->nw_Window->Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom-ng->ng_Height)-1);
	else h=piCH(o)+1;

	return (ULONG) (ng->ng_Gadget=NewObject(NULL, "gradientslider.gadget",
									ng->ng_RelX?GA_RelRight:GA_Left,x,
									ng->ng_RelY?GA_RelBottom:GA_Top,y,
									ng->ng_RelWidth?GA_RelWidth:GA_Width,w,
									ng->ng_RelHeight?GA_RelHeight:GA_Height,h,
									(msg->Window) ? GA_Previous : TAG_IGNORE, msg->Window,
									GA_ID, 0,
									TAG_MORE,ti));
}

ULONG GRADIENTSLIDER_DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct TagItem *ti,*oti;
	struct Node_Gadget *ng;
	WORD *penarray;
	ULONG pens,a;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
	piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&oti);

	if(ng->ng_Gadget && ng->ng_Window) RemoveGadget(((struct Node_Window *)OBJECT_DATA(ng->ng_Window))->nw_Window,ng->ng_Gadget);
	if(ng->ng_Gadget) DisposeObject(ng->ng_Gadget);
	ng->ng_Gadget=NULL;

	penarray=(WORD *)GetTagData(GRAD_PenArray,0,ti);
	pens=GetTagData(OWNTAG_Pens,0,oti);
	if(penarray)
	{
		for(a=0;a<pens;a++)
		{
			if(penarray[a]!=-1) ReleasePen(Scr->ViewPort.ColorMap,penarray[a]);
		}
		MyFreeVec(penarray);
		FindTagItem(GRAD_PenArray,ti)->ti_Data=NULL;
	}
	return piDoSuperMethodA(cl,o,msg);
}

int FN_PG_GRSL_FLAGS_MAXVAL_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_GRSL_FLAGS_CURVAL_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_GRSL_FLAGS_SKIPVAL_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_GRSL_FLAGS_PENS_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_GRSL_FLAGS_KNOBPIXELS_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_GRSL_FLAGS_FREEDOM_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	switch(PropertiesMsg.Code)
	{
		case 0: FindTagItem(PGA_Freedom,ti)->ti_Data=LORIENT_HORIZ; break;
		case 1: FindTagItem(PGA_Freedom,ti)->ti_Data=LORIENT_VERT; break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_GRSL_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
