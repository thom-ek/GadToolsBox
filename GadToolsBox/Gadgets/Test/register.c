#include<stdio.h>
#include<proto/dos.h>
#include<proto/exec.h>
#include<proto/graphics.h>
#include<proto/gadtools.h>
#include<proto/intuition.h>
#include<proto/utility.h>
#include<proto/colorwheel.h>
#include<exec/memory.h>
#include<intuition/imageclass.h>
#include<intuition/gadgetclass.h>
#include<libraries/gadtools.h>
#include<libraries/amigaguide.h>
#include<gadgets/register.h>
#include"register_pragmas.h"

Class * __saveds __asm CreateRegisterClass(void);
BOOL __saveds __asm  DisposeRegisterClass( register __a0 Class *);

#define unless(x) if(!(x))
#define LEAVE goto end

struct Library *RegisterBase=NULL,*ColorWheelBase=NULL;
struct Window *MainWnd=NULL;
Object *RegiGad=NULL,*Gad1=NULL,*Gad2=NULL,*ble=NULL;
Class *RegisterClass=NULL;
struct Screen *Scr=NULL;
struct PageNode Page1,Page2;
struct MinList PageList;

void main()
{
	ULONG class,code;
	struct Gadget *gadget;
	struct IntuiMessage *message;
	struct DrawInfo *dri;
	BOOL running=TRUE;
	LONG sysgtype,qhelp,region,page;

	NewList((struct List *)&PageList);

	unless(ColorWheelBase=OpenLibrary("gadgets/colorwheel.gadget",39)) LEAVE;
	unless(Scr=LockPubScreen(NULL)) LEAVE;
	unless(dri=GetScreenDrawInfo(Scr)) LEAVE;
	unless(RegisterBase=OpenLibrary("register.gadget",37L)) LEAVE;
	unless(RegisterClass=CreateRegisterClass()) LEAVE;

	unless(Gad1=Page1.lnp_Gadgets=NewObject(NULL,"colorwheel.gadget",
		GA_Left,30,
		GA_Top,70,
		GA_Width,100,
		GA_Height,50,
		GA_ID,6,
		GA_GadgetHelp,TRUE,
		GA_FollowMouse,TRUE,
		WHEEL_Screen,Scr,
		WHEEL_BevelBox,TRUE,
		WHEEL_MaxPens,64,
		TAG_DONE)) LEAVE;

	unless(ble=NewObject(NULL,FRBUTTONCLASS,
		GA_Left,190,
		GA_Top,70,
		GA_Width,100,
		GA_Height,11,
		GA_Text,"Gadget 1",
		GA_DrawInfo,dri,
		GA_ID,1,
		GA_GadgetHelp,TRUE,
		GA_RelVerify,TRUE,
		TAG_DONE)) LEAVE;
	unless(Gad2=Page2.lnp_Gadgets=NewObject(NULL,FRBUTTONCLASS,
		GA_Left,30,
		GA_Top,30,
		GA_Width,100,
		GA_Height,11,
		GA_Text,"Gadget 2",
		GA_DrawInfo,dri,
		GA_ID,2,
		GA_GadgetHelp,TRUE,
		GA_RelVerify,TRUE,
		TAG_DONE)) LEAVE;

	Page1.lnp_Name="Strona 1";
	Page2.lnp_Name="Strona 2";

	AddTail((struct List *)&PageList,(struct Node *)&Page1);
	AddTail((struct List *)&PageList,(struct Node *)&Page2);

	unless(RegiGad=NewObject(RegisterClass,NULL,
		GA_Width,170,
		GA_Height,150,
		GA_GadgetHelp,FALSE,
		GA_RelVerify,TRUE,
		GA_ID,0,
		REGI_Border,TRUE,
		REGI_Title,TRUE,
		REGI_Pages,&PageList,
		REGI_Page,0,
		TAG_DONE)) LEAVE;
	unless(MainWnd=OpenWindowTags(NULL,
		WA_Left,0,
		WA_Top,0,
		WA_InnerWidth,400,//170,
		WA_InnerHeight,150,
		WA_IDCMP,IDCMP_CLOSEWINDOW|IDCMP_GADGETHELP|IDCMP_MOUSEMOVE|IDCMP_GADGETUP,
		WA_Flags,WFLG_CLOSEGADGET|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_ACTIVATE,
		WA_CustomScreen,Scr,
		TAG_DONE)) LEAVE;
	SetAttrs(RegiGad,GA_Left,MainWnd->BorderLeft,GA_Top,MainWnd->BorderTop,TAG_DONE);
	HelpControl(MainWnd, HC_GADGETHELP);
	AddGadget(MainWnd,(struct Gadget *)RegiGad,-1);
	AddGadget(MainWnd,(struct Gadget *)ble,-1);
	RefreshGList((struct Gadget *)RegiGad,MainWnd,NULL,-1);

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
			}
		}
	}
	SetGadgetAttrs((struct Gadget *)RegiGad,MainWnd,NULL,
		REGI_Page,1,
		TAG_DONE);
	RefreshGList((struct Gadget *)RegiGad,MainWnd,NULL,-1);
	running=TRUE;
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
					switch(gadget->GadgetID)
					{
						case 0:
							GetAttr(REGI_Page,RegiGad,(ULONG *)&page);
							printf("GadgetUp: %d, Page: %d\n",gadget->GadgetID,page);
							break;
						default:
							printf("GadgetUp: %d, Code: %d\n",gadget->GadgetID,code);
							break;
					}
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
		RemoveGadget(MainWnd,(struct Gadget *)ble);
		RemoveGadget(MainWnd,(struct Gadget *)RegiGad);
		CloseWindow(MainWnd);
	}
	if(RegiGad) DisposeObject(RegiGad);
	if(Gad2) DisposeObject(Gad2);
	if(Gad1) DisposeObject(Gad1);
	if(ble) DisposeObject(ble);
	if(RegisterClass) DisposeRegisterClass(RegisterClass);
	if(RegisterBase) CloseLibrary(RegisterBase);
	if(dri) FreeScreenDrawInfo(Scr,dri);
	if(Scr) UnlockPubScreen(NULL,Scr);
	if(ColorWheelBase) CloseLibrary(ColorWheelBase);
}
