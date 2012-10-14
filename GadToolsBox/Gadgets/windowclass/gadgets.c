/*
** Gadget Class
** $VER: gadget.c 37.1 (15.4.98)
*/

#include<dos.h>
#include<stdio.h>
#include<string.h>
#include<clib/macros.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<proto/intuition.h>
#include<proto/gadtools.h>
#include<exec/memory.h>
#include<intuition/gadgetclass.h>
#include<intuition/intuitionbase.h>
#include<intuition/icclass.h>
#include"gadtoolsbox.h"

/*
** Defines
*/

#define unless(x) if(!(x))

/*
** Prototypes
*/

#define SetFlag(x,f) x|=f
#define GetFlag(x,f) if(x&f)
#define ClearFlag(x,f) x&=(f^-1)
#define DoFlag(t,x,f) if(t) SetFlag(x,f); else ClearFlag(x,f);

Object *gtbCreateGadget(ULONG kind, struct IClass *class,UBYTE *classname, ULONG tags, ...)
{
	struct Gadget *gad=NULL,*prev;
	struct TagItem *tag, *tstate;
	struct NewGadget ng={NULL};
	struct Screen *Scr;
	Object *ScrObj;
	BOOL sr=FALSE,gr=TRUE,bs=TRUE; // SelectRender, GadgetRender, BorderSniff

	switch(kind)
	{
		case BOOPSI_KIND:
			gad=(struct Gadget *)NewObjectA(class,classname,(struct TagItem *)&tags);
			break;
		default:
			switch(kind)
			{
				case BUTTON_KIND: ng.ng_Flags=PLACETEXT_IN; break;
				default: ng.ng_Flags=PLACETEXT_LEFT; break;
			}
			if(ScrObj=(Object *)GetTagData(GT_ScreenObj,NULL,(struct TagItem *)&tags))
			{
				GetAttr(SCRN_VisualInfo,ScrObj,(ULONG *)&ng.ng_VisualInfo);
				GetAttr(SCRN_Screen,ScrObj,(ULONG *)&Scr); ng.ng_TextAttr=Scr->Font;
			}
			tstate=(struct TagItem *)&tags;
			while(tag = NextTagItem( &tstate ))
			{
				switch(tag->ti_Tag)
				{
					case GA_Previous: prev=(struct Gadget *)tag->ti_Data; break;
					case GA_Left: ng.ng_LeftEdge=tag->ti_Data; break;
					case GA_Top: ng.ng_TopEdge=tag->ti_Data; break;
					case GA_Width: ng.ng_Width=tag->ti_Data; break;
					case GA_Height: ng.ng_Height=tag->ti_Data; break;
					case GA_Text: ng.ng_GadgetText=(UBYTE *)tag->ti_Data; break;
					case GA_ID: ng.ng_GadgetID=tag->ti_Data; break;
					case GT_Handler:
					case GA_UserData: ng.ng_UserData=(APTR)tag->ti_Data; break;
					case GA_TextAttr: ng.ng_TextAttr=(struct TextAttr *)tag->ti_Data; break;
					case GT_VisualInfo: ng.ng_VisualInfo=(APTR)tag->ti_Data; break;
					case GT_PlaceText: ng.ng_Flags=tag->ti_Data; break;
				}
			}
			gad=CreateGadgetA(kind,prev,&ng,(struct TagItem *)&tags);
			if(!gad) return FALSE;

			tstate=(struct TagItem *)&tags;
			while(tag = NextTagItem( &tstate ))
			{
				switch(tag->ti_Tag)
				{
					case GA_Highlight: gad->Flags|=tag->ti_Data; break;
					case GA_SysGType: gad->GadgetType|=tag->ti_Data; break;
					case GA_GZZGadget: DoFlag(tag->ti_Data,gad->GadgetType,GTYP_GZZGADGET); break;
					case GA_RightBorder: DoFlag(tag->ti_Data,gad->Activation,GACT_RIGHTBORDER); bs=TRUE; break;
					case GA_LeftBorder: DoFlag(tag->ti_Data,gad->Activation,GACT_LEFTBORDER); bs=TRUE; break;
					case GA_TopBorder: DoFlag(tag->ti_Data,gad->Activation,GACT_TOPBORDER); bs=TRUE; break;
					case GA_BottomBorder: DoFlag(tag->ti_Data,gad->Activation,GACT_BOTTOMBORDER); bs=TRUE; break;
					case GA_ToggleSelect: DoFlag(tag->ti_Data,gad->Activation,GACT_TOGGLESELECT); break;
					case GA_RelVerify: DoFlag(tag->ti_Data,gad->Activation,GACT_RELVERIFY); break;
					case GA_LabelImage:
						gad->Flags|=GFLG_GADGIMAGE|GFLG_GADGHIMAGE;
						gad->SelectRender=gad->GadgetRender=(ULONG *)tag->ti_Data;
						gr=TRUE; sr=TRUE;
						break;
					case GA_Image:
						gr=TRUE;
						gad->Flags|=GFLG_GADGIMAGE;
						gad->GadgetRender=(ULONG *)tag->ti_Data;
						break;
					case GA_SelectRender:
						sr=TRUE;
						gad->Flags|=GFLG_GADGHIMAGE;
						gad->SelectRender=(ULONG *)tag->ti_Data;
						break;
				}
			}
			if(bs) gad->Activation|=GACT_BORDERSNIFF;
			if(gr && !sr)
			{
				gad->Flags|=GFLG_GADGHIMAGE;
				gad->SelectRender=gad->GadgetRender;
			}
			break;
	}
	return (Object *) gad;
}

void gtbFreeGadgets(struct Gadget *glist)
{
	struct Gadget *gad,*prev;

goagain:
	for(gad=glist,prev=NULL;gad;gad=gad->NextGadget)
	{
		if(!(gad->GadgetType&0x100))		// BOOPSI Object
		{
			prev->NextGadget=gad->NextGadget;
			DisposeObject((Object *)gad);
			goto goagain;
		}
		prev=gad;
	}

	FreeGadgets(glist);
}

void gtbSetGadgetAttrs(Object *gadobj,Object *wndobj,ULONG tags,...)
{
	struct Gadget *gad;
	struct Window *wnd;

	GetAttr(WIND_Window,wndobj,(ULONG *)&wnd);
	gad=(struct Gadget *)gadobj;
	if(gad->GadgetType&0x100)
		GT_SetGadgetAttrsA(gad,wnd,NULL,(struct TagItem *)&tags);
	else
		SetGadgetAttrsA(gad,wnd,NULL,(struct TagItem *)&tags);
}

void gtbGetGadgetAttr(ULONG tag,Object *gadobj,ULONG *storage)
{
	struct Gadget *gad;

	gad=(struct Gadget *)gadobj;
	switch(tag)
	{
		case GT_Handler:
			*storage=(ULONG)gad->UserData;
			break;
		default:
			if(gad->GadgetType&0x100)
				GT_GetGadgetAttrs(gad,NULL,NULL,tag,(ULONG)storage,TAG_DONE);
			else
				GetAttr(tag,gadobj,storage);
			break;
	}
}
