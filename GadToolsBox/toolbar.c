/*
** GadToolsBox v3 - Toolbar management
*/

#include"includes.h"

int FN_CloseWindow_TB_Main(void)
{
	CloseWindow_TB_Main();
	SetWndSignals();
	if(TB_MainWnd || TB_WindowWnd || TB_PointerWnd)
		return 2;
	else
		return 3;
}

int FN_ChangeWindow_TB_Main()
{
	GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_MAIN].X=TB_MainWnd->LeftEdge;
	GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_MAIN].Y=TB_MainWnd->TopEdge;
	FindTagItem(WA_Left,(struct TagItem *)TB_MainWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_MAIN].X;
	FindTagItem(WA_Top,(struct TagItem *)TB_MainWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_MAIN].Y;
	return 1;
}

int FN_CloseWindow_TB_Window(void)
{
	CloseWindow_TB_Window();
	SetWndSignals();
	if(TB_MainWnd || TB_WindowWnd || TB_PointerWnd)
		return 2;
	else
		return 3;
}

int FN_ChangeWindow_TB_Window()
{
	GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_GADGETS].X=TB_WindowWnd->LeftEdge;
	GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_GADGETS].Y=TB_WindowWnd->TopEdge;
	FindTagItem(WA_Left,(struct TagItem *)TB_WindowWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_GADGETS].X;
	FindTagItem(WA_Top,(struct TagItem *)TB_WindowWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_GADGETS].Y;
	return 1;
}

int FN_CloseWindow_TB_Pointer(void)
{
	CloseWindow_TB_Pointer();
	SetWndSignals();
	if(TB_MainWnd || TB_WindowWnd || TB_PointerWnd)
		return 2;
	else
		return 3;
}

int FN_ChangeWindow_TB_Pointer()
{
	GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_IMAGE].X=TB_PointerWnd->LeftEdge;
	GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_IMAGE].Y=TB_PointerWnd->TopEdge;
	FindTagItem(WA_Left,(struct TagItem *)TB_PointerWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_IMAGE].X;
	FindTagItem(WA_Top,(struct TagItem *)TB_PointerWTags)->ti_Data=GUI.gui_Prefs->pr_WinPos[WIN_TOOLBAR_IMAGE].Y;
	return 1;
}

void ApplySelectionWindow(void)
{
	if((TB_WindowGadgets[GD_TBG_BUTTON]->Flags&GFLG_SELECTED) && (ActiveGadget!=BUTTON_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_BUTTON]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_BUTTON],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_CHECKBOX]->Flags&GFLG_SELECTED) && (ActiveGadget!=CHECKBOX_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_CHECKBOX]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_CHECKBOX],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_COLORWHEEL]->Flags&GFLG_SELECTED) && (ActiveGadget!=COLORWHEEL_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_COLORWHEEL]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_COLORWHEEL],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_CYCLE]->Flags&GFLG_SELECTED) && (ActiveGadget!=CYCLE_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_CYCLE]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_CYCLE],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_DATATYPE]->Flags&GFLG_SELECTED) && (ActiveGadget!=DATATYPE_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_DATATYPE]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_DATATYPE],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_GRADIENTSLIDER]->Flags&GFLG_SELECTED) && (ActiveGadget!=GRADIENTSLIDER_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_GRADIENTSLIDER]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_GRADIENTSLIDER],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_INTEGER]->Flags&GFLG_SELECTED) && (ActiveGadget!=INTEGER_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_INTEGER]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_INTEGER],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_LISTVIEW]->Flags&GFLG_SELECTED) && (ActiveGadget!=LISTVIEW_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_LISTVIEW]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_LISTVIEW],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_MX]->Flags&GFLG_SELECTED) && (ActiveGadget!=MX_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_MX]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_MX],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_NUMBER]->Flags&GFLG_SELECTED) && (ActiveGadget!=NUMBER_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_NUMBER]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_NUMBER],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_PAGE]->Flags&GFLG_SELECTED) && (ActiveGadget!=PAGE_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_PAGE]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_PAGE],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_PALETTE]->Flags&GFLG_SELECTED) && (ActiveGadget!=PALETTE_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_PALETTE]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_PALETTE],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_POPUPMENU]->Flags&GFLG_SELECTED) && (ActiveGadget!=POPUPMENU_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_POPUPMENU]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_POPUPMENU],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_PROGRESS]->Flags&GFLG_SELECTED) && (ActiveGadget!=PROGRESS_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_PROGRESS]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_PROGRESS],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_SCROLLER]->Flags&GFLG_SELECTED) && (ActiveGadget!=SCROLLER_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_SCROLLER]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_SCROLLER],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_SLIDER]->Flags&GFLG_SELECTED) && (ActiveGadget!=SLIDER_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_SLIDER]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_SLIDER],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_STRING]->Flags&GFLG_SELECTED) && (ActiveGadget!=STRING_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_STRING]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_STRING],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_TAPEDECK]->Flags&GFLG_SELECTED) && (ActiveGadget!=TAPEDECK_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_TAPEDECK]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_TAPEDECK],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_TEXT]->Flags&GFLG_SELECTED) && (ActiveGadget!=TEXT_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_TEXT]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_TEXT],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_TEXTFIELD]->Flags&GFLG_SELECTED) && (ActiveGadget!=TEXTFIELD_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_TEXTFIELD]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_TEXTFIELD],TB_WindowWnd,NULL,1); }
	if((TB_WindowGadgets[GD_TBG_TREEVIEW]->Flags&GFLG_SELECTED) && (ActiveGadget!=TREEVIEW_ID)) { ClearFlag(TB_WindowGadgets[GD_TBG_TREEVIEW]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_TREEVIEW],TB_WindowWnd,NULL,1); }
	if(ActiveGadget==NULL) { SetFlag(TB_WindowGadgets[GD_TBG_NONE]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_NONE],TB_WindowWnd,NULL,1); }
	else { ClearFlag(TB_WindowGadgets[GD_TBG_NONE]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_NONE],TB_WindowWnd,NULL,1); }

	if(GUI.gui_Mode!=MODE_TEST) { ClearFlag(TB_WindowGadgets[GD_TBG_MODE_TEST]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_MODE_TEST],TB_WindowWnd,NULL,1); }
	else { SetFlag(TB_WindowGadgets[GD_TBG_MODE_TEST]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_MODE_TEST],TB_WindowWnd,NULL,1); }
	if(GUI.gui_Mode!=MODE_GADGET) { ClearFlag(TB_WindowGadgets[GD_TBG_MODE_GADGET]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_MODE_GADGET],TB_WindowWnd,NULL,1); }
	else { SetFlag(TB_WindowGadgets[GD_TBG_MODE_GADGET]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_MODE_GADGET],TB_WindowWnd,NULL,1); }
	if(GUI.gui_Mode!=MODE_FRAME) { ClearFlag(TB_WindowGadgets[GD_TBG_MODE_FRAME]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_MODE_FRAME],TB_WindowWnd,NULL,1); }
	else { SetFlag(TB_WindowGadgets[GD_TBG_MODE_FRAME]->Flags,GFLG_SELECTED); RefreshGList(TB_WindowGadgets[GD_TBG_MODE_FRAME],TB_WindowWnd,NULL,1); }
}

int FN_TBG_NONE_Clicked(void)
{
	ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,TAG_DONE);
	return 1;
}

int FN_TBG_BUTTON_Clicked(void)
{
	if(ActiveGadget!=BUTTON_ID) ActiveGadget=BUTTON_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_CHECKBOX_Clicked(void)
{
	if(ActiveGadget!=CHECKBOX_ID) ActiveGadget=CHECKBOX_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_COLORWHEEL_Clicked(void)
{
	if(ActiveGadget!=COLORWHEEL_ID) ActiveGadget=COLORWHEEL_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_CYCLE_Clicked(void)
{
	if(ActiveGadget!=CYCLE_ID) ActiveGadget=CYCLE_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_DATATYPE_Clicked(void)
{
	if(ActiveGadget!=DATATYPE_ID) ActiveGadget=DATATYPE_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_GRADIENTSLIDER_Clicked(void)
{
	if(ActiveGadget!=GRADIENTSLIDER_ID) ActiveGadget=GRADIENTSLIDER_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_INTEGER_Clicked(void)
{
	if(ActiveGadget!=INTEGER_ID) ActiveGadget=INTEGER_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_LISTVIEW_Clicked(void)
{
	if(ActiveGadget!=LISTVIEW_ID) ActiveGadget=LISTVIEW_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_MX_Clicked(void)
{
	if(ActiveGadget!=MX_ID) ActiveGadget=MX_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_NUMBER_Clicked(void)
{
	if(ActiveGadget!=NUMBER_ID) ActiveGadget=NUMBER_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_PAGE_Clicked(void)
{
	if(ActiveGadget!=PAGE_ID) ActiveGadget=PAGE_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_PALETTE_Clicked(void)
{
	if(ActiveGadget!=PALETTE_ID) ActiveGadget=PALETTE_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_POPUPMENU_Clicked(void)
{
	if(ActiveGadget!=POPUPMENU_ID) ActiveGadget=POPUPMENU_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_PROGRESS_Clicked(void)
{
	if(ActiveGadget!=PROGRESS_ID) ActiveGadget=PROGRESS_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_SCROLLER_Clicked(void)
{
	if(ActiveGadget!=SCROLLER_ID) ActiveGadget=SCROLLER_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_SLIDER_Clicked(void)
{
	if(ActiveGadget!=SLIDER_ID) ActiveGadget=SLIDER_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_STRING_Clicked(void)
{
	if(ActiveGadget!=STRING_ID) ActiveGadget=STRING_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_TAPEDECK_Clicked(void)
{
	if(ActiveGadget!=TAPEDECK_ID) ActiveGadget=TAPEDECK_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_TEXT_Clicked(void)
{
	if(ActiveGadget!=TEXT_ID) ActiveGadget=TEXT_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_TEXTFIELD_Clicked(void)
{
	if(ActiveGadget!=TEXTFIELD_ID) ActiveGadget=TEXTFIELD_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_TBG_TREEVIEW_Clicked(void)
{
	if(ActiveGadget!=TREEVIEW_ID) ActiveGadget=TREEVIEW_ID; else ActiveGadget=NULL;
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return 1;
}

int FN_MainObjTypeTest_Selected(void);
int FN_MainObjTypeGadget_Selected(void);
int FN_MainObjTypeFrame_Selected(void);

int FN_TBG_MODE_TEST_Clicked(void)
{
	int ret;
	ret=FN_MainObjTypeTest_Selected();
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,TAG_DONE);
	return ret;
}

int FN_TBG_MODE_GADGET_Clicked(void)
{
	int ret;
	ret=FN_MainObjTypeGadget_Selected();
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,TAG_DONE);
	return ret;
}

int FN_TBG_MODE_FRAME_Clicked(void)
{
	int ret;
	ret=FN_MainObjTypeFrame_Selected();
	ApplySelectionWindow();
	SetWindowPointer(CurrentWindowNode->nw_Window,WA_Pointer,DrawPtr,TAG_DONE);
	return ret;
}




ULONG FindPen(struct Node_Image *ni,ULONG code)
{
	int a;

	for(a=0;a<(2<<ni->ni_Depth);a++)
		if(ni->ni_Pens[a]==(UBYTE)code)
			return (ULONG)a;
	return (ULONG)0;
}

int FN_TBP_PALETTE_Clicked(void)
{
	struct Node_Image *ni;

	piGetAttr(CurrentImage,OBJ_Node,(ULONG *)&ni);
	SetGadgetAttrs((struct Gadget *)ni->ni_Draw,ni->ni_Window,NULL,DRAW_ActualPen,FindPen(ni,TB_PointerMsg.Code),TAG_DONE);
	return 1;
}




int FN_TB_NEW_Clicked(void)
{
	return FN_MainProjectNew_Selected();
}

int FN_TB_OPEN_Clicked(void)
{
	return FN_MainProjectOpen_Selected();
}

int FN_TB_SAVE_Clicked(void)
{
	return FN_MainProjectSave_Selected();
}

int FN_TB_CUT_Clicked(void)
{
	return FN_MainEditCut_Selected();
}

int FN_TB_COPY_Clicked(void)
{
	return FN_MainEditCopy_Selected();
}

int FN_TB_PASTE_Clicked(void)
{
	return FN_MainEditPaste_Selected();
}

int FN_TB_UNDO_Clicked(void)
{
	return FN_MainEditPaste_Selected();
}

int FN_TB_NEWWINDOW_Clicked(void)
{
	return FN_MainObjWindowNew_Selected();
}

int FN_TB_NEWMENU_Clicked(void)
{
	return FN_MainObjMenuNew_Selected();
}

int FN_TB_NEWPOINTER_Clicked(void)
{
	return FN_MainObjImageNew_Selected();
}

int FN_TB_NEWTOOLBAR_Clicked(void)
{
//	piOpenWindow(CurrentToolbar);
	return 1;
}

int FN_TB_NEWIMAGE_Clicked(void)
{
	return FN_MainObjImageNew_Selected();
}

int FN_TB_NEWSTRING_Clicked(void)
{
	piOpenWindow(CurrentString);
	return 1;
}

int FN_TB_NEWVERSION_Clicked(void)
{
	piOpenWindow(CurrentVersion);
	return 1;
}

int FN_TB_NEWSCREEN_Clicked(void)
{
	piOpenWindow(CurrentScreen);
	return 1;
}

extern Object *AmigaGuideObj;

int FN_TB_HELP_Clicked(void)
{
	if(AmigaGuideObj) gtbSendAmigaGuideCmd(AmigaGuideObj,"ALINK main");
	return 1;
}

int FN_TB_ALIGNLEFT_Clicked(void)
{
	FN_MainEditAlignToLeft_Selected();
	return 1;
}

int FN_TB_ALIGNHCENTER_Clicked(void)
{
	FN_MainEditAlignToHCenter_Selected();
	return 1;
}

int FN_TB_ALIGNRIGHT_Clicked(void)
{
	FN_MainEditAlignToRight_Selected();
	return 1;
}

int FN_TB_ALIGNTOP_Clicked(void)
{
	FN_MainEditAlignToTop_Selected();
	return 1;
}

int FN_TB_ALIGNVCENTER_Clicked(void)
{
	FN_MainEditAlignToVCenter_Selected();
	return 1;
}

int FN_TB_ALIGNBOTTOM_Clicked(void)
{
	FN_MainEditAlignToBottom_Selected();
	return 1;
}

int FN_TB_SPREADHORIZ_Clicked(void)
{
	FN_MainEditSpreadHoriz_Selected();
	return 1;
}

int FN_TB_SPREADVERT_Clicked(void)
{
	FN_MainEditSpreadVertical_Selected();
	return 1;
}

int FN_TB_CLONEWIDTH_Clicked(void)
{
	FN_MainEditCloneWidth_Selected();
	return 1;
}

int FN_TB_CLONEHEIGHT_Clicked(void)
{
	FN_MainEditCloneHeight_Selected();
	return 1;
}

int FN_TB_CLONEBOTH_Clicked(void)
{
	FN_MainEditCloneBoth_Selected();
	return 1;
}

