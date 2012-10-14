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
#include"main.h"

extern struct GUI *GUI;
extern BOOL OPT_ARexx,OPT_UseLocale,OPT_GUIFuncs,OPT_Screen;
extern struct Screen *Scr;
extern struct GTBSemaphore *GTBSem;

void GADGET_GenerateOpenLib(BPTR,ULONG);
void GADGET_GenerateCloseLib(BPTR,ULONG);

BOOL CheckPlugIn(ULONG);
BOOL CheckToolBarWindow(piObject *);
BOOL CheckToolBarWindows(void);
BOOL CheckBOOPSIWindow(piObject *);
BOOL CheckDataTypeWindow(piObject *);
BOOL CheckGadToolsWindow(piObject *);
BOOL CheckPlugInWindow(piObject *,ULONG);
BOOL CheckAppWindow(void);
BOOL CheckProjectWindow(void);

//(APTR)GetTagData(GT_VisualInfo,0,tags);\n"

/*TEXT gadgetfuncs[]="struct TagItem *GetTagItemNum(ULONG cnt,struct TagItem *tags)\n"\
"{\n"\
"	ULONG lc,tc,*ti;\n\n	ti=(ULONG *)tags;\n"\
"	for(lc=0,tc=0;lc<cnt;lc++)\n"\
"	{\n"\
"		tc+=2;\n"\
"		while(ti[tc]!=TAG_DONE) tc+=2;\n"\
"		tc++;\n"\
"	}\n"\
"	return (struct TagItem *)&ti[tc+2];\n"\
"}\n\n"\
*/
TEXT adaptivityfuncs[]="UWORD AdaptX(UWORD value)\n"\
"{\n"\
"	return((UWORD)(((FontX*value)+%ld)/%ld));\n"\
"}\n"\
"\n"\
"UWORD AdaptY(UWORD value)\n"\
"{\n"\
"	return((UWORD)(((FontY*value)+%ld)/%ld));\n"\
"}\n"\
"\n"\
"void CalcAdaptivity(UWORD width, UWORD height)\n"\
"{\n"\
"	Font = &Attr;\n"\
"	Font->ta_Name = (STRPTR)Scr->RastPort.Font->tf_Message.mn_Node.ln_Name;\n"\
"	Font->ta_YSize = FontY = Scr->RastPort.Font->tf_YSize;\n"\
"	FontX = Scr->RastPort.Font->tf_XSize;\n"\
"\n"\
"	if(width && height)\n"\
"		if((AdaptX(width) > Scr->Width) || (AdaptY(height) > Scr->Height)) goto UseTopaz;\n"\
"	return;\n"\
"\n"\
"UseTopaz:\n"\
"	Font->ta_Name = (STRPTR)\"topaz.font\";\n"\
"	FontX = FontY = Font->ta_YSize = 8;\n"\
"}\n\n";

TEXT gadgetfuncs[]="struct Gadget *CreateGadgetExA(UWORD offx,UWORD offy,UWORD offr,UWORD offb,struct Gadget *prev,struct TagItem *tags)\n"\
"{\n"\
"	struct Gadget *g;\n"\
"	struct NewGadget ng;\n"\
"	ULONG kind,flags,x,y,w,h;\n"\
"	BOOL rx=0,ry=0,rw=0,rh=0;\n"\
"	struct TagItem *ti;\n"\
"	\n"\
"	kind=tags[0].ti_Tag;\n"\
"	flags=tags[0].ti_Data;\n"\
"	tags=&tags[1];\n"\
"\n"\
"	if(ti=FindTagItem(GA_Left,tags)) x=AdaptX(ti->ti_Data)+offx;\n"\
"	else { x=AdaptX(GetTagData(GA_RelRight,0,tags))-offr; rx=TRUE; }\n"\
"\n"\
"	if(ti=FindTagItem(GA_Top,tags)) y=AdaptY(ti->ti_Data)+offy;\n"\
"	else { y=AdaptY(GetTagData(GA_RelBottom,0,tags))-offb; ry=TRUE; }\n"\
"\n"\
"	if(ti=FindTagItem(GA_Width,tags)) w=AdaptX(ti->ti_Data);\n"\
"	else { w=AdaptX(GetTagData(GA_RelWidth,0,tags))-x-offr; rw=TRUE; }\n"\
"\n"\
"	if(ti=FindTagItem(GA_Height,tags)) h=AdaptY(ti->ti_Data);\n"\
"	else { h=AdaptY(GetTagData(GA_RelHeight,0,tags))-y-offb; rh=TRUE; }\n"\
"\n"\
"	switch(kind)\n"\
"	{\n";
TEXT gadtoolsfuncs[]="		case BUTTON_KIND:\n"\
"		case CHECKBOX_KIND:\n"\
"		case INTEGER_KIND:\n"\
"		case LISTVIEW_KIND:\n"\
"		case MX_KIND:\n"\
"		case NUMBER_KIND:\n"\
"		case CYCLE_KIND:\n"\
"		case PALETTE_KIND:\n"\
"		case SCROLLER_KIND:\n"\
"		case SLIDER_KIND:\n"\
"		case STRING_KIND:\n"\
"		case TEXT_KIND:\n"\
"			g=gtbCreateGadget(kind,NULL,NULL,(prev?GA_Previous:TAG_IGNORE),prev,GT_ScreenObj,MainScr,TAG_MORE,tags);\n"\
"			break;\n";
TEXT colorwheelfuncs[]="		case COLORWHEEL_KIND:\n"\
"			FindTagItem(WHEEL_Screen,ti)->ti_Data=(ULONG) Scr;\n"\
"			g=gtbCreateGadget(BOOPSI_KIND,NULL,\"colorwheel.gadget\",(prev?GA_Previous:TAG_IGNORE),prev,GT_ScreenObj,MainScr,TAG_MORE,tags);\n"\
"			break;\n";
TEXT gradientsliderfuncs[]="		case GRADIENTSLIDER_KIND:\n"\
"			g=gtbCreateGadget(BOOPSI_KIND,NULL,\"gradientslider.gadget\",(prev?GA_Previous:TAG_IGNORE),prev,GT_ScreenObj,MainScr,TAG_MORE,tags);\n"\
"			break;\n";
TEXT pagefuncs[]="		case PAGE_KIND:\n"\
"			g=gtbCreateGadget(BOOPSI_KIND,PageClass,NULL,(prev?GA_Previous:TAG_IGNORE),prev,GT_ScreenObj,MainScr,TAG_MORE,tags);\n"\
"			break;\n";
TEXT popupmenufuncs[]="		case POPUPMENU_KIND:\n"\
"			g=gtbCreateGadget(BOOPSI_KIND,PopUpMenuClass,NULL,(prev?GA_Previous:TAG_IGNORE),prev,GT_ScreenObj,MainScr,TAG_MORE,tags);\n"\
"			break;\n";
TEXT progressfuncs[]="		case PROGRESS_KIND:\n"\
"			g=gtbCreateGadget(BOOPSI_KIND,ProgressClass,NULL,(prev?GA_Previous:TAG_IGNORE),prev,GT_ScreenObj,MainScr,TAG_MORE,tags);\n"\
"			break;\n";
TEXT tapedeckfuncs[]="		case TAPEDECK_KIND:\n"\
"			g=gtbCreateGadget(BOOPSI_KIND,NULL,\"tapedeck.gadget\",(prev?GA_Previous:TAG_IGNORE),prev,GT_ScreenObj,MainScr,TAG_MORE,tags);\n"\
"			break;\n";
TEXT textfieldfuncs[]="		case TEXTFIELD_KIND:\n"\
"			g=gtbCreateGadget(BOOPSI_KIND,TextFieldClass,NULL,(prev?GA_Previous:TAG_IGNORE),prev,GT_ScreenObj,MainScr,TAG_MORE,tags);\n"\
"			break;\n";
TEXT treeviewfuncs[]="		case TREEVIEW_KIND:\n"\
"			g=gtbCreateGadget(BOOPSI_KIND,ListViewClass,NULL,(prev?GA_Previous:TAG_IGNORE),prev,GT_ScreenObj,MainScr,TAG_MORE,tags);\n"\
"			break;\n";
TEXT datatypefuncs[]="		case DATATYPE_KIND:\n"\
"			g=gtbCreateGadget(DATATYPE_KIND,(APTR)GetTagData(GA_Text,0,tags),NULL,(prev?GA_Previous:TAG_IGNORE),prev,GT_ScreenObj,MainScr,TAG_MORE,tags);\n"\
"			break;\n";
TEXT gadgetendfuncs[]="	}\n"\
"	return g;\n"\
"}\n"\
"\n"\
"void DeleteGadgetExA(struct Gadget *g,struct TagItem *tags)\n"\
"{\n"\
"	DisposeObject(g);\n"\
"}\n"\
"\n";
TEXT calcfuncs[]="void CalcLeftTop(UWORD *offx,UWORD *offy,struct TagItem *tags)\n"\
"{\n"\
"	struct TagItem *ti;\n"\
"	BOOL bl;\n"\
"\n"\
"	*offx=0;\n"\
"	*offy=0;\n"\
"\n"\
"	if(ti=FindTagItem(WA_Borderless,tags))\n"\
"		bl=ti->ti_Data;\n"\
"	else\n"\
"		bl=((GetTagData(WA_Flags,NULL,tags)&WFLG_BORDERLESS)?TRUE:FALSE);\n"\
"\n"\
"	if(ti=FindTagItem(WA_ToolDragBar,tags))\n"\
"	{\n"\
"		if(ti->ti_Data==TOOL_HORIZ)\n"\
"		{\n"\
"			(*offy)+=8;\n"\
"			if(!bl) (*offx)+=Scr->WBorLeft;\n"\
"		}\n"\
"		else\n"\
"		{\n"\
"			(*offx)+=8;\n"\
"			if(!bl) (*offy)+=Scr->WBorTop;\n"\
"		}\n"\
"	}\n"\
"	else\n"\
"	{\n"\
"		if(!bl)\n"\
"		{\n"\
"			(*offx)+=Scr->WBorLeft;\n"\
"			if(GetTagData(WA_Title,NULL,tags))\n"\
"				(*offy)+=Scr->WBorTop+Scr->RastPort.TxHeight+1;\n"\
"			else\n"\
"				(*offy)+=Scr->WBorTop;\n"\
"		}\n"\
"	}\n"\
"}\n"\
"\n"\
"void CalcRightBottom(UWORD *offx,UWORD *offy,struct TagItem *tags)\n"\
"{\n"\
"	struct TagItem *ti;\n"\
"	BOOL bl,sg,sbb,sbr;\n"\
"	UWORD sw,sh;\n"\
"	struct Image *im;\n"\
"\n"\
"	*offx=0;\n"\
"	*offy=0;\n"\
"\n"\
"	if(ti=FindTagItem(WA_Borderless,tags))\n"\
"		bl=ti->ti_Data;\n"\
"	else\n"\
"		bl=(GetTagData(WA_Flags,NULL,tags)&WFLG_BORDERLESS);\n"\
"\n"\
"	if(ti=FindTagItem(WA_SizeGadget,tags))\n"\
"		sg=ti->ti_Data;\n"\
"	else\n"\
"		sg=(GetTagData(WA_Flags,NULL,tags)&WFLG_SIZEGADGET);\n"\
"\n"\
"	if(ti=FindTagItem(WA_SizeBBottom,tags))\n"\
"		sbb=ti->ti_Data;\n"\
"	else\n"\
"		sbb=(GetTagData(WA_Flags,NULL,tags)&WFLG_SIZEBBOTTOM);\n"\
"\n"\
"	if(ti=FindTagItem(WA_SizeBRight,tags))\n"\
"		sbr=ti->ti_Data;\n"\
"	else\n"\
"		sbr=(GetTagData(WA_Flags,NULL,tags)&WFLG_SIZEBRIGHT);\n"\
"\n"\
"	if(im=(struct Image *)NewObject(NULL, SYSICLASS,\n"\
"		SYSIA_DrawInfo, DrawInfo,\n"\
"		SYSIA_Which, SIZEIMAGE,\n"\
"		SYSIA_Size, (Scr->Flags & SCREENHIRES ? SYSISIZE_MEDRES : SYSISIZE_LOWRES),\n"\
"		TAG_DONE))\n"\
"	{\n"\
"		sw=im->Width;\n"\
"		sh=im->Height;\n"\
"		DisposeObject((Object *)im);\n"\
"	}\n"\
"	else\n"\
"	{\n"\
"		sw=18;\n"\
"		sh=10;\n"\
"	}\n"\
"\n"\
"	if(!bl)\n"\
"	{\n"\
"		if(sg && (!sbb) && (!sbr)) { (*offx)+=sw; (*offy)+=Scr->WBorBottom; }\n"\
"		if(sg && (sbb) && (!sbr)) { (*offx)+=Scr->WBorRight; (*offy)+=sh; }\n"\
"		if(sg && (!sbb) && (sbr)) { (*offx)+=sw; (*offy)+=Scr->WBorBottom; }\n"\
"		if(sg && sbb && sbr) { (*offx)+=sw; (*offy)+=sh; }\n"\
"		if((!sg) && (sbb) && (!sbr)) { (*offx)+=Scr->WBorRight; (*offy)+=sh; }\n"\
"		if((!sg) && (!sbb) && (sbr)) { (*offx)+=sw; (*offy)+=Scr->WBorBottom; }\n"\
"		if((!sg) && (sbb) && (sbr)) { (*offx)+=sw; (*offy)+=sh; }\n"\
"	}\n"\
"}\n";


void WriteIDCMPFlags(BPTR handle,piObject *wo)
{
	struct Node_Window *nw;

	nw=(struct Node_Window *)OBJECT_DATA(wo);
	if(nw->nw_IDCMP==NULL) FPuts(handle,"NULL, ");
	GetFlag(nw->nw_IDCMP,IDCMP_CLOSEWINDOW) FPuts(handle,"IDCMP_CLOSEWINDOW|");
	GetFlag(nw->nw_IDCMP,IDCMP_MOUSEMOVE) FPuts(handle,"IDCMP_MOUSEMOVE|");
	GetFlag(nw->nw_IDCMP,IDCMP_GADGETDOWN) FPuts(handle,"IDCMP_GADGETDOWN|");
	GetFlag(nw->nw_IDCMP,IDCMP_GADGETUP) FPuts(handle,"IDCMP_GADGETUP|");
	GetFlag(nw->nw_IDCMP,IDCMP_MENUVERIFY) FPuts(handle,"IDCMP_MENUVERIFY|");
	GetFlag(nw->nw_IDCMP,IDCMP_SIZEVERIFY) FPuts(handle,"IDCMP_SIZEVERIFY|");
	GetFlag(nw->nw_IDCMP,IDCMP_REQVERIFY) FPuts(handle,"IDCMP_REQVERIFY|");
	GetFlag(nw->nw_IDCMP,IDCMP_REQSET) FPuts(handle,"IDCMP_REQSET|");
	GetFlag(nw->nw_IDCMP,IDCMP_REQCLEAR) FPuts(handle,"IDCMP_REQCLEAR|");
	GetFlag(nw->nw_IDCMP,IDCMP_NEWSIZE) FPuts(handle,"IDCMP_NEWSIZE|");
	GetFlag(nw->nw_IDCMP,IDCMP_REFRESHWINDOW) FPuts(handle,"IDCMP_REFRESHWINDOW|");
	GetFlag(nw->nw_IDCMP,IDCMP_MOUSEBUTTONS) FPuts(handle,"IDCMP_MOUSEBUTTONS|");
	GetFlag(nw->nw_IDCMP,IDCMP_MENUPICK) FPuts(handle,"IDCMP_MENUPICK|");
	GetFlag(nw->nw_IDCMP,IDCMP_RAWKEY) FPuts(handle,"IDCMP_RAWKEY|");
	GetFlag(nw->nw_IDCMP,IDCMP_VANILLAKEY) FPuts(handle,"IDCMP_VANILLAKEY|");
	GetFlag(nw->nw_IDCMP,IDCMP_INTUITICKS) FPuts(handle,"IDCMP_INTUITICKS|");
	GetFlag(nw->nw_IDCMP,IDCMP_DELTAMOVE) FPuts(handle,"IDCMP_DELTAMOVE|");
	GetFlag(nw->nw_IDCMP,IDCMP_NEWPREFS) FPuts(handle,"IDCMP_NEWPREFS|");
	GetFlag(nw->nw_IDCMP,IDCMP_ACTIVEWINDOW) FPuts(handle,"IDCMP_ACTIVEWINDOW|");
	GetFlag(nw->nw_IDCMP,IDCMP_INACTIVEWINDOW) FPuts(handle,"IDCMP_INACTIVEWINDOW|");
	GetFlag(nw->nw_IDCMP,IDCMP_DISKINSERTED) FPuts(handle,"IDCMP_DISKINSERTED|");
	GetFlag(nw->nw_IDCMP,IDCMP_DISKREMOVED) FPuts(handle,"IDCMP_DISKREMOVED|");
	GetFlag(nw->nw_IDCMP,IDCMP_IDCMPUPDATE) FPuts(handle,"IDCMP_IDCMPUPDATE|");
	GetFlag(nw->nw_IDCMP,IDCMP_CHANGEWINDOW) FPuts(handle,"IDCMP_CHANGEWINDOW|");
	GetFlag(nw->nw_IDCMP,IDCMP_MENUHELP) FPuts(handle,"IDCMP_MENUHELP|");
	GetFlag(nw->nw_IDCMP,IDCMP_GADGETHELP) FPuts(handle,"IDCMP_GADGETHELP|");
	if(CheckPlugInWindow(wo,BUTTON_ID)) FPuts(handle,"BUTTONIDCMP|");
	if(CheckPlugInWindow(wo,CHECKBOX_ID)) FPuts(handle,"CHECKBOXIDCMP|");
	if(CheckPlugInWindow(wo,INTEGER_ID)) FPuts(handle,"INTEGERIDCMP|");
	if(CheckPlugInWindow(wo,LISTVIEW_ID)) FPuts(handle,"LISTVIEWIDCMP|");
	if(CheckPlugInWindow(wo,MX_ID)) FPuts(handle,"MXIDCMP|");
	if(CheckPlugInWindow(wo,NUMBER_ID)) FPuts(handle,"NUMBERIDCMP|");
	if(CheckPlugInWindow(wo,CYCLE_ID)) FPuts(handle,"CYCLEIDCMP|");
	if(CheckPlugInWindow(wo,PALETTE_ID)) FPuts(handle,"PALETTEIDCMP|");
	if(CheckPlugInWindow(wo,SCROLLER_ID)) FPuts(handle,"SCROLLERIDCMP|");
	if(CheckPlugInWindow(wo,SLIDER_ID)) FPuts(handle,"SLIDERIDCMP|");
	if(CheckPlugInWindow(wo,STRING_ID)) FPuts(handle,"STRINGIDCMP|");
	if(CheckPlugInWindow(wo,TEXT_ID)) FPuts(handle,"TEXTIDCMP|");
	Seek(handle,-1,OFFSET_CURRENT);
	FPuts(handle,", ");
}

void WriteWindowFlags(BPTR handle,piObject *wo)
{
	struct Node_Window *nw;

	nw=(struct Node_Window *)OBJECT_DATA(wo);
	if(nw->nw_Flags==NULL) FPuts(handle,"NULL, ");
	GetFlag(nw->nw_Flags,WFLG_CLOSEGADGET) FPuts(handle,"WFLG_CLOSEGADGET|");
	if((!(nw->nw_MyFlags&MYWA_DragToolHoriz)) && (!(nw->nw_MyFlags&MYWA_DragToolVert))) GetFlag(nw->nw_Flags,WFLG_DRAGBAR) FPuts(handle,"WFLG_DRAGBAR|");
	GetFlag(nw->nw_Flags,WFLG_DEPTHGADGET) FPuts(handle,"WFLG_DEPTHGADGET|");
	GetFlag(nw->nw_Flags,WFLG_SIZEGADGET) FPuts(handle,"WFLG_SIZEGADGET|");
	GetFlag(nw->nw_Flags,WFLG_SIZEBRIGHT) FPuts(handle,"WFLG_SIZEBRIGHT|");
	GetFlag(nw->nw_Flags,WFLG_SIZEBBOTTOM) FPuts(handle,"WFLG_SIZEBBOTTOM|");
	GetFlag(nw->nw_Flags,WFLG_GIMMEZEROZERO) FPuts(handle,"WFLG_GIMMEZEROZERO|");
	GetFlag(nw->nw_Flags,WFLG_SIMPLE_REFRESH) FPuts(handle,"WFLG_SIMPLE_REFRESH|");
	GetFlag(nw->nw_Flags,WFLG_SMART_REFRESH) FPuts(handle,"WFLG_SMART_REFRESH|");
	GetFlag(nw->nw_Flags,WFLG_SUPER_BITMAP) FPuts(handle,"WFLG_SUPER_BITMAP|");
	GetFlag(nw->nw_Flags,WFLG_BACKDROP) FPuts(handle,"WFLG_BACKDROP|");
	GetFlag(nw->nw_Flags,WFLG_REPORTMOUSE) FPuts(handle,"WFLG_REPORTMOUSE|");
	GetFlag(nw->nw_Flags,WFLG_BORDERLESS) FPuts(handle,"WFLG_BORDERLESS|");
	GetFlag(nw->nw_Flags,WFLG_ACTIVATE) FPuts(handle,"WFLG_ACTIVATE|");
	GetFlag(nw->nw_Flags,WFLG_NOCAREREFRESH) FPuts(handle,"WFLG_NOCAREREFRESH|");
	GetFlag(nw->nw_Flags,WFLG_NEWLOOKMENUS) FPuts(handle,"WFLG_NEWLOOKMENUS|");
	Seek(handle,-1,OFFSET_CURRENT);
	FPuts(handle,", ");
}

void FUNC_GenerateWindowTags(BPTR handle,piObject *wo)
{
	struct Node_Window *nw;

	nw=(struct Node_Window *)OBJECT_DATA(wo);
	FPuts(handle,"	");
	switch(nw->nw_XPos)
	{
		case WPOS_CENTER: FPrintf(handle,"WIND_CenterX, TRUE, "); break;
		case WPOS_REL_LEFT_TOP: FPrintf(handle,"WIND_Left, %ld, ",nw->nw_X); break;
		case WPOS_REL_RIGHT_BOTTOM: FPrintf(handle,"WIND_RelRight, %ld, ",nw->nw_X); break;
	}
	switch(nw->nw_YPos)
	{
		case WPOS_CENTER: FPrintf(handle,"WIND_CenterY, TRUE, "); break;
		case WPOS_REL_LEFT_TOP: FPrintf(handle,"WIND_Top, %ld, ",nw->nw_Y); break;
		case WPOS_REL_RIGHT_BOTTOM: FPrintf(handle,"WIND_RelBottom, %ld, ",nw->nw_Y); break;
	}
	GetFlag(nw->nw_MyFlags,MYWA_InnerWidth) FPrintf(handle,"WIND_InnerWidth, %ld, ",nw->nw_Width);
	else FPrintf(handle,"WIND_Width, %ld, ",nw->nw_Width);
	GetFlag(nw->nw_MyFlags,MYWA_InnerWidth) FPrintf(handle,"WIND_InnerHeight, %ld, ",nw->nw_Height);
	else FPrintf(handle,"WIND_Height, %ld, ",nw->nw_Height);
	if(!(nw->nw_MyFlags&MYWA_DragToolHoriz))
		if(!(nw->nw_MyFlags&MYWA_DragToolVert))
			FPrintf(handle,"WA_Title, (ULONG) MSG_%s_WND, ",nw->nw_Label);
	if(nw->nw_MyFlags&MYWA_DragToolHoriz) FPuts(handle,"WIND_ToolDragBar, TOOL_HORIZ, ");
	if(nw->nw_MyFlags&MYWA_DragToolVert) FPuts(handle,"WIND_ToolDragBar, TOOL_VERT, ");
	if(nw->nw_ScreenTitle[0]) FPrintf(handle,"WIND_ScreenTitle, (ULONG) MSG_%s_ST_WND, ",nw->nw_Label);
	if(!(nw->nw_MyFlags&MYWA_Project)){ FPuts(handle,"WIND_IDCMP, "); WriteIDCMPFlags(handle,wo); }
	FPuts(handle,"WIND_Flags, "); WriteWindowFlags(handle,wo);
	GetFlag(nw->nw_MyFlags,MYWA_AutoAdjust) FPuts(handle,"WIND_AutoAdjust, TRUE, ");
	GetFlag(nw->nw_MyFlags,MYWA_PubScreenFallBack) FPuts(handle,"WIND_PubScreenFallBack, TRUE, ");
	GetFlag(nw->nw_MyFlags,MYWA_RMBTrap) FPuts(handle,"WIND_RMBTrap, TRUE, ");
	GetFlag(nw->nw_MyFlags,MYWA_AutoAdjust) FPuts(handle, "WIND_AutoAdjust, TRUE, ");
//	GetFlag(nw->nw_MyFlags,MYWA_FontAdaptive) FPuts(handle, "WIND_FontAdaptive, TRUE, ");
	GetFlag(nw->nw_Flags,WFLG_SIZEGADGET) FPuts(handle,"WIND_MaxWidth,-1, WIND_MinWidth,100, WIND_MaxHeight,-1, WIND_MinHeight, 20, ");
}

void FUNC_GenerateApplication(BPTR handle)
{
	struct Node_Version *nv;

	nv=(struct Node_Version *)OBJECT_DATA(GUI->gui_Node_Version);

	FPuts(handle,"/* Application functions */\n\n");
	if(OPT_UseLocale)
	{
		FPuts(handle,"UWORD InitApplication(STRPTR language)\n{\n");
		FPuts(handle,	 "	if(!(Locale=OpenLocale(NULL))) return GTBERR_LOCALE;\n");
		FPrintf(handle,"	Catalog=OpenCatalog(Locale,\"%s.catalog\",\n" \
									 "			OC_BuiltInLanguage,\"english\", OC_Version, VER_VERSION,\n" \
									 "			language?OC_Language:TAG_IGNORE, language, TAG_DONE);\n",nv->nv_ProgramName);
	}
	else
	{
		FPuts(handle,"UWORD InitApplication()\n{\n");
	}
	FPuts(handle,"	if(!(AppObj=gtbNewApplication(TAG_DONE))) return GTBERR_OPENLIB;\n");
	if(CheckAppWindow()) FPuts(handle,"	if(!(AppMsgPort=NewObject(AppObj->MsgClass,NULL,\n"\
																				MESG_Handler,(ULONG) Handle_AppMsgPort,\n"\
																				TAG_DONE))) return GTBERR_CREATEAPPPORT;\n");
	if(CheckProjectWindow()) FPuts(handle,"	if(!(ProjectMsgPort=NewObject(AppObj->MsgClass,NULL,\n"\
																				MESG_Handler,(ULONG) Handle_AppMsgPort,\n"\
																				TAG_DONE))) return GTBERR_CREATEPROJECTPORT;\n");
	if(CheckPlugIn(COLORWHEEL_ID)) GADGET_GenerateOpenLib(handle,COLORWHEEL_ID);
	if(CheckPlugIn(GRADIENTSLIDER_ID)) GADGET_GenerateOpenLib(handle,GRADIENTSLIDER_ID);
	if(CheckPlugIn(PAGE_ID)) GADGET_GenerateOpenLib(handle,PAGE_ID);
	if(CheckPlugIn(POPUPMENU_ID)) GADGET_GenerateOpenLib(handle,POPUPMENU_ID);
	if(CheckPlugIn(PROGRESS_ID)) GADGET_GenerateOpenLib(handle,PROGRESS_ID);
	if(CheckPlugIn(TAPEDECK_ID)) GADGET_GenerateOpenLib(handle,TAPEDECK_ID);
	if(CheckPlugIn(TEXTFIELD_ID)) GADGET_GenerateOpenLib(handle,TEXTFIELD_ID);
	if(CheckPlugIn(TREEVIEW_ID)) GADGET_GenerateOpenLib(handle,TREEVIEW_ID);
	if(CheckToolBarWindows())
	{
		FPuts(handle,"	if(!(SmartBitMapBase=OpenLibrary(\"images/smartbitmap.image\",39))) return GTBERR_OPENLIB_SMARTBITMAP;\n");
		FPuts(handle,"	SmartBitMapClass=GetSmartBitMapClass();\n");
	}
	FPuts(handle,"	return GTBERR_OK;\n}\n\n");

	FPuts(handle,"void FreeApplication()\n{\n");
	if(CheckToolBarWindows()) FPuts(handle,"	if(SmartBitMapBase) CloseLibrary(SmartBitMapBase); SmartBitMapBase=NULL; SmartBitMapClass=NULL;\n");
	if(CheckPlugIn(TREEVIEW_ID)) GADGET_GenerateCloseLib(handle,TREEVIEW_ID);
	if(CheckPlugIn(TEXTFIELD_ID)) GADGET_GenerateCloseLib(handle,TEXTFIELD_ID);
	if(CheckPlugIn(TAPEDECK_ID)) GADGET_GenerateCloseLib(handle,TAPEDECK_ID);
	if(CheckPlugIn(PROGRESS_ID)) GADGET_GenerateCloseLib(handle,PROGRESS_ID);
	if(CheckPlugIn(POPUPMENU_ID)) GADGET_GenerateCloseLib(handle,POPUPMENU_ID);
	if(CheckPlugIn(PAGE_ID)) GADGET_GenerateCloseLib(handle,PAGE_ID);
	if(CheckPlugIn(GRADIENTSLIDER_ID)) GADGET_GenerateCloseLib(handle,GRADIENTSLIDER_ID);
	if(CheckPlugIn(COLORWHEEL_ID)) GADGET_GenerateCloseLib(handle,COLORWHEEL_ID);
	if(CheckProjectWindow()) FPuts(handle,"	if(ProjectMsgPort) DisposeObject(ProjectMsgPort); ProjectMsgPort=NULL;\n");
	if(CheckAppWindow()) FPuts(handle,"	if(AppMsgPort) DisposeObject(AppMsgPort); AppMsgPort=NULL;\n");
	FPuts(handle,"	if(AppObj) gtbDisposeApplication(AppObj); AppObj=NULL:
	if(OPT_UseLocale)
	{
		FPuts(handle,"	if(Catalog) CloseCatalog(Catalog); Catalog=NULL;\n");
	}
	FPuts(handle,"}\n\n");

	if(OPT_UseLocale)
	{
		FPuts(handle,"STRPTR GetCatString(APTR fcstr)\n{\n" \
								 "	STRPTR defaultstr;\n" \
								 "	LONG strnum;\n\n" \
								 "	strnum=((struct FC_Type *)fcstr)->ID;\n" \
								 "	defaultstr=((struct FC_Type *)fcstr)->Str;\n\n" \
								 "	return(Catalog ? GetCatalogStr(Catalog,strnum,defaultstr) : defaultstr);\n" \
								 "}\n\n");
	}
	else
	{
		FPuts(handle,"STRPTR GetCatString(APTR fcstr)\n{\n" \
								 "	return ((struct FC_Type *)fcstr)->Str;\n" \
								 "}\n\n");
	}

	if(OPT_GUIFuncs)
	{
		FPuts(handle,gadgetfuncs);
		if(CheckToolBarWindows()) FPuts(handle,toolimagefuncs);
		FPuts(handle,gadtoolsfuncs);
		if(CheckPlugIn(COLORWHEEL_ID)) FPuts(handle,colorwheelfuncs);
		if(CheckPlugIn(DATATYPE_ID)) FPuts(handle,datatypefuncs);
		if(CheckPlugIn(GRADIENTSLIDER_ID)) FPuts(handle,gradientsliderfuncs);
		if(CheckPlugIn(PAGE_ID)) FPuts(handle,pagefuncs);
		if(CheckPlugIn(POPUPMENU_ID)) FPuts(handle,popupmenufuncs);
		if(CheckPlugIn(PROGRESS_ID)) FPuts(handle,progressfuncs);
		if(CheckPlugIn(TAPEDECK_ID)) FPuts(handle,tapedeckfuncs);
		if(CheckPlugIn(TEXTFIELD_ID)) FPuts(handle,textfieldfuncs);
		if(CheckPlugIn(TREEVIEW_ID)) FPuts(handle,treeviewfuncs);
		FPuts(handle,gadgetendfuncs);
		if(CheckPlugIn(DATATYPE_ID)) FPuts(handle,datatypeendfuncs);
		FPuts(handle,gadgetendendfuncs);
		FPuts(handle,windowfuncs);
		FPuts(handle,calcfuncs);
		FPrintf(handle,adaptivityfuncs,GTBSem->gs_FontTF->tf_XSize/2,GTBSem->gs_FontTF->tf_XSize,GTBSem->gs_FontTF->tf_YSize/2,GTBSem->gs_FontTF->tf_YSize);
	}
} /* FUNC_GenerateApplication */

void FUNC_GenerateScreen(BPTR handle)
{
	piObject *mo,*wo,*wo2,*go,*io,*fo;
	struct Node_Menu *nm;
	struct Node_Window *nw,*nw2;
	struct Node_Gadget *ng;
	struct Node_Screen *ns;
	struct Node_Image *ni;
	struct Node *n;
	ULONG cnt;

	ns=(struct Node_Screen *)OBJECT_DATA(GUI->gui_Node_Screen);
	FPuts(handle,"/* Screen functions */\n\n");
	FPuts(handle,"UWORD SetupScreen(UBYTE Num)\n{\n	UWORD OffX,OffY;\n\n");
	if(OPT_Screen)
	{
		if(ns->ns_Type!=TYPE_CUSTOMSCREEN) FPrintf(handle,"	sprintf(ScrPubName,\"%s\",Num);\n",ns->ns_Public);
		if(ns->ns_Type!=TYPE_WORKBENCH) FPrintf(handle,"	sprintf(ScrTitle,GetCatString(MSG_SCREENTITLE_SCR),Num);\n");
		FPrintf(handle,"	if(!(MainScr=NewObjectA(AppObj->ga_ScrClass,NULL,(struct TagItem *)ScrTags))) return GTBERR_OPENSCREEN;\n");
		FPuts(handle,	GetAttr(SCRN_Screen,MainScr,(ULONG)&Scr);\n");
	}
	if(OPT_GUIFuncs)
	{
		FPuts(handle,"	OffX=Scr->WBorLeft; OffY=Scr->RastPort.TxHeight+Scr->WBorTop+1;\n\n");
		FPuts(handle,"	CalcAdaptivity(0,0);\n");
	}

	if(CheckToolBarWindows())
		FPuts(handle,"	GetRGB32(Scr->ViewPort.ColorMap,DrawInfo->dri_Pens[BACKGROUNDPEN],1,&ToolBarPalette[0].R);\n\n");

	for(mo=(piObject *)GetHead(&GUI->gui_List_Menu);GetSucc(mo);mo=(piObject *)GetSucc(mo))
	{
		nm=(struct Node_Menu *)OBJECT_DATA(mo);
		FPrintf(handle,"	if(!MenuLocaleDone[MN_%s]) FixMenus(%sNewMenu);\n",nm->nm_Label,nm->nm_Label);
		FPrintf(handle,"	MenuLocaleDone[MN_%s]=TRUE;\n\n",nm->nm_Label);
		FPrintf(handle,"	if(!(%sMenus=CreateMenus(%sNewMenu, GTMN_FrontPen, %s, ",nm->nm_Label,nm->nm_Label,nm->nm_Flags&MYMN_NewLookMenus?"DrawInfo->dri_Pens[BARDETAILPEN]":"0");
		GetFlag(nm->nm_Flags,MYMN_NewLookMenus) FPuts(handle,"GTMN_NewLookMenus, TRUE, ");
		FPuts(handle,"TAG_DONE))) return GTBERR_CREATEMENUS;\n");
		FPrintf(handle,"	LayoutMenus(%sMenus, VisualInfo, TAG_DONE);\n\n",nm->nm_Label);
	}

	if(OPT_GUIFuncs)
	{
		for(wo=(piObject *)GetHead(&GUI->gui_List_Window);GetSucc(wo);wo=(piObject *)GetSucc(wo))
		{
			nw=(struct Node_Window *)OBJECT_DATA(wo);
			for(go=(piObject *)GetHead(&nw->nw_List_Gadget);GetSucc(go);go=(piObject *)GetSucc(go))
			{
				ng=(struct Node_Gadget *)OBJECT_DATA(go);
				if(ng->ng_PlugInID==PAGE_ID)
				{
					FPrintf(handle,"	BBOX_%s.Left=AdaptX(BBOX_%s.Left)+OffX;\n" \
												 "	BBOX_%s.Top=AdaptY(BBOX_%s.Top)+OffY;\n" \
												 "	BBOX_%s.Width=AdaptX(BBOX_%s.Width);\n" \
												 "	BBOX_%s.Height=AdaptY(BBOX_%s.Height);\n" \
												 "	BBOX_%s.BoxShine=DrawInfo->dri_Pens[SHINEPEN];\n" \
												 "	BBOX_%s.BoxShadow=DrawInfo->dri_Pens[SHADOWPEN];\n",ng->ng_Label,ng->ng_Label,ng->ng_Label,ng->ng_Label,ng->ng_Label,ng->ng_Label,ng->ng_Label,ng->ng_Label,ng->ng_Label,ng->ng_Label);
					for(n=GetHead(List(GetTagData(TAG_USER+1,0,ng->ng_OwnTags)));GetSucc(n);n=GetSucc(n))
					{
						for(wo2=(piObject *)GetHead(&GUI->gui_List_Window);GetSucc(wo2);wo2=(piObject *)GetSucc(wo2))
						{
							nw2=(struct Node_Window *)OBJECT_DATA(wo2);
							if(!strcmp(nw2->nw_Label,n->ln_Name))
								for(cnt=0,fo=(piObject *)GetHead(List(&nw2->nw_List_Frame));GetSucc(fo);fo=(piObject *)GetSucc(fo),cnt++)
								{
									FPrintf(handle,"	BBOX_%s_%s[%ld].Left=AdaptX(BBOX_%s_%s[%ld].Left)+OffX;\n" \
																 "	BBOX_%s_%s[%ld].Top=AdaptY(BBOX_%s_%s[%ld].Top)+OffY;\n" \
																 "	BBOX_%s_%s[%ld].Width=AdaptX(BBOX_%s_%s[%ld].Width);\n" \
																 "	BBOX_%s_%s[%ld].Height=AdaptY(BBOX_%s_%s[%ld].Height);\n" \
																 "	BBOX_%s_%s[%ld].BoxShine=DrawInfo->dri_Pens[SHINEPEN];\n" \
																 "	BBOX_%s_%s[%ld].BoxShadow=DrawInfo->dri_Pens[SHADOWPEN];\n",
																	ng->ng_Label,nw2->nw_Label,cnt,ng->ng_Label,nw2->nw_Label,cnt,ng->ng_Label,nw2->nw_Label,cnt,ng->ng_Label,nw2->nw_Label,cnt,ng->ng_Label,nw2->nw_Label,cnt,ng->ng_Label,nw2->nw_Label,cnt,ng->ng_Label,nw2->nw_Label,cnt,ng->ng_Label,nw2->nw_Label,cnt,ng->ng_Label,nw2->nw_Label,cnt,ng->ng_Label,nw2->nw_Label,cnt);
								}
						}
					}
				}
			}
		}
	}

	for(io=(piObject *)GetHead(&GUI->gui_List_Image);GetSucc(io);io=(piObject *)GetSucc(io))
	{
		ni=(struct Node_Image *)OBJECT_DATA(io);
		switch(ni->ni_Type)
		{
			case IMTYPE_POINTER:
				FPrintf(handle,"	if(!(%sPtr=NewObject(NULL,POINTERCLASS,POINTERA_BitMap,(ULONG)&%sBitMap,POINTERA_XOffset,%ld,POINTERA_YOffset,%ld,TAG_DONE))) return GTBERR_CREATEPOINTER;\n",ni->ni_Label,ni->ni_Label,-ni->ni_PointX,-ni->ni_PointY);
				break;
			case IMTYPE_NORMAL:
				FPrintf(handle,"	if(!(%sImg=NewObject(SmartBitMapClass,NULL,IA_Width,%ld,SMBM_Border,FALSE,SMBM_BitMap,(ULONG)&%sBitMap,SMBM_Screen,Scr,SMBM_Palette,(ULONG)%sPalette,TAG_DONE))) return GTBERR_CREATEIMAGE;\n",ni->ni_Label,(ULONG)ni->ni_Width,ni->ni_Label,ni->ni_Label);
				break;
			case IMTYPE_TOOLBAR:
				FPrintf(handle,"	if(!(%sTbr=NewObject(SmartBitMapClass,NULL,IA_Width,%ld,SMBM_Border,FALSE,SMBM_SelBorder,TRUE,SMBM_BitMap,(ULONG)&%sBitMap,SMBM_Screen,Scr,SMBM_Palette,(ULONG)ToolBarPalette,TAG_DONE))) return GTBERR_CREATEIMAGE;\n",ni->ni_Label,(ULONG)ni->ni_Width,ni->ni_Label);
				break;
		}
	}
	
	FPuts(handle,"	return GTBERR_OK;\n}\n\n");

	FPuts(handle,"void CloseDownScreen()\n{\n");
	for(io=(piObject *)GetHead(&GUI->gui_List_Image);GetSucc(io);io=(piObject *)GetSucc(io))
	{
		ni=(struct Node_Image *)OBJECT_DATA(io);
		switch(ni->ni_Type)
		{
			case IMTYPE_POINTER:
				FPrintf(handle,"	if(%sPtr) DisposeObject(%sPtr);\n",ni->ni_Label,ni->ni_Label);
				break;
			case IMTYPE_NORMAL:
				FPrintf(handle,"	if(%sImg) DisposeObject(%sImg);\n",ni->ni_Label,ni->ni_Label);
				break;
			case IMTYPE_TOOLBAR:
				FPrintf(handle,"	if(%sTbr) DisposeObject(%sTbr);\n",ni->ni_Label,ni->ni_Label);
				break;
		}
	}
	for(mo=(piObject *)GetHead(&GUI->gui_List_Menu);GetSucc(mo);mo=(piObject *)GetSucc(mo))
	{
		nm=(struct Node_Menu *)OBJECT_DATA(mo);
		FPrintf(handle,"	if(%sMenus) FreeMenus(%sMenus); %sMenus=NULL;\n",nm->nm_Label,nm->nm_Label,nm->nm_Label);
	}
	if(OPT_Screen)
	{
		FPuts(handle,"	if(Scr) DisposeObject(Scr); Scr=NULL;\n");
	}
	FPuts(handle,"}\n\n");
} /* FUNC_GenerateScreen */

void FUNC_GenerateCatalogUpdate(BPTR handle)
{
	if(OPT_GUIFuncs)
	{
		FPuts(handle,"void FixGadgetTags(struct TagItem *tags)\n" \
								 "{\n" \
								 "	ULONG kind,a;\n" \
								 "	UBYTE **labels;\n" \
								 "	struct TagItem *ti;\n" \
								 "	struct Node *node;\n");
		if(CheckPlugIn(PAGE_ID)) FPuts(handle,"	struct Page *page;\n");
		FPuts(handle,"\n" \
								 "	kind=tags[0].ti_Tag;\n" \
								 "	tags=&tags[1];\n" \
								 "\n" \
								 "	if(kind!=TOOLIMAGE_KIND && kind!=DATATYPE_KIND) if(ti=FindTagItem(GA_Text,tags)) ti->ti_Data=(ULONG) GetCatString((APTR)ti->ti_Data);\n" \
								 "\n" \
								 "	switch(kind)\n" \
								 "	{\n" \
								 "		case LISTVIEW_KIND:\n" \
								 "			if(ti=FindTagItem(GTMX_Labels,tags))\n" \
								 "				for(node=(struct Node *)ti->ti_Data;node->ln_Succ;node=node->ln_Succ)\n" \
								 "					node->ln_Name=GetCatString((APTR)node->ln_Name);\n" \
								 "			break;\n" \
								 "		case MX_KIND:\n" \
								 "			if(ti=FindTagItem(GTMX_Labels,tags))\n" \
								 "				for(labels=(UBYTE **) ti->ti_Data,a=0;labels[a];a++) labels[a]=(UBYTE *) GetCatString((APTR)labels[a]);\n" \
								 "			break;\n" \
								 "		case NUMBER_KIND:\n" \
								 "			if(ti=FindTagItem(GTNM_Format,tags)) ti->ti_Data=(ULONG) GetCatString((APTR)ti->ti_Data);\n" \
								 "			break;\n" \
								 "		case CYCLE_KIND:\n" \
								 "			if(ti=FindTagItem(GTCY_Labels,tags))\n" \
								 "				for(labels=(UBYTE **) ti->ti_Data,a=0;labels[a];a++) labels[a]=(UBYTE *) GetCatString((APTR)labels[a]);\n" \
								 "			break;\n" \
								 "		case SLIDER_KIND:\n" \
								 "			if(ti=FindTagItem(GTSL_LevelFormat,tags)) ti->ti_Data=(ULONG) GetCatString((APTR)ti->ti_Data);\n" \
								 "			break;\n" \
								 "		case STRING_KIND:\n" \
								 "			if(ti=FindTagItem(GTST_String,tags)) ti->ti_Data=(ULONG) GetCatString((APTR)ti->ti_Data);\n" \
								 "			break;\n" \
								 "		case TEXT_KIND:\n" \
								 "			if(ti=FindTagItem(GTTX_Text,tags)) ti->ti_Data=(ULONG) GetCatString((APTR)ti->ti_Data);\n" \
								 "			break;\n");
		if(CheckPlugIn(PAGE_ID)) 
		FPuts(handle,"		case PAGE_KIND:\n" \
								 "			if(ti=FindTagItem(PAGE_Pages,tags))\n" \
								 "				for(page=(struct Page *)ti->ti_Data;page;page=page->NextPage)\n" \
								 "					page->Name=GetCatString((APTR)page->Name);\n" \
								 "			break;\n");
		if(CheckPlugIn(POPUPMENU_ID)) 
		FPuts(handle,"		case POPUPMENU_KIND:\n" \
								 "			if(ti=FindTagItem(PUMG_Labels,tags))\n" \
								 "				for(node=(struct Node *)ti->ti_Data;node->ln_Succ;node=node->ln_Succ)\n" \
								 "					node->ln_Name=GetCatString((APTR)node->ln_Name);\n" \
								 "			break;\n");
		if(CheckPlugIn(PROGRESS_ID)) 
		FPuts(handle,"		case PROGRESS_KIND:\n" \
								 "			if(ti=FindTagItem(PROG_Format,tags)) ti->ti_Data=(ULONG) GetCatString((APTR)ti->ti_Data);\n" \
								 "			break;\n");
		if(CheckPlugIn(TREEVIEW_ID)) 
		FPuts(handle,"		case TREEVIEW_KIND:\n" \
								 "			if(ti=FindTagItem(LIST_Format,tags)) ti->ti_Data=(ULONG) GetCatString((APTR)ti->ti_Data);\n" \
								 "			break;\n");
		FPuts(handle,"	}\n" \
								 "}\n\n");
	
		FPuts(handle,"void FixWindowTags(struct TagItem *tags)\n" \
								 "{\n" \
								 "	struct TagItem *ti;\n" \
								 "\n" \
								 "	if(ti=FindTagItem(WA_Title,tags)) ti->ti_Data=(ULONG) GetCatString((APTR)ti->ti_Data);\n" \
								 "	if(ti=FindTagItem(WA_ScreenTitle,tags)) ti->ti_Data=(ULONG) GetCatString((APTR)ti->ti_Data);\n" \
								 "}\n\n");
	
		FPuts(handle,"void FixMenus(struct NewMenu *menu)\n" \
								 "{\n" \
								 "	ULONG a;\n" \
								 "\n" \
								 "	for(a=0;menu[a].nm_Type!=NM_END;a++)\n" \
								 "		if((menu[a].nm_Label!=NM_BARLABEL) && (menu[a].nm_Label))\n" \
								 "		{\n" \
								 "			menu[a].nm_CommKey=GetCatString(menu[a].nm_Label);\n" \
								 "			menu[a].nm_Label=menu[a].nm_CommKey+2;\n" \
								 "			if(menu[a].nm_CommKey[0]=='\\0') menu[a].nm_CommKey=NULL;\n" \
								 "		}\n" \
								 "}\n\n");
	}
}

void FUNC_GenerateWindow(BPTR handle,piObject *wo)
{
	struct Node_Window *nw,*nw2;
	struct Node_Gadget *ng;
	struct Node_Frame *nf;
	struct Node *n;
	piObject *go,*wo2,*fo;
	ULONG cnt,f;
	BOOL rel=FALSE;

	nw=(struct Node_Window *)OBJECT_DATA(wo);
	if(nw->nw_PageGadget==0)
	{
/* HandleIDCMP */
		FPrintf(handle,"ULONG Handler_%s(Object *WindowObj,ULONG type,APTR message)\n{\n",nw->nw_Label);
		FPuts(handle,	 "	struct IntuiMessage *m;\n");
		GetFlag(nw->nw_IDCMP,IDCMP_MENUPICK)
			FPuts(handle,	 "	struct MenuItem *n;\n");
		FPuts(handle,	 "	int (*func)(void);\n\n");

		FPuts(handle,"	switch(type)\n	{\n"\
			"	case TYPE_INTUIMESSAGE:\n"\
			"	m=(struct IntuiMessage *)message;\n"\
			"	switch(m->Class)\n"\
			"	{\n");
		GetFlag(nw->nw_IDCMP,IDCMP_MENUVERIFY)
			FPrintf(handle,"		case IDCMP_MENUVERIFY:\n" \
									 "			running=FN_MenuVerify_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_SIZEVERIFY)
			FPrintf(handle,"		case IDCMP_SIZEVERIFY:\n" \
									 "			running=FN_SizeVerify_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_REQVERIFY)
			FPrintf(handle,"		case IDCMP_REQVERIFY:\n" \
									 "			running=FN_ReqVerify_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_REQSET)
			FPrintf(handle,"		case IDCMP_REQSET:\n" \
									 "			running=FN_ReqSet_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_REQCLEAR)
			FPrintf(handle,"		case IDCMP_REQCLEAR:\n" \
									 "			running=FN_ReqClear_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_NEWSIZE)
			FPrintf(handle,"		case IDCMP_NEWSIZE:\n" \
									 "			running=FN_NewSize_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_NEWPREFS)
			FPrintf(handle,"		case IDCMP_NEWPREFS:\n" \
									 "			running=FN_NewPrefs_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_MENUHELP)
			FPrintf(handle,"		case IDCMP_MENUHELP:\n" \
									 "			running=FN_MenuHelp_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_GADGETHELP)
			FPrintf(handle,"		case IDCMP_GADGETHELP:\n" \
									 "			running=FN_GadgetHelp_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_MOUSEMOVE)
			FPrintf(handle,"		case IDCMP_MOUSEMOVE:\n" \
									 "			running=FN_MouseMove_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_MOUSEBUTTONS)
			FPrintf(handle,"		case IDCMP_MOUSEBUTTONS:\n" \
									 "			running=FN_MouseButtons_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_RAWKEY)
			FPrintf(handle,"		case IDCMP_RAWKEY:\n" \
									 "			running=FN_RawKey_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_VANILLAKEY)
			FPrintf(handle,"		case IDCMP_VANILLAKEY:\n" \
									 "			running=FN_VanillaKey_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_INTUITICKS)
			FPrintf(handle,"		case IDCMP_INTUITICKS:\n" \
									 "			running=FN_IntuiTicks_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_IDCMPUPDATE)
			FPrintf(handle,"		case IDCMP_IDCMPUPDATE:\n" \
									 "			running=FN_IDCMPUpdate_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_REFRESHWINDOW)
		{
			FPrintf(handle,"		case IDCMP_REFRESHWINDOW:\n" \
										 "			GT_BeginRefresh(%sWnd);\n",nw->nw_Label);
			if(!IsListEmpty(List(&nw->nw_List_Frame)))
				FPrintf(handle,"			Render_%s();\n",nw->nw_Label);
			FPrintf(handle,"			GT_EndRefresh(%sWnd,TRUE);\n" \
										 "			break;\n",nw->nw_Label);
		}
		GetFlag(nw->nw_IDCMP,IDCMP_CHANGEWINDOW)
			FPrintf(handle,"		case IDCMP_CHANGEWINDOW:\n" \
									 "			running=FN_ChangeWindow_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_CLOSEWINDOW)
			FPrintf(handle,"		case IDCMP_CLOSEWINDOW:\n" \
									 "			running=FN_CloseWindow_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_ACTIVEWINDOW)
			FPrintf(handle,"		case IDCMP_ACTIVEWINDOW:\n" \
									 "			running=FN_ActiveWindow_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_INACTIVEWINDOW)
			FPrintf(handle,"		case IDCMP_INACTIVEWINDOW:\n" \
									 "			running=FN_InActiveWindow_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_DISKREMOVED)
			FPrintf(handle,"		case IDCMP_DISKREMOVED:\n" \
									 "			running=FN_DiskRemoved_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_DISKINSERTED)
			FPrintf(handle,"		case IDCMP_DISKINSERTED:\n" \
									 "			running=FN_DiskInserted_%s();\n" \
									 "			break;\n",nw->nw_Label);
		GetFlag(nw->nw_IDCMP,IDCMP_MENUPICK)
			FPrintf(handle,"		case IDCMP_MENUPICK:\n" \
									 "			while(%sMsg.Code!=MENUNULL)\n" \
									 "			{\n" \
									 "				n=ItemAddress(%sMenus,%sMsg.Code);\n" \
									 "				func=(int (*)(void))(GTMENUITEM_USERDATA(n));\n" \
									 "				running=func();\n" \
									 "				%sMsg.Code=n->NextSelect;\n" \
									 "			}\n" \
									 "			break;\n",nw->nw_Label,nw->nw_MenuTitle,nw->nw_Label,nw->nw_Label);
		if((!IsListEmpty(List(&nw->nw_List_Gadget))) && (nw->nw_IDCMP&IDCMP_GADGETUP || nw->nw_IDCMP&IDCMP_GADGETDOWN))
		{
			GetFlag(nw->nw_IDCMP,IDCMP_GADGETUP) FPuts(handle,"		case IDCMP_GADGETUP:\n");
			GetFlag(nw->nw_IDCMP,IDCMP_GADGETDOWN) FPuts(handle,"		case IDCMP_GADGETDOWN:\n");
			FPrintf(handle,"			func=(int (*)(void))(( struct Gadget * )%sMsg.IAddress )->UserData;\n" \
									 "			running=func();\n" \
									 "			break;\n",nw->nw_Label);
		}
		FPuts(handle,  "	}\n" \
									 "	return(running);\n" \
									 "}\n\n");

/* Render Window */
		if(!IsListEmpty(List(&nw->nw_List_Frame)))
		{
			FPrintf(handle,"void Render_%s()\n{\n",nw->nw_Label);
			FPrintf(handle,"	UWORD OffX=%sWnd->BorderLeft, OffY=%sWnd->BorderTop;\n\n",nw->nw_Label,nw->nw_Label);
			FPrintf(handle,"	if(FindTagItem(WA_FontAdaptive,(struct TagItem *)%sWTags)) CalcAdaptivity(GetTagData(WA_Width,GetTagData(WA_InnerWidth,GetTagData(WA_RelWidth,0,(struct TagItem *)%sWTags),(struct TagItem *)%sWTags),(struct TagItem *)%sWTags),GetTagData(WA_Height,GetTagData(WA_InnerHeight,GetTagData(WA_RelHeight,0,(struct TagItem *)%sWTags),(struct TagItem *)%sWTags),(struct TagItem *)%sWTags)); else { FontX=%ld; FontY=%ld; }\n\n",nw->nw_Label,nw->nw_Label,nw->nw_Label,nw->nw_Label,nw->nw_Label,nw->nw_Label,nw->nw_Label,Scr->RastPort.Font->tf_XSize,Scr->RastPort.Font->tf_YSize);
			for(fo=(piObject *)GetHead(&nw->nw_List_Frame);GetSucc(fo);fo=(piObject *)GetSucc(fo))
			{
				nf=(struct Node_Frame *)OBJECT_DATA(fo);
				switch(nf->nf_FrameType)
				{
					case FTYPE_BUTTON:
						FPrintf(handle,"	DrawBevelBox( %sWnd->RPort, AdaptX(%ld)+OffX, AdaptY(%ld)+OffY, AdaptX(%ld), AdaptY(%ld), GT_VisualInfo, VisualInfo, GTBB_FrameType, BBFT_BUTTON",nw->nw_Label,nf->nf_X,nf->nf_Y,nf->nf_Width+1,nf->nf_Height+1);
						break;
					case FTYPE_ICONDROPBOX:
						FPrintf(handle,"	DrawBevelBox( %sWnd->RPort, AdaptX(%ld)+OffX, AdaptY(%ld)+OffY, AdaptX(%ld), AdaptY(%ld), GT_VisualInfo, VisualInfo, GTBB_FrameType, BBFT_ICONDROPBOX",nw->nw_Label,nf->nf_X,nf->nf_Y,nf->nf_Width+1,nf->nf_Height+1);
						break;
					case FTYPE_RIDGE:
						FPrintf(handle,"	DrawBevelBox( %sWnd->RPort, AdaptX(%ld)+OffX, AdaptY(%ld)+OffY, AdaptX(%ld), AdaptY(%ld), GT_VisualInfo, VisualInfo, GTBB_FrameType, BBFT_RIDGE",nw->nw_Label,nf->nf_X,nf->nf_Y,nf->nf_Width+1,nf->nf_Height+1);
						break;
					case FTYPE_HORIZONTALLINE:
						FPrintf(handle,"	DrawBevelBox( %sWnd->RPort, AdaptX(%ld)+OffX, AdaptY(%ld)+OffY, AdaptX(%ld), AdaptY(%ld), GT_VisualInfo, VisualInfo, GTBB_FrameType, BBFT_BUTTON",nw->nw_Label,nf->nf_X,nf->nf_Y,nf->nf_Width+1,2);
						break;
					case FTYPE_VERTICALLINE:
						FPrintf(handle,"	DrawBevelBox( %sWnd->RPort, AdaptX(%ld)+OffX, AdaptY(%ld)+OffY, AdaptX(%ld), AdaptY(%ld), GT_VisualInfo, VisualInfo, GTBB_FrameType, BBFT_BUTTON",nw->nw_Label,nf->nf_X,nf->nf_Y,2h,nf->nf_Height+1);
						break;
				}
				GetFlag(nf->nf_Flags,MYFR_Recessed) FPuts(handle,", GTBB_Recessed, TRUE");
				FPuts(handle,", TAG_DONE);\n");
			}
			FPuts(handle,"}\n\n");
		}
	}

/* OpenWindow */
	if(!IsListEmpty(List(&nw->nw_List_Gadget)))
	{
		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
		{
			ng=(struct Node_Gadget *)OBJECT_DATA(go);
			if(ng->ng_RelX || ng->ng_RelY || ng->ng_RelWidth || ng->ng_RelHeight)
			{
				rel=TRUE;
				break;
			}
		}
		
	}
	FPrintf(handle,"UWORD OpenWindow_%s()\n{\n",nw->nw_Label);
	if(!IsListEmpty(List(&nw->nw_List_Gadget)))
	{
		FPuts(handle,"	Object *g;\n" \
								 "	UWORD lc,tc;\n"\
								 "	UWORD OffX,OffY;\n");
		if(rel) FPuts(handle,"	UWORD OffR,OffB;\n");
	}
	if(CheckPlugInWindow(wo,PAGE_ID)) FPuts(handle,"	UWORD retval;\n");

	if(!IsListEmpty(List(&nw->nw_List_Gadget)))
	{
		FPrintf(handle,"\n	if(!(g=(Object *)CreateContext(&%sGList))) return GTBERR_CREATECONTEXT;\n\n",nw->nw_Label);
		if(nw->nw_PageGadget)
		{
			for(wo2=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo2);wo2=(piObject *)GetSucc(wo2))
			{
				nw2=(struct Node_Window *)OBJECT_DATA(wo2);
				for(cnt=0,go=(piObject *)GetHead(List(&nw2->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go),cnt++)
				{
					ng=(struct Node_Gadget *)OBJECT_DATA(go);
					if(ng->ng_PlugInID==PAGE_ID)
						for(n=GetHead(List(GetTagData(TAG_USER+1,0,ng->ng_OwnTags)));GetSucc(n);n=GetSucc(n))
							if(!strcmp(nw->nw_Label,n->ln_Name))
							{
								FPrintf(handle,"	CalcLeftTop(&OffX,&OffY,(struct TagItem *)%sWTags);\n",nw2->nw_Label);
								if(rel) FPrintf(handle,"	CalcRightBottom(&OffR,&OffB,(struct TagItem *)%sWTags);\n",nw2->nw_Label);
								FPrintf(handle,"	if(FindTagItem(WA_FontAdaptive,(struct TagItem *)%sWTags)) CalcAdaptivity(GetTagData(WA_Width,GetTagData(WA_InnerWidth,GetTagData(WA_RelWidth,0,(struct TagItem *)%sWTags),(struct TagItem *)%sWTags),(struct TagItem *)%sWTags),GetTagData(WA_Height,GetTagData(WA_InnerHeight,GetTagData(WA_RelHeight,0,(struct TagItem *)%sWTags),(struct TagItem *)%sWTags),(struct TagItem *)%sWTags)); else { FontX=%ld; FontY=%ld; }\n\n",nw2->nw_Label,nw2->nw_Label,nw2->nw_Label,nw2->nw_Label,nw2->nw_Label,nw2->nw_Label,nw2->nw_Label,Scr->RastPort.Font->tf_XSize,Scr->RastPort.Font->tf_YSize);
							}
				}
			}
		}
		else
		{
			FPrintf(handle,"	CalcLeftTop(&OffX,&OffY,(struct TagItem *)%sWTags);\n",nw->nw_Label);
			if(rel) FPrintf(handle,"	CalcRightBottom(&OffR,&OffB,(struct TagItem *)%sWTags);\n",nw->nw_Label);
			FPrintf(handle,"	if(FindTagItem(WA_FontAdaptive,(struct TagItem *)%sWTags)) CalcAdaptivity(GetTagData(WA_Width,GetTagData(WA_InnerWidth,GetTagData(WA_RelWidth,0,(struct TagItem *)%sWTags),(struct TagItem *)%sWTags),(struct TagItem *)%sWTags),GetTagData(WA_Height,GetTagData(WA_InnerHeight,GetTagData(WA_RelHeight,0,(struct TagItem *)%sWTags),(struct TagItem *)%sWTags),(struct TagItem *)%sWTags)); else { FontX=%ld; FontY=%ld; }\n\n",nw->nw_Label,nw->nw_Label,nw->nw_Label,nw->nw_Label,nw->nw_Label,nw->nw_Label,nw->nw_Label,Scr->RastPort.Font->tf_XSize,Scr->RastPort.Font->tf_YSize);
		}

		/* Create gadgets on page.gadget */
		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
		{
			ng=(struct Node_Gadget *)OBJECT_DATA(go);
			if(ng->ng_PlugInID==PAGE_ID)
				for(n=GetHead(List(GetTagData(TAG_USER+1,0,ng->ng_OwnTags)));GetSucc(n);n=GetSucc(n))
					FPrintf(handle,"	if(retval=OpenWindow_%s()) return(retval);\n",n->ln_Name);
		}

		/* Update tags for page.gadget */
		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
		{
			ng=(struct Node_Gadget *)OBJECT_DATA(go);
			if(ng->ng_PlugInID==PAGE_ID)
				for(cnt=0,n=GetHead(List(GetTagData(TAG_USER+1,0,ng->ng_OwnTags)));GetSucc(n);n=GetSucc(n),cnt++)
				{
					f=FALSE;
					for(wo2=(piObject *)GetHead(List(&GUI->gui_List_Window));GetSucc(wo2);wo2=(piObject *)GetSucc(wo2))
					{
						nw2=(struct Node_Window *)OBJECT_DATA(wo2);
						if(!strcmp(nw2->nw_Label,n->ln_Name))
							if(!IsListEmpty(List(&nw2->nw_List_Gadget)))
								f=TRUE;
					}
					if(f) FPrintf(handle,"	PAGES_%s[%ld].FirstGadget=%sGList;\n",ng->ng_Label,cnt,n->ln_Name);
					else FPrintf(handle,"	PAGES_%s[%ld].FirstGadget=NULL;\n",ng->ng_Label,cnt);
				}
		}

		/* Create Gadgets on this Window */
		FPrintf(handle,"\n	for(lc=0,tc=0;lc<CNT_%s;lc++)\n" \
									 "	{\n",nw->nw_Label);
		FPrintf(handle,"		if(!GadgetLocaleDone[WND_%s]) FixGadgetTags((struct TagItem *)&%sGTags[tc]);\n",nw->nw_Label,nw->nw_Label);
		if(!rel) FPrintf(handle,"		%sGadgets[lc]=g=CreateGadgetExA(OffX,OffY,0,0,g,(struct TagItem *) &%sGTags[tc]);\n",nw->nw_Label,nw->nw_Label,nw->nw_Label);
		else FPrintf(handle,"		%sGadgets[lc]=g=CreateGadgetExA(OffX,OffY,OffR,OffB,g,(struct TagItem *) &%sGTags[tc]);\n",nw->nw_Label,nw->nw_Label,nw->nw_Label);
		FPuts(handle,  "		tc+=2;\n");
		FPrintf(handle,"		while(%sGTags[tc]!=TAG_DONE) tc+=2;\n",nw->nw_Label);
		FPuts(handle,	 "		tc++;\n" \
									 "		if(!(g)) return GTBERR_CREATEGADGET;\n" \
									 "	}\n\n");
		FPrintf(handle,"	GadgetLocaleDone[WND_%s]=TRUE;\n\n",nw->nw_Label);
	}
	/* Open this Window */
	if(nw->nw_PageGadget==0)
	{
		FPrintf(handle,"	if(!WindowLocaleDone[WND_%s]) FixWindowTags((struct TagItem *)%sWTags);\n",nw->nw_Label,nw->nw_Label);
		FPrintf(handle,"	WindowLocaleDone[WND_%s]=TRUE;\n\n",nw->nw_Label);
		FPrintf(handle,"	if(!(%sWnd=NewObject(AppObj->ga_WndClass,NULL,\n",nw->nw_Label);
		if(!IsListEmpty(List(&nw->nw_List_Gadget))) FPrintf(handle,"				WIND_Gadgets,%sGList,\n",nw->nw_Label);
		FPuts(handle,	 "				WIND_ScreenObject, MainScr,\n");
		FPrintf(handle,"				TAG_MORE, %sWTags ))) return GTBERR_OPENWINDOW;\n\n",nw->nw_Label);

		if(nw->nw_MenuTitle[0]) FPrintf(handle,"	SetMenuStrip(%sWnd,%sMenus);\n",nw->nw_Label,nw->nw_MenuTitle);
		FPrintf(handle,"	GT_RefreshWindow(%sWnd,NULL);\n",nw->nw_Label);
		if(!IsListEmpty(List(&nw->nw_List_Frame)))
			FPrintf(handle,"	Render_%s();\n",nw->nw_Label);
		if(CheckDataTypeWindow(wo))
		{
			FPrintf(handle,"	for(lc=0,tc=0;lc<CNT_%s;lc++)\n",nw->nw_Label);
			FPuts(handle,	 "	{\n");
			FPrintf(handle,"		if(%sGTags[tc]==DATATYPE_KIND) RefreshDTObject((Object *)%sGadgets[lc],%sWnd,NULL,TAG_DONE);\n",nw->nw_Label,nw->nw_Label,nw->nw_Label);
			FPuts(handle,  "		tc+=2;\n");
			FPrintf(handle,"		while(%sGTags[tc]!=TAG_DONE) tc+=2;\n",nw->nw_Label);
			FPuts(handle,  "		tc++;\n" \
										 "	}\n");
		}
	}
	FPuts(handle,"\treturn GTBERR_OK;\n}\n\n");

	FPrintf(handle,"void CloseWindow_%s()\n{\n",nw->nw_Label);
	if(!IsListEmpty(List(&nw->nw_List_Gadget)))
		if(CheckBOOPSIWindow(wo) || CheckDataTypeWindow(wo) || CheckToolBarWindow(wo))
			FPuts(handle,"	UWORD lc,tc;\n\n");
	if(nw->nw_PageGadget==0)
	{
		if(CheckPlugInWindow(wo,PAGE_ID))
		{
			FPrintf(handle,"		for(lc=0,tc=0;lc<CNT_%s;lc++)\n",nw->nw_Label);
			FPuts(handle,	 "		{\n");
			FPrintf(handle,"			if(%sGTags[tc]==PAGE_KIND) RemoveGList(%sWnd,%sGadgets[lc],-1);\n",nw->nw_Label,nw->nw_Label,nw->nw_Label);
			FPuts(handle,  "			tc+=2;\n");
			FPrintf(handle,"			while(%sGTags[tc]!=TAG_DONE) tc+=2;\n",nw->nw_Label);
			FPuts(handle,  "			tc++;\n" \
										 "		}\n");
		}
		if(nw->nw_MenuTitle[0])
			FPrintf(handle,"	if(%sWnd) ClearMenuStrip(%sWnd);\n",nw->nw_Label,nw->nw_Label);
		FPrintf(handle,"	if(%sWnd) DisposeObject(%sWnd); %sWnd=NULL;\n",nw->nw_Label,nw->nw_Label,nw->nw_Label);
	}
	if(!IsListEmpty(List(&nw->nw_List_Gadget)))
	{
		FPrintf(handle,"	if(%sGList)\n"\
									 "	{\n",nw->nw_Label);
		if(CheckBOOPSIWindow(wo) || CheckDataTypeWindow(wo) || CheckToolBarWindow(wo))
		{
			FPrintf(handle,"		for(lc=0,tc=0;lc<CNT_%s;lc++)\n",nw->nw_Label);
			FPuts(handle,	 "		{\n");
			FPrintf(handle,"			if(%sGTags[tc]==TOOLIMAGE_KIND) DeleteGadgetExA(%sGadgets[lc],(struct TagItem *)&%sGTags[tc]);\n",nw->nw_Label,nw->nw_Label,nw->nw_Label);
			FPuts(handle,  "			tc+=2;\n");
			FPrintf(handle,"			while(%sGTags[tc]!=TAG_DONE) tc+=2;\n",nw->nw_Label);
			FPuts(handle,  "			tc++;\n" \
										 "		}\n");
		}
		FPrintf(handle,"		gtbFreeGadgets(%sGList);\n",nw->nw_Label);
		FPrintf(handle,"		%sGList=NULL;\n",nw->nw_Label);
		if(CheckBOOPSIWindow(wo) || CheckDataTypeWindow(wo) || CheckToolBarWindow(wo))
		{
			FPrintf(handle,"		for(lc=0,tc=0;lc<CNT_%s;lc++)\n",nw->nw_Label);
			FPuts(handle,	 "		{\n");
			FPrintf(handle,"			if(%sGTags[tc]!=TOOLIMAGE_KIND) DeleteGadgetExA(%sGadgets[lc],(struct TagItem *)&%sGTags[tc]);\n",nw->nw_Label,nw->nw_Label,nw->nw_Label);
			FPuts(handle,  "			tc+=2;\n");
			FPrintf(handle,"			while(%sGTags[tc]!=TAG_DONE) tc+=2;\n",nw->nw_Label);
			FPuts(handle,  "			tc++;\n" \
										 "		}\n");
		}
		FPuts(handle,		 "	}\n");
	}
	for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(go);
		if(ng->ng_PlugInID==PAGE_ID)
			for(n=GetHead(List(GetTagData(TAG_USER+1,0,ng->ng_OwnTags)));GetSucc(n);n=GetSucc(n))
				FPrintf(handle,"	CloseWindow_%s();\n",n->ln_Name);
	}
	FPuts(handle,"}\n\n");

} /* FUNC_GenerateWindow */

void FUNC_GenerateErrors(BPTR handle)
{
	FPuts(handle,"#define GTBERR_OK 0\n");
	if(OPT_ARexx) FPuts(handle,"#define GTBERR_AREXX 11\n");
	if(OPT_UseLocale) FPuts(handle,"#define GTBERR_LOCALE 12\n");
	if(CheckAppWindow()) FPuts(handle,"#define GTBERR_CREATEAPPPORT 13\n");
	if(CheckProjectWindow()) FPuts(handle,"#define GTBERR_CREATEPROJECTPORT 14\n");
	FPuts(handle,"#define GTBERR_CREATEPOINTER 15\n");
	FPuts(handle,"#define GTBERR_CREATEIMAGE 16\n");
	if(CheckPlugIn(COLORWHEEL_ID)) FPuts(handle,"#define GTBERR_OPENLIB_COLORWHEEL 100\n");
	if(CheckPlugIn(GRADIENTSLIDER_ID)) FPuts(handle,"#define GTBERR_OPENLIB_GRADIENTSLIDER 101\n");
	if(CheckPlugIn(TAPEDECK_ID)) FPuts(handle,"#define GTBERR_OPENLIB_TAPEDECK 102\n");
	if(CheckPlugIn(PAGE_ID)) FPuts(handle,"#define GTBERR_OPENLIB_PAGE 103\n");
	if(CheckPlugIn(POPUPMENU_ID)) FPuts(handle,"#define GTBERR_OPENLIB_POPUPMENU 104\n");
	if(CheckPlugIn(PROGRESS_ID)) FPuts(handle,"#define GTBERR_OPENLIB_PROGRESS 105\n");
	if(CheckPlugIn(TEXTFIELD_ID)) FPuts(handle,"#define GTBERR_OPENLIB_TEXTFIELD 106\n");
	if(CheckPlugIn(TREEVIEW_ID)) FPuts(handle,"#define GTBERR_OPENLIB_TREEVIEW 107\n");
	if(CheckToolBarWindows()) FPuts(handle,"#define GTBERR_OPENLIB_SMARTBITMAP 150\n");
	FPuts(handle,"#define GTBERR_OPENSCREEN 200\n" \
							 "#define GTBERR_VISUALINFO 201\n" \
							 "#define GTBERR_SCRDRAWINFO 202\n" \
							 "#define GTBERR_CREATECONTEXT 300\n" \
							 "#define GTBERR_CREATEGADGET 301\n" \
							 "#define GTBERR_CREATEMENUS 302\n" \
							 "#define GTBERR_OPENWINDOW 303\n");
	if(CheckAppWindow()) FPuts(handle,"#define GTBERR_ADDAPPWINDOW 304\n");
	FPuts(handle,"\n");
} /* FUNC_GenerateErrors */

void FUNC_GenerateTemplate(BPTR handle,piObject *wo)
{
	struct Node_Window *nw;

	nw=(struct Node_Window *)OBJECT_DATA(wo);

	GetFlag(nw->nw_IDCMP,IDCMP_MENUVERIFY)
		FPrintf(handle,"int FN_MenuVerify_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_SIZEVERIFY)
		FPrintf(handle,"int FN_SizeVerify_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_REQVERIFY)
		FPrintf(handle,"int FN_ReqVerify_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_REQSET)
		FPrintf(handle,"int FN_ReqSet_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_REQCLEAR)
		FPrintf(handle,"int FN_ReqClear_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_NEWSIZE)
		FPrintf(handle,"int FN_NewSize_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_NEWPREFS)
		FPrintf(handle,"int FN_NewPrefs_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_MENUHELP)
		FPrintf(handle,"int FN_MenuHelp_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_GADGETHELP)
		FPrintf(handle,"int FN_GadgetHelp_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_MOUSEMOVE)
		FPrintf(handle,"int FN_MouseMove_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_MOUSEBUTTONS)
		FPrintf(handle,"int FN_MouseButtons_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_RAWKEY)
		FPrintf(handle,"int FN_RawKey_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_VANILLAKEY)
		FPrintf(handle,"int FN_VanillaKey_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_INTUITICKS)
		FPrintf(handle,"int FN_IntuiTicks_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_IDCMPUPDATE)
		FPrintf(handle,"int FN_IDCMPUpdate_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_CHANGEWINDOW)
		FPrintf(handle,"int FN_ChangeWindow_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_CLOSEWINDOW)
		FPrintf(handle,"int FN_CloseWindow_%s(void)\n{\n	return 0;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_ACTIVEWINDOW)
		FPrintf(handle,"int FN_ActiveWindow_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_INACTIVEWINDOW)
		FPrintf(handle,"int FN_InActiveWindow_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_DISKREMOVED)
		FPrintf(handle,"int FN_DiskRemoved_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
	GetFlag(nw->nw_IDCMP,IDCMP_DISKINSERTED)
		FPrintf(handle,"int FN_DiskInserted_%s(void)\n{\n	return 1;\n}\n\n",nw->nw_Label);
} /* FUNC_GenerateTemplate */
