/*
** GadToolsBox v3 - Load/Save fuctions
*/

#include"includes.h"

/*
** Defines
*/

#define IFF_Error(num)		\
		{											\
			printf("IFFError #%d",num);	\
			goto skip;	\
		}


/*
** Functions
*/

void SaveGUI(STRPTR name)
{
	struct IFFHandle *iffh;
	LONG error=0;

	if(iffh=AllocIFF())
	{
		if(iffh->iff_Stream=Open(name,MODE_NEWFILE))
		{
			SetGadgetAttrs(GTBSem.gs_Progress,GTBSem.gs_Window,NULL,PROG_MaxVal,10,TAG_DONE);
			InitIFFasDOS(iffh);
			unless(error=OpenIFF(iffh,IFFF_WRITE))
			{
				unless(PushChunk(iffh,ID_GTBX,ID_FORM,IFFSIZE_UNKNOWN))
				{
					SetWorkingProgress(0);
					if(error=WriteGUI(iffh)) IFF_Error(error);
					SetWorkingProgress(1);
					piWriteObject(CurrentVersion,iffh);
					SetWorkingProgress(2);
					piWriteObject(CurrentScreen,iffh);
					SetWorkingProgress(3);
					piWriteWindow(iffh);
					SetWorkingProgress(5);
					piWriteMenu(iffh);
					SetWorkingProgress(6);
					piWriteObject(CurrentString,iffh);
					SetWorkingProgress(8);
					piWriteImage(iffh);
					SetWorkingProgress(10);
					if(error=PopChunk(iffh)) IFF_Error(error);
				}
skip:;
				CloseIFF(iffh);
			} else IFF_Error(error);
			Close(iffh->iff_Stream);
		}
		FreeIFF(iffh);
	}
}

void LoadGUI(STRPTR name)
{
	struct IFFHandle *iffh;
	struct ContextNode *cn;
	LONG error=0,a;
	piObject *wo,*mo=NULL;
	struct GTB_Version gtbver;

	if(iffh=AllocIFF())
	{
		if(iffh->iff_Stream=Open(name,MODE_OLDFILE))
		{
			Seek(iffh->iff_Stream,0,OFFSET_END);
			SetGadgetAttrs(GTBSem.gs_Progress,GTBSem.gs_Window,NULL,PROG_MaxVal,Seek(iffh->iff_Stream,0,OFFSET_BEGINNING),TAG_DONE);
			InitIFFasDOS(iffh);
			unless(error=OpenIFF(iffh,IFFF_READ))
			{
					/* GadToolsBox v3 FORMs */
				if(error=StopChunk(iffh,ID_GTBX,ID_GGUI)) IFF_Error(error);
				if(error=StopChunk(iffh,ID_GTBX,ID_WIND)) IFF_Error(error);
				if(error=StopChunk(iffh,ID_GTBX,ID_SCRN)) IFF_Error(error);
				if(error=StopChunk(iffh,ID_GTBX,ID_FRME)) IFF_Error(error);
				if(error=StopChunk(iffh,ID_GTBX,ID_LSTR)) IFF_Error(error);
				if(error=StopChunk(iffh,ID_GTBX,ID_MENU)) IFF_Error(error);
				if(error=StopChunk(iffh,ID_GTBX,ID_SVER)) IFF_Error(error);
				if(error=StopChunk(iffh,ID_GTBX,ID_IMGE)) IFF_Error(error);
				if(error=StopChunk(iffh,ID_GTBX,ID_PNTR)) IFF_Error(error);
				for(a=0;a<PLUGIN_GADGETS;a++) if(error=StopChunk(iffh,ID_GTBX,piGadgets[a].piID)) IFF_Error(error);

				if(CheckRegistration())
				{
						/* GadToolsBox v2 FORMs */
					if(error=StopChunk(iffh,ID_GXUI,ID_GGUI)) IFF_Error(error);
					if(error=StopChunk(iffh,ID_GXWD,ID_WDDA)) IFF_Error(error);
					if(error=StopChunk(iffh,ID_GXBX,ID_BBOX)) IFF_Error(error);
					if(error=StopChunk(iffh,ID_GXGA,ID_GADA)) IFF_Error(error);
					if(error=StopChunk(iffh,ID_GXMN,ID_MEDA)) IFF_Error(error);
				}

				while(1)
				{
					SetWorkingProgress(Seek(iffh->iff_Stream,0,OFFSET_CURRENT));
					error=ParseIFF(iffh,IFFPARSE_SCAN);
					if(error==IFFERR_EOC) continue;       /* enter next context */
					else if(error) break;
					if(cn=CurrentChunk(iffh))
					{
						switch(cn->cn_Type)
						{
							case ID_GTBX:
								switch(cn->cn_ID)
								{
									case ID_GGUI:
										ReadGUI(iffh);
										break;
									case ID_VERS:
										ReadChunkBytes(iffh,(APTR)&gtbver,sizeof(struct GTB_Version));
										if(gtbver.vr_Version>VER_VERSION || (gtbver.vr_Revision>VER_REVISION && gtbver.vr_Version>VER_VERSION))
										{
											RequestOk(GetCatString(MSG_ERROR),GetCatString(MSG_YOUNEEDNEWERVERSION),GetCatString(MSG_OK),gtbver.vr_Version,gtbver.vr_Revision);
											goto skip;
										}
										break;
									case ID_WIND:
										piReadWindow(iffh,&wo);
										break;
									case ID_FRME:
										if(wo) piReadFrame(iffh,wo);
										break;
									case ID_MENU:
										piReadMenu(iffh);
										break;
									case ID_IMGE:
										piReadImage(iffh);
										break;
									case ID_PNTR:
										piReadPointer(iffh);
										break;
									case ID_SCRN:
										piReadObject(CurrentScreen,iffh);
										break;
									case ID_SVER:
										piReadObject(CurrentVersion,iffh);
										break;
									case ID_LSTR:
										piReadObject(CurrentString,iffh);
										break;
									case BUTTON_ID:
									case CHECKBOX_ID:
									case COLORWHEEL_ID:
									case CYCLE_ID:
									case DATATYPE_ID:
									case GRADIENTSLIDER_ID:
									case INTEGER_ID:
									case LISTVIEW_ID:
									case MX_ID:
									case NUMBER_ID:
									case PAGE_ID:
									case PALETTE_ID:
									case POPUPMENU_ID:
									case PROGRESS_ID:
									case SCROLLER_ID:
									case SLIDER_ID:
									case STRING_ID:
									case TAPEDECK_ID:
									case TEXT_ID:
									case TEXTFIELD_ID:
									case TREEVIEW_ID:
										if(wo) piReadGadget(iffh,wo);
										break;
								}
								break;
							case ID_GXUI:
								if(cn->cn_ID==ID_GGUI) gtb2_ReadGUI(iffh);
								break;
							case ID_GXWD:
								if(cn->cn_ID==ID_WDDA) gtb2_piReadWindow(iffh,&wo);
								mo=NULL;
								break;
							case ID_GXGA:
								if(cn->cn_ID==ID_GADA && wo) gtb2_piReadGadget(iffh,wo);
								break;
							case ID_GXBX:
								if(cn->cn_ID==ID_BBOX && wo) gtb2_piReadFrame(iffh,wo);
								break;
							case ID_GXMN:
								if(cn->cn_ID==ID_MEDA) gtb2_piReadMenu(iffh,mo,wo,&mo);
								break;
						}
					}
				}
skip:;
				CloseIFF(iffh);
			} else IFF_Error(error);
			Close(iffh->iff_Stream);
		}
		FreeIFF(iffh);
	}
}

struct GUI_Store
{
	UWORD gs_Grid[2];
	BOOL gs_GridOn,gs_GridShow;
	UBYTE gs_Mode;
};

LONG WriteGUI(struct IFFHandle *iffh)
{
	LONG error=0;
	struct GUI_Store gui;
	struct GTB_Version gtbver={3,0,NULL,NULL};

	gui.gs_Mode=GUI.gui_Mode;
	gui.gs_GridOn=GUI.gui_GridOn;
	gui.gs_GridShow=GUI.gui_GridShow;
	gui.gs_Grid[0]=GUI.gui_Grid[0];
	gui.gs_Grid[1]=GUI.gui_Grid[1];

	if(error=PushChunk(iffh,NULL,ID_GGUI,sizeof(struct GUI_Store))) return error;
	WriteChunkBytes(iffh,(APTR)&gui,sizeof(struct GUI_Store));
	if(error=PopChunk(iffh)) return error;

	if(error=PushChunk(iffh,NULL,ID_VERS,sizeof(struct GTB_Version))) return error;
	WriteChunkBytes(iffh,(APTR)&gtbver,sizeof(struct GTB_Version));
	if(error=PopChunk(iffh)) return error;
	return 0;
}

LONG ReadGUI(struct IFFHandle *iffh)
{
	struct GUI_Store gui;

	ReadChunkBytes(iffh,(APTR)&gui,sizeof(struct GUI_Store));
	GUI.gui_Mode=gui.gs_Mode;
	GUI.gui_GridOn=gui.gs_GridOn;
	GUI.gui_GridShow=gui.gs_GridShow;
	GUI.gui_Grid[0]=gui.gs_Grid[0];
	GUI.gui_Grid[1]=gui.gs_Grid[1];

	return 0;
}

LONG gtb2_ReadGUI(struct IFFHandle *iffh)
{
	GUIDATA gd;
	struct Node_Screen *ns;

	ReadChunkBytes(iffh,(APTR)&gd,sizeof(GUIDATA));
	piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);

	GUI.gui_Grid[0]=gd.gui_GridX;
	GUI.gui_Grid[1]=gd.gui_GridY;

//	GlobalOffX=gd.gui_Font

	ns->ns_Width=gd.gui_Width;
	ns->ns_Height=gd.gui_Height;
	ns->ns_Depth=gd.gui_Depth;
	ns->ns_DisplayID=gd.gui_DisplayID;
	strncpy(ns->ns_FontName,gd.gui_FontName,30);
	strcpy(ns->ns_Title,gd.gui_ScreenTitle);
	if(gd.gui_Font.ta_YSize) ns->ns_FontSize=gd.gui_Font.ta_YSize;
	else ns->ns_FontSize=8;

	if(EditFontTF) CloseFont(EditFontTF);
	EditFontTA.ta_Name=ns->ns_FontName;
	EditFontTA.ta_YSize=ns->ns_FontSize;
	EditFontTA.ta_Style=FS_NORMAL;
	EditFontTA.ta_Flags=FPF_DISKFONT|FPF_DESIGNED;
	EditFontTF=OpenDiskFont(&EditFontTA);

	GetFlag(gd.gui_Flags0,GU0_AUTOSCROLL) SetFlag(ns->ns_MyFlags,MYSC_AutoScroll); else ClearFlag(ns->ns_MyFlags,MYSC_AutoScroll);
	GetFlag(gd.gui_Flags0,GU0_WORKBENCH) ns->ns_Type=TYPE_WORKBENCH;
	GetFlag(gd.gui_Flags0,GU0_PUBLIC) ns->ns_Type=TYPE_PUBLICSCREEN;
	GetFlag(gd.gui_Flags0,GU0_CUSTOM) ns->ns_Type=TYPE_CUSTOMSCREEN;
	return 0;
}

LONG piReadWindow(struct IFFHandle *iffh,piObject **ro)
{
	piObject *o;

	if(o=piNewObject(piWindowClass,TAG_DONE))
	{
		piAddObject(o,NULL);
		piReadObject(o,iffh);
	}
	*ro=o;
	return 0;
}

LONG gtb2_piReadWindow(struct IFFHandle *iffh,piObject **ro)
{
	piObject *o;
	WINDOWDATA wd;
	struct Node_Window *nw;

	if(o=piNewObject(piWindowClass,TAG_DONE))
	{
		piGetAttr(o,OBJ_Node,(ULONG *)&nw);
		piAddObject(o,NULL);
		ReadChunkBytes(iffh,(APTR)&wd,sizeof(WINDOWDATA));
		strcpy(nw->nw_Label,wd.wda_Name);
		strcpy(nw->nw_WindowTitle,wd.wda_Title);
		strcpy(nw->nw_ScreenTitle,wd.wda_ScreenTitle);
		nw->nw_IDCMP=wd.wda_IDCMP;
		nw->nw_Flags=wd.wda_WindowFlags|WFLG_NEWLOOKMENUS;
		ClearFlag(nw->nw_Flags,WFLG_RMBTRAP);
//		ClearFlag(nw->nw_Flags,WFLG_AUTOADJUST);
//		ClearFlag(nw->nw_Flags,WFLG_PUBSCREENFALLBACK);
//		ClearFlag(nw->nw_Flags,WFLG_NOTIFYDEPTH);
		nw->nw_MouseQueue=wd.wda_MouseQueue;
		nw->nw_RptQueue=wd.wda_RptQueue;
		nw->nw_X=wd.wda_LeftBorder;
		nw->nw_Y=wd.wda_TopBorder;
		GetFlag(wd.wda_WindowFlags,WDF_INNERWIDTH)
		{
			SetFlag(nw->nw_MyFlags,MYWA_InnerWidth);
			nw->nw_Width=wd.wda_InnerWidth;
		}
		else
			nw->nw_Width=wd.wda_InnerWidth+8;
		GetFlag(wd.wda_WindowFlags,WDF_INNERHEIGHT)
		{
			SetFlag(nw->nw_MyFlags,MYWA_InnerHeight);
			nw->nw_Height=wd.wda_InnerHeight;
		}
		else
			nw->nw_Height=wd.wda_InnerHeight+2;
		GetFlag(wd.wda_WindowFlags,WDF_ZOOM) SetFlag(nw->nw_MyFlags,MYWA_Zoom);
		GetFlag(wd.wda_WindowFlags,WDF_MOUSEQUEUE) SetFlag(nw->nw_MyFlags,MYWA_MouseQueue);
		GetFlag(wd.wda_WindowFlags,WDF_RPTQUEUE) SetFlag(nw->nw_MyFlags,MYWA_RptQueue);
		GetFlag(wd.wda_WindowFlags,WDF_AUTOADJUST) SetFlag(nw->nw_MyFlags,MYWA_AutoAdjust);
		GetFlag(wd.wda_WindowFlags,WDF_FALLBACK) SetFlag(nw->nw_MyFlags,MYWA_PubScreenFallBack);
	}
	*ro=o;
	return 0;
}

LONG piWriteWindow(struct IFFHandle *iffh)
{
	piObject *o;
	LONG error=0;

	if(!IsListEmpty(List(&GUI.gui_List_Window)))
	{
		for(o=(piObject *)GetHead(&GUI.gui_List_Window);GetSucc(o);o=(piObject *)GetSucc(o)) piWriteObject(o,iffh);
	}
	return error;
}

LONG piReadGadget(struct IFFHandle *iffh,piObject *wo)
{
	piObject *o;
	struct ContextNode *cn;

	if(cn=CurrentChunk(iffh))
	{
		if(o=piNewObject(piGadgets[GetGadgetPtr(cn->cn_ID)].Class,TAG_DONE))
		{
			piAddObject(o,wo);
			piReadObject(o,iffh);
		}
	}
	return 0;
}

LONG gtb2_piReadGadget(struct IFFHandle *iffh,piObject *wo)
{
	piObject *o;
	GADGETDATA gd;
	ULONG idkind;
	struct Node_Gadget *ng;
	struct GadToolsData *gtd;
	struct Node_Screen *ns;

	ReadChunkBytes(iffh,(APTR)&gd,sizeof(GADGETDATA));
	switch(gd.gd_Kind)
	{
		case GENERIC_KIND: idkind=BUTTON_ID; break;
		case BUTTON_KIND: idkind=BUTTON_ID; break;
		case CHECKBOX_KIND: idkind=CHECKBOX_ID; break;
		case INTEGER_KIND: idkind=INTEGER_ID; break;
		case LISTVIEW_KIND: idkind=LISTVIEW_ID; break;
		case MX_KIND: idkind=MX_ID; break;
		case NUMBER_KIND: idkind=NUMBER_ID; break;
		case CYCLE_KIND: idkind=CYCLE_ID; break;
		case PALETTE_KIND: idkind=PALETTE_ID; break;
		case SCROLLER_KIND: idkind=SCROLLER_ID; break;
		case SLIDER_KIND: idkind=SLIDER_ID; break;
		case STRING_KIND: idkind=STRING_ID; break;
		case TEXT_KIND: idkind=TEXT_ID; break;
	}
	if(o=piNewObject(piGadgets[GetGadgetPtr(idkind)].Class,TAG_DONE))
	{
		piGetAttr(o,OBJ_Node,(ULONG *)&ng);
		piGetAttr(o,GAOBJ_NodeData,(ULONG *)&gtd);
		piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);
		piAddObject(o,wo);
		ng->ng_X=gd.gd_NewGadget.ng_LeftEdge-4;
		ng->ng_Y=gd.gd_NewGadget.ng_TopEdge-ns->ns_FontSize-ns->ns_FontSize/3;
		ng->ng_Width=gd.gd_NewGadget.ng_Width-1;
		ng->ng_Height=gd.gd_NewGadget.ng_Height-1;
		ng->ng_PlugInID=idkind;
		ng->ng_Type=gd.gd_NumTags;
		strcpy(ng->ng_Label,gd.gd_GadgetLabel);
		strcpy(gtd->g_Text,gd.gd_GadgetText);
		gtd->g_Flags=gd.gd_NewGadget.ng_Flags;
		gtd->g_Kind=gd.gd_Kind;
		piReadObject(o,iffh);
		ng->ng_Type=GTYPE_GADTOOLS;
	}
	return 0;
}

LONG piReadFrame(struct IFFHandle *iffh,piObject *wo)
{
	piObject *o;

	if(o=piNewObject(piFrameClass,TAG_DONE))
	{
		piAddObject(o,wo);
		piReadObject(o,iffh);
	}
	return 0;
}

LONG gtb2_piReadFrame(struct IFFHandle *iffh,piObject *wo)
{
	piObject *o;
	BBOXDATA bd;
	struct Node_Frame *nf;
	struct Node_Screen *ns;

	if(o=piNewObject(piFrameClass,TAG_DONE))
	{
		piGetAttr(o,OBJ_Node,(ULONG *)&nf);
		piGetAttr(CurrentScreen,OBJ_Node,(ULONG *)&ns);
		piAddObject(o,wo);
		ReadChunkBytes(iffh,(APTR)&bd,sizeof(BBOXDATA));
		nf->nf_X=bd.bbx_Left-4;
		nf->nf_Y=bd.bbx_Top-ns->ns_FontSize-ns->ns_FontSize/3;
		nf->nf_Width=bd.bbx_Width-1;
		nf->nf_Height=bd.bbx_Height-1;
		GetFlag(bd.bbx_Flags,BBF_RECESSED) SetFlag(nf->nf_Flags,MYFR_Recessed);
		GetFlag(bd.bbx_Flags,BBF_DROPBOX) nf->nf_FrameType=FTYPE_ICONDROPBOX; else nf->nf_FrameType=FTYPE_BUTTON;
	}
	return 0;
}

LONG piReadMenu(struct IFFHandle *iffh)
{
	piObject *o;

	if(o=piNewObject(piMenuClass,TAG_DONE))
	{
		piAddObject(o,NULL);
		piReadObject(o,iffh);
	}
	return 0;
}

LONG gtb2_piReadMenu(struct IFFHandle *iffh,piObject *o,piObject *wo,piObject **ro)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;
	struct Node_Window *nw;
	MENUDATA md;

	if(!o)
	{
		if(o=piNewObject(piMenuClass,TAG_DONE))
		{
			piAddObject(o,NULL);
			piGetAttr(o,OBJ_Node,(ULONG *)&nm);
			piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
			strcpy(nm->nm_Label,nw->nw_Label);
			SetFlag(nm->nm_Flags,MYMN_NewLookMenus);
		}
	}
	*ro=o;
	piGetAttr(o,OBJ_Node,(ULONG *)&nm);
	ReadChunkBytes(iffh,(APTR)&md,sizeof(MENUDATA));
	if(ni=(struct Node_MenuItem *)MyAllocVec(sizeof(struct Node_MenuItem),MEMF_ANY|MEMF_CLEAR))
	{
		ni->nm_Node.ln_Name=ni->nm_Title;
		AddTail(List(&nm->nm_MenuItem),Node(ni));
		strcpy(ni->nm_Label,md.mda_Label);
		strcpy(ni->nm_Title,md.mda_Title);
		strcpy(ni->nm_Key,md.mda_ShortCut);
		ni->nm_Type=md.mda_NewMenu.nm_Type;
		if(!strcmp("NM_BARLABEL",md.mda_Title)) SetFlag(ni->nm_Flags,MYMN_BarLabel);
		GetFlag(md.mda_NewMenu.nm_Flags,CHECKIT) SetFlag(ni->nm_Flags,MYMN_Toggle);
		GetFlag(md.mda_NewMenu.nm_Flags,CHECKED) SetFlag(ni->nm_Flags,MYMN_Checked);
		GetFlag(md.mda_NewMenu.nm_Flags,NM_MENUDISABLED) SetFlag(ni->nm_Flags,MYMN_Disabled);
		GetFlag(md.mda_NewMenu.nm_Flags,NM_ITEMDISABLED) SetFlag(ni->nm_Flags,MYMN_Disabled);
		if(ni->nm_Type==NM_TITLE) SetFlag(ni->nm_Flags,MYMN_PopUp);
		if(ni->nm_Type==NM_SUB && ((struct Node_MenuItem *)ni->nm_Node.ln_Pred)->nm_Type==NM_ITEM)
			SetFlag(((struct Node_MenuItem *)ni->nm_Node.ln_Pred)->nm_Flags,MYMN_PopUp);
	}
	return 0;
}

LONG piWriteMenu(struct IFFHandle *iffh)
{
	LONG error=0;
	piObject *o;

	if(!IsListEmpty(List(&GUI.gui_List_Menu)))
	{
		for(o=(piObject *)GetHead(&GUI.gui_List_Menu);GetSucc(o);o=(piObject *)GetSucc(o)) piWriteObject(o,iffh);
	}
	return error;
}

LONG piReadImage(struct IFFHandle *iffh)
{
	piObject *o;

	if(o=piNewObject(piImageClass,TAG_DONE))
	{
		piAddObject(o,NULL);
		piReadObject(o,iffh);
	}
	return 0;
}

LONG piReadPointer(struct IFFHandle *iffh)
{
	piObject *o;
	struct Node_Pointer np;
	struct Node_Image *ni;

	if(o=piNewObject(piImageClass,TAG_DONE))
	{
		piAddObject(o,NULL);

		piGetAttr(o,OBJ_Node,(ULONG *)&ni);
		ReadChunkBytes(iffh,(APTR)np.ni_Label,SIZE(Node_Pointer,ni_Label));

		CopyMem(&np.ni_ImageData[0],ni->ni_BitMap->Planes[0],32*32/8);
		CopyMem(&np.ni_ImageData[32*32/8],ni->ni_BitMap->Planes[1],32*32/8);
		strcpy(ni->ni_Label,np.ni_Label);
		ni->ni_Width=32;
		ni->ni_Height=32;
		ni->ni_Depth=2;
		ni->ni_Type=IMTYPE_POINTER;
		ni->ni_PointX=np.ni_PointX;
		ni->ni_PointY=np.ni_PointY;
	}
	return 0;
}

LONG piWriteImage(struct IFFHandle *iffh)
{
	LONG error=0;
	piObject *o;

	if(!IsListEmpty(List(&GUI.gui_List_Image)))
	{
		for(o=(piObject *)GetHead(&GUI.gui_List_Image);GetSucc(o);o=(piObject *)GetSucc(o)) piWriteObject(o,iffh);
	}
	return error;
}

LONG WriteTagItem(struct IFFHandle *iffh,struct TagItem *t,UBYTE type)
{
	LONG error=0;
	struct Node *n;
	int a;
	ULONG ld;
	UWORD wd;
	UBYTE bd;

	bd=type;
	WriteChunkBytes(iffh,(APTR)&bd,1);
	ld=t->ti_Tag;
	WriteChunkBytes(iffh,(APTR)&ld,4);
	switch(type)
	{
		case TAGTYPE_NONE:
			break;
		case TAGTYPE_NUMBER:
			ld=t->ti_Data;
			WriteChunkBytes(iffh,(APTR)&ld,4);
			break;
		case TAGTYPE_TEXT:
			ld=strlen((char *)t->ti_Data)+1;
			WriteChunkBytes(iffh,(APTR)&ld,4);
			WriteChunkBytes(iffh,(APTR)t->ti_Data,ld);
			break;
		case TAGTYPE_LIST:
			for(a=0,n=GetHead(List(t->ti_Data));GetSucc(n);a++,n=GetSucc(n));
			ld=a;
			WriteChunkBytes(iffh,(APTR)&ld,4);

			for(n=GetHead(List(t->ti_Data));GetSucc(n);n=GetSucc(n))
			{
				wd=strlen(n->ln_Name)+1;
				WriteChunkBytes(iffh,(APTR)&wd,2);
				WriteChunkBytes(iffh,(APTR)n->ln_Name,strlen(n->ln_Name)+1);
			}
			break;
	}
	return error;
}

LONG ReadTagItem(struct IFFHandle *iffh,struct TagItem *t)
{
	UBYTE type;
	UWORD l2;
	ULONG len,a;
	struct Node *n;

	ReadChunkBytes(iffh,(APTR)&type,1);
	ReadChunkBytes(iffh,(APTR)&t->ti_Tag,4);
	switch(type)
	{
		case TAGTYPE_NONE:
			break;
		case TAGTYPE_NUMBER:
			ReadChunkBytes(iffh,(APTR)(&t->ti_Data),4);
			break;
		case TAGTYPE_TEXT:
			ReadChunkBytes(iffh,(APTR)&len,4);
			t->ti_Data=(ULONG)MyAllocVec(len,MEMF_ANY);
			ReadChunkBytes(iffh,(APTR)t->ti_Data,len);
			break;
		case TAGTYPE_LIST:
			t->ti_Data=(ULONG)MyAllocVec(sizeof(struct List),MEMF_ANY);
			NewList(List(t->ti_Data));
			ReadChunkBytes(iffh,(APTR)&len,4);
			for(a=0;a<len;a++)
			{
				ReadChunkBytes(iffh,(APTR)&l2,2);
				n=MyAllocVec(sizeof(struct Node),MEMF_ANY);
				n->ln_Name=MyAllocVec(l2,MEMF_ANY);
				ReadChunkBytes(iffh,(APTR)n->ln_Name,l2);
				AddTail(List(t->ti_Data),n);
			}
			break;
	}
	return 0;
}







extern UWORD ScrWidth,ScrHeight,ScrDepth;
extern ULONG ScrDisplayID;

void SavePrefs(STRPTR name)
{
	struct IFFHandle *iffh;
	struct PrefHeader prefhdr={0,0,0};
	LONG error=0;

	if(iffh=AllocIFF())
	{
		if(iffh->iff_Stream=Open(name,MODE_NEWFILE))
		{
			InitIFFasDOS(iffh);
			unless(error=OpenIFF(iffh,IFFF_WRITE))
			{
				unless(PushChunk(iffh,ID_PREF,ID_FORM,IFFSIZE_UNKNOWN))
				{
					if(error=PushChunk(iffh,NULL,ID_PRHD,sizeof(struct PrefHeader))) IFF_Error(error);
					WriteChunkBytes(iffh,(APTR)&prefhdr,sizeof(struct PrefHeader));
					if(error=PopChunk(iffh)) IFF_Error(error);
					if(error=PushChunk(iffh,NULL,ID_GTBX,sizeof(struct Prefs))) IFF_Error(error);
					WriteChunkBytes(iffh,(APTR)GUI.gui_Prefs,sizeof(struct Prefs));
					if(error=PopChunk(iffh)) IFF_Error(error);
				}
skip:;
				CloseIFF(iffh);
			} else IFF_Error(error);
			Close(iffh->iff_Stream);
		}
		FreeIFF(iffh);
	}
}

void LoadPrefs(STRPTR name)
{
	struct IFFHandle *iffh;
	struct ContextNode *cn;
	LONG error=0;

	if(iffh=AllocIFF())
	{
		if(iffh->iff_Stream=Open(name,MODE_OLDFILE))
		{
			InitIFFasDOS(iffh);
			unless(error=OpenIFF(iffh,IFFF_READ))
			{
				if(error=StopChunk(iffh,ID_PREF,ID_GTBX)) IFF_Error(error);
				while(1)
				{
					error=ParseIFF(iffh,IFFPARSE_SCAN);
					if(error==IFFERR_EOC) continue;       /* enter next context */
					else if(error) break;
					if(cn=CurrentChunk(iffh))
					{
						if(cn->cn_Type==ID_PREF)
						{
							switch(cn->cn_ID)
							{
								case ID_GTBX:
									ReadChunkBytes(iffh,(APTR)GUI.gui_Prefs,sizeof(struct Prefs));
									ScrWidth=GUI.gui_Prefs->pr_Width;
									ScrHeight=GUI.gui_Prefs->pr_Height;
									ScrDepth=GUI.gui_Prefs->pr_Depth;
									ScrDisplayID=GUI.gui_Prefs->pr_DisplayID;
									FindTagItem(WA_Left,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].X;
									FindTagItem(WA_Top,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].Y;
									FindTagItem(WA_Width,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].X;
									FindTagItem(WA_Height,(struct TagItem *)ObjectWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].Y;
									FindTagItem(WA_Left,(struct TagItem *)PropertiesWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_PROPERTIES].X;
									FindTagItem(WA_Top,(struct TagItem *)PropertiesWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_PROPERTIES].Y;
									FindTagItem(WA_Left,(struct TagItem *)TB_MainWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_MAIN].X;
									FindTagItem(WA_Top,(struct TagItem *)TB_MainWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_MAIN].Y;
									FindTagItem(WA_Left,(struct TagItem *)TB_WindowWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_GADGETS].X;
									FindTagItem(WA_Top,(struct TagItem *)TB_WindowWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_GADGETS].Y;
									FindTagItem(WA_Left,(struct TagItem *)TB_PointerWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_IMAGE].X;
									FindTagItem(WA_Top,(struct TagItem *)TB_PointerWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_IMAGE].Y;
									FindTagItem(WA_Left,(struct TagItem *)StatusWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_STATUS].X;
									FindTagItem(WA_Top,(struct TagItem *)StatusWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_STATUS].Y;
									break;
							}
						}
					}
				}
skip:;
				CloseIFF(iffh);
			} else IFF_Error(error);
			Close(iffh->iff_Stream);
		}
		else
		{
			if(AslRequestTags(ScreenModeReq,
				ASLSM_DoWidth,TRUE,
				ASLSM_DoHeight,TRUE,
				ASLSM_DoDepth,TRUE,
				ASLSM_PropertyMask,NULL,
				TAG_DONE))
			{
				ScrDisplayID=ScreenModeReq->sm_DisplayID;
				ScrWidth=ScreenModeReq->sm_DisplayWidth;
				ScrHeight=ScreenModeReq->sm_DisplayHeight;
				ScrDepth=ScreenModeReq->sm_DisplayDepth;
			}

/*
			ScrWidth=640;
			ScrHeight=256;
			ScrDepth=4;
			ScrDisplayID=HIRES_KEY;
*/
			GUI.gui_Prefs->pr_Flags=0;
			GUI.gui_Prefs->pr_Grid[0]=8;
			GUI.gui_Prefs->pr_Grid[1]=4;
			strcpy(GUI.gui_Prefs->pr_FontName,"topaz.font");
			GUI.gui_Prefs->pr_FontSize=8;
			strcpy(GUI.gui_Prefs->pr_Generator,"c.generator");
			GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].X=FindTagItem(WA_Left,(struct TagItem *)ObjectWTags)->ti_Data=0;
			GUI.gui_Prefs->pr_WinPos[WIN_OBJECT].Y=FindTagItem(WA_Top,(struct TagItem *)ObjectWTags)->ti_Data=0;
			GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].X=FindTagItem(WA_Width,(struct TagItem *)ObjectWTags)->ti_Data=150;
			GUI.gui_Prefs->pr_WinPos[WIN_OBJECT_DIMS].Y=FindTagItem(WA_Height,(struct TagItem *)ObjectWTags)->ti_Data=200;
			GUI.gui_Prefs->pr_WinPos[WIN_PROPERTIES].X=FindTagItem(WA_Left,(struct TagItem *)PropertiesWTags)->ti_Data=0;
			GUI.gui_Prefs->pr_WinPos[WIN_PROPERTIES].Y=FindTagItem(WA_Top,(struct TagItem *)PropertiesWTags)->ti_Data=0;
			GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_MAIN].X=FindTagItem(WA_Left,(struct TagItem *)TB_MainWTags)->ti_Data=0;
			GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_MAIN].Y=FindTagItem(WA_Top,(struct TagItem *)TB_MainWTags)->ti_Data=0;
			GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_GADGETS].X=FindTagItem(WA_Left,(struct TagItem *)TB_WindowWTags)->ti_Data=0;
			GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_GADGETS].Y=FindTagItem(WA_Top,(struct TagItem *)TB_WindowWTags)->ti_Data=0;
			GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_IMAGE].X=FindTagItem(WA_Left,(struct TagItem *)TB_PointerWTags)->ti_Data=0;
			GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_IMAGE].Y=FindTagItem(WA_Top,(struct TagItem *)TB_PointerWTags)->ti_Data=0;
			GUI.gui_Prefs->pr_WinPos[WIN_STATUS].X=FindTagItem(WA_Left,(struct TagItem *)StatusWTags)->ti_Data=0;
			GUI.gui_Prefs->pr_WinPos[WIN_STATUS].Y=FindTagItem(WA_Top,(struct TagItem *)StatusWTags)->ti_Data=0;
		}
		FreeIFF(iffh);
	}
}
