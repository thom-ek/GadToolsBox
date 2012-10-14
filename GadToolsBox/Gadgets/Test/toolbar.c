#include<stdio.h>
#include<proto/dos.h>
#include<proto/exec.h>
#include<proto/graphics.h>
#include<proto/gadtools.h>
#include<proto/intuition.h>
#include<proto/utility.h>
#include<intuition/imageclass.h>
#include<intuition/gadgetclass.h>
#include<libraries/gadtools.h>
#include<libraries/amigaguide.h>
#include<gadgets/toolbar.h>
#include"toolbar/toolbar_pragmas.h"

void __saveds __asm Toolbar_GetDimensions(register __a0 struct List *,register __a1 struct TagItem *);
Class * __saveds __asm CreateToolbarClass(void);
BOOL __saveds __asm  DisposeToolbarClass( register __a0 Class *);

void Toolbar_GetDimensionsTags(struct List *, ULONG, ...);

#define unless(x) if(!(x))
#define LEAVE goto end

struct Library *ToolbarBase=NULL;
struct Window *MainWnd=NULL;
Object *ToolGad=NULL;
Class *ToolbarClass=NULL;
struct Screen *Scr=NULL;

extern struct MinList TBARList;
extern struct TBARPPList TBARPPList;

struct TBARNode TBARNodes[] =
{
	{{(struct MinNode *) &TBARNodes[1],      (struct MinNode *) &TBARList.mlh_Head}, TID_NEW,  TTYPE_Normal,  IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[2],      (struct MinNode *) &TBARNodes[0]},      TID_OPEN, TTYPE_Normal,  IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[3],      (struct MinNode *) &TBARNodes[1]},      TID_SAVE, TTYPE_Normal,  IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[4],      (struct MinNode *) &TBARNodes[2]},      TID_PRINT, TTYPE_Normal, IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[5],      (struct MinNode *) &TBARNodes[3]},      TID_CUT, TTYPE_Normal,   IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[6],      (struct MinNode *) &TBARNodes[4]},      TID_COPY, TTYPE_Normal,  IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[7],      (struct MinNode *) &TBARNodes[5]},      TID_PASTE, TTYPE_Normal, IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[8],      (struct MinNode *) &TBARNodes[6]},      TID_UNDO, TTYPE_Normal,  IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[9],      (struct MinNode *) &TBARNodes[7]},      TID_BOLD, TTYPE_Toggle,  IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[10],     (struct MinNode *) &TBARNodes[8]},      TID_ITALIC, TTYPE_Toggle,IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[11],     (struct MinNode *) &TBARNodes[9]},      TID_UNDERLINE, TTYPE_Toggle, IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[12],     (struct MinNode *) &TBARNodes[10]},     2, TTYPE_PopUp,          IDS_NORMAL, (UBYTE *)&TBARPPList},
	{{(struct MinNode *) &TBARNodes[13],     (struct MinNode *) &TBARNodes[11]},     1, TTYPE_TToggle,        IDS_NORMAL, "Tekôcik"},
	{{(struct MinNode *) &TBARNodes[14],     (struct MinNode *) &TBARNodes[12]},     TID_FIND, TTYPE_Normal,  IDS_NORMAL, NULL},
	{{(struct MinNode *) &TBARNodes[15],     (struct MinNode *) &TBARNodes[13]},     0, TTYPE_Space,          0,          NULL},
	{{(struct MinNode *) &TBARList.mlh_Tail, (struct MinNode *) &TBARNodes[14]},     TID_HELP, TTYPE_Normal,  IDS_NORMAL, NULL}
};

struct MinList TBARList = {
	( struct MinNode * )&TBARNodes[0], ( struct MinNode * )NULL, ( struct MinNode * )&TBARNodes[15]
};

struct TBARPPNode TBARPPNodes[] =
{
	{{(struct MinNode *) &TBARPPNodes[1],      (struct MinNode *) &TBARPPList.tpl_List.mlh_Head}, "Linia 01"},
	{{(struct MinNode *) &TBARPPNodes[2],      (struct MinNode *) &TBARPPNodes[0]},      "Linia 02"},
	{{(struct MinNode *) &TBARPPNodes[3],      (struct MinNode *) &TBARPPNodes[1]},      "Linia 03"},
	{{(struct MinNode *) &TBARPPNodes[4],      (struct MinNode *) &TBARPPNodes[2]},      "Linia 04"},
	{{(struct MinNode *) &TBARPPNodes[5],      (struct MinNode *) &TBARPPNodes[3]},      "Linia 05"},
	{{(struct MinNode *) &TBARPPNodes[6],      (struct MinNode *) &TBARPPNodes[4]},      "Linia 06"},
	{{(struct MinNode *) &TBARPPNodes[7],      (struct MinNode *) &TBARPPNodes[5]},      "Linia 07"},
	{{(struct MinNode *) &TBARPPNodes[8],      (struct MinNode *) &TBARPPNodes[6]},      "Linia 08"},
	{{(struct MinNode *) &TBARPPNodes[9],      (struct MinNode *) &TBARPPNodes[7]},      "Linia 09"},
	{{(struct MinNode *) &TBARPPNodes[10],      (struct MinNode *) &TBARPPNodes[8]},      "Linia 10"},
	{{(struct MinNode *) &TBARPPNodes[11],      (struct MinNode *) &TBARPPNodes[9]},      "Linia 11"},
	{{(struct MinNode *) &TBARPPNodes[12],      (struct MinNode *) &TBARPPNodes[10]},      "Linia 12"},
	{{(struct MinNode *) &TBARPPNodes[13],      (struct MinNode *) &TBARPPNodes[11]},      "Linia 13"},
	{{(struct MinNode *) &TBARPPNodes[14],      (struct MinNode *) &TBARPPNodes[12]},      "Linia 14"},
	{{(struct MinNode *) &TBARPPNodes[15],      (struct MinNode *) &TBARPPNodes[13]},      "Linia 15"},
	{{(struct MinNode *) &TBARPPNodes[16],      (struct MinNode *) &TBARPPNodes[14]},      "Linia 16"},
	{{(struct MinNode *) &TBARPPNodes[17],      (struct MinNode *) &TBARPPNodes[15]},      "Linia 17"},
	{{(struct MinNode *) &TBARPPNodes[18],      (struct MinNode *) &TBARPPNodes[16]},      "Linia 18"},
	{{(struct MinNode *) &TBARPPNodes[19],      (struct MinNode *) &TBARPPNodes[17]},      "Linia 19"},
	{{(struct MinNode *) &TBARPPNodes[20],      (struct MinNode *) &TBARPPNodes[18]},      "Linia 20"},
	{{(struct MinNode *) &TBARPPNodes[21],      (struct MinNode *) &TBARPPNodes[19]},      "Linia 21"},
	{{(struct MinNode *) &TBARPPNodes[22],      (struct MinNode *) &TBARPPNodes[20]},      "Linia 22"},
	{{(struct MinNode *) &TBARPPNodes[23],      (struct MinNode *) &TBARPPNodes[21]},      "Linia 23"},
	{{(struct MinNode *) &TBARPPNodes[24],      (struct MinNode *) &TBARPPNodes[22]},      "Linia 24"},
	{{(struct MinNode *) &TBARPPNodes[25],      (struct MinNode *) &TBARPPNodes[23]},      "Linia 25"},
	{{(struct MinNode *) &TBARPPNodes[26],      (struct MinNode *) &TBARPPNodes[24]},      "Linia 26"},
	{{(struct MinNode *) &TBARPPNodes[27],      (struct MinNode *) &TBARPPNodes[25]},      "Linia 27"},
	{{(struct MinNode *) &TBARPPNodes[28],      (struct MinNode *) &TBARPPNodes[26]},      "Linia 28"},
	{{(struct MinNode *) &TBARPPNodes[29],      (struct MinNode *) &TBARPPNodes[27]},      "Linia 29"},
	{{(struct MinNode *) &TBARPPNodes[30],      (struct MinNode *) &TBARPPNodes[28]},      "Linia 30"},
	{{(struct MinNode *) &TBARPPList.tpl_List.mlh_Tail, (struct MinNode *) &TBARPPNodes[29]},      "Linia 31"},
};

struct TBARPPList TBARPPList = {
	( struct MinNode * )&TBARPPNodes[0], ( struct MinNode * )NULL, ( struct MinNode * )&TBARPPNodes[30],100,TTYPEPP_String,0,0
};

void main()
{
	ULONG Width=10,Height=10;
	ULONG class,code;
	struct Gadget *gadget;
	struct IntuiMessage *message;
	struct DrawInfo *dri;
	BOOL running=TRUE;
	LONG sysgtype,qhelp,region,id,state;

	unless(Scr=LockPubScreen(NULL)) LEAVE;
	unless(dri=GetScreenDrawInfo(Scr)) LEAVE;
	unless(ToolbarBase=OpenLibrary("toolbar.gadget",36L)) LEAVE;
	unless(ToolbarClass=CreateToolbarClass()) LEAVE;
	Toolbar_GetDimensionsTags((struct List *)&TBARList,
		GA_Width,&Width,
		GA_Height,&Height,
		TBAR_Position,TPOS_HORIZ,
		TBAR_Border,TRUE,
		TBAR_Offset,TRUE,
		TBAR_Font,dri->dri_Font,
		TAG_DONE);
	unless(ToolGad=NewObject(ToolbarClass,NULL,
		GA_Width,Width,
		GA_Height,Height,
		GA_GadgetHelp,TRUE,
		GA_RelVerify,TRUE,
		TBAR_Screen,Scr,
		TBAR_Gadgets,&TBARList,
		TBAR_Position,TPOS_HORIZ,
		TBAR_Border,TRUE,
		TBAR_Offset,TRUE,
		TAG_DONE)) LEAVE;
	unless(MainWnd=OpenWindowTags(NULL,
		WA_Left,0,
		WA_Top,0,
		WA_Width,600,
		WA_Height,200,
		WA_IDCMP,IDCMP_CLOSEWINDOW|IDCMP_GADGETHELP|IDCMP_MOUSEMOVE|IDCMP_GADGETUP,
		WA_Flags,WFLG_CLOSEGADGET|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_ACTIVATE|WFLG_SIZEGADGET,
		WA_CustomScreen,Scr,
		TAG_DONE)) LEAVE;
	HelpControl(MainWnd, HC_GADGETHELP);
	SetAttrs(ToolGad,
		GA_Left,MainWnd->BorderLeft,
		GA_Top,MainWnd->BorderTop,
//		GA_LeftBorder,TRUE,
//		GA_TopBorder,TRUE,
		TAG_DONE);
	AddGadget(MainWnd,(struct Gadget *)ToolGad,-1);
	RefreshGList((struct Gadget *)ToolGad,MainWnd,NULL,-1);

	while(running)
	{
		Wait(1L<<MainWnd->UserPort->mp_SigBit);
		while(message=GT_GetIMsg(MainWnd->UserPort))
		{
			class=message->Class;
			code=message->Code;
			gadget=(struct Gadget *)message->IAddress;
			GT_ReplyIMsg(message);
			switch(class)
			{
				case IDCMP_CLOSEWINDOW:
					running=FALSE;
					break;
				case IDCMP_GADGETUP:
					GetAttr(TBAR_ID,ToolGad,(ULONG *)&id);
					GetAttr(TBAR_State,ToolGad,(ULONG *)&state);
					printf("GadgetUp: %d, SubID: %d, State: %d\n",gadget->GadgetID,id,state);
					break;
				case IDCMP_GADGETHELP:
					qhelp=region=-1;
					if(gadget==NULL)
					{
						printf("Not over our window\n");
					}
					else
					{
						if(gadget==(struct Gadget *)MainWnd)
						{
							printf("Over window\n");
							qhelp=region=0;
						}
						else
						{
							sysgtype = gadget->GadgetType & 0xF0;
							qhelp=(sysgtype>>4)+5;
							region=HTFC_SYSGADS+sysgtype;
							switch(sysgtype)
							{
								case GTYP_WDRAGGING: printf("GadgetHelp: DragBar\n"); break;
								case GTYP_WUPFRONT: printf("GadgetHelp: Depth\n"); break;
								case GTYP_CLOSE: printf("GadgetHelp: Close\n"); break;
								case GTYP_SIZING: printf("GadgetHelp: Size\n"); break;
								case GTYP_WZOOM: printf("GadgetHelp: Zoom\n"); break;
								default: printf("GadgetHelp: Other\n"); break;
								case 0:
									printf("GadgetHelp: %d, SubID: %d\n",gadget->GadgetID,code);
									qhelp=region=gadget->GadgetID;
									break;
							}
						}
					}
					break;
			}
		}
	}
end:
	if(MainWnd)
	{
		RemoveGadget(MainWnd,(struct Gadget *)ToolGad);
		CloseWindow(MainWnd);
	}
	if(ToolGad) DisposeObject(ToolGad);
	if(ToolbarClass) DisposeToolbarClass(ToolbarClass);
	if(ToolbarBase) CloseLibrary(ToolbarBase);
	if(dri) FreeScreenDrawInfo(Scr,dri);
	if(Scr) UnlockPubScreen(NULL,Scr);
}

void Toolbar_GetDimensionsTags(struct List *list, ULONG tag, ...)
{
	Toolbar_GetDimensions(list,(struct TagItem *)&tag);
}
