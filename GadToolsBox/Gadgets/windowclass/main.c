#include<stdio.h>
#include<proto/dos.h>
#include<proto/exec.h>
#include<proto/intuition.h>
#include<proto/commodities.h>
#include<proto/amigaguide.h>
#include<proto/gadtools.h>
#include<rexx/errors.h>
#include<intuition/icclass.h>
#include<intuition/gadgetclass.h>
#include"gadtoolsbox.h"

#define unless(x) if(!(x))
#define Leave(x) { if(x) printf("%s\n",x); goto end; }

#define APPNAME	"WndTest"
#define APPTITLE "WndTest v1.0"
#define APPDESCRIPTION "To jest test"
#define APPPUBNAME "WNDTEST.1"
#define APPEXTENSION "wtest"
#define APPGUIDE "WndTest.guide"

Object *MainWnd=NULL;
Object *Cdity=NULL;
Object *CtrlC=NULL;
Object *Scrn=NULL;
Object *ARexx=NULL;
Object *AmigaGuide=NULL;

struct gtbRexxCommand commands[]=
{
	{"test_quit",0,NULL},
	{"test_about",1,NULL},
	{"test_help",2,"INDEX/A"},
	NULL
};

ULONG ARexxHandler(Object *Obj,struct gtbRexxCommand *rxcmd,struct gtbRexxMessage *rxmsg)
{
	TEXT command[256];
	if(!rxcmd) printf("Bad arguments\n");
	else
	switch(rxcmd->rc_ID)
	{
		case 0:
			printf("test_quit called\n");
			return FALSE;
		case 1:
			printf("test_about called\n");
			rxmsg->rm_Result="To jest program testowy";
			break;
		case 2:
			printf("test_help called: %s\n",rxmsg->rm_ArgList[0]);
			sprintf(command,"ALINK %s",rxmsg->rm_ArgList[0]);
			gtbSendAmigaGuideCmd(AmigaGuide,command);
			break;
	}
	return TRUE;
}

ULONG CtrlCHandler(Object *Obj,ULONG type,APTR msg)
{
	return FALSE;
}

ULONG CdityHandler(Object *CxObj,ULONG type,struct CxMessage *cxmsg)
{
	switch(type)
	{
		case CXM_COMMAND:
			switch(cxmsg->cx_ID)
			{
				case CXCMD_KILL:
					return FALSE;
			}
			break;
	}
	return TRUE;
}

ULONG Gad1Handler(Object *GadgetObj,ULONG type,struct IntuiMessage *message)
{
	printf("pierwszy\n");
	return TRUE;
}

ULONG WndHandler(Object *WindowObj,ULONG type,APTR message)
{
	struct IntuiMessage *im;
	ULONG var;

	switch(type)
	{
		case TYPE_INTUIMESSAGE:
			im=(struct IntuiMessage *)message;
			switch(im->Class)
			{
				case IDCMP_CLOSEWINDOW: return FALSE;
				case IDCMP_GADGETUP:
					printf("Gadget clicked\n");
					break;
			}
			break;
		case TYPE_VERTSCROLL:
			GetAttr(WIND_VertTop,WindowObj,&var);
			printf("v: %d\n",var);
			break;
		case TYPE_HORIZSCROLL:
			GetAttr(WIND_HorizTop,WindowObj,&var);
			printf("h: %d\n",var);
			break;
	}
	return TRUE;
}

void main(int argc, char *argv[])
{
	struct gtbApplication *appl=NULL;
	struct Window *Wnd;
	struct Gadget *GList=NULL;
	Object *prev;

	unless(appl=gtbNewApplication(TAG_DONE)) Leave("Can't create application");
	unless(Scrn=NewObject(appl->ga_ScrClass,NULL,
		SCRN_LikeWorkbench,TRUE,
		SCRN_PubName,APPPUBNAME,
		SCRN_Private,FALSE,
		SCRN_Title,APPPUBNAME,
		TAG_DONE)) Leave("Can't create screen");
	unless(Cdity=NewObject(appl->ga_CxClass,NULL,
		COMM_Name,APPNAME,
		COMM_Title,APPTITLE,
		COMM_Description,APPDESCRIPTION,
		COMM_Unique,TRUE,
		COMM_Notify,TRUE,
		COMM_ShowHide,TRUE,
		COMM_Handler,(ULONG)CdityHandler,
		COMM_AddHotKeyStr,"control t",COMM_AddHotKeyID,1,
		TAG_DONE)) Leave("Can't create commodity");
	unless(CtrlC=NewObject(appl->ga_MsgClass,NULL,
		MESG_SigBit,SIGBREAKF_CTRL_C,
		MESG_Handler,(ULONG)CtrlCHandler,
		TAG_DONE)) Leave("Can't create message handler");
	unless(ARexx=NewObject(appl->ga_ARexxClass,NULL,
		REXX_Name,APPPUBNAME,
		REXX_Extension,APPEXTENSION,
		REXX_CommandTable,commands,
		REXX_Handler,(ULONG)ARexxHandler,
		TAG_DONE)) Leave("Can't create arexx");
	unless(AmigaGuide=NewObject(appl->ga_AGClass,NULL,
		AGUI_Name,APPGUIDE,
		AGUI_ScreenObject,Scrn,
		AGUI_ARexxObject,ARexx,
		AGUI_Activate,TRUE,
		AGUI_BaseName,APPNAME,
		TAG_DONE)) Leave("Can't create amigaguide");
	unless(MainWnd=NewObject(appl->ga_WndClass,NULL,WIND_CenterX,600,WIND_CenterY,20,WIND_Width,200,WIND_Height,200,
		WIND_DragBar,TRUE,
		WIND_DepthGadget,TRUE,
		WIND_SizeGadget,TRUE,
		WIND_CloseGadget,TRUE,
		WIND_Title,APPTITLE,
		WIND_ScreenObject,Scrn,
		WIND_VertScroll,TRUE,
		WIND_VertTotal,10,
		WIND_HorizScroll,TRUE,
		WIND_HorizTotal,10,
		WIND_HorizVisible,5,
		WIND_IDCMP,IDCMP_CLOSEWINDOW|IDCMP_GADGETUP,
		WIND_Handler,(ULONG)WndHandler,
		WIND_Activate,TRUE,
		WIND_MaxWidth,-1,
		WIND_MaxHeight,-1,
		TAG_DONE)) Leave("Can't create window");
	GetAttr(WIND_Window,MainWnd,(ULONG *)&Wnd);

	unless(prev=(Object *)CreateContext(&GList)) Leave("Can't create context");
	unless(prev=gtbCreateGadget(BUTTON_KIND,NULL,NULL,
		GA_Previous,prev,
		GA_Left,Wnd->BorderLeft,
		GA_Top,Wnd->BorderTop,
		GA_Width,100,
		GA_Height,20,
		GA_Text,"Ble",
		GT_ScreenObj,Scrn,
		GT_Handler,(ULONG)Gad1Handler,
		TAG_DONE)) Leave("Can't create gadget 1");
	unless(prev=gtbCreateGadget(STRING_KIND,NULL,NULL,
		GA_Previous,prev,
		GA_Left,Wnd->BorderLeft,
		GA_Top,Wnd->BorderTop+30,
		GA_Width,100,
		GA_Height,20,
		GA_Text,"Ble2",
		GT_ScreenObj,Scrn,
		TAG_DONE)) Leave("Can't create gadget 2");
	unless(prev=gtbCreateGadget(BOOPSI_KIND,NULL,FRBUTTONCLASS,
		GA_Previous,prev,
		GA_Left,Wnd->BorderLeft,
		GA_Top,Wnd->BorderTop+60,
		GA_Width,100,
		GA_Height,20,
		GA_Text,"Ble3",
		ICA_TARGET,ICTARGET_IDCMP,
		TAG_DONE)) Leave("Can't create gadget 3");

	DoMethod(MainWnd,OM_ADDMEMBER,GList);
	RefreshGList(GList,Wnd,NULL,-1);
	GT_RefreshWindow(Wnd,NULL);

	gtbExecuteRexx(ARexx,argv[1]);

	while(gtbHandleObject(MainWnd,Cdity,CtrlC,ARexx,AmigaGuide,TAG_DONE));

	RemoveGList(Wnd,GList,-1);
	gtbFreeGadgets(GList);
end:
	if(AmigaGuide) DisposeObject(AmigaGuide);
	if(ARexx) DisposeObject(ARexx);
	if(MainWnd) DisposeObject(MainWnd);
	if(CtrlC) DisposeObject(CtrlC);
	if(Cdity) DisposeObject(Cdity);
	if(Scrn) DisposeObject(Scrn);
	if(appl) gtbDisposeApplication(appl);
}
