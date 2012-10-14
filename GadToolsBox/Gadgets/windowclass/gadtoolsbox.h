#ifndef GADTOOLSBOX_H
#define GADTOOLSBOX_H

/*
**	$VER: gadtoolsbox.h 39.1 (2.6.98)
**	Includes Release 39.1
**
**	C prototypes. For use with 32 bit integers only.
**
**	(C) Copyright 1998 Tomasz Muszynski.
**	    All Rights Reserved
*/

#include<exec/types.h>
#include<intuition/gadgetclass.h>
#include<gadtoolsbox/window.h>
#include<gadtoolsbox/commodities.h>
#include<gadtoolsbox/messages.h>
#include<gadtoolsbox/gadgets.h>
#include<gadtoolsbox/screen.h>
#include<gadtoolsbox/arexx.h>
#include<gadtoolsbox/amigaguide.h>

/*
** Methods
*/

#define GTB_HANDLEMESSAGE 500
#define GTB_WAITMESSAGE 501
#define GTB_GETMESSAGE 502

struct gtbWaitMessage
{
	ULONG MethodID;
	ULONG Signals;
};

/*
** Application structrure
*/

struct gtbApplication
{
	Class *ga_GTClass;
	struct MinList ga_ListMemory;

	Class *ga_WndClass;
	struct MinList ga_ListWindows;

	Class *ga_CxClass;
	struct MinList ga_ListCommodities;

	Class *ga_MsgClass;
	struct MinList ga_ListMessages;

	Class *ga_ScrClass;
	struct MinList ga_ListScreens;

	Class *ga_ARexxClass;
	struct MinList ga_ListARexx;

	Class *ga_AGClass;
	struct MinList ga_ListAmigaGuide;
};

/*
** Memory header
*/

struct gtbMemHeader
{
	struct MinNode gmh_MinNode;
	ULONG gmh_Size;
};

/*
** Prototypes
*/

struct gtbApplication *gtbNewApplication(ULONG,...);
void gtbDisposeApplication(struct gtbApplication *);
ULONG gtbHandleObject(Object *, ...);
void *gtbAllocVec(ULONG,ULONG);
void gtbFreeVec(void *);
void gtbExecuteRexx(Object *,STRPTR);
BOOL gtbSendAmigaGuideCmd(Object *,STRPTR);
Object *gtbCreateGadget(ULONG, struct IClass *,UBYTE *, ULONG, ...);
void gtbFreeGadgets(struct Gadget *);
void gtbSetGadgetAttrs(Object *,Object *,ULONG,...);
void gtbGetGadgetAttr(ULONG,Object *,ULONG *);

#endif /* GADTOOLSBOX_H */
