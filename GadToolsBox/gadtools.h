#ifndef gadtools_H
#define gadtools_H
/*
** gadtools.h Header File
*/

#include<images/smartbitmap.h>

#include<exec/types.h>
#include<intuition/classusr.h>

#define GetString( g )  ((( struct StringInfo * )g->SpecialInfo )->Buffer  )
#define GetInteger( g ) ((( struct StringInfo * )g->SpecialInfo )->LongInt )
#define TOOL_HORIZ 0
#define TOOL_VERT 1

#define DRAG_ID MAKE_ID('D','R','A','G')

#define COLORWHEEL_KIND 10000
#define DATATYPE_KIND 10001
#define GRADIENTSLIDER_KIND 10002
#define PAGE_KIND 10003
#define POPUPMENU_KIND 10004
#define PROGRESS_KIND 10005
#define TAPEDECK_KIND 10006
#define TEXTFIELD_KIND 10007
#define TOOLIMAGE_KIND 10008

#define TREEVIEW_KIND 10009
#define VER_PROGRAMNAME "GadToolsBox"
#define VER_AUTHOR "Tomasz Muszynski"
#define VER_VERSION 3
#define VER_REVISION 0
#define VER_SVER "\0$VER: "VER_PROGRAMNAME" 3.0 "__AMIGADATE__""

#define WA_CenterX (WA_Dummy + 0x100)
#define WA_CenterY (WA_Dummy + 0x101)
#define WA_RelRight (WA_Dummy + 0x102)
#define WA_RelBottom (WA_Dummy + 0x103)
#define WA_RelWidth (WA_Dummy + 0x104)
#define WA_RelHeight (WA_Dummy + 0x105)
#define WA_ToolDragBar (WA_Dummy + 0x106)
#define WA_FontAdaptive (WA_Dummy + 0x107)
#define GTBERR_OK 0
#define GTBERR_LOCALE 12
#define GTBERR_CREATEPROJECTPORT 14
#define GTBERR_CREATEPOINTER 15
#define GTBERR_CREATEIMAGE 16
#define GTBERR_OPENLIB_COLORWHEEL 100
#define GTBERR_OPENLIB_GRADIENTSLIDER 101
#define GTBERR_OPENLIB_PAGE 103
#define GTBERR_OPENLIB_PROGRESS 105
#define GTBERR_OPENLIB_TEXTFIELD 106
#define GTBERR_OPENLIB_TREEVIEW 107
#define GTBERR_OPENLIB_SMARTBITMAP 150
#define GTBERR_OPENSCREEN 200
#define GTBERR_VISUALINFO 201
#define GTBERR_SCRDRAWINFO 202
#define GTBERR_CREATECONTEXT 300
#define GTBERR_CREATEGADGET 301
#define GTBERR_CREATEMENUS 302
#define GTBERR_OPENWINDOW 303

extern TEXT ScrTitle[];
extern TEXT ScrPubName[];
extern UWORD ScrWidth,ScrHeight,ScrDepth;
extern ULONG ScrDisplayID;
extern struct Screen *Scr;
extern struct DrawInfo *DrawInfo;
extern APTR VisualInfo;
extern struct MsgPort *ProjectMsgPort;
extern struct TextAttr *Font, Attr;

extern struct Gadget *PGBO_GeneralGList;
extern struct Gadget *PGBO_GeneralGadgets[];
#define WND_PGBO_General 0
extern struct Window *AboutWnd;
extern struct Gadget *AboutGList;
extern ULONG AboutWTags[];
extern struct IntuiMessage AboutMsg;
extern struct Gadget *AboutGadgets[];
#define WND_About 1
extern struct Gadget *PG_BUTT_FlagsGList;
extern struct Gadget *PG_BUTT_FlagsGadgets[];
#define WND_PG_BUTT_Flags 2
extern struct Gadget *PG_CBOX_FlagsGList;
extern struct Gadget *PG_CBOX_FlagsGadgets[];
#define WND_PG_CBOX_Flags 3
extern struct Gadget *PG_CWHL_FlagsGList;
extern struct Gadget *PG_CWHL_FlagsGadgets[];
#define WND_PG_CWHL_Flags 4
extern struct Gadget *PG_CYCE_FlagsGList;
extern struct Gadget *PG_CYCE_FlagsGadgets[];
#define WND_PG_CYCE_Flags 5
extern struct Gadget *PG_ListsGList;
extern struct Gadget *PG_ListsGadgets[];
#define WND_PG_Lists 6
extern struct Gadget *PG_DTYP_FlagsGList;
extern struct Gadget *PG_DTYP_FlagsGadgets[];
#define WND_PG_DTYP_Flags 7
extern struct Gadget *PG_GRSL_FlagsGList;
extern struct Gadget *PG_GRSL_FlagsGadgets[];
#define WND_PG_GRSL_Flags 8
extern struct Gadget *PG_INTR_FlagsGList;
extern struct Gadget *PG_INTR_FlagsGadgets[];
#define WND_PG_INTR_Flags 9
extern struct Gadget *PG_INTR_VarGList;
extern struct Gadget *PG_INTR_VarGadgets[];
#define WND_PG_INTR_Var 10
extern struct Gadget *PG_LVIE_FlagsGList;
extern struct Gadget *PG_LVIE_FlagsGadgets[];
#define WND_PG_LVIE_Flags 11
extern struct Gadget *PG_MX_FlagsGList;
extern struct Gadget *PG_MX_FlagsGadgets[];
#define WND_PG_MX_Flags 12
extern struct Gadget *PG_NUMB_FlagsGList;
extern struct Gadget *PG_NUMB_FlagsGadgets[];
#define WND_PG_NUMB_Flags 13
extern struct Gadget *PGGT_GeneralGList;
extern struct Gadget *PGGT_GeneralGadgets[];
#define WND_PGGT_General 14
extern struct Gadget *PG_PAGE_FlagsGList;
extern struct Gadget *PG_PAGE_FlagsGadgets[];
#define WND_PG_PAGE_Flags 15
extern struct Gadget *PG_PAGE_ListsGList;
extern struct Gadget *PG_PAGE_ListsGadgets[];
#define WND_PG_PAGE_Lists 16
extern struct Gadget *PG_PALT_FlagsGList;
extern struct Gadget *PG_PALT_FlagsGadgets[];
#define WND_PG_PALT_Flags 17
extern struct Gadget *PG_PUPM_FlagsGList;
extern struct Gadget *PG_PUPM_FlagsGadgets[];
#define WND_PG_PUPM_Flags 18
extern struct Gadget *PG_PRGR_FlagsGList;
extern struct Gadget *PG_PRGR_FlagsGadgets[];
#define WND_PG_PRGR_Flags 19
extern struct Gadget *PG_SCRL_FlagsGList;
extern struct Gadget *PG_SCRL_FlagsGadgets[];
#define WND_PG_SCRL_Flags 20
extern struct Gadget *PG_SCRL_VarGList;
extern struct Gadget *PG_SCRL_VarGadgets[];
#define WND_PG_SCRL_Var 21
extern struct Gadget *PG_SLIR_FlagsGList;
extern struct Gadget *PG_SLIR_FlagsGadgets[];
#define WND_PG_SLIR_Flags 22
extern struct Gadget *PG_SLIR_VarGList;
extern struct Gadget *PG_SLIR_VarGadgets[];
#define WND_PG_SLIR_Var 23
extern struct Gadget *PG_STRG_FlagsGList;
extern struct Gadget *PG_STRG_FlagsGadgets[];
#define WND_PG_STRG_Flags 24
extern struct Gadget *PG_STRG_VarGList;
extern struct Gadget *PG_STRG_VarGadgets[];
#define WND_PG_STRG_Var 25
extern struct Gadget *PG_TDEK_FlagsGList;
extern struct Gadget *PG_TDEK_FlagsGadgets[];
#define WND_PG_TDEK_Flags 26
extern struct Gadget *PG_TEXT_FlagsGList;
extern struct Gadget *PG_TEXT_FlagsGadgets[];
#define WND_PG_TEXT_Flags 27
extern struct Gadget *PG_TFLD_FlagsGList;
extern struct Gadget *PG_TFLD_FlagsGadgets[];
#define WND_PG_TFLD_Flags 28
extern struct Gadget *PG_TFLD_Flags2GList;
extern struct Gadget *PG_TFLD_Flags2Gadgets[];
#define WND_PG_TFLD_Flags2 29
extern struct Gadget *PG_TFLD_VarGList;
extern struct Gadget *PG_TFLD_VarGadgets[];
#define WND_PG_TFLD_Var 30
extern struct Window *PropertiesWnd;
extern struct Gadget *PropertiesGList;
extern ULONG PropertiesWTags[];
extern struct IntuiMessage PropertiesMsg;
extern struct Gadget *PropertiesGadgets[];
#define WND_Properties 31
extern struct Window *EditWnd;
extern APTR EditWApp;
extern ULONG EditWTags[];
extern struct IntuiMessage EditMsg;
#define WND_Edit 32
extern struct Window *PreferencesWnd;
extern struct Gadget *PreferencesGList;
extern ULONG PreferencesWTags[];
extern struct IntuiMessage PreferencesMsg;
extern struct Gadget *PreferencesGadgets[];
#define WND_Preferences 33
extern struct Window *GridWnd;
extern struct Gadget *GridGList;
extern ULONG GridWTags[];
extern struct IntuiMessage GridMsg;
extern struct Gadget *GridGadgets[];
#define WND_Grid 34
extern struct Gadget *PGWN_GeneralGList;
extern struct Gadget *PGWN_GeneralGadgets[];
#define WND_PGWN_General 35
extern struct Gadget *PGWN_Flags1GList;
extern struct Gadget *PGWN_Flags1Gadgets[];
#define WND_PGWN_Flags1 36
extern struct Gadget *PGWN_StyleGList;
extern struct Gadget *PGWN_StyleGadgets[];
#define WND_PGWN_Style 37
extern struct Gadget *PGWN_Flags2GList;
extern struct Gadget *PGWN_Flags2Gadgets[];
#define WND_PGWN_Flags2 38
extern struct Gadget *PGWN_Flags3GList;
extern struct Gadget *PGWN_Flags3Gadgets[];
#define WND_PGWN_Flags3 39
extern struct Window *WN_IDCMPFlagsWnd;
extern struct Gadget *WN_IDCMPFlagsGList;
extern ULONG WN_IDCMPFlagsWTags[];
extern struct IntuiMessage WN_IDCMPFlagsMsg;
extern struct Gadget *WN_IDCMPFlagsGadgets[];
#define WND_WN_IDCMPFlags 40
extern struct Window *TB_MainWnd;
extern struct Gadget *TB_MainGList;
extern ULONG TB_MainWTags[];
extern struct IntuiMessage TB_MainMsg;
extern struct Gadget *TB_MainGadgets[];
#define WND_TB_Main 41
extern struct Window *TB_WindowWnd;
extern struct Gadget *TB_WindowGList;
extern ULONG TB_WindowWTags[];
extern struct IntuiMessage TB_WindowMsg;
extern struct Gadget *TB_WindowGadgets[];
#define WND_TB_Window 42
extern struct Gadget *PGMN_GeneralGList;
extern struct Gadget *PGMN_GeneralGadgets[];
#define WND_PGMN_General 43
extern struct Window *WorkingWnd;
extern struct Gadget *WorkingGList;
extern ULONG WorkingWTags[];
extern struct IntuiMessage WorkingMsg;
extern struct Gadget *WorkingGadgets[];
#define WND_Working 44
extern struct Gadget *PGFR_GeneralGList;
extern struct Gadget *PGFR_GeneralGadgets[];
#define WND_PGFR_General 45
#define WND_PGEMPTY 46
extern struct Window *EditMenuWnd;
extern APTR EditMenuWApp;
extern ULONG EditMenuWTags[];
extern struct IntuiMessage EditMenuMsg;
#define WND_EditMenu 47
extern struct Window *EditImageWnd;
extern APTR EditImageWApp;
extern ULONG EditImageWTags[];
extern struct IntuiMessage EditImageMsg;
#define WND_EditImage 48
extern struct Window *ListWnd;
extern struct Gadget *ListGList;
extern ULONG ListWTags[];
extern struct IntuiMessage ListMsg;
extern struct Gadget *ListGadgets[];
#define WND_List 49
extern struct Gadget *PGMN_ItemGList;
extern struct Gadget *PGMN_ItemGadgets[];
#define WND_PGMN_Item 50
extern struct Gadget *PGPI_GeneralGList;
extern struct Gadget *PGPI_GeneralGadgets[];
#define WND_PGPI_General 51
extern struct Window *TB_PointerWnd;
extern struct Gadget *TB_PointerGList;
extern ULONG TB_PointerWTags[];
extern struct IntuiMessage TB_PointerMsg;
extern struct Gadget *TB_PointerGadgets[];
#define WND_TB_Pointer 52
extern struct Window *ScreenWnd;
extern struct Gadget *ScreenGList;
extern ULONG ScreenWTags[];
extern struct IntuiMessage ScreenMsg;
extern struct Gadget *ScreenGadgets[];
#define WND_Screen 53
extern struct Gadget *PGSCR_GeneralGList;
extern struct Gadget *PGSCR_GeneralGadgets[];
#define WND_PGSCR_General 54
extern struct Gadget *PGSCR_FlagsGList;
extern struct Gadget *PGSCR_FlagsGadgets[];
#define WND_PGSCR_Flags 55
extern struct Window *EditStringWnd;
extern struct Gadget *EditStringGList;
extern ULONG EditStringWTags[];
extern struct IntuiMessage EditStringMsg;
extern struct Gadget *EditStringGadgets[];
#define WND_EditString 56
extern struct Gadget *PGSTR_GeneralGList;
extern struct Gadget *PGSTR_GeneralGadgets[];
#define WND_PGSTR_General 57
extern struct Window *EditVersionWnd;
extern struct Gadget *EditVersionGList;
extern ULONG EditVersionWTags[];
extern struct IntuiMessage EditVersionMsg;
extern struct Gadget *EditVersionGadgets[];
#define WND_EditVersion 58
extern struct Gadget *PG_TRLV_FlagsGList;
extern struct Gadget *PG_TRLV_FlagsGadgets[];
#define WND_PG_TRLV_Flags 59
extern struct Window *ObjectWnd;
extern struct Gadget *ObjectGList;
extern ULONG ObjectWTags[];
extern struct IntuiMessage ObjectMsg;
extern struct Gadget *ObjectGadgets[];
#define WND_Object 60
extern struct Window *EditPaletteWnd;
extern struct Gadget *EditPaletteGList;
extern ULONG EditPaletteWTags[];
extern struct IntuiMessage EditPaletteMsg;
extern struct Gadget *EditPaletteGadgets[];
#define WND_EditPalette 61
extern struct Gadget *PGWN_TabOrderGList;
extern struct Gadget *PGWN_TabOrderGadgets[];
#define WND_PGWN_TabOrder 62
extern struct Window *StatusWnd;
extern struct Gadget *StatusGList;
extern ULONG StatusWTags[];
extern struct IntuiMessage StatusMsg;
extern struct Gadget *StatusGadgets[];
#define WND_Status 63

#define CNTWND_WINDOWS 65

extern ULONG PGBO_GeneralGTags[];
extern ULONG AboutGTags[];
extern ULONG PG_BUTT_FlagsGTags[];
extern ULONG PG_CBOX_FlagsGTags[];
extern ULONG PG_CWHL_FlagsGTags[];
extern ULONG PG_CYCE_FlagsGTags[];
extern ULONG PG_ListsGTags[];
extern ULONG PG_DTYP_FlagsGTags[];
extern ULONG PG_GRSL_FlagsGTags[];
extern ULONG PG_INTR_FlagsGTags[];
extern ULONG PG_INTR_VarGTags[];
extern ULONG PG_LVIE_FlagsGTags[];
extern ULONG PG_MX_FlagsGTags[];
extern ULONG PG_NUMB_FlagsGTags[];
extern ULONG PGGT_GeneralGTags[];
extern ULONG PG_PAGE_FlagsGTags[];
extern ULONG PG_PAGE_ListsGTags[];
extern ULONG PG_PALT_FlagsGTags[];
extern ULONG PG_PUPM_FlagsGTags[];
extern ULONG PG_PRGR_FlagsGTags[];
extern ULONG PG_SCRL_FlagsGTags[];
extern ULONG PG_SCRL_VarGTags[];
extern ULONG PG_SLIR_FlagsGTags[];
extern ULONG PG_SLIR_VarGTags[];
extern ULONG PG_STRG_FlagsGTags[];
extern ULONG PG_STRG_VarGTags[];
extern ULONG PG_TDEK_FlagsGTags[];
extern ULONG PG_TEXT_FlagsGTags[];
extern ULONG PG_TFLD_FlagsGTags[];
extern ULONG PG_TFLD_Flags2GTags[];
extern ULONG PG_TFLD_VarGTags[];
extern ULONG PropertiesGTags[];
extern ULONG PreferencesGTags[];
extern ULONG GridGTags[];
extern ULONG PGWN_GeneralGTags[];
extern ULONG PGWN_Flags1GTags[];
extern ULONG PGWN_StyleGTags[];
extern ULONG PGWN_Flags2GTags[];
extern ULONG PGWN_Flags3GTags[];
extern ULONG WN_IDCMPFlagsGTags[];
extern ULONG TB_MainGTags[];
extern ULONG TB_WindowGTags[];
extern ULONG PGMN_GeneralGTags[];
extern ULONG WorkingGTags[];
extern ULONG PGFR_GeneralGTags[];
extern ULONG ListGTags[];
extern ULONG PGMN_ItemGTags[];
extern ULONG PGPI_GeneralGTags[];
extern ULONG TB_PointerGTags[];
extern ULONG ScreenGTags[];
extern ULONG PGSCR_GeneralGTags[];
extern ULONG PGSCR_FlagsGTags[];
extern ULONG EditStringGTags[];
extern ULONG PGSTR_GeneralGTags[];
extern ULONG EditVersionGTags[];
extern ULONG PG_TRLV_FlagsGTags[];
extern ULONG ObjectGTags[];
extern ULONG EditPaletteGTags[];
extern ULONG PGWN_TabOrderGTags[];
extern ULONG StatusGTags[];
extern struct SmartBitMapIRGB ToolBarPalette[];extern Object *Size45UDPtr;
extern struct BitMap Size45UDBitMap;
extern Object *Size45DUPtr;
extern struct BitMap Size45DUBitMap;
extern Object *SizeVertPtr;
extern struct BitMap SizeVertBitMap;
extern Object *SizeHorizPtr;
extern struct BitMap SizeHorizBitMap;
extern Object *MovePtr;
extern struct BitMap MoveBitMap;
extern Object *SplitHorizPtr;
extern struct BitMap SplitHorizBitMap;
extern Object *SplitVertPtr;
extern struct BitMap SplitVertBitMap;
extern Object *DrawPtr;
extern struct BitMap DrawBitMap;
extern Object *TBNEWTbr;
extern struct BitMap TBNEWBitMap;
extern Object *TBOPENTbr;
extern struct BitMap TBOPENBitMap;
extern Object *TBSAVETbr;
extern struct BitMap TBSAVEBitMap;
extern Object *TBCUTTbr;
extern struct BitMap TBCUTBitMap;
extern Object *TBCOPYTbr;
extern struct BitMap TBCOPYBitMap;
extern Object *TBPASTETbr;
extern struct BitMap TBPASTEBitMap;
extern Object *TBUNDOTbr;
extern struct BitMap TBUNDOBitMap;
extern Object *TBHELPTbr;
extern struct BitMap TBHELPBitMap;
extern Object *TBNEWWINDOWTbr;
extern struct BitMap TBNEWWINDOWBitMap;
extern Object *TBNEWMENUTbr;
extern struct BitMap TBNEWMENUBitMap;
extern Object *TBNEWIMAGETbr;
extern struct BitMap TBNEWIMAGEBitMap;
extern Object *TBNEWSTRINGTbr;
extern struct BitMap TBNEWSTRINGBitMap;
extern Object *TBNEWSCREENTbr;
extern struct BitMap TBNEWSCREENBitMap;
extern Object *TBNEWVERSIONTbr;
extern struct BitMap TBNEWVERSIONBitMap;
extern Object *TBBUTTONTbr;
extern struct BitMap TBBUTTONBitMap;
extern Object *TBCYCLETbr;
extern struct BitMap TBCYCLEBitMap;
extern Object *TBCHECKMARKTbr;
extern struct BitMap TBCHECKMARKBitMap;
extern Object *TBCOLORWHEELTbr;
extern struct BitMap TBCOLORWHEELBitMap;
extern Object *TBDATATYPETbr;
extern struct BitMap TBDATATYPEBitMap;
extern Object *TBGRADIENTSLIDERTbr;
extern struct BitMap TBGRADIENTSLIDERBitMap;
extern Object *TBINTEGERTbr;
extern struct BitMap TBINTEGERBitMap;
extern Object *TBLISTVIEWTbr;
extern struct BitMap TBLISTVIEWBitMap;
extern Object *TBMXTbr;
extern struct BitMap TBMXBitMap;
extern Object *TBNUMBERTbr;
extern struct BitMap TBNUMBERBitMap;
extern Object *TBPAGETbr;
extern struct BitMap TBPAGEBitMap;
extern Object *TBPALETTETbr;
extern struct BitMap TBPALETTEBitMap;
extern Object *TBPOPUPMENUTbr;
extern struct BitMap TBPOPUPMENUBitMap;
extern Object *TBPROGRESSTbr;
extern struct BitMap TBPROGRESSBitMap;
extern Object *TBSCROLLERTbr;
extern struct BitMap TBSCROLLERBitMap;
extern Object *TBSLIDERTbr;
extern struct BitMap TBSLIDERBitMap;
extern Object *TBSTRINGTbr;
extern struct BitMap TBSTRINGBitMap;
extern Object *TBTAPEDECKTbr;
extern struct BitMap TBTAPEDECKBitMap;
extern Object *TBTEXTTbr;
extern struct BitMap TBTEXTBitMap;
extern Object *TBTEXTFIELDTbr;
extern struct BitMap TBTEXTFIELDBitMap;
extern Object *TBTREEVIEWTbr;
extern struct BitMap TBTREEVIEWBitMap;
extern Object *TBTESTTbr;
extern struct BitMap TBTESTBitMap;
extern Object *TBGADGETSTbr;
extern struct BitMap TBGADGETSBitMap;
extern Object *TBFRAMESTbr;
extern struct BitMap TBFRAMESBitMap;
extern Object *TBPOINTERTbr;
extern struct BitMap TBPOINTERBitMap;
extern Object *TBALIGNLEFTTbr;
extern struct BitMap TBALIGNLEFTBitMap;
extern Object *TBALIGNHCENTERTbr;
extern struct BitMap TBALIGNHCENTERBitMap;
extern Object *TBALIGNRIGHTTbr;
extern struct BitMap TBALIGNRIGHTBitMap;
extern Object *TBALIGNTOPTbr;
extern struct BitMap TBALIGNTOPBitMap;
extern Object *TBALIGNVCENTERTbr;
extern struct BitMap TBALIGNVCENTERBitMap;
extern Object *TBALIGNBOTTOMTbr;
extern struct BitMap TBALIGNBOTTOMBitMap;
extern Object *TBSPREADHORIZTbr;
extern struct BitMap TBSPREADHORIZBitMap;
extern Object *TBSPREADVERTTbr;
extern struct BitMap TBSPREADVERTBitMap;
extern Object *TBCLONEWIDTHTbr;
extern struct BitMap TBCLONEWIDTHBitMap;
extern Object *TBCLONEHEIGHTTbr;
extern struct BitMap TBCLONEHEIGHTBitMap;
extern Object *TBCLONEBOTHTbr;
extern struct BitMap TBCLONEBOTHBitMap;
extern Object *GTBLOGOImg;
extern struct BitMap GTBLOGOBitMap;
extern struct SmartBitMapIRGB GTBLOGOPalette[];

extern struct Menu *MainMenus;
extern struct NewMenu MainNewMenu[];
#define MN_Main 0
#define CNTMN_MENUS 2


#define CNT_PGBO_General 9
#define GD_PGBO_LABEL 0
#define GD_PGBO_X 1
#define GD_PGBO_Y 2
#define GD_PGBO_WIDTH 3
#define GD_PGBO_HEIGHT 4
#define GD_PGBO_REL_X 5
#define GD_PGBO_REL_Y 6
#define GD_PGBO_REL_W 7
#define GD_PGBO_REL_H 8

#define CNT_About 2
#define GD_ABOUT_TEXT 0
#define GD_ABOUT_OK 1

#define CNT_PG_BUTT_Flags 5
#define GD_PG_BUTT_FLAGS_IMMEDIATE 0
#define GD_PG_BUTT_FLAGS_UNDERSCORE 1
#define GD_PG_BUTT_FLAGS_DISABLED 2
#define GD_PG_BUTT_FLAGS_TITLETYPE 3
#define GD_PG_BUTT_FLAGS_TOGGLE 4

#define CNT_PG_CBOX_Flags 5
#define GD_PG_CBOX_FLAGS_SCALED 0
#define GD_PG_CBOX_FLAGS_CHECKED 1
#define GD_PG_CBOX_FLAGS_HIGHLABEL 2
#define GD_PG_CBOX_FLAGS_UNDERSCORE 3
#define GD_PG_CBOX_FLAGS_DISABLED 4

#define CNT_PG_CWHL_Flags 6
#define GD_PG_CWHL_FLAGS_MAXPENS 0
#define GD_PG_CWHL_FLAGS_ABBRV 1
#define GD_PG_CWHL_FLAGS_GSLIDER 2
#define GD_PG_CWHL_FLAGS_BEVELBOX 3
#define GD_PG_CWHL_FLAGS_DISABLED 4
#define GD_PG_CWHL_FLAGS_FOLLOWMOUSE 5

#define CNT_PG_CYCE_Flags 3
#define GD_PG_CYCE_FLAGS_HIGHLABEL 0
#define GD_PG_CYCE_FLAGS_UNDERSCORE 1
#define GD_PG_CYCE_FLAGS_DISABLED 2

#define CNT_PG_Lists 7
#define GD_PG_LISTS_EDIT 0
#define GD_PG_LISTS_ACTIVE 1
#define GD_PG_LISTS_LIST 2
#define GD_PG_LISTS_UP 3
#define GD_PG_LISTS_DOWN 4
#define GD_PG_LISTS_ADD 5
#define GD_PG_LISTS_DEL 6

#define CNT_PG_DTYP_Flags 2
#define GD_PG_DTYP_FLAGS_OBJECT 0
#define GD_PG_DTYP_FLAGS_DISABLED 1

#define CNT_PG_GRSL_Flags 7
#define GD_PG_GRSL_FLAGS_KNOBPIXELS 0
#define GD_PG_GRSL_FLAGS_MAXVAL 1
#define GD_PG_GRSL_FLAGS_CURVAL 2
#define GD_PG_GRSL_FLAGS_SKIPVAL 3
#define GD_PG_GRSL_FLAGS_PENS 4
#define GD_PG_GRSL_FLAGS_FREEDOM 5
#define GD_PG_GRSL_FLAGS_DISABLED 6

#define CNT_PG_INTR_Flags 6
#define GD_PG_INTR_FLAGS_DISABLED 0
#define GD_PG_INTR_FLAGS_UNDERSCORE 1
#define GD_PG_INTR_FLAGS_HIGHLABEL 2
#define GD_PG_INTR_FLAGS_TABCYCLE 3
#define GD_PG_INTR_FLAGS_REPLACEMODE 4
#define GD_PG_INTR_FLAGS_EXITHELP 5

#define CNT_PG_INTR_Var 3
#define GD_PG_INTR_VAR_MAXCHARS 0
#define GD_PG_INTR_VAR_NUMBER 1
#define GD_PG_INTR_VAR_JUSTIFICATION 2

#define CNT_PG_LVIE_Flags 8
#define GD_PG_LVIE_FLAGS_ARROWS 0
#define GD_PG_LVIE_FLAGS_SPACING 1
#define GD_PG_LVIE_FLAGS_DISABLED 2
#define GD_PG_LVIE_FLAGS_UNDERSCORE 3
#define GD_PG_LVIE_FLAGS_HIGHLABEL 4
#define GD_PG_LVIE_FLAGS_READONLY 5
#define GD_PG_LVIE_FLAGS_SHOWSELECTED 6
#define GD_PG_LVIE_FLAGS_ARROWS_SEL 7

#define CNT_PG_MX_Flags 6
#define GD_PG_MX_FLAGS_DISABLED 0
#define GD_PG_MX_FLAGS_UNDERSCORE 1
#define GD_PG_MX_FLAGS_HIGHLABEL 2
#define GD_PG_MX_FLAGS_SCALED 3
#define GD_PG_MX_FLAGS_PLACEMENT 4
#define GD_PG_MX_FLAGS_SPACING 5

#define CNT_PG_NUMB_Flags 8
#define GD_PG_NUMB_FLAGS_FORMAT 0
#define GD_PG_NUMB_FLAGS_DEFAULT 1
#define GD_PG_NUMB_FLAGS_MAXNUMLEN 2
#define GD_PG_NUMB_FLAGS_DISABLED 3
#define GD_PG_NUMB_FLAGS_HIGHLABEL 4
#define GD_PG_NUMB_FLAGS_BORDER 5
#define GD_PG_NUMB_FLAGS_CLIPPED 6
#define GD_PG_NUMB_FLAGS_JUSTIFICATION 7

#define CNT_PGGT_General 11
#define GD_PGGT_LABEL 0
#define GD_PGGT_TITLE 1
#define GD_PGGT_X 2
#define GD_PGGT_Y 3
#define GD_PGGT_WIDTH 4
#define GD_PGGT_HEIGHT 5
#define GD_PGGT_PLACEMENT 6
#define GD_PGGT_REL_X 7
#define GD_PGGT_REL_Y 8
#define GD_PGGT_REL_W 9
#define GD_PGGT_REL_H 10

#define CNT_PG_PAGE_Flags 4
#define GD_PG_PAGE_FLAGS_DISABLED 0
#define GD_PG_PAGE_FLAGS_UNDERSCORE 1
#define GD_PG_PAGE_FLAGS_LAYOUT 2
#define GD_PG_PAGE_FLAGS_STYLE 3

#define CNT_PG_PAGE_Lists 7
#define GD_PG_PAGE_LISTS_LIST 0
#define GD_PG_PAGE_LISTS_ADD 1
#define GD_PG_PAGE_LISTS_DEL 2
#define GD_PG_PAGE_LISTS_UP 3
#define GD_PG_PAGE_LISTS_DOWN 4
#define GD_PG_PAGE_LISTS_WINDOWS 5
#define GD_PG_PAGE_LISTS_ACTIVE 6

#define CNT_PG_PALT_Flags 8
#define GD_PG_PALT_FLAGS_COLOR 0
#define GD_PG_PALT_FLAGS_OFFSET 1
#define GD_PG_PALT_FLAGS_SHOWSELECTED_VAR 2
#define GD_PG_PALT_FLAGS_SHOWSELECTED 3
#define GD_PG_PALT_FLAGS_DISABLED 4
#define GD_PG_PALT_FLAGS_UNDERSCORE 5
#define GD_PG_PALT_FLAGS_HIGHLABEL 6
#define GD_PG_PALT_FLAGS_TYPE 7

#define CNT_PG_PUPM_Flags 2
#define GD_PG_PUPM_FLAGS_DISABLED 0
#define GD_PG_PUPM_FLAGS_NEWLOOK 1

#define CNT_PG_PRGR_Flags 7
#define GD_PG_PRGR_FLAGS_MAX 0
#define GD_PG_PRGR_FLAGS_CURRENT 1
#define GD_PG_PRGR_FLAGS_FORMAT 2
#define GD_PG_PRGR_FLAGS_DISABLED 3
#define GD_PG_PRGR_FLAGS_BORDER 4
#define GD_PG_PRGR_FLAGS_PERCENTS 5
#define GD_PG_PRGR_FLAGS_PLACEMENT 6

#define CNT_PG_SCRL_Flags 5
#define GD_PG_SCRL_FLAGS_DISABLED 0
#define GD_PG_SCRL_FLAGS_UNDERSCORE 1
#define GD_PG_SCRL_FLAGS_HIGHLABEL 2
#define GD_PG_SCRL_FLAGS_IMMEDIATE 3
#define GD_PG_SCRL_FLAGS_RELVERIFY 4

#define CNT_PG_SCRL_Var 6
#define GD_PG_SCRL_VAR_TOP 0
#define GD_PG_SCRL_VAR_TOTAL 1
#define GD_PG_SCRL_VAR_VISIBLE 2
#define GD_PG_SCRL_VAR_FREEDOM 3
#define GD_PG_SCRL_VAR_ARROWS 4
#define GD_PG_SCRL_VAR_ARROW_TYPE 5

#define CNT_PG_SLIR_Flags 8
#define GD_PG_SLIR_FLAGS_FORMAT 0
#define GD_PG_SLIR_FLAGS_MAXPIXELLEN 1
#define GD_PG_SLIR_FLAGS_MAXPIXELLEN_SET 2
#define GD_PG_SLIR_FLAGS_DISABLED 3
#define GD_PG_SLIR_FLAGS_UNDERSCORE 4
#define GD_PG_SLIR_FLAGS_HIGHLABEL 5
#define GD_PG_SLIR_FLAGS_IMMEDIATE 6
#define GD_PG_SLIR_FLAGS_RELVERIFY 7

#define CNT_PG_SLIR_Var 7
#define GD_PG_SLIR_VAR_MIN 0
#define GD_PG_SLIR_VAR_MAX 1
#define GD_PG_SLIR_VAR_LEVEL 2
#define GD_PG_SLIR_VAR_MAXLEVLEN 3
#define GD_PG_SLIR_VAR_JUSTIFICATION 4
#define GD_PG_SLIR_VAR_FREEDOM 5
#define GD_PG_SLIR_VAR_LEVELPLACE 6

#define CNT_PG_STRG_Flags 6
#define GD_PG_STRG_FLAGS_DISABLED 0
#define GD_PG_STRG_FLAGS_UNDERSCORE 1
#define GD_PG_STRG_FLAGS_HIGHLABEL 2
#define GD_PG_STRG_FLAGS_TABCYCLE 3
#define GD_PG_STRG_FLAGS_REPLACEMODE 4
#define GD_PG_STRG_FLAGS_EXITHELP 5

#define CNT_PG_STRG_Var 3
#define GD_PG_STRG_VAR_MAXCHARS 0
#define GD_PG_STRG_VAR_STRING 1
#define GD_PG_STRG_VAR_JUSTIFICATION 2

#define CNT_PG_TDEK_Flags 5
#define GD_PG_TDEK_FLAGS_FRAMES 0
#define GD_PG_TDEK_FLAGS_CURRENT 1
#define GD_PG_TDEK_FLAGS_DISABLED 2
#define GD_PG_TDEK_FLAGS_TAPE 3
#define GD_PG_TDEK_FLAGS_PAUSED 4

#define CNT_PG_TEXT_Flags 7
#define GD_PG_TEXT_FLAGS_DISABLED 0
#define GD_PG_TEXT_FLAGS_HIGHLABEL 1
#define GD_PG_TEXT_FLAGS_BORDER 2
#define GD_PG_TEXT_FLAGS_CLIPPED 3
#define GD_PG_TEXT_FLAGS_COPYTEXT 4
#define GD_PG_TEXT_FLAGS_JUSTIFICATION 5
#define GD_PG_TEXT_FLAGS_DEFAULT 6

#define CNT_PG_TFLD_Flags 10
#define GD_PG_TFLD_FLAGS_DISABLED 0
#define GD_PG_TFLD_FLAGS_TABCYCLE 1
#define GD_PG_TFLD_FLAGS_READONLY 2
#define GD_PG_TFLD_FLAGS_INVERTED 3
#define GD_PG_TFLD_FLAGS_PARTIAL 4
#define GD_PG_TFLD_FLAGS_BLOCKCURSOR 5
#define GD_PG_TFLD_FLAGS_VCENTER 6
#define GD_PG_TFLD_FLAGS_PASSCOMMAND 7
#define GD_PG_TFLD_FLAGS_CLIPBOARD 8
#define GD_PG_TFLD_FLAGS_UNDO 9

#define CNT_PG_TFLD_Flags2 8
#define GD_PG_TFLD_FLAGS_NONPRINTCHARS 0
#define GD_PG_TFLD_FLAGS_MAXSIZEBEEP 1
#define GD_PG_TFLD_FLAGS_NOGHOST 2
#define GD_PG_TFLD_FLAGS_MODIFIED 3
#define GD_PG_TFLD_FLAGS_USERALIGN 4
#define GD_PG_TFLD_FLAGS_RULEDPAPER 5
#define GD_PG_TFLD_FLAGS_BORDER 6
#define GD_PG_TFLD_FLAGS_ALIGN 7

#define CNT_PG_TFLD_Var 7
#define GD_PG_TFLD_VAR_BLINKRATE 0
#define GD_PG_TFLD_VAR_MAXSIZE 1
#define GD_PG_TFLD_VAR_TABSPACES 2
#define GD_PG_TFLD_VAR_SPACING 3
#define GD_PG_TFLD_VAR_DELIMITERS 4
#define GD_PG_TFLD_VAR_ACCEPTCHARS 5
#define GD_PG_TFLD_VAR_REJECTCHARS 6

#define CNT_Properties 3
#define GD_PROPERTIES_LOCK 0
#define GD_PROPERTIES_APPLY 1
#define GD_PROPERTIES_PAGES 2


#define CNT_Preferences 11
#define GD_PREFS_SAVE 0
#define GD_PREFS_USE 1
#define GD_PREFS_CANCEL 2
#define GD_PG_PREFS_GENERAL_AUTOBACKUP 3
#define GD_PG_PREFS_GENERAL_SIZEZOOM 4
#define GD_PG_PREFS_GENERAL_FASTREDRAW 5
#define GD_PG_PREFS_GENERAL_GETSCREEN 6
#define GD_PG_PREFS_GENERAL_SCREENMODE 7
#define GD_PG_PREFS_GEN_LIST 8
#define GD_PG_PREFS_GENERAL_FONTNAME 9
#define GD_PG_PREFS_GENERAL_EDITFONT 10

#define CNT_Grid 6
#define GD_GRID_X 0
#define GD_GRID_Y 1
#define GD_GRID_SHOW 2
#define GD_GRID_SNAP 3
#define GD_GRID_OK 4
#define GD_GRID_CANCEL 5

#define CNT_PGWN_General 11
#define GD_PGWN_LABEL 0
#define GD_PGWN_X 1
#define GD_PGWN_Y 2
#define GD_PGWN_WIDTH 3
#define GD_PGWN_HEIGHT 4
#define GD_PGWN_INNERHEIGHT 5
#define GD_PGWN_INNERWIDTH 6
#define GD_PGWN_REL_H 7
#define GD_PGWN_REL_W 8
#define GD_PGWN_REL_Y 9
#define GD_PGWN_REL_X 10

#define CNT_PGWN_Flags1 8
#define GD_PGWN_SCREEN_TITLE 0
#define GD_PGWN_WINDOW_TITLE 1
#define GD_PGWN_PAGED 2
#define GD_PGWN_FONTADAPTIVE 3
#define GD_PGWN_APPWINDOW 4
#define GD_PGWN_PROJECT 5
#define GD_PGWN_MENU_GET 6
#define GD_PGWN_MENU 7

#define CNT_PGWN_Style 9
#define GD_PGWN_ZOOM 0
#define GD_PGWN_BACKDROP 1
#define GD_PGWN_BORDERLESS 2
#define GD_PGWN_SIZEBBOTTOM 3
#define GD_PGWN_SIZEBRIGHT 4
#define GD_PGWN_CLOSEGADGET 5
#define GD_PGWN_DEPTHGADGET 6
#define GD_PGWN_SIZEGADGET 7
#define GD_PGWN_DRAGGADGET 8

#define CNT_PGWN_Flags2 9
#define GD_PGWN_FALLBACK 0
#define GD_PGWN_REPORTMOUSE 1
#define GD_PGWN_AUTOADJUST 2
#define GD_PGWN_ACTIVATE 3
#define GD_PGWN_RMBTRAP 4
#define GD_PGWN_NEWLOOKMENUS 5
#define GD_PGWN_NOCAREREFRESH 6
#define GD_PGWN_GZZ 7
#define GD_PGWN_REFRESH 8

#define CNT_PGWN_Flags3 8
#define GD_PGWN_MOUSEQUEUE 0
#define GD_PGWN_REPEATQUEUE 1
#define GD_PGWN_IDCMP 2
#define GD_PGWN_REPEATQUEUE_SET 3
#define GD_PGWN_MOUSEQUEUE_SET 4
#define GD_PGWN_NOTIFYDEPTH 5
#define GD_PGWN_MENUHELP 6
#define GD_PGWN_POINTERDELAY 7

#define CNT_WN_IDCMPFlags 28
#define GD_IDCMP_CANCEL 0
#define GD_IDCMP_OK 1
#define GD_IDCMP_MENUPICK 2
#define GD_IDCMP_GADGETHELP 3
#define GD_IDCMP_MENUHELP 4
#define GD_IDCMP_IDCMPUPDATE 5
#define GD_IDCMP_DISKINSERTED 6
#define GD_IDCMP_DISKREMOVED 7
#define GD_IDCMP_INACTIVEWINDOW 8
#define GD_IDCMP_ACTIVEWINDOW 9
#define GD_IDCMP_INTUITICKS 10
#define GD_IDCMP_CLOSEWINDOW 11
#define GD_IDCMP_VANILLAKEY 12
#define GD_IDCMP_RAWKEY 13
#define GD_IDCMP_GADGETDOWN 14
#define GD_IDCMP_GADGETUP 15
#define GD_IDCMP_MOUSEBUTTONS 16
#define GD_IDCMP_NEWPREFS 17
#define GD_IDCMP_CHANGEWINDOW 18
#define GD_IDCMP_REFRESHWINDOW 19
#define GD_IDCMP_NEWSIZE 20
#define GD_IDCMP_REQCLEAR 21
#define GD_IDCMP_REQSET 22
#define GD_IDCMP_REQVERIFY 23
#define GD_IDCMP_SIZEVERIFY 24
#define GD_IDCMP_MENUVERIFY 25
#define GD_IDCMP_MOUSEMOVE 26
#define GD_IDCMP_DELTAMOVE 27

#define CNT_TB_Main 25
#define GD_TB_NEW 0
#define GD_TB_OPEN 1
#define GD_TB_SAVE 2
#define GD_TB_CUT 3
#define GD_TB_COPY 4
#define GD_TB_PASTE 5
#define GD_TB_UNDO 6
#define GD_TB_HELP 7
#define GD_TB_NEWWINDOW 8
#define GD_TB_NEWMENU 9
#define GD_TB_NEWPOINTER 10
#define GD_TB_NEWSTRING 11
#define GD_TB_NEWVERSION 12
#define GD_TB_NEWSCREEN 13
#define GD_TB_ALIGNLEFT 14
#define GD_TB_ALIGNHCENTER 15
#define GD_TB_ALIGNRIGHT 16
#define GD_TB_ALIGNTOP 17
#define GD_TB_ALIGNVCENTER 18
#define GD_TB_ALIGNBOTTOM 19
#define GD_TB_SPREADHORIZ 20
#define GD_TB_SPREADVERT 21
#define GD_TB_CLONEWIDTH 22
#define GD_TB_CLONEHEIGHT 23
#define GD_TB_CLONEBOTH 24

#define CNT_TB_Window 25
#define GD_TBG_NONE 0
#define GD_TBG_TEXTFIELD 1
#define GD_TBG_TEXT 2
#define GD_TBG_TAPEDECK 3
#define GD_TBG_STRING 4
#define GD_TBG_SLIDER 5
#define GD_TBG_SCROLLER 6
#define GD_TBG_PROGRESS 7
#define GD_TBG_POPUPMENU 8
#define GD_TBG_PALETTE 9
#define GD_TBG_PAGE 10
#define GD_TBG_NUMBER 11
#define GD_TBG_MX 12
#define GD_TBG_LISTVIEW 13
#define GD_TBG_INTEGER 14
#define GD_TBG_GRADIENTSLIDER 15
#define GD_TBG_DATATYPE 16
#define GD_TBG_CYCLE 17
#define GD_TBG_COLORWHEEL 18
#define GD_TBG_CHECKBOX 19
#define GD_TBG_BUTTON 20
#define GD_TBG_MODE_TEST 21
#define GD_TBG_MODE_GADGET 22
#define GD_TBG_MODE_FRAME 23
#define GD_TBG_TREEVIEW 24

#define CNT_PGMN_General 2
#define GD_PGMN_LABEL 0
#define GD_PGMN_NEWLOOK 1

#define CNT_Working 1
#define GD_WORK_PROGRESS 0

#define CNT_PGFR_General 6
#define GD_PGFR_X 0
#define GD_PGFR_Y 1
#define GD_PGFR_WIDTH 2
#define GD_PGFR_HEIGHT 3
#define GD_PGFR_FRAME 4
#define GD_PGFR_RECESSED 5




#define CNT_List 1
#define GD_LIST 0

#define CNT_PGMN_Item 8
#define GD_PGMN_ITEM_LABEL 0
#define GD_PGMN_ITEM_TITLE 1
#define GD_PGMN_ITEM_COMMKEY 2
#define GD_PGMN_ITEM_POPUP 3
#define GD_PGMN_ITEM_BARLABEL 4
#define GD_PGMN_ITEM_DISABLED 5
#define GD_PGMN_ITEM_CHECKED 6
#define GD_PGMN_ITEM_TOGGLE 7

#define CNT_PGPI_General 7
#define GD_PGPI_LABEL 0
#define GD_PGPI_WIDTH 1
#define GD_PGPI_HEIGHT 2
#define GD_PGPI_DEPTH 3
#define GD_PGPI_TYPE 4
#define GD_PGPI_SETPOINT 5
#define GD_PGPI_PALETTE 6

#define CNT_TB_Pointer 1
#define GD_TBP_PALETTE 0

#define CNT_Screen 1
#define GD_SCREEN_PAGES 0

#define CNT_PGSCR_General 10
#define GD_PGSCR_WIDTH 0
#define GD_PGSCR_HEIGHT 1
#define GD_PGSCR_DEPTH 2
#define GD_PGSCR_TITLE 3
#define GD_PGSCR_PUBNAME 4
#define GD_PGSCR_USE 5
#define GD_PGSCR_MAKEPUBLIC 6
#define GD_PGSCR_MODEID 7
#define GD_PGSCR_MODEID_GET 8
#define GD_PGSCR_DEFAULTID 9

#define CNT_PGSCR_Flags 11
#define GD_PGSCR_SHOWTITLE 0
#define GD_PGSCR_BEHIND 1
#define GD_PGSCR_QUIET 2
#define GD_PGSCR_AUTOSCROLL 3
#define GD_PGSCR_FULLPALETTE 4
#define GD_PGSCR_DRAGGABLE 5
#define GD_PGSCR_EXCLUSIVE 6
#define GD_PGSCR_SHAREPENS 7
#define GD_PGSCR_INTERLEAVED 8
#define GD_PGSCR_LIKEWORKBENCH 9
#define GD_PGSCR_MINIMIZEISG 10

#define CNT_EditString 4
#define GD_PGSTR_STRINGS 0
#define GD_PGSTR_ADD 1
#define GD_PGSTR_DELETE 2
#define GD_PGSTR_EDIT 3

#define CNT_PGSTR_General 2
#define GD_PGSTR_LABEL 0
#define GD_PGSTR_STRING 1

#define CNT_EditVersion 5
#define GD_PGVER_PROGRAMNAME 0
#define GD_PGVER_AUTHOR 1
#define GD_PGVER_VERSION 2
#define GD_PGVER_DATE 3
#define GD_PGVER_REVISION 4

#define CNT_PG_TRLV_Flags 9
#define GD_PG_TRLV_FLAGS_COLUMNS 0
#define GD_PG_TRLV_FLAGS_FORMAT 1
#define GD_PG_TRLV_FLAGS_DISABLED 2
#define GD_PG_TRLV_FLAGS_MULTISELECT 3
#define GD_PG_TRLV_FLAGS_BORDER 4
#define GD_PG_TRLV_FLAGS_READONLY 5
#define GD_PG_TRLV_FLAGS_SHOWTREELINES 6
#define GD_PG_TRLV_FLAGS_SHOWSELECTED 7
#define GD_PG_TRLV_FLAGS_TREE 8

#define CNT_Object 1
#define GD_OBJLIST 0

#define CNT_EditPalette 8
#define GD_PAL_PALETTE 0
#define GD_PAL_GREEN 1
#define GD_PAL_BLUE 2
#define GD_PAL_RED 3
#define GD_PAL_CANCEL 4
#define GD_PAL_OK 5
#define GD_PAL_GSLIDER 6
#define GD_PAL_COLORWHEEL 7

#define CNT_PGWN_TabOrder 3
#define GD_PGWN_TABORDER_DOWN 0
#define GD_PGWN_TABORDER_UP 1
#define GD_PGWN_TABORDER_LIST 2

#define CNT_Status 2
#define GD_STATUS_CHANGED 0
#define GD_STATUS_TEXT 1

/* Prototypes */
int FN_PGBO_LABEL_Clicked(void);
int FN_PGBO_X_Clicked(void);
int FN_PGBO_Y_Clicked(void);
int FN_PGBO_WIDTH_Clicked(void);
int FN_PGBO_HEIGHT_Clicked(void);
int FN_PGBO_REL_X_Clicked(void);
int FN_PGBO_REL_Y_Clicked(void);
int FN_PGBO_REL_W_Clicked(void);
int FN_PGBO_REL_H_Clicked(void);

int FN_ABOUT_TEXT_Clicked(void);
int FN_ABOUT_OK_Clicked(void);

int FN_PG_BUTT_FLAGS_IMMEDIATE_Clicked(void);
int FN_PG_BUTT_FLAGS_UNDERSCORE_Clicked(void);
int FN_PG_BUTT_FLAGS_DISABLED_Clicked(void);
int FN_PG_BUTT_FLAGS_TITLETYPE_Clicked(void);
int FN_PG_BUTT_FLAGS_TOGGLE_Clicked(void);

int FN_PG_CBOX_FLAGS_SCALED_Clicked(void);
int FN_PG_CBOX_FLAGS_CHECKED_Clicked(void);
int FN_PG_CBOX_FLAGS_HIGHLABEL_Clicked(void);
int FN_PG_CBOX_FLAGS_UNDERSCORE_Clicked(void);
int FN_PG_CBOX_FLAGS_DISABLED_Clicked(void);

int FN_PG_CWHL_FLAGS_MAXPENS_Clicked(void);
int FN_PG_CWHL_FLAGS_ABBRV_Clicked(void);
int FN_PG_CWHL_FLAGS_GSLIDER_Clicked(void);
int FN_PG_CWHL_FLAGS_BEVELBOX_Clicked(void);
int FN_PG_CWHL_FLAGS_DISABLED_Clicked(void);
int FN_PG_CWHL_FLAGS_FOLLOWMOUSE_Clicked(void);

int FN_PG_CYCE_FLAGS_HIGHLABEL_Clicked(void);
int FN_PG_CYCE_FLAGS_UNDERSCORE_Clicked(void);
int FN_PG_CYCE_FLAGS_DISABLED_Clicked(void);

int FN_PG_LISTS_EDIT_Clicked(void);
int FN_PG_LISTS_ACTIVE_Clicked(void);
int FN_PG_LISTS_LIST_Clicked(void);
int FN_PG_LISTS_UP_Clicked(void);
int FN_PG_LISTS_DOWN_Clicked(void);
int FN_PG_LISTS_ADD_Clicked(void);
int FN_PG_LISTS_DEL_Clicked(void);

int FN_PG_DTYP_FLAGS_OBJECT_Clicked(void);
int FN_PG_DTYP_FLAGS_DISABLED_Clicked(void);

int FN_PG_GRSL_FLAGS_KNOBPIXELS_Clicked(void);
int FN_PG_GRSL_FLAGS_MAXVAL_Clicked(void);
int FN_PG_GRSL_FLAGS_CURVAL_Clicked(void);
int FN_PG_GRSL_FLAGS_SKIPVAL_Clicked(void);
int FN_PG_GRSL_FLAGS_PENS_Clicked(void);
int FN_PG_GRSL_FLAGS_FREEDOM_Clicked(void);
int FN_PG_GRSL_FLAGS_DISABLED_Clicked(void);

int FN_PG_INTR_FLAGS_DISABLED_Clicked(void);
int FN_PG_INTR_FLAGS_UNDERSCORE_Clicked(void);
int FN_PG_INTR_FLAGS_HIGHLABEL_Clicked(void);
int FN_PG_INTR_FLAGS_TABCYCLE_Clicked(void);
int FN_PG_INTR_FLAGS_REPLACEMODE_Clicked(void);
int FN_PG_INTR_FLAGS_EXITHELP_Clicked(void);

int FN_PG_INTR_VAR_MAXCHARS_Clicked(void);
int FN_PG_INTR_VAR_NUMBER_Clicked(void);
int FN_PG_INTR_VAR_JUSTIFICATION_Clicked(void);

int FN_PG_LVIE_FLAGS_ARROWS_Clicked(void);
int FN_PG_LVIE_FLAGS_SPACING_Clicked(void);
int FN_PG_LVIE_FLAGS_DISABLED_Clicked(void);
int FN_PG_LVIE_FLAGS_UNDERSCORE_Clicked(void);
int FN_PG_LVIE_FLAGS_HIGHLABEL_Clicked(void);
int FN_PG_LVIE_FLAGS_READONLY_Clicked(void);
int FN_PG_LVIE_FLAGS_SHOWSELECTED_Clicked(void);
int FN_PG_LVIE_FLAGS_ARROWS_SEL_Clicked(void);

int FN_PG_MX_FLAGS_DISABLED_Clicked(void);
int FN_PG_MX_FLAGS_UNDERSCORE_Clicked(void);
int FN_PG_MX_FLAGS_HIGHLABEL_Clicked(void);
int FN_PG_MX_FLAGS_SCALED_Clicked(void);
int FN_PG_MX_FLAGS_PLACEMENT_Clicked(void);
int FN_PG_MX_FLAGS_SPACING_Clicked(void);

int FN_PG_NUMB_FLAGS_FORMAT_Clicked(void);
int FN_PG_NUMB_FLAGS_DEFAULT_Clicked(void);
int FN_PG_NUMB_FLAGS_MAXNUMLEN_Clicked(void);
int FN_PG_NUMB_FLAGS_DISABLED_Clicked(void);
int FN_PG_NUMB_FLAGS_HIGHLABEL_Clicked(void);
int FN_PG_NUMB_FLAGS_BORDER_Clicked(void);
int FN_PG_NUMB_FLAGS_CLIPPED_Clicked(void);
int FN_PG_NUMB_FLAGS_JUSTIFICATION_Clicked(void);

int FN_PGGT_LABEL_Clicked(void);
int FN_PGGT_TITLE_Clicked(void);
int FN_PGGT_X_Clicked(void);
int FN_PGGT_Y_Clicked(void);
int FN_PGGT_WIDTH_Clicked(void);
int FN_PGGT_HEIGHT_Clicked(void);
int FN_PGGT_PLACEMENT_Clicked(void);
int FN_PGGT_REL_X_Clicked(void);
int FN_PGGT_REL_Y_Clicked(void);
int FN_PGGT_REL_W_Clicked(void);
int FN_PGGT_REL_H_Clicked(void);

int FN_PG_PAGE_FLAGS_DISABLED_Clicked(void);
int FN_PG_PAGE_FLAGS_UNDERSCORE_Clicked(void);
int FN_PG_PAGE_FLAGS_LAYOUT_Clicked(void);
int FN_PG_PAGE_FLAGS_STYLE_Clicked(void);

int FN_PG_PAGE_LISTS_LIST_Clicked(void);
int FN_PG_PAGE_LISTS_ADD_Clicked(void);
int FN_PG_PAGE_LISTS_DEL_Clicked(void);
int FN_PG_PAGE_LISTS_UP_Clicked(void);
int FN_PG_PAGE_LISTS_DOWN_Clicked(void);
int FN_PG_PAGE_LISTS_WINDOWS_Clicked(void);
int FN_PG_PAGE_LISTS_ACTIVE_Clicked(void);

int FN_PG_PALT_FLAGS_COLOR_Clicked(void);
int FN_PG_PALT_FLAGS_OFFSET_Clicked(void);
int FN_PG_PALT_FLAGS_SHOWSELECTED_VAR_Clicked(void);
int FN_PG_PALT_FLAGS_SHOWSELECTED_Clicked(void);
int FN_PG_PALT_FLAGS_DISABLED_Clicked(void);
int FN_PG_PALT_FLAGS_UNDERSCORE_Clicked(void);
int FN_PG_PALT_FLAGS_HIGHLABEL_Clicked(void);
int FN_PG_PALT_FLAGS_TYPE_Clicked(void);

int FN_PG_PUPM_FLAGS_DISABLED_Clicked(void);
int FN_PG_PUPM_FLAGS_NEWLOOK_Clicked(void);

int FN_PG_PRGR_FLAGS_MAX_Clicked(void);
int FN_PG_PRGR_FLAGS_CURRENT_Clicked(void);
int FN_PG_PRGR_FLAGS_FORMAT_Clicked(void);
int FN_PG_PRGR_FLAGS_DISABLED_Clicked(void);
int FN_PG_PRGR_FLAGS_BORDER_Clicked(void);
int FN_PG_PRGR_FLAGS_PERCENTS_Clicked(void);
int FN_PG_PRGR_FLAGS_PLACEMENT_Clicked(void);

int FN_PG_SCRL_FLAGS_DISABLED_Clicked(void);
int FN_PG_SCRL_FLAGS_UNDERSCORE_Clicked(void);
int FN_PG_SCRL_FLAGS_HIGHLABEL_Clicked(void);
int FN_PG_SCRL_FLAGS_IMMEDIATE_Clicked(void);
int FN_PG_SCRL_FLAGS_RELVERIFY_Clicked(void);

int FN_PG_SCRL_VAR_TOP_Clicked(void);
int FN_PG_SCRL_VAR_TOTAL_Clicked(void);
int FN_PG_SCRL_VAR_VISIBLE_Clicked(void);
int FN_PG_SCRL_VAR_FREEDOM_Clicked(void);
int FN_PG_SCRL_VAR_ARROWS_Clicked(void);
int FN_PG_SCRL_VAR_ARROW_TYPE_Clicked(void);

int FN_PG_SLIR_FLAGS_FORMAT_Clicked(void);
int FN_PG_SLIR_FLAGS_MAXPIXELLEN_Clicked(void);
int FN_PG_SLIR_FLAGS_MAXPIXELLEN_SET_Clicked(void);
int FN_PG_SLIR_FLAGS_DISABLED_Clicked(void);
int FN_PG_SLIR_FLAGS_UNDERSCORE_Clicked(void);
int FN_PG_SLIR_FLAGS_HIGHLABEL_Clicked(void);
int FN_PG_SLIR_FLAGS_IMMEDIATE_Clicked(void);
int FN_PG_SLIR_FLAGS_RELVERIFY_Clicked(void);

int FN_PG_SLIR_VAR_MIN_Clicked(void);
int FN_PG_SLIR_VAR_MAX_Clicked(void);
int FN_PG_SLIR_VAR_LEVEL_Clicked(void);
int FN_PG_SLIR_VAR_MAXLEVLEN_Clicked(void);
int FN_PG_SLIR_VAR_JUSTIFICATION_Clicked(void);
int FN_PG_SLIR_VAR_FREEDOM_Clicked(void);
int FN_PG_SLIR_VAR_LEVELPLACE_Clicked(void);

int FN_PG_STRG_FLAGS_DISABLED_Clicked(void);
int FN_PG_STRG_FLAGS_UNDERSCORE_Clicked(void);
int FN_PG_STRG_FLAGS_HIGHLABEL_Clicked(void);
int FN_PG_STRG_FLAGS_TABCYCLE_Clicked(void);
int FN_PG_STRG_FLAGS_REPLACEMODE_Clicked(void);
int FN_PG_STRG_FLAGS_EXITHELP_Clicked(void);

int FN_PG_STRG_VAR_MAXCHARS_Clicked(void);
int FN_PG_STRG_VAR_STRING_Clicked(void);
int FN_PG_STRG_VAR_JUSTIFICATION_Clicked(void);

int FN_PG_TDEK_FLAGS_FRAMES_Clicked(void);
int FN_PG_TDEK_FLAGS_CURRENT_Clicked(void);
int FN_PG_TDEK_FLAGS_DISABLED_Clicked(void);
int FN_PG_TDEK_FLAGS_TAPE_Clicked(void);
int FN_PG_TDEK_FLAGS_PAUSED_Clicked(void);

int FN_PG_TEXT_FLAGS_DISABLED_Clicked(void);
int FN_PG_TEXT_FLAGS_HIGHLABEL_Clicked(void);
int FN_PG_TEXT_FLAGS_BORDER_Clicked(void);
int FN_PG_TEXT_FLAGS_CLIPPED_Clicked(void);
int FN_PG_TEXT_FLAGS_COPYTEXT_Clicked(void);
int FN_PG_TEXT_FLAGS_JUSTIFICATION_Clicked(void);
int FN_PG_TEXT_FLAGS_DEFAULT_Clicked(void);

int FN_PG_TFLD_FLAGS_DISABLED_Clicked(void);
int FN_PG_TFLD_FLAGS_TABCYCLE_Clicked(void);
int FN_PG_TFLD_FLAGS_READONLY_Clicked(void);
int FN_PG_TFLD_FLAGS_INVERTED_Clicked(void);
int FN_PG_TFLD_FLAGS_PARTIAL_Clicked(void);
int FN_PG_TFLD_FLAGS_BLOCKCURSOR_Clicked(void);
int FN_PG_TFLD_FLAGS_VCENTER_Clicked(void);
int FN_PG_TFLD_FLAGS_PASSCOMMAND_Clicked(void);
int FN_PG_TFLD_FLAGS_CLIPBOARD_Clicked(void);
int FN_PG_TFLD_FLAGS_UNDO_Clicked(void);

int FN_PG_TFLD_FLAGS_NONPRINTCHARS_Clicked(void);
int FN_PG_TFLD_FLAGS_MAXSIZEBEEP_Clicked(void);
int FN_PG_TFLD_FLAGS_NOGHOST_Clicked(void);
int FN_PG_TFLD_FLAGS_MODIFIED_Clicked(void);
int FN_PG_TFLD_FLAGS_USERALIGN_Clicked(void);
int FN_PG_TFLD_FLAGS_RULEDPAPER_Clicked(void);
int FN_PG_TFLD_FLAGS_BORDER_Clicked(void);
int FN_PG_TFLD_FLAGS_ALIGN_Clicked(void);

int FN_PG_TFLD_VAR_BLINKRATE_Clicked(void);
int FN_PG_TFLD_VAR_MAXSIZE_Clicked(void);
int FN_PG_TFLD_VAR_TABSPACES_Clicked(void);
int FN_PG_TFLD_VAR_SPACING_Clicked(void);
int FN_PG_TFLD_VAR_DELIMITERS_Clicked(void);
int FN_PG_TFLD_VAR_ACCEPTCHARS_Clicked(void);
int FN_PG_TFLD_VAR_REJECTCHARS_Clicked(void);

int FN_PROPERTIES_LOCK_Clicked(void);
int FN_PROPERTIES_APPLY_Clicked(void);
int FN_PROPERTIES_PAGES_Clicked(void);


int FN_PREFS_SAVE_Clicked(void);
int FN_PREFS_USE_Clicked(void);
int FN_PREFS_CANCEL_Clicked(void);
int FN_PG_PREFS_GENERAL_AUTOBACKUP_Clicked(void);
int FN_PG_PREFS_GENERAL_SIZEZOOM_Clicked(void);
int FN_PG_PREFS_GENERAL_FASTREDRAW_Clicked(void);
int FN_PG_PREFS_GENERAL_GETSCREEN_Clicked(void);
int FN_PG_PREFS_GEN_LIST_Clicked(void);
int FN_PG_PREFS_GENERAL_EDITFONT_Clicked(void);

int FN_GRID_X_Clicked(void);
int FN_GRID_Y_Clicked(void);
int FN_GRID_SHOW_Clicked(void);
int FN_GRID_SNAP_Clicked(void);
int FN_GRID_OK_Clicked(void);
int FN_GRID_CANCEL_Clicked(void);

int FN_PGWN_LABEL_Clicked(void);
int FN_PGWN_X_Clicked(void);
int FN_PGWN_Y_Clicked(void);
int FN_PGWN_WIDTH_Clicked(void);
int FN_PGWN_HEIGHT_Clicked(void);
int FN_PGWN_INNERHEIGHT_Clicked(void);
int FN_PGWN_INNERWIDTH_Clicked(void);
int FN_PGWN_REL_H_Clicked(void);
int FN_PGWN_REL_W_Clicked(void);
int FN_PGWN_REL_Y_Clicked(void);
int FN_PGWN_REL_X_Clicked(void);

int FN_PGWN_SCREEN_TITLE_Clicked(void);
int FN_PGWN_WINDOW_TITLE_Clicked(void);
int FN_PGWN_PAGED_Clicked(void);
int FN_PGWN_FONTADAPTIVE_Clicked(void);
int FN_PGWN_APPWINDOW_Clicked(void);
int FN_PGWN_PROJECT_Clicked(void);
int FN_PGWN_MENU_GET_Clicked(void);

int FN_PGWN_ZOOM_Clicked(void);
int FN_PGWN_BACKDROP_Clicked(void);
int FN_PGWN_BORDERLESS_Clicked(void);
int FN_PGWN_SIZEBBOTTOM_Clicked(void);
int FN_PGWN_SIZEBRIGHT_Clicked(void);
int FN_PGWN_CLOSEGADGET_Clicked(void);
int FN_PGWN_DEPTHGADGET_Clicked(void);
int FN_PGWN_SIZEGADGET_Clicked(void);
int FN_PGWN_DRAGGADGET_Clicked(void);

int FN_PGWN_FALLBACK_Clicked(void);
int FN_PGWN_REPORTMOUSE_Clicked(void);
int FN_PGWN_AUTOADJUST_Clicked(void);
int FN_PGWN_ACTIVATE_Clicked(void);
int FN_PGWN_RMBTRAP_Clicked(void);
int FN_PGWN_NEWLOOKMENUS_Clicked(void);
int FN_PGWN_NOCAREREFRESH_Clicked(void);
int FN_PGWN_GZZ_Clicked(void);
int FN_PGWN_REFRESH_Clicked(void);

int FN_PGWN_MOUSEQUEUE_Clicked(void);
int FN_PGWN_REPEATQUEUE_Clicked(void);
int FN_PGWN_IDCMP_Clicked(void);
int FN_PGWN_REPEATQUEUE_SET_Clicked(void);
int FN_PGWN_MOUSEQUEUE_SET_Clicked(void);
int FN_PGWN_NOTIFYDEPTH_Clicked(void);
int FN_PGWN_MENUHELP_Clicked(void);
int FN_PGWN_POINTERDELAY_Clicked(void);

int FN_IDCMP_CANCEL_Clicked(void);
int FN_IDCMP_OK_Clicked(void);
int FN_IDCMP_MENUPICK_Clicked(void);
int FN_IDCMP_GADGETHELP_Clicked(void);
int FN_IDCMP_MENUHELP_Clicked(void);
int FN_IDCMP_IDCMPUPDATE_Clicked(void);
int FN_IDCMP_DISKINSERTED_Clicked(void);
int FN_IDCMP_DISKREMOVED_Clicked(void);
int FN_IDCMP_INACTIVEWINDOW_Clicked(void);
int FN_IDCMP_ACTIVEWINDOW_Clicked(void);
int FN_IDCMP_INTUITICKS_Clicked(void);
int FN_IDCMP_CLOSEWINDOW_Clicked(void);
int FN_IDCMP_VANILLAKEY_Clicked(void);
int FN_IDCMP_RAWKEY_Clicked(void);
int FN_IDCMP_GADGETDOWN_Clicked(void);
int FN_IDCMP_GADGETUP_Clicked(void);
int FN_IDCMP_MOUSEBUTTONS_Clicked(void);
int FN_IDCMP_NEWPREFS_Clicked(void);
int FN_IDCMP_CHANGEWINDOW_Clicked(void);
int FN_IDCMP_REFRESHWINDOW_Clicked(void);
int FN_IDCMP_NEWSIZE_Clicked(void);
int FN_IDCMP_REQCLEAR_Clicked(void);
int FN_IDCMP_REQSET_Clicked(void);
int FN_IDCMP_REQVERIFY_Clicked(void);
int FN_IDCMP_SIZEVERIFY_Clicked(void);
int FN_IDCMP_MENUVERIFY_Clicked(void);
int FN_IDCMP_MOUSEMOVE_Clicked(void);
int FN_IDCMP_DELTAMOVE_Clicked(void);

int FN_TB_NEW_Clicked(void);
int FN_TB_OPEN_Clicked(void);
int FN_TB_SAVE_Clicked(void);
int FN_TB_CUT_Clicked(void);
int FN_TB_COPY_Clicked(void);
int FN_TB_PASTE_Clicked(void);
int FN_TB_UNDO_Clicked(void);
int FN_TB_HELP_Clicked(void);
int FN_TB_NEWWINDOW_Clicked(void);
int FN_TB_NEWMENU_Clicked(void);
int FN_TB_NEWPOINTER_Clicked(void);
int FN_TB_NEWSTRING_Clicked(void);
int FN_TB_NEWVERSION_Clicked(void);
int FN_TB_NEWSCREEN_Clicked(void);
int FN_TB_ALIGNLEFT_Clicked(void);
int FN_TB_ALIGNHCENTER_Clicked(void);
int FN_TB_ALIGNRIGHT_Clicked(void);
int FN_TB_ALIGNTOP_Clicked(void);
int FN_TB_ALIGNVCENTER_Clicked(void);
int FN_TB_ALIGNBOTTOM_Clicked(void);
int FN_TB_SPREADHORIZ_Clicked(void);
int FN_TB_SPREADVERT_Clicked(void);
int FN_TB_CLONEWIDTH_Clicked(void);
int FN_TB_CLONEHEIGHT_Clicked(void);
int FN_TB_CLONEBOTH_Clicked(void);

int FN_TBG_NONE_Clicked(void);
int FN_TBG_TEXTFIELD_Clicked(void);
int FN_TBG_TEXT_Clicked(void);
int FN_TBG_TAPEDECK_Clicked(void);
int FN_TBG_STRING_Clicked(void);
int FN_TBG_SLIDER_Clicked(void);
int FN_TBG_SCROLLER_Clicked(void);
int FN_TBG_PROGRESS_Clicked(void);
int FN_TBG_POPUPMENU_Clicked(void);
int FN_TBG_PALETTE_Clicked(void);
int FN_TBG_PAGE_Clicked(void);
int FN_TBG_NUMBER_Clicked(void);
int FN_TBG_MX_Clicked(void);
int FN_TBG_LISTVIEW_Clicked(void);
int FN_TBG_INTEGER_Clicked(void);
int FN_TBG_GRADIENTSLIDER_Clicked(void);
int FN_TBG_DATATYPE_Clicked(void);
int FN_TBG_CYCLE_Clicked(void);
int FN_TBG_COLORWHEEL_Clicked(void);
int FN_TBG_CHECKBOX_Clicked(void);
int FN_TBG_BUTTON_Clicked(void);
int FN_TBG_MODE_TEST_Clicked(void);
int FN_TBG_MODE_GADGET_Clicked(void);
int FN_TBG_MODE_FRAME_Clicked(void);
int FN_TBG_TREEVIEW_Clicked(void);

int FN_PGMN_LABEL_Clicked(void);
int FN_PGMN_NEWLOOK_Clicked(void);


int FN_PGFR_X_Clicked(void);
int FN_PGFR_Y_Clicked(void);
int FN_PGFR_WIDTH_Clicked(void);
int FN_PGFR_HEIGHT_Clicked(void);
int FN_PGFR_FRAME_Clicked(void);
int FN_PGFR_RECESSED_Clicked(void);




int FN_LIST_Clicked(void);

int FN_PGMN_ITEM_LABEL_Clicked(void);
int FN_PGMN_ITEM_TITLE_Clicked(void);
int FN_PGMN_ITEM_COMMKEY_Clicked(void);
int FN_PGMN_ITEM_POPUP_Clicked(void);
int FN_PGMN_ITEM_BARLABEL_Clicked(void);
int FN_PGMN_ITEM_DISABLED_Clicked(void);
int FN_PGMN_ITEM_CHECKED_Clicked(void);
int FN_PGMN_ITEM_TOGGLE_Clicked(void);

int FN_PGPI_LABEL_Clicked(void);
int FN_PGPI_WIDTH_Clicked(void);
int FN_PGPI_HEIGHT_Clicked(void);
int FN_PGPI_DEPTH_Clicked(void);
int FN_PGPI_TYPE_Clicked(void);
int FN_PGPI_SETPOINT_Clicked(void);
int FN_PGPI_PALETTE_Clicked(void);

int FN_TBP_PALETTE_Clicked(void);

int FN_SCREEN_PAGES_Clicked(void);

int FN_PGSCR_WIDTH_Clicked(void);
int FN_PGSCR_HEIGHT_Clicked(void);
int FN_PGSCR_DEPTH_Clicked(void);
int FN_PGSCR_TITLE_Clicked(void);
int FN_PGSCR_PUBNAME_Clicked(void);
int FN_PGSCR_USE_Clicked(void);
int FN_PGSCR_MAKEPUBLIC_Clicked(void);
int FN_PGSCR_MODEID_GET_Clicked(void);
int FN_PGSCR_DEFAULTID_Clicked(void);

int FN_PGSCR_SHOWTITLE_Clicked(void);
int FN_PGSCR_BEHIND_Clicked(void);
int FN_PGSCR_QUIET_Clicked(void);
int FN_PGSCR_AUTOSCROLL_Clicked(void);
int FN_PGSCR_FULLPALETTE_Clicked(void);
int FN_PGSCR_DRAGGABLE_Clicked(void);
int FN_PGSCR_EXCLUSIVE_Clicked(void);
int FN_PGSCR_SHAREPENS_Clicked(void);
int FN_PGSCR_INTERLEAVED_Clicked(void);
int FN_PGSCR_LIKEWORKBENCH_Clicked(void);
int FN_PGSCR_MINIMIZEISG_Clicked(void);

int FN_PGSTR_STRINGS_Clicked(void);
int FN_PGSTR_ADD_Clicked(void);
int FN_PGSTR_DELETE_Clicked(void);
int FN_PGSTR_EDIT_Clicked(void);

int FN_PGSTR_LABEL_Clicked(void);
int FN_PGSTR_STRING_Clicked(void);

int FN_PGVER_PROGRAMNAME_Clicked(void);
int FN_PGVER_AUTHOR_Clicked(void);
int FN_PGVER_VERSION_Clicked(void);
int FN_PGVER_REVISION_Clicked(void);

int FN_PG_TRLV_FLAGS_COLUMNS_Clicked(void);
int FN_PG_TRLV_FLAGS_FORMAT_Clicked(void);
int FN_PG_TRLV_FLAGS_DISABLED_Clicked(void);
int FN_PG_TRLV_FLAGS_MULTISELECT_Clicked(void);
int FN_PG_TRLV_FLAGS_BORDER_Clicked(void);
int FN_PG_TRLV_FLAGS_READONLY_Clicked(void);
int FN_PG_TRLV_FLAGS_SHOWTREELINES_Clicked(void);
int FN_PG_TRLV_FLAGS_SHOWSELECTED_Clicked(void);
int FN_PG_TRLV_FLAGS_TREE_Clicked(void);

int FN_OBJLIST_Clicked(void);

int FN_PAL_PALETTE_Clicked(void);
int FN_PAL_GREEN_Clicked(void);
int FN_PAL_BLUE_Clicked(void);
int FN_PAL_RED_Clicked(void);
int FN_PAL_CANCEL_Clicked(void);
int FN_PAL_OK_Clicked(void);
int FN_PAL_GSLIDER_Clicked(void);
int FN_PAL_COLORWHEEL_Clicked(void);

int FN_PGWN_TABORDER_DOWN_Clicked(void);
int FN_PGWN_TABORDER_UP_Clicked(void);
int FN_PGWN_TABORDER_LIST_Clicked(void);


int FN_MainProject_Selected(void);
int FN_MainProjectNew_Selected(void);
int FN_MainProjectOpen_Selected(void);
int FN_MainProjectSave_Selected(void);
int FN_MainProjectSaveAs_Selected(void);
int FN_MainProjectSaveSource_Selected(void);
int FN_MainProjectPreferences_Selected(void);
int FN_MainProjectAbout_Selected(void);
int FN_MainProjectQuit_Selected(void);
int FN_MainEdit_Selected(void);
int FN_MainEditUndo_Selected(void);
int FN_MainEditCut_Selected(void);
int FN_MainEditCopy_Selected(void);
int FN_MainEditPaste_Selected(void);
int FN_MainEditClear_Selected(void);
int FN_MainEditSelectAll_Selected(void);
int FN_MainEditGrid_Selected(void);
int FN_MainEditAlign_Selected(void);
int FN_MainEditAlignToLeft_Selected(void);
int FN_MainEditAlignToHCenter_Selected(void);
int FN_MainEditAlignToRight_Selected(void);
int FN_MainEditAlignToTop_Selected(void);
int FN_MainEditAlignToVCenter_Selected(void);
int FN_MainEditAlignToBottom_Selected(void);
int FN_MainEditClone_Selected(void);
int FN_MainEditCloneWidth_Selected(void);
int FN_MainEditCloneHeight_Selected(void);
int FN_MainEditCloneBoth_Selected(void);
int FN_MainEditSpread_Selected(void);
int FN_MainEditSpreadHoriz_Selected(void);
int FN_MainEditSpreadVertical_Selected(void);
int FN_MainObject_Selected(void);
int FN_MainObjWindow_Selected(void);
int FN_MainObjWindowNew_Selected(void);
int FN_MainObjWindowClose_Selected(void);
int FN_MainObjWindowDelete_Selected(void);
int FN_MainObjWindowRefresh_Selected(void);
int FN_MainObjMenu_Selected(void);
int FN_MainObjMenuNew_Selected(void);
int FN_MainObjMenuClose_Selected(void);
int FN_MainObjMenuDelete_Selected(void);
int FN_MainObjImage_Selected(void);
int FN_MainObjImageNew_Selected(void);
int FN_MainObjImageClose_Selected(void);
int FN_MainObjImageDelete_Selected(void);
int FN_MainObjStrings_Selected(void);
int FN_MainObjStringsOpen_Selected(void);
int FN_MainObjStringsClose_Selected(void);
int FN_MainObjScreen_Selected(void);
int FN_MainObjScreenOpen_Selected(void);
int FN_MainObjScreenClose_Selected(void);
int FN_MainObjVersion_Selected(void);
int FN_MainObjVersionOpen_Selected(void);
int FN_MainObjVersionClose_Selected(void);
int FN_MainWindow_Selected(void);
int FN_MainWindowProperties_Selected(void);
int FN_MainWindowObjects_Selected(void);
int FN_MainWindowStatus_Selected(void);
int FN_MainWindowHideAll_Selected(void);
int FN_MainWindowHideAll_Selected(void);

UWORD OpenWindow_PGBO_General(void);
void CloseWindow_PGBO_General(void);

int HandleIDCMP_About(void);
void Render_About(void);
int FN_CloseWindow_About(void);
UWORD OpenWindow_About(void);
void CloseWindow_About(void);

UWORD OpenWindow_PG_BUTT_Flags(void);
void CloseWindow_PG_BUTT_Flags(void);

UWORD OpenWindow_PG_CBOX_Flags(void);
void CloseWindow_PG_CBOX_Flags(void);

UWORD OpenWindow_PG_CWHL_Flags(void);
void CloseWindow_PG_CWHL_Flags(void);

UWORD OpenWindow_PG_CYCE_Flags(void);
void CloseWindow_PG_CYCE_Flags(void);

UWORD OpenWindow_PG_Lists(void);
void CloseWindow_PG_Lists(void);

UWORD OpenWindow_PG_DTYP_Flags(void);
void CloseWindow_PG_DTYP_Flags(void);

UWORD OpenWindow_PG_GRSL_Flags(void);
void CloseWindow_PG_GRSL_Flags(void);

UWORD OpenWindow_PG_INTR_Flags(void);
void CloseWindow_PG_INTR_Flags(void);

UWORD OpenWindow_PG_INTR_Var(void);
void CloseWindow_PG_INTR_Var(void);

UWORD OpenWindow_PG_LVIE_Flags(void);
void CloseWindow_PG_LVIE_Flags(void);

UWORD OpenWindow_PG_MX_Flags(void);
void CloseWindow_PG_MX_Flags(void);

UWORD OpenWindow_PG_NUMB_Flags(void);
void CloseWindow_PG_NUMB_Flags(void);

UWORD OpenWindow_PGGT_General(void);
void CloseWindow_PGGT_General(void);

UWORD OpenWindow_PG_PAGE_Flags(void);
void CloseWindow_PG_PAGE_Flags(void);

UWORD OpenWindow_PG_PAGE_Lists(void);
void CloseWindow_PG_PAGE_Lists(void);

UWORD OpenWindow_PG_PALT_Flags(void);
void CloseWindow_PG_PALT_Flags(void);

UWORD OpenWindow_PG_PUPM_Flags(void);
void CloseWindow_PG_PUPM_Flags(void);

UWORD OpenWindow_PG_PRGR_Flags(void);
void CloseWindow_PG_PRGR_Flags(void);

UWORD OpenWindow_PG_SCRL_Flags(void);
void CloseWindow_PG_SCRL_Flags(void);

UWORD OpenWindow_PG_SCRL_Var(void);
void CloseWindow_PG_SCRL_Var(void);

UWORD OpenWindow_PG_SLIR_Flags(void);
void CloseWindow_PG_SLIR_Flags(void);

UWORD OpenWindow_PG_SLIR_Var(void);
void CloseWindow_PG_SLIR_Var(void);

UWORD OpenWindow_PG_STRG_Flags(void);
void CloseWindow_PG_STRG_Flags(void);

UWORD OpenWindow_PG_STRG_Var(void);
void CloseWindow_PG_STRG_Var(void);

UWORD OpenWindow_PG_TDEK_Flags(void);
void CloseWindow_PG_TDEK_Flags(void);

UWORD OpenWindow_PG_TEXT_Flags(void);
void CloseWindow_PG_TEXT_Flags(void);

UWORD OpenWindow_PG_TFLD_Flags(void);
void CloseWindow_PG_TFLD_Flags(void);

UWORD OpenWindow_PG_TFLD_Flags2(void);
void CloseWindow_PG_TFLD_Flags2(void);

UWORD OpenWindow_PG_TFLD_Var(void);
void CloseWindow_PG_TFLD_Var(void);

int HandleIDCMP_Properties(void);
void Render_Properties(void);
int FN_MouseMove_Properties(void);
int FN_IntuiTicks_Properties(void);
int FN_ChangeWindow_Properties(void);
int FN_CloseWindow_Properties(void);
int FN_InActiveWindow_Properties(void);
UWORD OpenWindow_Properties(void);
void CloseWindow_Properties(void);

int HandleIDCMP_Edit(void);
void Render_Edit(void);
int FN_MenuVerify_Edit(void);
int FN_NewSize_Edit(void);
int FN_MouseMove_Edit(void);
int FN_MouseButtons_Edit(void);
int FN_RawKey_Edit(void);
int FN_ChangeWindow_Edit(void);
int FN_CloseWindow_Edit(void);
UWORD OpenWindow_Edit(void);
void CloseWindow_Edit(void);

int HandleIDCMP_Preferences(void);
void Render_Preferences(void);
int FN_CloseWindow_Preferences(void);
UWORD OpenWindow_Preferences(void);
void CloseWindow_Preferences(void);

int HandleIDCMP_Grid(void);
void Render_Grid(void);
int FN_CloseWindow_Grid(void);
UWORD OpenWindow_Grid(void);
void CloseWindow_Grid(void);

UWORD OpenWindow_PGWN_General(void);
void CloseWindow_PGWN_General(void);

UWORD OpenWindow_PGWN_Flags1(void);
void CloseWindow_PGWN_Flags1(void);

UWORD OpenWindow_PGWN_Style(void);
void CloseWindow_PGWN_Style(void);

UWORD OpenWindow_PGWN_Flags2(void);
void CloseWindow_PGWN_Flags2(void);

UWORD OpenWindow_PGWN_Flags3(void);
void CloseWindow_PGWN_Flags3(void);

int HandleIDCMP_WN_IDCMPFlags(void);
void Render_WN_IDCMPFlags(void);
int FN_CloseWindow_WN_IDCMPFlags(void);
UWORD OpenWindow_WN_IDCMPFlags(void);
void CloseWindow_WN_IDCMPFlags(void);

int HandleIDCMP_TB_Main(void);
void Render_TB_Main(void);
int FN_ChangeWindow_TB_Main(void);
int FN_CloseWindow_TB_Main(void);
UWORD OpenWindow_TB_Main(void);
void CloseWindow_TB_Main(void);

int HandleIDCMP_TB_Window(void);
void Render_TB_Window(void);
int FN_ChangeWindow_TB_Window(void);
int FN_CloseWindow_TB_Window(void);
UWORD OpenWindow_TB_Window(void);
void CloseWindow_TB_Window(void);

UWORD OpenWindow_PGMN_General(void);
void CloseWindow_PGMN_General(void);

int HandleIDCMP_Working(void);
void Render_Working(void);
int FN_CloseWindow_Working(void);
UWORD OpenWindow_Working(void);
void CloseWindow_Working(void);

UWORD OpenWindow_PGFR_General(void);
void CloseWindow_PGFR_General(void);

UWORD OpenWindow_PGEMPTY(void);
void CloseWindow_PGEMPTY(void);

int HandleIDCMP_EditMenu(void);
void Render_EditMenu(void);
int FN_MenuVerify_EditMenu(void);
int FN_NewSize_EditMenu(void);
int FN_MouseMove_EditMenu(void);
int FN_MouseButtons_EditMenu(void);
int FN_CloseWindow_EditMenu(void);
int FN_ActiveWindow_EditMenu(void);
UWORD OpenWindow_EditMenu(void);
void CloseWindow_EditMenu(void);

int HandleIDCMP_EditImage(void);
void Render_EditImage(void);
int FN_MenuVerify_EditImage(void);
int FN_NewSize_EditImage(void);
int FN_MouseButtons_EditImage(void);
int FN_IDCMPUpdate_EditImage(void);
int FN_CloseWindow_EditImage(void);
int FN_ActiveWindow_EditImage(void);
UWORD OpenWindow_EditImage(void);
void CloseWindow_EditImage(void);

int HandleIDCMP_List(void);
void Render_List(void);
int FN_CloseWindow_List(void);
UWORD OpenWindow_List(void);
void CloseWindow_List(void);

UWORD OpenWindow_PGMN_Item(void);
void CloseWindow_PGMN_Item(void);

UWORD OpenWindow_PGPI_General(void);
void CloseWindow_PGPI_General(void);

int HandleIDCMP_TB_Pointer(void);
void Render_TB_Pointer(void);
int FN_ChangeWindow_TB_Pointer(void);
int FN_CloseWindow_TB_Pointer(void);
UWORD OpenWindow_TB_Pointer(void);
void CloseWindow_TB_Pointer(void);

int HandleIDCMP_Screen(void);
void Render_Screen(void);
int FN_CloseWindow_Screen(void);
int FN_ActiveWindow_Screen(void);
UWORD OpenWindow_Screen(void);
void CloseWindow_Screen(void);

UWORD OpenWindow_PGSCR_General(void);
void CloseWindow_PGSCR_General(void);

UWORD OpenWindow_PGSCR_Flags(void);
void CloseWindow_PGSCR_Flags(void);

int HandleIDCMP_EditString(void);
void Render_EditString(void);
int FN_MouseButtons_EditString(void);
int FN_IDCMPUpdate_EditString(void);
int FN_ChangeWindow_EditString(void);
int FN_CloseWindow_EditString(void);
int FN_ActiveWindow_EditString(void);
UWORD OpenWindow_EditString(void);
void CloseWindow_EditString(void);

UWORD OpenWindow_PGSTR_General(void);
void CloseWindow_PGSTR_General(void);

int HandleIDCMP_EditVersion(void);
void Render_EditVersion(void);
int FN_CloseWindow_EditVersion(void);
int FN_ActiveWindow_EditVersion(void);
UWORD OpenWindow_EditVersion(void);
void CloseWindow_EditVersion(void);

UWORD OpenWindow_PG_TRLV_Flags(void);
void CloseWindow_PG_TRLV_Flags(void);

int HandleIDCMP_Object(void);
void Render_Object(void);
int FN_NewSize_Object(void);
int FN_MouseMove_Object(void);
int FN_RawKey_Object(void);
int FN_IDCMPUpdate_Object(void);
int FN_ChangeWindow_Object(void);
int FN_CloseWindow_Object(void);
UWORD OpenWindow_Object(void);
void CloseWindow_Object(void);

int HandleIDCMP_EditPalette(void);
void Render_EditPalette(void);
int FN_IDCMPUpdate_EditPalette(void);
UWORD OpenWindow_EditPalette(void);
void CloseWindow_EditPalette(void);

UWORD OpenWindow_PGWN_TabOrder(void);
void CloseWindow_PGWN_TabOrder(void);

int HandleIDCMP_Status(void);
void Render_Status(void);
int FN_ChangeWindow_Status(void);
UWORD OpenWindow_Status(void);
void CloseWindow_Status(void);

UWORD SetupScreen(UBYTE);
void CloseDownScreen(void);
UWORD InitApplication(STRPTR);
void FreeApplication(void);

void CloseWindowSafely(struct Window *);
void StripIntuiMessages(struct MsgPort *,struct Window *);
struct Gadget *CreateGadgetExA(UWORD,UWORD,UWORD,UWORD,struct Gadget *,struct TagItem *);
void DeleteGadgetExA(struct Gadget *,struct TagItem *);
struct Window *OpenWindowTagsEx(ULONG, ...);
void CloseWindowEx(struct Window *);
void CalcLeftTop(UWORD *,UWORD *,struct TagItem *);
void CalcRightBottom(UWORD *,UWORD *,struct TagItem *);
UWORD AdaptX(UWORD);
UWORD AdaptY(UWORD);
void CalcAdaptivity(UWORD,UWORD);
STRPTR GetCatString(APTR);

#endif /* gadtools_H */
