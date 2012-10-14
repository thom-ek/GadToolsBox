/*
** GadToolsBox Foundation Class 
** $VER: gadtoolsbox.c 37.1 (15.4.98)
*/

#include<stdio.h>
#include<proto/dos.h>
#include<proto/exec.h>
#include<proto/intuition.h>
#include<proto/amigaguide.h>
#include<proto/gadtools.h>
#include<intuition/gadgetclass.h>
#include<exec/memory.h>
#include"gadtoolsbox.h"

extern Class *CreateWindowClass(struct gtbApplication *);
extern Class *CreateCommoditiesClass(struct gtbApplication *);
extern Class *CreateMessageClass(struct gtbApplication *);
extern Class *CreateGadgetClass(struct gtbApplication *);
extern Class *CreateScreenClass(struct gtbApplication *);
extern Class *CreateARexxClass(struct gtbApplication *);
extern Class *CreateAmigaGuideClass(struct gtbApplication *);
extern void DisposeWindowClass(Class *);
extern void DisposeCommoditiesClass(Class *);
extern void DisposeMessageClass(Class *);
extern void DisposeGadgetClass(Class *);
extern void DisposeScreenClass(Class *);
extern void DisposeARexxClass(Class *);
extern void DisposeAmigaGuideClass(Class *);

struct gtbApplication *_gtbGlobalAppPtr=NULL;

struct gtbApplication *gtbNewApplication(ULONG tags,...)
{
	struct gtbApplication *gtbappl;
//	struct TagItem *tag, *tstate;

	if(gtbappl=AllocVec(sizeof(struct gtbApplication),MEMF_ANY))
	{
		NewList((struct List *)&gtbappl->ga_ListWindows);
		NewList((struct List *)&gtbappl->ga_ListScreens);
		NewList((struct List *)&gtbappl->ga_ListCommodities);
		NewList((struct List *)&gtbappl->ga_ListMessages);
		NewList((struct List *)&gtbappl->ga_ListMemory);
		NewList((struct List *)&gtbappl->ga_ListARexx);
		NewList((struct List *)&gtbappl->ga_ListAmigaGuide);

		if(!(gtbappl->ga_WndClass=CreateWindowClass(gtbappl))) goto err;
		if(!(gtbappl->ga_CxClass=CreateCommoditiesClass(gtbappl))) goto err;
		if(!(gtbappl->ga_MsgClass=CreateMessageClass(gtbappl))) goto err;
		if(!(gtbappl->ga_ScrClass=CreateScreenClass(gtbappl))) goto err;
		if(!(gtbappl->ga_ARexxClass=CreateARexxClass(gtbappl))) goto err;
		if(!(gtbappl->ga_AGClass=CreateAmigaGuideClass(gtbappl))) goto err;

		_gtbGlobalAppPtr=gtbappl;

/*
		tstate=(struct TagItem *)&tags;
		while(tag = NextTagItem( &tstate ))
		{
			switch(tag->ti_Tag)
			{
				case APPL_NewObject:
					break;
			}
		}
*/

		return gtbappl;
err:
		if(gtbappl->ga_AGClass) DisposeAmigaGuideClass(gtbappl->ga_AGClass);
		if(gtbappl->ga_ARexxClass) DisposeARexxClass(gtbappl->ga_ARexxClass);
		if(gtbappl->ga_ScrClass) DisposeScreenClass(gtbappl->ga_ScrClass);
		if(gtbappl->ga_MsgClass) DisposeMessageClass(gtbappl->ga_MsgClass);
		if(gtbappl->ga_CxClass) DisposeCommoditiesClass(gtbappl->ga_CxClass);
		if(gtbappl->ga_WndClass) DisposeWindowClass(gtbappl->ga_WndClass);
		FreeVec(gtbappl);
	}
	return NULL;
}

void gtbDisposeApplication(struct gtbApplication *gtbappl)
{
	Object *ostate,*member;
	struct Node *n;

	if(gtbappl)
	{
		ostate = (Object *) gtbappl->ga_ListWindows.mlh_Head;
		while (member = NextObject (&ostate)) DisposeObject(member);
		ostate = (Object *) gtbappl->ga_ListCommodities.mlh_Head;
		while (member = NextObject (&ostate)) DisposeObject(member);
		ostate = (Object *) gtbappl->ga_ListMessages.mlh_Head;
		while (member = NextObject (&ostate)) DisposeObject(member);
		ostate = (Object *) gtbappl->ga_ListARexx.mlh_Head;
		while (member = NextObject (&ostate)) DisposeObject(member);
		ostate = (Object *) gtbappl->ga_ListAmigaGuide.mlh_Head;
		while (member = NextObject (&ostate)) DisposeObject(member);
		ostate = (Object *) gtbappl->ga_ListScreens.mlh_Head;
		while (member = NextObject (&ostate)) DisposeObject(member);

		if(gtbappl->ga_AGClass) DisposeARexxClass(gtbappl->ga_AGClass);
		if(gtbappl->ga_ARexxClass) DisposeARexxClass(gtbappl->ga_ARexxClass);
		if(gtbappl->ga_ScrClass) DisposeScreenClass(gtbappl->ga_ScrClass);
		if(gtbappl->ga_WndClass) DisposeWindowClass(gtbappl->ga_WndClass);
		if(gtbappl->ga_CxClass) DisposeCommoditiesClass(gtbappl->ga_CxClass);
		if(gtbappl->ga_MsgClass) DisposeMessageClass(gtbappl->ga_MsgClass);
		while(n=RemTail((struct List *)&gtbappl->ga_ListMemory)) gtbFreeVec(n);
		FreeVec(gtbappl);
	}
}

ULONG gtbHandleObject(Object *o, ...)
{
	ULONG signals=NULL,s,rs;
	Object **obj;

	obj=&o;
	while(*obj!=TAG_DONE)
	{
		GetAttr(WIND_SigBit,*obj,(ULONG *)&s);
		signals|=s;
		obj++;
	}
	rs=Wait(signals);
	obj=&o;
	while(*obj!=TAG_DONE)
	{
		GetAttr(WIND_SigBit,*obj,(ULONG *)&s);
		if(rs & s) return DoMethod(*obj,GTB_HANDLEMESSAGE,rs);
		obj++;
	}
	return rs;
}

void *gtbAllocVec(ULONG size,ULONG type)
{
	struct gtbMemHeader *buf;

	if(buf=AllocMem(size+sizeof(struct gtbMemHeader),type))
	{
		buf->gmh_Size=size+sizeof(struct gtbMemHeader);
		AddTail((struct List *)&_gtbGlobalAppPtr->ga_ListMemory,(struct Node *)buf);
		return (void *)(((UBYTE *)buf)+sizeof(struct gtbMemHeader));
	}
	return NULL;
}

void gtbFreeVec(void *pointer)
{
	struct gtbMemHeader *buf;

	if(pointer)
	{
		buf=(struct gtbMemHeader *)(((UBYTE *)pointer)-sizeof(struct gtbMemHeader));
		Remove((struct Node *)buf);
		FreeMem(pointer,buf->gmh_Size);
	}
}

void gtbExecuteRexx(Object *rexx,STRPTR command)
{
	DoMethod(rexx,GTB_REXX_SENDMSG,command,NULL);
}

BOOL gtbSendAmigaGuideCmd(Object *ag,STRPTR command)
{
	AMIGAGUIDECONTEXT agc;

	if(ag)
	{
		GetAttr(AGUI_Context,ag,(ULONG *)&agc);
		return (BOOL) SendAmigaGuideCmd(agc,command,TAG_DONE);
	}
	else return NULL;
}

