#include<stdio.h>
#include<string.h>
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
#include<proto/listview.h>
#include<exec/memory.h>

#define unless(x) if(!(x))
#define LEAVE goto end

struct Library *ListViewBase=NULL;
struct Window *MainWnd=NULL;
Object *LVGad=NULL;
Class *LVClass=NULL;
struct Screen *Scr=NULL;

struct LVList list;
struct LVNode node[]=
{
	NULL,NULL,NULL,"Ble1|kupa",NULL,NULL,0,
	NULL,NULL,0,"SubThere1|blada",NULL,NULL,0,
	NULL,NULL,NULL,"sub1|is",NULL,NULL,0,
	NULL,NULL,NULL,"sub2|there",NULL,NULL,0,
	NULL,NULL,NULL,"sub3|for",NULL,NULL,0,
	NULL,NULL,NULL,"Ble3|all",NULL,NULL,0,
	NULL,NULL,0,"subthere4|the",NULL,NULL,0,
	NULL,NULL,LV_OPEN,"sub5|years",NULL,NULL,0,
	NULL,NULL,NULL,"sub6|forever",NULL,NULL,0,
	NULL,NULL,NULL,"Ble7|and",NULL,NULL,0,
	NULL,NULL,NULL,"Ble8|always",NULL,NULL,0,
	NULL,NULL,NULL,"Ble9|will",NULL,NULL,0,
	NULL,NULL,NULL,"Ble10|be",NULL,NULL,0,
	NULL,NULL,NULL,"Ble11|with",NULL,NULL,0,
	NULL,NULL,NULL,"Ble12|us",NULL,NULL,0,
	NULL,NULL,NULL,"Ble13|isn't",NULL,NULL,0,
	NULL,NULL,NULL,"Ble14|it",NULL,NULL,0,
	NULL,NULL,NULL,"Ble15|?",NULL,NULL,0
};

ULONG __saveds __asm showsub_func(register __a0 struct Hook *hook,register __a2 Object *object,register __a1 APTR msg)
{
	struct TLVSubMsg *tlvsm=msg;
	struct TLVEditMsg *tlvem=msg;

	switch(tlvsm->tlvm_MethodID)
	{
		case TLV_OPENSUB:
			DisplayBeep(Scr);
			return TLVCB_OK;
		case TLV_CLOSESUB:
			return TLVCB_OK;
		case TLV_DOEDIT:
			if(tlvem->tlvm_Column!=0) return TLVCB_OK;
			break;
		case TLV_EDIT:
			FreeVec(tlvem->tlvm_Node->ln_Name);
			if(tlvem->tlvm_Node->ln_Name=AllocVec(strlen(tlvem->tlvm_String)+4,MEMF_ANY))
				sprintf(tlvem->tlvm_Node->ln_Name,"%s|or",tlvem->tlvm_String);
			return TLVCB_OK;
	}
	return TLVCB_UNKNOWN;
}

struct Hook showsub_hook={NULL,NULL,(HOOKFUNC)showsub_func,NULL,NULL};

void main()
{
	ULONG class,code,Top=0;
	struct Gadget *gadget;
	struct IntuiMessage *message;
	struct DrawInfo *dri;
	BOOL running=TRUE;
	APTR VI=NULL;

	unless(ListViewBase=OpenLibrary("gadgets/listview.gadget",36L)) LEAVE;
	unless(Scr=LockPubScreen("SASPRJ.1")) LEAVE;
	unless(dri=GetScreenDrawInfo(Scr)) LEAVE;
	unless(LVClass=GetListViewClass()) LEAVE;
	unless(VI=GetVisualInfo(Scr,TAG_DONE)) LEAVE;

	node[0].ln_Name=AllocVec(11,MEMF_ANY);
	strcpy(node[0].ln_Name,"Edit me|or");
	Tree_NewList(&list);
	Tree_AddTail(&list,&node[0]);
	Tree_AddTail(&list,&node[1]);
	Tree_AddSubTail(&list,&node[1],&node[2]);
	Tree_AddSubTail(&list,&node[1],&node[3]);
	Tree_AddSubTail(&list,&node[1],&node[4]);
	Tree_AddTail(&list,&node[5]);
	Tree_AddTail(&list,&node[6]);
	Tree_AddSubTail(&list,&node[6],&node[7]);
	Tree_AddSubTail(&list,&node[7],&node[8]);
	Tree_AddSubTail(&list,&node[6],&node[9]);
	Tree_AddSubTail(&list,&node[6],&node[10]);
	Tree_AddTail(&list,&node[11]);
	Tree_AddTail(&list,&node[12]);
	Tree_AddTail(&list,&node[13]);
	Tree_AddTail(&list,&node[14]);
	Tree_AddTail(&list,&node[15]);
	Tree_AddTail(&list,&node[16]);
	Tree_AddTail(&list,&node[17]);

	unless(MainWnd=OpenWindowTags(NULL,
		WA_Left,0,
		WA_Top,0,
		WA_Width,600,
		WA_Height,300,
		WA_IDCMP,IDCMP_CLOSEWINDOW|IDCMP_GADGETUP|IDCMP_MOUSEMOVE|IDCMP_GADGETDOWN|IDCMP_MOUSEBUTTONS,
		WA_Flags,WFLG_CLOSEGADGET|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_ACTIVATE|WFLG_SIZEGADGET,
		WA_CustomScreen,Scr,
		WA_RMBTrap,TRUE,
//		WA_Gadgets,LVGad,
		TAG_DONE)) LEAVE;
	unless(LVGad=NewObject(LVClass,NULL,
		GA_Left,25,
		GA_Top,25,
		GA_Width,200,
		GA_Height,200,
		LIST_Border,TRUE,
		LIST_Labels,&list,
		LIST_ShowTreeLines,TRUE,
		LIST_IsTree,TRUE,
		LIST_Columns,2,
		LIST_Format,"2bJeden|2brTekscior",
		LIST_TabChar,"|",
		LIST_HookSub,&showsub_hook,
		LIST_HookEdit,&showsub_hook,
		TAG_DONE)) LEAVE;

	AddGList(MainWnd,LVGad,-1,-1,NULL);
	RefreshGList(LVGad,MainWnd,NULL,-1);
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
				case IDCMP_MOUSEBUTTONS:
					switch(code)
					{
						case SELECTUP:
							Top++;
							SetGadgetAttrs((struct Gadget *)LVGad,MainWnd,NULL,LIST_Top,Top,TAG_DONE);
							break;
						case MENUUP:
							if(Top>0) Top--;
							SetGadgetAttrs((struct Gadget *)LVGad,MainWnd,NULL,LIST_Top,Top,TAG_DONE);
							break;
					}
					break;
			}
		}
	}
end:
	RemoveGList(MainWnd,LVGad,-1);
	if(MainWnd)
	{
		CloseWindow(MainWnd);
	}
	if(VI) FreeVisualInfo(VI);
	if(LVGad) DisposeObject(LVGad);
	if(ListViewBase) CloseLibrary(ListViewBase);
	if(dri) FreeScreenDrawInfo(Scr,dri);
	if(Scr) UnlockPubScreen(NULL,Scr);
	FreeVec(node[0].ln_Name);
}
