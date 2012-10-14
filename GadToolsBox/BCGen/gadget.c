/*
** Gadgets code generator
*/

#include<string.h>
#include<stdio.h>
#include<proto/dos.h>
#include<proto/gadtools.h>
#include<proto/intuition.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<intuition/gadgetclass.h>
#include<gadgets/colorwheel.h>
#include<gadgets/gradientslider.h>
#include<gadgets/page.h>
#include<gadgets/popupmenu.h>
#include<gadgets/progress.h>
#include<gadgets/tapedeck.h>
#include<gadgets/textfield.h>
#include<gadgets/listview.h>
#include"main.h"

extern struct GUI *GUI;
extern struct Screen *Scr;

void NWCalcLeftTop(UWORD *offx,UWORD *offy,struct Node_Window *nw)
{
	BOOL bl,aaa=FALSE;

	*offx=0;
	*offy=0;

	if(nw->nw_Flags&WFLG_BORDERLESS) bl=TRUE;
	else bl=FALSE;

	if(nw->nw_MyFlags&MYWA_DragToolHoriz)
	{
		(*offy)+=8;
		if(!bl) (*offx)+=Scr->WBorLeft;
		aaa=TRUE;
	}
	if(nw->nw_MyFlags&MYWA_DragToolVert)
	{
		(*offx)+=8;
		if(!bl) (*offy)+=Scr->WBorTop;
		aaa=TRUE;
	}
	if(!aaa)
	{
		if(!bl)
		{
			(*offx)+=Scr->WBorLeft;
			if(nw->nw_WindowTitle[0])
				(*offy)+=Scr->WBorTop+Scr->RastPort.TxHeight+1;
			else
				(*offy)+=Scr->WBorTop;
		}
	}
}

void NWCalcRightBottom(UWORD *offx,UWORD *offy,struct Node_Window *nw)
{
	BOOL bl,sg,sbb,sbr;
	UWORD sw,sh;
	struct Image *im;
	struct DrawInfo *DrawInfo;

	*offx=0;
	*offy=0;

	if(nw->nw_Flags&WFLG_BORDERLESS) bl=TRUE;
	else bl=FALSE;

	if(nw->nw_Flags&WFLG_SIZEGADGET) sg=TRUE;
	else sg=FALSE;

	if(nw->nw_Flags&WFLG_SIZEBBOTTOM) sbb=TRUE;
	else sbb=FALSE;

	if(nw->nw_Flags&WFLG_SIZEBRIGHT) sbr=TRUE;
	else sbr=FALSE;

	if((DrawInfo=GetScreenDrawInfo(Scr)) && (im=(struct Image *)NewObject(NULL, SYSICLASS,
			SYSIA_DrawInfo, DrawInfo,
			SYSIA_Which, SIZEIMAGE,
			SYSIA_Size, (Scr->Flags & SCREENHIRES ? SYSISIZE_MEDRES : SYSISIZE_LOWRES),
			TAG_DONE)))
	{
		sw=im->Width;
		sh=im->Height;
		DisposeObject((Object *)im);
		FreeScreenDrawInfo(Scr,DrawInfo);
	}
	else
	{
		sw=18;
		sh=10;
	}

	if(!bl)
	{
		if(sg && (!sbb) && (!sbr)) { (*offx)+=sw; (*offy)+=Scr->WBorBottom; }
		if(sg && (sbb) && (!sbr)) { (*offx)+=Scr->WBorRight; (*offy)+=sh; }
		if(sg && (!sbb) && (sbr)) { (*offx)+=sw; (*offy)+=Scr->WBorBottom; }
		if(sg && sbb && sbr) { (*offx)+=sw; (*offy)+=sh; }
		if((!sg) && (sbb) && (!sbr)) { (*offx)+=Scr->WBorRight; (*offy)+=sh; }
		if((!sg) && (!sbb) && (sbr)) { (*offx)+=sw; (*offy)+=Scr->WBorBottom; }
		if((!sg) && (sbb) && (sbr)) { (*offx)+=sw; (*offy)+=sh; }
	}
}

void WriteNewGadgetFlags(BPTR handle,ULONG flags)
{
	GetFlag(flags,PLACETEXT_LEFT) FPuts(handle,"PLACETEXT_LEFT |");
	GetFlag(flags,PLACETEXT_RIGHT) FPuts(handle,"PLACETEXT_RIGHT |");
	GetFlag(flags,PLACETEXT_ABOVE) FPuts(handle,"PLACETEXT_ABOVE |");
	GetFlag(flags,PLACETEXT_BELOW) FPuts(handle,"PLACETEXT_BELOW |");
	GetFlag(flags,PLACETEXT_IN) FPuts(handle,"PLACETEXT_IN |");
	GetFlag(flags,NG_HIGHLABEL) FPuts(handle,"NG_HIGHLABEL |");
	Seek(handle,-2,OFFSET_CURRENT);
	FPuts(handle,", ");
}

void GADGET_GenerateStruct(BPTR handle,piObject *go)
{
	struct Node_Frame *nf;
	struct Node_Window *nw;
	struct Node_Gadget *ng;
	struct TagItem *ti;
	struct Node *n;
	ULONG a,w,h;
	piObject *wo,*fo;

	ng=(struct Node_Gadget *)OBJECT_DATA(go);
	switch(ng->ng_PlugInID)
	{
		case CYCLE_ID:
		case MX_ID:
			if(ti=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				FPrintf(handle,"UBYTE *LABELS_%s[]=\n{\n",ng->ng_Label);
				for(n=GetHead(ti->ti_Data),a=0;GetSucc(n);n=GetSucc(n),a++)
					FPrintf(handle,"	(UBYTE *)MSG_%s_L%ld_GAD,\n",ng->ng_Label,a);
				FPuts(handle,"	NULL\n};\n");
			}
			break;
		case LISTVIEW_ID:
			if(ti=FindTagItem(GTLV_Labels,ng->ng_Tags))
			{
				if(!IsListEmpty(List(ti->ti_Data)))
				{
					FPrintf(handle,"extern struct MinList LABELS_%s;",ng->ng_Label);
					FPrintf(handle,"struct Node NODES_%s[]=\n{\n",ng->ng_Label);
					for(a=0,n=GetHead(ti->ti_Data);GetSucc(n);n=GetSucc(n),a++)
					{
						if(!GetSucc(GetSucc(n))) FPrintf(handle,"(struct Node *)&LABELS_%s.mlh_Tail, ",ng->ng_Label);
						else FPrintf(handle,"	&NODES_%s[%ld], ",ng->ng_Label,a+1);
						if(!GetPred(GetPred(n))) FPrintf(handle,"(struct Node *)&LABELS_%s.mlh_Head, ",ng->ng_Label);
						else FPrintf(handle,"	&NODES_%s[%ld], ",ng->ng_Label,a-1);
						FPrintf(handle,"0, 0, (STRPTR) MSG_%s_L%ld_GAD,\n",ng->ng_Label,a);
					}
					Seek(handle,-2,OFFSET_CURRENT);
					FPuts(handle,"\n};\n\n");
					FPrintf(handle,"struct MinList LABELS_%s={\n	(struct MinNode *)&NODES_%s[0], NULL, (struct MinNode *)&NODES_%s[%ld]\n};\n",ng->ng_Label,ng->ng_Label,ng->ng_Label,a);
				}
			}
			break;
		case PAGE_ID:
			if(ti=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				if(!IsListEmpty(List(ti->ti_Data)))
				{
					if(wo=(piObject *)FindName(List(&GUI->gui_List_Window),GetHead(List(ti->ti_Data))->ln_Name))
					{
						nw=(struct Node_Window *)OBJECT_DATA(wo);
						w=nw->nw_Width+10-Scr->WBorLeft-(nw->nw_Flags&WFLG_SIZEGADGET?18:Scr->WBorRight);
						h=nw->nw_Height+Scr->WBorBottom-(Scr->RastPort.TxHeight+Scr->WBorTop+1)-4;
						FPrintf(handle,"struct BevelBox BBOX_%s=\n{\n	NULL, %ld, %ld, %ld, %ld, BBFLG_BACKFILL|BBFLG_FRAMEBUTTON, 0, 0, 0, 0\n};\n\n",ng->ng_Label,ng->ng_X-5,ng->ng_Y+ng->ng_Height,w,h);
					}
					for(n=GetHead(List(ti->ti_Data));GetSucc(n);n=GetSucc(n))
					{
						if(wo=(piObject *)FindName(List(&GUI->gui_List_Window),n->ln_Name))
						{
							nw=(struct Node_Window *)OBJECT_DATA(wo);
							if(!IsListEmpty(List(&nw->nw_List_Frame)))
							{
								FPrintf(handle,"struct BevelBox BBOX_%s_%s[]=\n{\n",ng->ng_Label,n->ln_Name);
								for(a=0,fo=(piObject *)GetHead(List(&nw->nw_List_Frame));GetSucc(fo);fo=(piObject *)GetSucc(fo),a++)
								{
									nf=(struct Node_Frame *)OBJECT_DATA(fo);
									if(!GetSucc(GetSucc(fo))) FPuts(handle,"	NULL, ");
									else FPrintf(handle,"	&BBOX_%s_%s[%ld], ",ng->ng_Label,n->ln_Name,a+1);
									FPrintf(handle,"%ld, %ld, %ld, %ld, ",ng->ng_X+nf->nf_X,ng->ng_Y+nf->nf_Y+ng->ng_Height,(nf->nf_FrameType==FTYPE_VERTICALLINE)?1L:nf->nf_Width,(nf->nf_FrameType==FTYPE_HORIZONTALLINE)?1L:nf->nf_Height);
									switch(nf->nf_FrameType)
									{
										case FTYPE_BUTTON: FPuts(handle,"BBFLG_FRAMEBUTTON"); break;
										case FTYPE_RIDGE: FPuts(handle,"BBFLG_FRAMERIDGE"); break;
										case FTYPE_HORIZONTALLINE: FPuts(handle,"BBFLG_FRAMEBUTTON"); break;
										case FTYPE_VERTICALLINE: FPuts(handle,"BBFLG_FRAMEBUTTON"); break;
									}
									GetFlag(nf->nf_Flags,MYFR_Recessed) FPuts(handle,"|BBFLG_RECESSED, ");
									else FPuts(handle,", ");
									FPuts(handle,"0, 0, 0, 0,\n");
								}
								Seek(handle,-2,OFFSET_CURRENT);
								FPuts(handle,"\n};\n\n");
							}
						}
					}
					FPrintf(handle,"struct Page PAGES_%s[]=\n{\n",ng->ng_Label);
					for(a=0,n=GetHead(List(ti->ti_Data));GetSucc(n);n=GetSucc(n),a++)
					{
						if(wo=(piObject *)FindName(List(&GUI->gui_List_Window),n->ln_Name))
						{
							nw=(struct Node_Window *)OBJECT_DATA(wo);
							if(!GetSucc(GetSucc(n))) FPuts(handle,"	NULL, ");
							else FPrintf(handle,"\t&PAGES_%s[%ld], ",ng->ng_Label,a+1);
							FPrintf(handle,"(STRPTR) MSG_%s_L%ld_GAD, FS_NORMAL, NULL, NULL, ",ng->ng_Label,a);
							if(wo=(piObject *)FindName(List(&GUI->gui_List_Window),n->ln_Name))
							{
								nw=(struct Node_Window *)OBJECT_DATA(wo);
								if(IsListEmpty(List(&nw->nw_List_Frame))) FPuts(handle,"NULL, ");
								else FPrintf(handle,"BBOX_%s_%s, ",ng->ng_Label,n->ln_Name);
							}
						}
						FPuts(handle,"NULL, NULL,\n");
					}
					Seek(handle,-2,OFFSET_CURRENT);
					FPuts(handle,"\n};\n\n");
				}
			}
			break;
		case POPUPMENU_ID:
			if(ti=FindTagItem(PUMG_Labels,ng->ng_Tags))
			{
				if(!IsListEmpty(List(ti->ti_Data)))
				{
					FPrintf(handle,"extern struct MinList LABELS_%s;\n",ng->ng_Label);
					FPrintf(handle,"struct Node NODES_%s[]=\n{\n",ng->ng_Label);
					for(a=0,n=GetHead(ti->ti_Data);GetSucc(n);n=GetSucc(n),a++)
					{
						if(!GetSucc(GetSucc(n))) FPrintf(handle,"(struct Node *)&LABELS_%s.mlh_Tail, ",ng->ng_Label);
						else FPrintf(handle,"	&NODES_%s[%ld], ",ng->ng_Label,a+1);
						if(!GetPred(GetPred(n))) FPrintf(handle,"(struct Node *)&LABELS_%s.mlh_Head, ",ng->ng_Label);
						else FPrintf(handle,"	&NODES_%s[%ld], ",ng->ng_Label,a-1);
						FPrintf(handle,"0, 0, (STRPTR) MSG_%s_L%ld_GAD,\n",ng->ng_Label,a);
					}
					Seek(handle,-2,OFFSET_CURRENT);
					FPuts(handle,"\n};\n\n");
					FPrintf(handle,"struct MinList LABELS_%s={\n	(struct MinNode *)&NODES_%s[0], NULL, (struct MinNode *)&NODES_%s[%ld]\n};\n",ng->ng_Label,ng->ng_Label,ng->ng_Label,a);
				}
			}
			break;
	}
} /* GADGET_GenerateStruct */

void GADGET_GenerateTags(BPTR handle,piObject *go)
{
	piObject *wo2,*go2;
	struct Node_Gadget *ng,*ng2,*ngp=NULL;
	struct Node_Window *nw,*nw2;
	struct Node *n;
	struct GadToolsData *gtd;
	struct TagItem *t;
	LONG x,y,w,h;
	UWORD offl,offt,offr,offb;

	ng=(struct Node_Gadget *)OBJECT_DATA(go);
	nw=(struct Node_Window *)OBJECT_DATA(ng->ng_Window);
	gtd=(struct GadToolsData *)GadgetData(ng);

	if(nw->nw_PageGadget)
	{
		for(wo2=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo2);wo2=(piObject *)GetSucc(wo2))
		{
			nw2=(struct Node_Window *)OBJECT_DATA(wo2);
			for(go2=(piObject *)GetHead(List(&nw2->nw_List_Gadget));GetSucc(go2);go2=(piObject *)GetSucc(go2))
			{
				ng2=(struct Node_Gadget *)OBJECT_DATA(go2);
				if(ng2->ng_PlugInID==PAGE_ID)
					for(n=GetHead(List(GetTagData(TAG_USER+1,0,ng2->ng_OwnTags)));GetSucc(n);n=GetSucc(n))
						if(!strcmp(nw->nw_Label,n->ln_Name))
						{
							ngp=ng2;
						}
			}
		}
	}

	FPuts(handle,"	");
	switch(ng->ng_PlugInID)
	{
		case BUTTON_ID:
			FPuts(handle,"BUTTON_KIND, NULL,"); break;
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GA_Immediate,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Immediate, TRUE, ");
			if(t=FindTagItem(TAG_USER+2,ng->ng_OwnTags)) FPrintf(handle,"GA_ToggleSelect, %s, ",t->ti_Data?"TRUE":"FALSE");
			switch(FindTagItem(TAG_USER+1,ng->ng_OwnTags)->ti_Data)
			{
				case 0: FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label); break;
				case 1: FPrintf(handle,"GA_LabelImage, (ULONG) &%sTbr, ",gtd->g_Text); break;
				case 2: FPrintf(handle,"GA_LabelImage, (ULONG) &%sImg, ",gtd->g_Text); break;
			}
			break;
		case CHECKBOX_ID:
			FPuts(handle,"CHECKBOX_KIND, NULL,");
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GTCB_Checked,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GTCB_Checked, TRUE, ");
			if(t=FindTagItem(GTCB_Scaled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GTCB_Scaled, TRUE, ");
			FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label);
			break;
		case COLORWHEEL_ID:
			FPuts(handle,"COLORWHEEL_KIND, NULL, ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GA_FollowMouse,ng->ng_Tags)) FPrintf(handle,"GA_FollowMouse, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(WHEEL_BevelBox,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"WHEEL_BevelBox, TRUE, ");
			if(t=FindTagItem(WHEEL_Abbrv,ng->ng_Tags)) if(((STRPTR)t->ti_Data)[0]) FPrintf(handle,"WHEEL_Abbrv, (ULONG) \"%s\", ",t->ti_Data);
			if(t=FindTagItem(WHEEL_MaxPens,ng->ng_Tags)) if(t->ti_Data) FPrintf(handle,"WHEEL_MaxPens, %ld, ",t->ti_Data);
			if(t=FindTagItem(WHEEL_GradientSlider,ng->ng_Tags)) if(t->ti_Data) FPrintf(handle,"WHEEL_GradientSlider, %sGadgets[GA_%s], ",ng->ng_Label,t->ti_Data);
			FPuts(handle,"WHEEL_Screen, NULL, ");
			break;
		case CYCLE_ID:
			FPuts(handle,"CYCLE_KIND, NULL,");
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GTCY_Active,ng->ng_Tags)) FPrintf(handle,"GTCY_Active, %ld, ",t->ti_Data);
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags)) if(t->ti_Data) if(!IsListEmpty(List(t->ti_Data))) FPrintf(handle,"GTCY_Labels, (ULONG) &LABELS_%s, ",ng->ng_Label);
			FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label);
			break;
		case DATATYPE_ID:
			FPuts(handle,"DATATYPE_KIND, NULL, ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags)) if(t->ti_Data)
				{
					if(((STRPTR)t->ti_Data)[0]) FPrintf(handle,"GA_Text, \"%s\", ",t->ti_Data);
					else FPuts(handle,"GA_Text, NULL, ");
				}
				else FPuts(handle,"GA_Text, NULL, ");
			break;
		case GRADIENTSLIDER_ID:
			FPuts(handle,"GRADIENTSLIDER_KIND, NULL, ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GRAD_MaxVal,ng->ng_Tags)) FPrintf(handle,"GRAD_MaxVal, 0x%lx, ",t->ti_Data);
			if(t=FindTagItem(GRAD_CurVal,ng->ng_Tags)) FPrintf(handle,"GRAD_CurVal, 0x%lx, ",t->ti_Data);
			if(t=FindTagItem(GRAD_SkipVal,ng->ng_Tags)) FPrintf(handle,"GRAD_SkipVal, 0x%lx, ",t->ti_Data);
			if(t=FindTagItem(GRAD_KnobPixels,ng->ng_Tags)) FPrintf(handle,"GRAD_KnobPixels, %ld, ",t->ti_Data);
			if(t=FindTagItem(GRAD_PenArray,ng->ng_Tags)) FPrintf(handle,"GRAD_PenArray, NULL, ");
//			if(t=FindTagItem(GRAD_PenArray,ng->ng_Tags)) FPrintf(handle,"GRAD_PenArray, PENS_%s, ",ng->ng_Label);
			if(t=FindTagItem(PGA_Freedom,ng->ng_Tags))
			{
				FPuts(handle,"PGA_Freedom, ");
				switch(t->ti_Data)
				{
					case LORIENT_HORIZ: FPuts(handle,"LORIENT_HORIZ"); break;
					case LORIENT_VERT: FPuts(handle,"LORIENT_VERT"); break;
				}
				FPuts(handle,", ");
			}
			break;
		case INTEGER_ID:
			FPuts(handle,"INTEGER_KIND, NULL,");
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GA_Immediate,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Immediate, TRUE, ");
			if(t=FindTagItem(GA_TabCycle,ng->ng_Tags)) FPrintf(handle,"GA_TabCycle, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(GTIN_Number,ng->ng_Tags)) FPrintf(handle,"GTIN_Number, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTIN_MaxChars,ng->ng_Tags)) FPrintf(handle,"GTIN_MaxChars, %ld, ",t->ti_Data);
			if(t=FindTagItem(STRINGA_ExitHelp,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"STRINGA_ExitHelp, TRUE, ");
			if(t=FindTagItem(STRINGA_ReplaceMode,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"STRINGA_ReplaceMode, TRUE, ");
			if(t=FindTagItem(STRINGA_Justification,ng->ng_Tags))
			{
				FPuts(handle,"STRINGA_Justification, ");
				switch(t->ti_Data)
				{
					case STRINGLEFT: FPuts(handle,"STRINGLEFT"); break;
					case STRINGCENTER: FPuts(handle,"STRINGCENTER"); break;
					case STRINGRIGHT: FPuts(handle,"STRINGRIGHT"); break;
				}
				FPuts(handle,", ");
			}
			FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label);
			break;
		case LISTVIEW_ID:
			FPuts(handle,"LISTVIEW_KIND, NULL,");
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GTLV_ReadOnly,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GTLV_ReadOnly, TRUE, ");
			if(t=FindTagItem(GTLV_Labels,ng->ng_Tags)) if(t->ti_Data) if(!IsListEmpty(List(t->ti_Data))) FPrintf(handle,"GTLV_Labels, (ULONG) &LABELS_%s, ",ng->ng_Label);
			if(t=FindTagItem(GTLV_ShowSelected,ng->ng_Tags)) FPuts(handle,"GTLV_ShowSelected, NULL, ");
			if(t=FindTagItem(LAYOUTA_Spacing,ng->ng_Tags)) if(t->ti_Data) FPrintf(handle,"LAYOUTA_Spacing, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTLV_ScrollWidth,ng->ng_Tags)) if(t->ti_Data) FPrintf(handle,"GTLV_ScrollWidth, %ld, ",t->ti_Data);
			FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label);
			break;
		case MX_ID:
			FPuts(handle,"MX_KIND, NULL,");
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags)) if(t->ti_Data) if(!IsListEmpty(List(t->ti_Data))) FPrintf(handle,"GTMX_Labels, (ULONG) &LABELS_%s, ",ng->ng_Label);
			if(t=FindTagItem(GTMX_Active,ng->ng_Tags)) FPrintf(handle,"GTMX_Active, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTMX_Scaled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GTMX_Scaled, TRUE, ");
			if(t=FindTagItem(GTMX_Spacing,ng->ng_Tags)) FPrintf(handle,"GTMX_Spacing, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTMX_TitlePlace,ng->ng_Tags))
			{
				FPuts(handle,"GTMX_TitlePlace, ");
				switch(t->ti_Data)
				{
					case PLACETEXT_LEFT: FPuts(handle,"PLACETEXT_LEFT"); break;
					case PLACETEXT_RIGHT: FPuts(handle,"PLACETEXT_RIGHT"); break;
					case PLACETEXT_ABOVE: FPuts(handle,"PLACETEXT_ABOVE"); break;
					case PLACETEXT_BELOW: FPuts(handle,"PLACETEXT_BELOW"); break;
				}
				FPuts(handle,", ");
			}
			FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label);
			break;
		case NUMBER_ID:
			FPuts(handle,"NUMBER_KIND, NULL,");
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GTNM_Number,ng->ng_Tags)) FPrintf(handle,"GTNM_Number, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTNM_Border,ng->ng_Tags)) FPrintf(handle,"GTNM_Border, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(GTNM_Clipped,ng->ng_Tags)) FPrintf(handle,"GTNM_Clipped, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(GTNM_Format,ng->ng_Tags)) FPrintf(handle,"GTNM_Format, (ULONG) MSG_%s_FORMAT_GAD, ",ng->ng_Label);
			if(t=FindTagItem(GTNM_MaxNumberLen,ng->ng_Tags)) FPrintf(handle,"GTNM_MaxNumberLen, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTNM_Justification,ng->ng_Tags))
			{
				FPuts(handle,"GTNM_Justification, ");
				switch(t->ti_Data)
				{
					case GTJ_LEFT: FPuts(handle,"GTJ_LEFT"); break;
					case GTJ_CENTER: FPuts(handle,"GTJ_CENTER"); break;
					case GTJ_RIGHT: FPuts(handle,"GTJ_RIGHT"); break;
				}
				FPuts(handle,", ");
			}
			FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label);
			break;
		case PAGE_ID:
			FPuts(handle,"PAGE_KIND, NULL, ");
			FPuts(handle,"GA_RelVerify, TRUE, ");
			if(t=FindTagItem(PAGE_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(PAGE_Pages,ng->ng_Tags)) FPrintf(handle,"PAGE_Pages, (ULONG) PAGES_%s, ",ng->ng_Label);
			if(t=FindTagItem(PAGE_Active,ng->ng_Tags)) FPrintf(handle,"PAGE_Active, %ld, ",t->ti_Data);
			if(t=FindTagItem(PAGE_Layout,ng->ng_Tags))
			{
				FPuts(handle,"PAGE_Layout, ");
				switch(t->ti_Data)
				{
					case GTJ_LEFT: FPuts(handle,"PGLYT_SIMPLE"); break;
					case GTJ_CENTER: FPuts(handle,"PGLYT_MUISTYLE"); break;
					case GTJ_RIGHT: FPuts(handle,"PGLYT_GADTOOLS"); break;
				}
				FPuts(handle,", ");
			}
			if(t=FindTagItem(PAGE_ActiveStyle,ng->ng_Tags))
			{
				FPuts(handle,"PAGE_ActiveStyle, ");
				switch(t->ti_Data)
				{
					case GTJ_LEFT: FPuts(handle,"PGAST_NORMAL"); break;
					case GTJ_CENTER: FPuts(handle,"PGAST_HILIGHT"); break;
					case GTJ_RIGHT: FPuts(handle,"PGAST_SHADOW"); break;
				}
				FPuts(handle,", ");
			}
			FPrintf(handle,"PAGE_ClearBBox, (ULONG) &BBOX_%s, ",ng->ng_Label);
			break;
		case PALETTE_ID:
			FPuts(handle,"PALETTE_KIND, NULL,");
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GTPA_Depth,ng->ng_Tags)) FPrintf(handle,"GTPA_Depth, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTPA_Color,ng->ng_Tags)) FPrintf(handle,"GTPA_Color, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTPA_ColorOffset,ng->ng_Tags)) if(t->ti_Data) FPrintf(handle,"GTPA_ColorOffset, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTPA_IndicatorWidth,ng->ng_Tags)) if(t->ti_Data) FPrintf(handle,"GTPA_IndicatorWidth, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTPA_IndicatorHeight,ng->ng_Tags)) if(t->ti_Data) FPrintf(handle,"GTPA_IndicatorHeight, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTPA_NumColors,ng->ng_Tags)) FPrintf(handle,"GTPA_NumColors, %ld, ",t->ti_Data);
			FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label);
			break;
		case POPUPMENU_ID:
			FPuts(handle,"POPUPMENU_KIND, NULL, ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(PUMG_Labels,ng->ng_Tags)) if(t->ti_Data) if(!IsListEmpty(List(t->ti_Data))) FPrintf(handle,"PUMG_Labels, (ULONG) &LABELS_%s, ",ng->ng_Label);
			if(t=FindTagItem(PUMG_Active,ng->ng_Tags)) FPrintf(handle,"PUMG_Active, %ld, ",t->ti_Data);
			if(t=FindTagItem(PUMG_NewLook,ng->ng_Tags)) FPrintf(handle,"PUMG_NewLook, %s, ",t->ti_Data?"TRUE":"FALSE");
			break;
		case PROGRESS_ID:
			FPuts(handle,"PROGRESS_KIND, NULL, ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(PROG_MaxVal,ng->ng_Tags)) FPrintf(handle,"PROG_MaxVal, %ld, ",t->ti_Data);
			if(t=FindTagItem(PROG_CurVal,ng->ng_Tags)) FPrintf(handle,"PROG_CurVal, %ld, ",t->ti_Data);
			if(t=FindTagItem(PROG_ConvertToPercents,ng->ng_Tags)) FPrintf(handle,"PROG_ConvertToPercents, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(PROG_Format,ng->ng_Tags)) FPrintf(handle,"PROG_Format, (ULONG) MSG_%s_FORMAT_GAD, ",ng->ng_Label);
			if(t=FindTagItem(PROG_Border,ng->ng_Tags)) FPrintf(handle,"PROG_Border, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(PROG_Placement,ng->ng_Tags))
			{
				FPuts(handle,"PROG_Placement, ");
				switch(t->ti_Data)
				{
					case PLACETEXT_IN: FPuts(handle,"PLACETEXT_IN"); break;
					case PLACETEXT_LEFT: FPuts(handle,"PLACETEXT_LEFT"); break;
					case PLACETEXT_RIGHT: FPuts(handle,"PLACETEXT_RIGHT"); break;
					case PLACETEXT_ABOVE: FPuts(handle,"PLACETEXT_ABOVE"); break;
					case PLACETEXT_BELOW: FPuts(handle,"PLACETEXT_BELOW"); break;
				}
				FPuts(handle,", ");
			}
			break;
		case SCROLLER_ID:
			FPuts(handle,"SCROLLER_KIND, NULL,");
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GA_Immediate,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Immediate, TRUE, ");
			if(t=FindTagItem(GA_RelVerify,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_RelVerify, TRUE, ");
			if(t=FindTagItem(GTSC_Top,ng->ng_Tags)) FPrintf(handle,"GTSC_Top, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTSC_Total,ng->ng_Tags)) FPrintf(handle,"GTSC_Total, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTSC_Visible,ng->ng_Tags)) FPrintf(handle,"GTSC_Visible, %ld, ",t->ti_Data);
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
				switch(t->ti_Data)
				{
					case 0: break; // None
					case 1: if(t=FindTagItem(GTSC_Arrows,ng->ng_Tags)) if(t) FPrintf(handle,"GTSC_Arrows, %ld, ",t->ti_Data); break;
					case 2: FPrintf(handle,"GTSC_Arrows, %ld, ",ng->ng_Height); break;
				}
			if(t=FindTagItem(PGA_Freedom,ng->ng_Tags))
			{
				FPuts(handle,"PGA_Freedom, ");
				switch(t->ti_Data)
				{
					case LORIENT_HORIZ: FPuts(handle,"LORIENT_HORIZ"); break;
					case LORIENT_VERT: FPuts(handle,"LORIENT_VERT"); break;
				}
				FPuts(handle,", ");
			}
			FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label);
			break;
		case SLIDER_ID:
			FPuts(handle,"SLIDER_KIND, NULL,");
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GA_Immediate,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Immediate, TRUE, ");
			if(t=FindTagItem(GA_RelVerify,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_RelVerify, TRUE, ");
			if(t=FindTagItem(GTSL_Min,ng->ng_Tags)) FPrintf(handle,"GTSL_Min, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTSL_Max,ng->ng_Tags)) FPrintf(handle,"GTSL_Max, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTSL_Level,ng->ng_Tags)) FPrintf(handle,"GTSL_Level, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTSL_MaxLevelLen,ng->ng_Tags)) FPrintf(handle,"GTSL_MaxLevelLen, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTSL_MaxPixelLen,ng->ng_Tags)) FPrintf(handle,"GTSL_MaxPixelLen, %ld, ",t->ti_Data);
			if(t=FindTagItem(GTSL_LevelFormat,ng->ng_Tags)) FPrintf(handle,"GTSL_LevelFormat, (ULONG) MSG_%s_FORMAT_GAD, ",ng->ng_Label);
			if(t=FindTagItem(GTSL_LevelPlace,ng->ng_Tags))
			{
				FPuts(handle,"GTSL_LevelPlace, ");
				switch(t->ti_Data)
				{
					case PLACETEXT_LEFT: FPuts(handle,"PLACETEXT_LEFT"); break;
					case PLACETEXT_RIGHT: FPuts(handle,"PLACETEXT_RIGHT"); break;
					case PLACETEXT_ABOVE: FPuts(handle,"PLACETEXT_ABOVE"); break;
					case PLACETEXT_BELOW: FPuts(handle,"PLACETEXT_BELOW"); break;
				}
				FPuts(handle,", ");
			}
			if(t=FindTagItem(GTSL_Justification,ng->ng_Tags))
			{
				FPuts(handle,"GTSL_Justification, ");
				switch(t->ti_Data)
				{
					case GTJ_LEFT: FPuts(handle,"GTJ_LEFT"); break;
					case GTJ_CENTER: FPuts(handle,"GTJ_CENTER"); break;
					case GTJ_RIGHT: FPuts(handle,"GTJ_RIGHT"); break;
				}
				FPuts(handle,", ");
			}
			if(t=FindTagItem(PGA_Freedom,ng->ng_Tags))
			{
				FPuts(handle,"PGA_Freedom, ");
				switch(t->ti_Data)
				{
					case LORIENT_HORIZ: FPuts(handle,"LORIENT_HORIZ"); break;
					case LORIENT_VERT: FPuts(handle,"LORIENT_VERT"); break;
				}
				FPuts(handle,", ");
			}
			FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label);
			break;
		case STRING_ID:
			FPuts(handle,"STRING_KIND, NULL,");
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GA_Immediate,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Immediate, TRUE, ");
			if(t=FindTagItem(GA_TabCycle,ng->ng_Tags)) FPrintf(handle,"GA_TabCycle, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(GTST_String,ng->ng_Tags)) if(((char *)t->ti_Data)[0]) FPrintf(handle,"GTST_String, (ULONG) MSG_%s_STRING_GAD, ",ng->ng_Label);
			if(t=FindTagItem(GTST_MaxChars,ng->ng_Tags)) FPrintf(handle,"GTST_MaxChars, %ld, ",t->ti_Data);
			if(t=FindTagItem(STRINGA_ExitHelp,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"STRINGA_ExitHelp, TRUE, ");
			if(t=FindTagItem(STRINGA_ReplaceMode,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"STRINGA_ReplaceMode, TRUE, ");
			if(t=FindTagItem(STRINGA_Justification,ng->ng_Tags))
			{
				FPuts(handle,"STRINGA_Justification, ");
				switch(t->ti_Data)
				{
					case STRINGLEFT: FPuts(handle,"STRINGLEFT"); break;
					case STRINGCENTER: FPuts(handle,"STRINGCENTER"); break;
					case STRINGRIGHT: FPuts(handle,"STRINGRIGHT"); break;
				}
				FPuts(handle,", ");
			}
			FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label);
			break;
		case TAPEDECK_ID:
			FPuts(handle,"TAPEDECK_KIND, NULL, ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(TDECK_Paused,ng->ng_Tags)) FPrintf(handle,"TDECK_Paused, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TDECK_Tape,ng->ng_Tags)) FPrintf(handle,"TDECK_Tape, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TDECK_Tape,ng->ng_Tags))
			{
				if(t=FindTagItem(TDECK_Frames,ng->ng_Tags)) FPrintf(handle,"TDECK_Frames, %ld, ",t->ti_Data);
				if(t=FindTagItem(TDECK_CurrentFrame,ng->ng_Tags)) FPrintf(handle,"TDECK_CurrentFrame, %ld, ",t->ti_Data);
			}
			break;
		case TEXT_ID:
			FPuts(handle,"TEXT_KIND, NULL,");
			FPuts(handle,"GT_PlaceText, %s,\n",WriteNewGadgetFlags(handle,gtd->g_Flags));
			if(t=FindTagItem(GT_Underscore,ng->ng_Tags)) FPuts(handle,"GT_Underscore, '_', ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(GTTX_Text,ng->ng_Tags)) if(((char *)t->ti_Data)[0]) FPrintf(handle,"GTTX_Text, (ULONG) MSG_%s_STRING_GAD, ",ng->ng_Label);
			if(t=FindTagItem(GTTX_Border,ng->ng_Tags)) FPrintf(handle,"GTTX_Border, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(GTTX_Clipped,ng->ng_Tags)) FPrintf(handle,"GTTX_Clipped, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(GTTX_CopyText,ng->ng_Tags)) FPrintf(handle,"GTTX_CopyText, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(GTTX_Justification,ng->ng_Tags))
			{
				FPuts(handle,"GTTX_Justification, ");
				switch(t->ti_Data)
				{
					case GTJ_LEFT: FPuts(handle,"GTJ_LEFT"); break;
					case GTJ_CENTER: FPuts(handle,"GTJ_CENTER"); break;
					case GTJ_RIGHT: FPuts(handle,"GTJ_RIGHT"); break;
				}
				FPuts(handle,", ");
			}
			FPrintf(handle,"GA_Text, (ULONG) MSG_%s_GAD, ",ng->ng_Label);
			break;
		case TEXTFIELD_ID:
			FPuts(handle,"TEXTFIELD_KIND, NULL, ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) FPrintf(handle,"GA_Disabled, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_ReadOnly,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_ReadOnly, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_Partial,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_Partial, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_BlockCursor,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_BlockCursor, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_VCenter,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_VCenter, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_PassCommand,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_PassCommand, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_NonPrintChars,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_NonPrintChars, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_MaxSizeBeep,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_MaxSizeBeep, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_NoGhost,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_NoGhost, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_Modified,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_Modified, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_UserAlign,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_UserAlign, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_RuledPaper,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_RuledPaper, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_Inverted,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_Inverted, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(GA_TabCycle,ng->ng_Tags)) FPrintf(handle,"GA_TabCycle, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(TEXTFIELD_Delimiters,ng->ng_Tags)) if(((STRPTR)t->ti_Data)[0]) FPrintf(handle,"TEXTFIELD_Delimiters, (ULONG) \"%s\", ",t->ti_Data);
			if(t=FindTagItem(TEXTFIELD_AcceptChars,ng->ng_Tags)) if(((STRPTR)t->ti_Data)[0]) FPrintf(handle,"TEXTFIELD_AcceptChars, (ULONG) \"%s\", ",t->ti_Data);
			if(t=FindTagItem(TEXTFIELD_RejectChars,ng->ng_Tags)) if(((STRPTR)t->ti_Data)[0]) FPrintf(handle,"TEXTFIELD_RejectChars, (ULONG) \"%s\", ",t->ti_Data);
			if(t=FindTagItem(TEXTFIELD_BlinkRate,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_BlinkRate, %ld, ",t->ti_Data);
			if(t=FindTagItem(TEXTFIELD_MaxSize,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_MaxSize, %ld, ",t->ti_Data);
			if(t=FindTagItem(TEXTFIELD_TabSpaces,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_TabSpaces, %ld, ",t->ti_Data);
			if(t=FindTagItem(TEXTFIELD_Spacing,ng->ng_Tags)) FPrintf(handle,"TEXTFIELD_Spacing, %ld, ",t->ti_Data);
			if(t=FindTagItem(TEXTFIELD_Border,ng->ng_Tags))
			{
				FPuts(handle,"TEXTFIELD_Border, ");
				switch(t->ti_Data)
				{
					case TEXTFIELD_BORDER_NONE: FPuts(handle,"TEXTFIELD_BORDER_NONE"); break;
					case TEXTFIELD_BORDER_BEVEL: FPuts(handle,"TEXTFIELD_BORDER_BEVEL"); break;
					case TEXTFIELD_BORDER_DOUBLEBEVEL: FPuts(handle,"TEXTFIELD_BORDER_DOUBLEBEVEL"); break;
				}
				FPuts(handle,", ");
			}
			if(t=FindTagItem(TEXTFIELD_Alignment,ng->ng_Tags))
			{
				FPuts(handle,"TEXTFIELD_Alignment, ");
				switch(t->ti_Data)
				{
					case TEXTFIELD_ALIGN_LEFT: FPuts(handle,"TEXTFIELD_ALIGN_LEFT"); break;
					case TEXTFIELD_ALIGN_RIGHT: FPuts(handle,"TEXTFIELD_ALIGN_RIGHT"); break;
					case TEXTFIELD_ALIGN_CENTER: FPuts(handle,"TEXTFIELD_ALIGN_CENTER"); break;
				}
				FPuts(handle,", ");
			}
			break;
		case TREEVIEW_ID:
			FPuts(handle,"TREEVIEW_KIND, NULL, ");
			FPuts(handle,"GA_RelVerify, TRUE, ");
			FPuts(handle,"GA_Immediate, TRUE, ");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GA_Disabled, TRUE, ");
			if(t=FindTagItem(LIST_Labels,ng->ng_Tags)) FPrintf(handle,"LIST_Labels, NULL, ");
			if(t=FindTagItem(LIST_ReadOnly,ng->ng_Tags)) if(t->ti_Data) FPrintf(handle,"LIST_ReadOnly, TRUE, ");
			if(t=FindTagItem(LIST_Border,ng->ng_Tags)) FPrintf(handle,"LIST_Border, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(LIST_MultiSelect,ng->ng_Tags)) FPrintf(handle,"LIST_MultiSelect, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(LIST_ShowTreeLines,ng->ng_Tags)) FPrintf(handle,"LIST_ShowTreeLines, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(LIST_IsTree,ng->ng_Tags)) FPrintf(handle,"LIST_IsTree, %s, ",t->ti_Data?"TRUE":"FALSE");
			if(t=FindTagItem(LIST_Columns,ng->ng_Tags)) FPrintf(handle,"LIST_Columns, %ld, ",t->ti_Data);
			if(t=FindTagItem(LIST_Format,ng->ng_Tags)) if(((char *)t->ti_Data)[0]) FPrintf(handle,"LIST_Format, (ULONG) MSG_%s_FORMAT_GAD, ",ng->ng_Label);
			break;
	}

	NWCalcLeftTop(&offl,&offt,nw);
	NWCalcRightBottom(&offr,&offb,nw);

	if(ng->ng_RelX) x=(LONG)((WORD)-(nw->nw_Width-offl-ng->ng_X-1));
	else x=(LONG)((WORD)ng->ng_X);
	if(ng->ng_RelY) y=(LONG)((WORD)-(nw->nw_Height-offt-ng->ng_Y-1));
	else y=(LONG)((WORD)ng->ng_Y);
	if(ng->ng_RelWidth) w=(LONG)((WORD)-(nw->nw_Width-ng->ng_Width-offl-offr-ng->ng_X-1));
	else w=(LONG)((WORD)ng->ng_Width+1);
	if(ng->ng_RelHeight) h=(LONG)((WORD)-(nw->nw_Height-ng->ng_Height-offt-offb-ng->ng_Y-1));
	else h=(LONG)((WORD)ng->ng_Height+1);
	if(ngp)
	{
		x+=(LONG)((WORD)ngp->ng_X);
		y+=(LONG)((WORD)(ngp->ng_Y+ngp->ng_Height));
	}

	if(ng->ng_RelX) FPrintf(handle,"GA_Right, %ld, ",x);
	else FPrintf(handle,"GA_Left, %ld, ",x);
	if(ng->ng_RelY) FPrintf(handle,"GA_Bottom, %ld, ",y);
	else FPrintf(handle,"GA_Top, %ld, ",y);
	if(ng->ng_RelWidth) FPrintf(handle,"GA_RelWidth, %ld, ",w);
	else FPrintf(handle,"GA_Width, %ld, ",w);
	if(ng->ng_RelHeight) FPrintf(handle,"GA_RelHeight, %ld, ",h);
	else FPrintf(handle,"GA_Height, %ld, ",h);

	if((ng->ng_PlugInID!=TEXT_ID) && (ng->ng_PlugInID!=NUMBER_ID) && (ng->ng_PlugInID!=PROGRESS_ID))
		FPrintf(handle,"GA_UserData, (ULONG) FN_%s_Clicked, ",ng->ng_Label);

	FPuts(handle,"TAG_DONE,\n");
} /* GADGET_GenerateTags */

void GADGET_GenerateOpenLib(BPTR handle,ULONG id)
{
	switch(id)
	{
		case COLORWHEEL_ID:
			FPuts(handle,"	if(!(ColorWheelBase=OpenLibrary(\"gadgets/colorwheel.gadget\",39))) return GTBERR_OPENLIB_COLORWHEEL;\n");
			break;
		case GRADIENTSLIDER_ID:
			FPuts(handle,"	if(!(GradientSliderBase=OpenLibrary(\"gadgets/gradientslider.gadget\",39))) return GTBERR_OPENLIB_GRADIENTSLIDER;\n");
			break;
		case TAPEDECK_ID:
			FPuts(handle,"	if(!(TapeDeckBase=OpenLibrary(\"gadgets/tapedeck.gadget\",39))) return GTBERR_OPENLIB_TAPEDECK;\n");
			break;
		case PAGE_ID:
			FPuts(handle,"	if(!(PageBase=OpenLibrary(\"gadgets/page.gadget\",1))) return GTBERR_OPENLIB_PAGE;\n");
			FPuts(handle,"	PageClass=PAGE_GetClass();\n");
			break;
		case POPUPMENU_ID:
			FPuts(handle,"	if(!(PopUpMenuClass=CreatePopUpMenuClass())) return GTBERR_OPENLIB_POPUPMENU;\n");
			break;
		case PROGRESS_ID:
			FPuts(handle,"	if(!(ProgressBase=OpenLibrary(\"gadgets/progress.gadget\",37))) return GTBERR_OPENLIB_PROGRESS;\n");
			FPuts(handle,"	ProgressClass=GetProgressClass();\n");
			break;
		case TEXTFIELD_ID:
			FPuts(handle,"	if(!(TextFieldBase=OpenLibrary(\"gadgets/textfield.gadget\",3))) return GTBERR_OPENLIB_TEXTFIELD;\n");
			FPuts(handle,"	TextFieldClass=TEXTFIELD_GetClass();\n");
			break;
		case TREEVIEW_ID:
			FPuts(handle,"	if(!(ListViewBase=OpenLibrary(\"gadgets/listview.gadget\",37))) return GTBERR_OPENLIB_TREEVIEW;\n");
			FPuts(handle,"	ListViewClass=GetListViewClass();\n");
			break;
	}
} /* GADGET_GenerateOpenLib */

void GADGET_GenerateCloseLib(BPTR handle,ULONG id)
{
	switch(id)
	{
		case COLORWHEEL_ID:
			FPuts(handle,"	if(ColorWheelBase) CloseLibrary(ColorWheelBase); ColorWheelBase=NULL;\n");
			break;
		case GRADIENTSLIDER_ID:
			FPuts(handle,"	if(GradientSliderBase) CloseLibrary(GradientSliderBase); GradientSliderBase=NULL;\n");
			break;
		case TAPEDECK_ID:
			FPuts(handle,"	if(TapeDeckBase) CloseLibrary(TapeDeckBase); TapeDeckBase=NULL;\n");
			break;
		case PAGE_ID:
			FPuts(handle,"	if(PageBase) CloseLibrary(PageBase); PageBase=NULL; PageClass=NULL;\n");
			break;
		case POPUPMENU_ID:
			FPuts(handle,"	if(PopUpMenuClass) DisposePopUpMenuClass(PopUpMenuClass); PopUpMenuClass=NULL;\n");
			break;
		case PROGRESS_ID:
			FPuts(handle,"	if(ProgressBase) CloseLibrary(ProgressBase); ProgressBase=NULL; ProgressClass=NULL;\n");
			break;
		case TEXTFIELD_ID:
			FPuts(handle,"	if(TextFieldBase) CloseLibrary(TextFieldBase); TextFieldBase=NULL; TextFieldClass=NULL;\n");
			break;
		case TREEVIEW_ID:
			FPuts(handle,"	if(ListViewBase) CloseLibrary(ListViewBase); ListViewBase=NULL; ListViewClass=NULL;\n");
			break;
	}
} /* GADGET_GenerateCloseLib */

void GADGET_GenerateProtos(BPTR handle,piObject *go)
{
	struct Node_Gadget *ng;

	ng=(struct Node_Gadget *)OBJECT_DATA(go);
	switch(ng->ng_PlugInID)
	{
		case NUMBER_ID:
		case TEXT_ID:
		case PROGRESS_ID:
			break;
		default:
			FPrintf(handle,"int FN_%s_Clicked(void);\n",ng->ng_Label);
			break;
	}
} /* GADGET_GenerateProtos */

void GADGET_GenerateTemplate(BPTR handle,piObject *go)
{
	struct Node_Gadget *ng;

	ng=(struct Node_Gadget *)OBJECT_DATA(go);
	switch(ng->ng_PlugInID)
	{
		case NUMBER_ID:
		case TEXT_ID:
		case PROGRESS_ID:
			break;
		default:
			FPrintf(handle,"int FN_%s_Clicked()\n{\n	return 1;\n}\n\n",ng->ng_Label);
			break;
	}
} /* GADGET_GenerateTemplate */

void GADGET_GenerateCatalogSource(BPTR handle,piObject *go,LONG *index)
{
	piObject *wo;
	struct Node_Window *nw;
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	struct Node *n;
	struct TagItem *t;
	LONG a;

	ng=(struct Node_Gadget *)OBJECT_DATA(go);
	gtd=(struct GadToolsData *)GadgetData(ng);
	switch(ng->ng_PlugInID)
	{
		case BUTTON_ID:
			if(FindTagItem(TAG_USER+1,ng->ng_OwnTags)->ti_Data==0)
			{
				FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
				(*index)++;
			}
			break;
		case CHECKBOX_ID:
			FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
			(*index)++;
			break;
		case COLORWHEEL_ID:
			break;
		case CYCLE_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
				(*index)++;
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++,(*index)++)
					FPrintf(handle,"const struct FC_Type _MSG_%s_L%ld_GAD = { %ld, \"%s\" };\n",ng->ng_Label,a,*index,n->ln_Name);
			}
			break;
		case DATATYPE_ID:
			break;
		case GRADIENTSLIDER_ID:
			break;
		case INTEGER_ID:
			FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
			(*index)++;
			break;
		case LISTVIEW_ID:
			if(t=FindTagItem(GTLV_Labels,ng->ng_Tags))
			{
				FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
				(*index)++;
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++,(*index)++)
					FPrintf(handle,"const struct FC_Type _MSG_%s_L%ld_GAD = { %ld, \"%s\" };\n",ng->ng_Label,a,*index,n->ln_Name);
			}
			break;
		case MX_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
				(*index)++;
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++,(*index)++)
					FPrintf(handle,"const struct FC_Type _MSG_%s_L%ld_GAD = { %ld, \"%s\" };\n",ng->ng_Label,a,*index,n->ln_Name);
			}
			break;
		case NUMBER_ID:
			FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
			(*index)++;
			if(t=FindTagItem(GTNM_Format,ng->ng_Tags))
			{
				FPrintf(handle,"const struct FC_Type _MSG_%s_FORMAT_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,t->ti_Data);
				(*index)++;
			}
			break;
		case PAGE_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				for(a=0,n=GetHead(List(t->ti_Data));GetSucc(n);n=GetSucc(n),a++,(*index)++)
				{
					if(wo=(piObject *)FindName(List(&GUI->gui_List_Window),n->ln_Name))
					{
						nw=(struct Node_Window *)OBJECT_DATA(wo);
						FPrintf(handle,"const struct FC_Type _MSG_%s_L%ld_GAD = { %ld, \"%s\" };\n",ng->ng_Label,a,*index,nw->nw_WindowTitle);
					}
				}
			}
			break;
		case PALETTE_ID:
			FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
			(*index)++;
			break;
		case POPUPMENU_ID:
			if(t=FindTagItem(PUMG_Labels,ng->ng_Tags))
			{
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++,(*index)++)
					FPrintf(handle,"const struct FC_Type _MSG_%s_L%ld_GAD = { %ld, \"%s\" };\n",ng->ng_Label,a,*index,n->ln_Name);
			}
			break;
		case PROGRESS_ID:
			if(t=FindTagItem(PROG_Format,ng->ng_Tags))
			{
				FPrintf(handle,"const struct FC_Type _MSG_%s_FORMAT_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,t->ti_Data);
				(*index)++;
			}
			break;
		case SCROLLER_ID:
			FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
			(*index)++;
			break;
		case SLIDER_ID:
			FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
			(*index)++;
			if(t=FindTagItem(GTSL_LevelFormat,ng->ng_Tags))
			{
				FPrintf(handle,"const struct FC_Type _MSG_%s_FORMAT_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,t->ti_Data);
				(*index)++;
			}
			break;
		case STRING_ID:
			FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
			(*index)++;
			if(t=FindTagItem(GTST_String,ng->ng_Tags))
			{
				if(((char *)t->ti_Data)[0])
				{
					FPrintf(handle,"const struct FC_Type _MSG_%s_STRING_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,t->ti_Data);
					(*index)++;
				}
			}
			break;
		case TAPEDECK_ID:
			break;
		case TEXT_ID:
			FPrintf(handle,"const struct FC_Type _MSG_%s_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,gtd->g_Text);
			(*index)++;
			if(t=FindTagItem(GTTX_Text,ng->ng_Tags))
			{
				if(((char *)t->ti_Data)[0])
				{
					FPrintf(handle,"const struct FC_Type _MSG_%s_STRING_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,t->ti_Data);
					(*index)++;
				}
			}
			break;
		case TEXTFIELD_ID:
			break;
		case TREEVIEW_ID:
			if(t=FindTagItem(LIST_Format,ng->ng_Tags))
			{
				if(((char *)t->ti_Data)[0])
				{
					FPrintf(handle,"const struct FC_Type _MSG_%s_FORMAT_GAD = { %ld, \"%s\" };\n",ng->ng_Label,*index,t->ti_Data);
					(*index)++;
				}
			}
			break;
	}
} /* GADGET_GenerateCatalogSource */

void GADGET_GenerateCatalogHeader(BPTR handle,piObject *go)
{
	piObject *wo;
	struct Node_Window *nw;
	struct Node_Gadget *ng;
	struct Node *n;
	struct TagItem *t;
	LONG a;

	ng=(struct Node_Gadget *)OBJECT_DATA(go);
	switch(ng->ng_PlugInID)
	{
		case BUTTON_ID:
			if(FindTagItem(TAG_USER+1,ng->ng_OwnTags)->ti_Data==0)
			{
				FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
				FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
			}
			break;
		case CHECKBOX_ID:
			FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
			FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
			break;
		case COLORWHEEL_ID:
			break;
		case CYCLE_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
				FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
				{
					FPrintf(handle,"extern const struct FC_Type _MSG_%s_L%ld_GAD;\n",ng->ng_Label,a);
					FPrintf(handle,"#define MSG_%s_L%ld_GAD ((APTR) &_MSG_%s_L%ld_GAD)\n",ng->ng_Label,a,ng->ng_Label,a);
				}
			}
			break;
		case DATATYPE_ID:
			break;
		case GRADIENTSLIDER_ID:
			break;
		case INTEGER_ID:
			FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
			FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
			break;
		case LISTVIEW_ID:
			if(t=FindTagItem(GTLV_Labels,ng->ng_Tags))
			{
				FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
				FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
				{
					FPrintf(handle,"extern const struct FC_Type _MSG_%s_L%ld_GAD;\n",ng->ng_Label,a);
					FPrintf(handle,"#define MSG_%s_L%ld_GAD ((APTR) &_MSG_%s_L%ld_GAD)\n",ng->ng_Label,a,ng->ng_Label,a);
				}
			}
			break;
		case MX_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
				FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
				{
					FPrintf(handle,"extern const struct FC_Type _MSG_%s_L%ld_GAD;\n",ng->ng_Label,a);
					FPrintf(handle,"#define MSG_%s_L%ld_GAD ((APTR) &_MSG_%s_L%ld_GAD)\n",ng->ng_Label,a,ng->ng_Label,a);
				}
			}
			break;
		case NUMBER_ID:
			FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
			FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
			if(t=FindTagItem(GTNM_Format,ng->ng_Tags))
			{
				FPrintf(handle,"extern const struct FC_Type _MSG_%s_FORMAT_GAD;\n",ng->ng_Label);
				FPrintf(handle,"#define MSG_%s_FORMAT_GAD ((APTR) &_MSG_%s_FORMAT_GAD)\n",ng->ng_Label,ng->ng_Label);
			}
			break;
		case PAGE_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				for(a=0,n=GetHead(List(t->ti_Data));GetSucc(n);n=GetSucc(n),a++)
				{
					if(wo=(piObject *)FindName(List(&GUI->gui_List_Window),n->ln_Name))
					{
						nw=(struct Node_Window *)OBJECT_DATA(wo);
						FPrintf(handle,"extern const struct FC_Type _MSG_%s_L%ld_GAD;\n",ng->ng_Label,a);
						FPrintf(handle,"#define MSG_%s_L%ld_GAD ((APTR) &_MSG_%s_L%ld_GAD)\n",ng->ng_Label,a,ng->ng_Label,a);
					}
				}
			}
			break;
		case PALETTE_ID:
			FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
			FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
			break;
		case POPUPMENU_ID:
			if(t=FindTagItem(PUMG_Labels,ng->ng_Tags))
			{
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
				{
					FPrintf(handle,"extern const struct FC_Type _MSG_%s_L%ld_GAD;\n",ng->ng_Label,a);
					FPrintf(handle,"#define MSG_%s_L%ld_GAD ((APTR) &_MSG_%s_L%ld_GAD)\n",ng->ng_Label,a,ng->ng_Label,a);
				}
			}
			break;
		case PROGRESS_ID:
			if(t=FindTagItem(PROG_Format,ng->ng_Tags))
			{
				FPrintf(handle,"extern const struct FC_Type _MSG_%s_FORMAT_GAD;\n",ng->ng_Label);
				FPrintf(handle,"#define MSG_%s_FORMAT_GAD ((APTR) &_MSG_%s_FORMAT_GAD)\n",ng->ng_Label,ng->ng_Label);
			}
			break;
		case SCROLLER_ID:
			FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
			FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
			break;
		case SLIDER_ID:
			FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
			FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
			if(t=FindTagItem(GTSL_LevelFormat,ng->ng_Tags))
			{
				FPrintf(handle,"extern const struct FC_Type _MSG_%s_FORMAT_GAD;\n",ng->ng_Label);
				FPrintf(handle,"#define MSG_%s_FORMAT_GAD ((APTR) &_MSG_%s_FORMAT_GAD)\n",ng->ng_Label,ng->ng_Label);
			}
			break;
		case STRING_ID:
			FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
			FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
			if(t=FindTagItem(GTST_String,ng->ng_Tags))
			{
				if(((char *)t->ti_Data)[0])
				{
					FPrintf(handle,"extern const struct FC_Type _MSG_%s_STRING_GAD;\n",ng->ng_Label);
					FPrintf(handle,"#define MSG_%s_STRING_GAD ((APTR) &_MSG_%s_STRING_GAD)\n",ng->ng_Label,ng->ng_Label);
				}
			}
			break;
		case TAPEDECK_ID:
			break;
		case TEXT_ID:
			FPrintf(handle,"extern const struct FC_Type _MSG_%s_GAD;\n",ng->ng_Label);
			FPrintf(handle,"#define MSG_%s_GAD ((APTR) &_MSG_%s_GAD)\n",ng->ng_Label,ng->ng_Label);
			if(t=FindTagItem(GTTX_Text,ng->ng_Tags))
			{
				if(((char *)t->ti_Data)[0])
				{
					FPrintf(handle,"extern const struct FC_Type _MSG_%s_STRING_GAD;\n",ng->ng_Label);
					FPrintf(handle,"#define MSG_%s_STRING_GAD ((APTR) &_MSG_%s_STRING_GAD)\n",ng->ng_Label,ng->ng_Label);
				}
			}
			break;
		case TEXTFIELD_ID:
			break;
		case TREEVIEW_ID:
			if(t=FindTagItem(LIST_Format,ng->ng_Tags))
			{
				if(((char *)t->ti_Data)[0])
				{
					FPrintf(handle,"extern const struct FC_Type _MSG_%s_FORMAT_GAD;\n",ng->ng_Label);
					FPrintf(handle,"#define MSG_%s_FORMAT_GAD ((APTR) &_MSG_%s_FORMAT_GAD)\n",ng->ng_Label,ng->ng_Label);
				}
			}
			break;
	}
} /* GADGET_GenerateCatalogHeader */

void GADGET_GenerateCatalogDescription(BPTR handle,piObject *go)
{
	piObject *wo;
	struct Node_Window *nw;
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	struct Node *n;
	struct TagItem *t;
	LONG a;

	ng=(struct Node_Gadget *)OBJECT_DATA(go);
	gtd=(struct GadToolsData *)GadgetData(ng);
	switch(ng->ng_PlugInID)
	{
		case BUTTON_ID:
			if(FindTagItem(TAG_USER+1,ng->ng_OwnTags)->ti_Data==0)
				FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
			break;
		case CHECKBOX_ID:
			FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
			break;
		case COLORWHEEL_ID:
			break;
		case CYCLE_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
					FPrintf(handle,"MSG_%s_L%ld_GAD (//)\n%s\n;\n",ng->ng_Label,a,n->ln_Name);
			}
			break;
		case DATATYPE_ID:
			break;
		case GRADIENTSLIDER_ID:
			break;
		case INTEGER_ID:
			FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
			break;
		case LISTVIEW_ID:
			if(t=FindTagItem(GTLV_Labels,ng->ng_Tags))
			{
				FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
					FPrintf(handle,"MSG_%s_L%ld_GAD (//)\n%s\n;\n",ng->ng_Label,a,n->ln_Name);
			}
			break;
		case MX_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
					FPrintf(handle,"MSG_%s_L%ld_GAD (//)\n%s\n;\n",ng->ng_Label,a,n->ln_Name);
			}
			break;
		case NUMBER_ID:
			FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
			if(t=FindTagItem(GTNM_Format,ng->ng_Tags))
				FPrintf(handle,"MSG_%s_FORMAT_GAD (//)\n%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
		case PAGE_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				for(a=0,n=GetHead(List(t->ti_Data));GetSucc(n);n=GetSucc(n),a++)
				{
					if(wo=(piObject *)FindName(List(&GUI->gui_List_Window),n->ln_Name))
					{
						nw=(struct Node_Window *)OBJECT_DATA(wo);
						FPrintf(handle,"MSG_%s_L%ld_GAD (//)\n%s\n;\n",ng->ng_Label,a,nw->nw_WindowTitle);
					}
				}
			}
			break;
		case PALETTE_ID:
			FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
			break;
		case POPUPMENU_ID:
			if(t=FindTagItem(PUMG_Labels,ng->ng_Tags))
			{
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
					FPrintf(handle,"MSG_%s_L%ld_GAD (//)\n%s\n;\n",ng->ng_Label,a,n->ln_Name);
			}
			break;
		case PROGRESS_ID:
			if(t=FindTagItem(PROG_Format,ng->ng_Tags))
				FPrintf(handle,"MSG_%s_FORMAT_GAD (//)\n%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
		case SCROLLER_ID:
			FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
			break;
		case SLIDER_ID:
			FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
			if(t=FindTagItem(GTSL_LevelFormat,ng->ng_Tags))
				FPrintf(handle,"MSG_%s_FORMAT_GAD (//)\n%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
		case STRING_ID:
			FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
			if(t=FindTagItem(GTST_String,ng->ng_Tags))
				if(((char *)t->ti_Data)[0])
					FPrintf(handle,"MSG_%s_FORMAT_GAD (//)\n%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
		case TAPEDECK_ID:
			break;
		case TEXT_ID:
			FPrintf(handle,"MSG_%s_GAD (//)\n%s\n;\n",ng->ng_Label,gtd->g_Text);
			if(t=FindTagItem(GTTX_Text,ng->ng_Tags))
				if(((char *)t->ti_Data)[0])
					FPrintf(handle,"MSG_%s_FORMAT_GAD (//)\n%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
		case TEXTFIELD_ID:
			break;
		case TREEVIEW_ID:
			if(t=FindTagItem(LIST_Format,ng->ng_Tags))
				if(((char *)t->ti_Data)[0])
					FPrintf(handle,"MSG_%s_FORMAT_GAD (//)\n%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
	}
} /* GADGET_GenerateCatalogDescription */

void GADGET_GenerateCatalogTranslation(BPTR handle,piObject *go)
{
	piObject *wo;
	struct Node_Window *nw;
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	struct Node *n;
	struct TagItem *t;
	LONG a;

	ng=(struct Node_Gadget *)OBJECT_DATA(go);
	gtd=(struct GadToolsData *)GadgetData(ng);
	switch(ng->ng_PlugInID)
	{
		case BUTTON_ID:
			if(FindTagItem(TAG_USER+1,ng->ng_OwnTags)->ti_Data==0)
				FPrintf(handle,"MSG_%s_GAD\n\n;%s\n;\n",ng->ng_Label,gtd->g_Text);
			break;
		case CHECKBOX_ID:
			FPrintf(handle,"MSG_%s_GAD\n\n;%s\n;\n",ng->ng_Label,gtd->g_Text);
			break;
		case COLORWHEEL_ID:
			break;
		case CYCLE_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				FPrintf(handle,"MSG_%s_GAD\n\n;%s\n;\n",ng->ng_Label,gtd->g_Text);
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
					FPrintf(handle,"MSG_%s_L%ld_GAD\n\n;%s\n;\n",ng->ng_Label,a,n->ln_Name);
			}
			break;
		case DATATYPE_ID:
			break;
		case GRADIENTSLIDER_ID:
			break;
		case INTEGER_ID:
			FPrintf(handle,"MSG_%s_GAD\n\n;%s\n;\n",ng->ng_Label,gtd->g_Text);
			break;
		case LISTVIEW_ID:
			if(t=FindTagItem(GTLV_Labels,ng->ng_Tags))
			{
				FPrintf(handle,"MSG_%s_GAD\n\n;%s\n;\n",ng->ng_Label,gtd->g_Text);
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
					FPrintf(handle,"MSG_%s_L%ld_GAD\n\n;%s\n;\n",ng->ng_Label,a,n->ln_Name);
			}
			break;
		case MX_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				FPrintf(handle,"MSG_%s_GAD\n\n;%s\n;\n",ng->ng_Label,gtd->g_Text);
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
					FPrintf(handle,"MSG_%s_L%ld_GAD\n\n;%s\n;\n",ng->ng_Label,a,n->ln_Name);
			}
			break;
		case NUMBER_ID:
			FPrintf(handle,"MSG_%s_GAD\n\n;%s\n;\n",ng->ng_Label,gtd->g_Text);
			if(t=FindTagItem(GTNM_Format,ng->ng_Tags))
				FPrintf(handle,"MSG_%s_FORMAT_GAD\n\n;%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
		case PAGE_ID:
			if(t=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				for(a=0,n=GetHead(List(t->ti_Data));GetSucc(n);n=GetSucc(n),a++)
				{
					if(wo=(piObject *)FindName(List(&GUI->gui_List_Window),n->ln_Name))
					{
						nw=(struct Node_Window *)OBJECT_DATA(wo);
						FPrintf(handle,"MSG_%s_L%ld_GAD\n\n;%s\n;\n",ng->ng_Label,a,nw->nw_WindowTitle);
					}
				}
			}
			break;
		case PALETTE_ID:
			FPrintf(handle,"MSG_%s_GAD\n\n;%s\n;\n",ng->ng_Label,gtd->g_Text);
			break;
		case POPUPMENU_ID:
			if(t=FindTagItem(PUMG_Labels,ng->ng_Tags))
			{
				for(a=0,n=GetHead(t->ti_Data);GetSucc(n);n=GetSucc(n),a++)
					FPrintf(handle,"MSG_%s_L%ld_GAD\n\n;%s\n;\n",ng->ng_Label,a,n->ln_Name);
			}
			break;
		case PROGRESS_ID:
			if(t=FindTagItem(PROG_Format,ng->ng_Tags))
				FPrintf(handle,"MSG_%s_FORMAT_GAD\n\n;%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
		case SCROLLER_ID:
			FPrintf(handle,"MSG_%s_GAD\n%s\n;\n",ng->ng_Label,gtd->g_Text);
			break;
		case SLIDER_ID:
			FPrintf(handle,"MSG_%s_GAD\n\n;%s\n;\n",ng->ng_Label,gtd->g_Text);
			if(t=FindTagItem(GTSL_LevelFormat,ng->ng_Tags))
				FPrintf(handle,"MSG_%s_FORMAT_GAD\n\n;%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
		case STRING_ID:
			FPrintf(handle,"MSG_%s_GAD\n\n;%s\n;\n",ng->ng_Label,gtd->g_Text);
			if(t=FindTagItem(GTST_String,ng->ng_Tags))
				if(((char *)t->ti_Data)[0])
					FPrintf(handle,"MSG_%s_FORMAT_GAD\n\n;%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
		case TAPEDECK_ID:
			break;
		case TEXT_ID:
			FPrintf(handle,"MSG_%s_GAD\n\n;%s\n;\n",ng->ng_Label,gtd->g_Text);
			if(t=FindTagItem(GTTX_Text,ng->ng_Tags))
				if(((char *)t->ti_Data)[0])
					FPrintf(handle,"MSG_%s_FORMAT_GAD\n\n;%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
		case TEXTFIELD_ID:
			break;
		case TREEVIEW_ID:
			if(t=FindTagItem(LIST_Format,ng->ng_Tags))
				if(((char *)t->ti_Data)[0])
					FPrintf(handle,"MSG_%s_FORMAT_GAD\n\n;%s\n;\n",ng->ng_Label,t->ti_Data);
			break;
	}
} /* GADGET_GenerateCatalogTranslation */
