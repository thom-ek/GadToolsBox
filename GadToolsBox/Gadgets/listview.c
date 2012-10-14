/*
** ListView Class
** $VER: listview.c 37.1 (15.1.98)
*/

#include<dos.h>
#include<string.h>
#include<clib/macros.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<proto/graphics.h>
#include<proto/intuition.h>
#include<proto/gadtools.h>
#include<proto/layers.h>
#include<gadgets/listview.h>
#include<graphics/gfxmacros.h>
#include<exec/memory.h>
#include<libraries/gadtools.h>
#include<intuition/imageclass.h>
#include<intuition/gadgetclass.h>
#include<intuition/intuitionbase.h>
#include<intuition/icclass.h>
#include"listview.gadget_rev.h"

TEXT version[]=VERSTAG;

/*
** Defines
*/

#define abs(x) ((x)<0?-(x):(x))
#define unless(x) if(!(x))
#define LIST_SetTagArg(tag,id,data)	{ tag.ti_Tag = (ULONG)(id); tag.ti_Data = (ULONG)(data); }
typedef ULONG (*HookFunction)(void);
#define SAVEDS 
#define ASMFUNC __saveds __asm
#define REG(x) register __## x
#define LISTVIEWGADGET "listview.gadget"

/*
** Prototypes
*/

Class * ASMFUNC GetListViewClass(void);
ULONG ASMFUNC LIST_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG LIST_NEW(Class *,Object *,struct opSet *);
ULONG STRG_NEW(Class *,Object *,struct opSet *);
ULONG LIST_DISPOSE(Class *, Object *, Msg);
ULONG LIST_SET(Class *, Object *, struct opSet *);
ULONG LIST_GET(Class *,Object *,struct opGet *);
ULONG LIST_UPDATE(Class *,Object *,struct opUpdate *);
ULONG LIST_NOTIFY(Class *,Object *,struct opUpdate *);
ULONG LIST_RENDER(Class *,Object *,struct gpRender *);
ULONG LIST_GOACTIVE(Class *,Object *,struct gpInput *);
ULONG LIST_HANDLEINPUT(Class *,Object *,struct gpInput *);
ULONG LIST_GOINACTIVE(Class *,Object *,struct gpGoInactive *);
ULONG STRG_GOINACTIVE(Class *,Object *,struct gpGoInactive *);
void LIST_GetGadgetRect( Object *,struct GadgetInfo *,struct Rectangle *);
void SPrintf(Class *,STRPTR buffer, STRPTR format, ...);
void TextColor(Class *,Object *,struct RastPort *,STRPTR,ULONG,struct DrawInfo *,struct IBox *);
void LineOut(Class *,Object *,struct RastPort *,struct LVNode *,struct IBox *,ULONG,struct DrawInfo *);

/*
** Variables
*/

struct LISTData
{
	struct LVList *ld_Labels;
	ULONG ld_OldTop,ld_Top;
	ULONG ld_Columns;
	ULONG ld_Visible;
	BOOL ld_MakeVisible;
	BOOL ld_Border;
	BOOL ld_MultiSelect;
	BOOL ld_ShowTreeLines;
	BOOL ld_IsTree;
	BOOL ld_ReadOnly;
	STRPTR ld_TabChar;
	Object *ld_FrameImage;
	struct TextFont *ld_TextFont;
	struct LVNode *ld_TopNode,*ld_ActualNode,*ld_PrevSelected;
	BOOL ld_ActiveFromMouse;
	BOOL ld_Do,ld_Init,ld_EditCol;
	STRPTR ld_Format;
	ULONG ld_Secs,ld_Mics,ld_Cnt;
	UWORD *ld_ColPos,ld_SelCol,ld_SelColPos,ld_PrevColPos;
	struct Hook *ld_HookSub,*ld_HookEdit,*ld_HookDraw;
	Class *ld_StringClass;
	Object *ld_StringObj;
	ULONG ld_StringRedraw;
	ULONG ld_StringColumn;
	struct Window *ld_StringWnd;
	TEXT ld_StringBuffer[256];
	Object *ld_ListViewObj;
};

struct STRGData
{
	struct LISTData *sd_LISTData;
};

Class *cl=NULL;

/*
** Create Class
*/

int ASMFUNC __UserLibInit(REG(a6) struct MyLibrary *libbase)
{
	if(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",36L))
	{
		if(UtilityBase=OpenLibrary("utility.library",36L))
		{
			if(GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",36L))
			{
				if(GadToolsBase=OpenLibrary("gadtools.library",36L))
				{
					if(LayersBase=OpenLibrary("layers.library",36L))
					{
						if (cl = MakeClass (NULL, GADGETCLASS, NULL, sizeof(struct LISTData), 0L))
						{
							cl->cl_Dispatcher.h_Entry = (HookFunction)  LIST_Dispatcher;
							cl->cl_UserData = (ULONG) libbase;
//							AddClass (cl);
							return(FALSE);
						}
						CloseLibrary(LayersBase);
					}
					CloseLibrary(GadToolsBase);
				}
				CloseLibrary((struct Library *)GfxBase);
			}
			CloseLibrary(UtilityBase);
		}
		CloseLibrary((struct Library *)IntuitionBase);
	}
	return(TRUE);
}

void ASMFUNC __UserLibCleanup(REG(a6) struct MyLibrary *libbase)
{
	FreeClass(cl);
	if(IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
	if(UtilityBase) CloseLibrary(UtilityBase);
	if(GfxBase) CloseLibrary((struct Library *)GfxBase);
}


Class * ASMFUNC GetListViewClass()
{
	return cl;
}

void ASMFUNC Tree_NewList(REG(a0)struct LVList *list)
{
	NewList((struct List *)list);
//	list->lh_Head=(struct LVNode *) &list->lh_Tail;
//	list->lh_TailPred=(struct LVNode *)list;
}

void ASMFUNC Tree_AddTail(REG(a0) struct LVList *list,REG(a1) struct LVNode *node)
{
	AddTail((struct List *)list,(struct Node *)node);
	node->ln_Parent=NULL;
}

void ASMFUNC Tree_AddHead(REG(a0) struct LVList *list,REG(a1) struct LVNode *node)
{
	AddHead((struct List *)list,(struct Node *)node);
	node->ln_Parent=NULL;
}

void ASMFUNC Tree_AddSubTail(REG(a0) struct LVList *list,REG(a1) struct LVNode *parentnode,REG(a2) struct LVNode *node)
{
	struct LVNode *parnode;

	if(parentnode) // not in sub
	{
		if(parentnode->ln_Sub)
		{
			parnode=parentnode->ln_Sub;
			while(parnode->ln_Succ!=parentnode) parnode=parnode->ln_Succ;
			node->ln_Succ=parnode->ln_Succ;
			node->ln_Pred=parnode;
			parnode->ln_Succ=node;
		}
		else
		{
			parentnode->ln_Sub=node;
			node->ln_Succ=parentnode;
			node->ln_Pred=parentnode;
		}
		node->ln_Indent=parentnode->ln_Indent+1;
		node->ln_Parent=parentnode;
	}
	else
	{
		Tree_AddTail(list,node);
	}
}

void ASMFUNC Tree_AddSubHead(REG(a0) struct LVList *list,REG(a1) struct LVNode *parentnode,REG(a2) struct LVNode *node)
{
	struct LVNode *parnode;

	if(parentnode) // not in sub
	{
		if(parentnode->ln_Sub)
		{
			parnode=parentnode->ln_Sub;
			parentnode->ln_Sub=node;
			node->ln_Succ=parnode;
			node->ln_Pred=parentnode;
		}
		else
		{
			parentnode->ln_Sub=node;
			node->ln_Succ=parentnode;
			node->ln_Pred=parentnode;
		}
		node->ln_Indent=parentnode->ln_Indent+1;
		node->ln_Parent=parentnode;
	}
	else
	{
		Tree_AddHead(list,node);
	}
}

struct LVNode *Tree_WhileParent(struct LVNode *node)
{
	while(node->ln_Succ==node->ln_Parent) if(node) node=node->ln_Succ; else return NULL;
	if(node) node=node->ln_Succ;
	return node;
}

struct LVNode *ASMFUNC Tree_NextNode(REG(a0)struct LVNode *node)
{
	if(node->ln_Succ)
		return (((node->ln_Flags&LV_OPEN) && (node->ln_Sub)) ? (node->ln_Sub) : Tree_WhileParent(node));
	else return NULL;
}

struct LVNode *ASMFUNC Tree_NextSubNode(REG(a0)struct LVNode *node)
{
	if(node->ln_Succ)
		return ((node->ln_Sub) ? (node->ln_Sub) : Tree_WhileParent(node));
	else return NULL;
}

struct LVNode *ASMFUNC Tree_RemHead(REG(a0)struct LVList *list)
{
	return (struct LVNode *)RemHead((struct List *)list);
}

struct LVNode *ASMFUNC Tree_RemSubHead(REG(a0)struct LVNode *node)
{
	struct LVNode *parnode;
	
	if(node->ln_Sub)
	{
		parnode=node->ln_Sub;
		node->ln_Sub=parnode->ln_Succ;
		parnode->ln_Succ->ln_Pred=node;
		return parnode;
	}
	return NULL;
}

struct LVNode *ASMFUNC Tree_RemTail(REG(a0)struct LVList *list)
{
	return (struct LVNode *)RemTail((struct List *)list);
}

struct LVNode *ASMFUNC Tree_RemSubTail(REG(a0)struct LVNode *node)
{
	struct LVNode *parnode;

	if(node->ln_Sub)
	{
		parnode=node->ln_Sub;
		while(parnode->ln_Succ!=node) parnode=parnode->ln_Succ;
		if(parnode->ln_Pred==parnode->ln_Succ) node->ln_Sub=NULL;
		else parnode->ln_Pred->ln_Succ=parnode->ln_Succ;
		return parnode;
	}
	return NULL;
}


void ASMFUNC Tree_Remove(REG(a0)struct LVNode *node)
{
	if(node->ln_Parent) // in sub
	{
		if((node->ln_Succ==node->ln_Parent) && (node->ln_Pred==node->ln_Parent)) // one node in the list
		{
			node->ln_Parent->ln_Sub=NULL;
		}
		else
		if((node->ln_Succ==node->ln_Parent) && (node->ln_Pred!=node->ln_Parent)) // last node
		{
			node->ln_Pred->ln_Succ=node->ln_Succ;
		}
		else
		if((node->ln_Succ!=node->ln_Parent) && (node->ln_Pred==node->ln_Parent)) // first node
		{
			node->ln_Parent->ln_Sub=node->ln_Succ;
			node->ln_Succ->ln_Pred=node->ln_Parent;
		}
		else
		if((node->ln_Succ!=node->ln_Parent) && (node->ln_Pred!=node->ln_Parent)) // middle node
		{
			Remove((struct Node *)node);
		}
	}
	else
		Remove((struct Node *)node);
}

void ASMFUNC Tree_Insert(REG(a0)struct LVList *list,REG(a1)struct LVNode *node,REG(a2)struct LVNode *listNode)
{
	if(listNode)
	{
		if(listNode->ln_Parent) // in sub
		{
			node->ln_Succ=listNode->ln_Succ;
			listNode->ln_Succ=node;
			node->ln_Pred=listNode;
		}
		else
			Insert((struct List *)list,(struct Node *)node,(struct Node *)listNode);
	}
	else
		Insert((struct List *)list,(struct Node *)node,(struct Node *)listNode);
}

/*
** Dispatcher
*/

ULONG ASMFUNC STRG_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = STRG_NEW(cl, o, (struct opSet *)msg );
			break;
		case GM_GOINACTIVE:
			retval = STRG_GOINACTIVE(cl, o, (struct gpGoInactive *)msg );
			break;
		default:
			retval = DoSuperMethodA(cl, o, msg);
			break;
	}
	return(retval);
}

ULONG ASMFUNC LIST_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = LIST_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = LIST_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = LIST_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = LIST_GET(cl, o, (struct opGet *)msg );
			break;
		case OM_UPDATE:
			retval = LIST_UPDATE(cl, o, (struct opUpdate *)msg );
			break;
		case OM_NOTIFY:
			retval = LIST_NOTIFY(cl, o, (struct opUpdate *)msg );
			break;
		case GM_RENDER:
			retval = LIST_RENDER(cl, o, (struct gpRender *)msg );
			break;
		case GM_GOACTIVE:
			retval = LIST_GOACTIVE(cl, o, (struct gpInput *)msg );
			break;
		case GM_HANDLEINPUT:
			retval = LIST_HANDLEINPUT(cl, o, (struct gpInput *)msg );
			break;
		case GM_GOINACTIVE:
			retval = LIST_GOINACTIVE(cl, o, (struct gpGoInactive *)msg );
			break;
		default:
			retval = DoSuperMethodA(cl, o, msg);
			break;
	}
	return(retval);
}

/*
** OM_NEW
*/

ULONG SAVEDS STRG_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct STRGData *SD;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		SD = INST_DATA( cl, object );
		SD->sd_LISTData = (struct LISTData *)GetTagData( TAG_USER+1, NULL, ops->ops_AttrList );
	}
	return( (ULONG)object );
}

ULONG SAVEDS LIST_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct LISTData *LD;
	UWORD *ColPos;
	int a;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		LD = INST_DATA( cl, object );

		if (LD->ld_StringClass = MakeClass (NULL, STRGCLASS, NULL, sizeof(struct STRGData), 0L))
		{
			LD->ld_StringClass->cl_Dispatcher.h_Entry = (HookFunction)  STRG_Dispatcher;
			LD->ld_StringClass->cl_UserData = (ULONG) cl->cl_UserData;
		}
		LD->ld_StringObj=NULL;
		LD->ld_ListViewObj=object;
		LD->ld_OldTop = LD->ld_Top = GetTagData( LIST_Top, 0, ops->ops_AttrList );
		LD->ld_HookSub = (struct Hook *)GetTagData( LIST_HookSub, NULL, ops->ops_AttrList );
		LD->ld_HookEdit = (struct Hook *)GetTagData( LIST_HookEdit, NULL, ops->ops_AttrList );
		LD->ld_HookDraw = (struct Hook *)GetTagData( LIST_HookDraw, NULL, ops->ops_AttrList );
		LD->ld_MakeVisible = GetTagData( LIST_MakeVisible, FALSE, ops->ops_AttrList );
		LD->ld_Labels = (struct LVList *)GetTagData( LIST_Labels, NULL, ops->ops_AttrList );
		LD->ld_ReadOnly = GetTagData( LIST_ReadOnly, FALSE, ops->ops_AttrList );
		LD->ld_Border = GetTagData( LIST_Border, FALSE, ops->ops_AttrList );
		LD->ld_Columns = GetTagData( LIST_Columns, 1, ops->ops_AttrList );
		LD->ld_Format = (STRPTR) GetTagData( LIST_Format, NULL, ops->ops_AttrList );
		LD->ld_MultiSelect = GetTagData( LIST_MultiSelect, FALSE, ops->ops_AttrList );
		LD->ld_TextFont = (struct TextFont *)GetTagData( LIST_TextFont, NULL, ops->ops_AttrList);
		LD->ld_ShowTreeLines= GetTagData( LIST_ShowTreeLines, FALSE, ops->ops_AttrList );
		LD->ld_IsTree = GetTagData( LIST_IsTree, FALSE, ops->ops_AttrList );
		LD->ld_TabChar = (STRPTR) GetTagData( LIST_TabChar, (ULONG) "\t", ops->ops_AttrList );
		if(ColPos = (UWORD *) GetTagData( LIST_ColumnsPos, NULL, ops->ops_AttrList ))
		{
			for(a=0;a<LD->ld_Columns;a++) LD->ld_ColPos[a]=ColPos[a];
			LD->ld_Init = FALSE;
		}
		else LD->ld_Init = TRUE;
		if(LD->ld_Labels) LD->ld_TopNode=LD->ld_Labels->lh_Head; else LD->ld_TopNode=NULL;
		LD->ld_PrevSelected = NULL;
		if(LD->ld_Columns==0) LD->ld_Columns=1;
		unless(LD->ld_ColPos=AllocVec((LD->ld_Columns+1)*sizeof(UWORD),MEMF_ANY))
		{
			CoerceMethod(cl, o, OM_DISPOSE);
			object = NULL;
			return( (ULONG)object );
		}
		LD->ld_SelCol=LD->ld_SelColPos=LD->ld_PrevColPos=0;
		LD->ld_EditCol=FALSE;

		if(LD->ld_Border)
		{
			unless(LD->ld_FrameImage=NewObject(NULL,"frameiclass",
				IA_Recessed,    LD->ld_ReadOnly,
				IA_EdgesOnly,   TRUE,
				IA_FrameType,   FRAME_BUTTON,
				TAG_END))
			{
				CoerceMethod(cl, o, OM_DISPOSE);
				object = NULL;
				return( (ULONG)object );
			}
		}
	}
	return( (ULONG)object );
}

/*
** OM_DISPOSE
*/

ULONG SAVEDS LIST_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct LISTData *LD = INST_DATA( cl, o );

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	if(LD->ld_StringObj)
	{
		RemoveGadget(LD->ld_StringWnd,(struct Gadget *)LD->ld_StringObj);
		DisposeObject(LD->ld_StringObj);
	}
	if(LD->ld_StringClass) FreeClass(LD->ld_StringClass);
	if(LD->ld_FrameImage) DisposeObject(LD->ld_FrameImage);
	if(LD->ld_ColPos) FreeVec(LD->ld_ColPos);
	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS LIST_SET(Class *cl, Object *o, struct opSet *ops)
{
	ULONG retval,u=FALSE;
	struct LISTData *LD = INST_DATA( cl, o );
	struct TagItem *tag, *tstate, notify;
	struct RastPort *rp;
	struct LVNode *node;
	UWORD *ColPos;
	int a;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	tstate=ops->ops_AttrList;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case LIST_ColumnsPos:
				ColPos=(UWORD *)tag->ti_Data;
				for(a=0;a<LD->ld_Columns;a++) LD->ld_ColPos[a]=ColPos[a];
				retval=FALSE; u=FALSE;
				break;
			case LIST_HookSub:
				LD->ld_HookSub=(struct Hook *)tag->ti_Data;
				retval=TRUE; u=FALSE;
				break;
			case LIST_HookDraw:
				LD->ld_HookDraw=(struct Hook *)tag->ti_Data;
				retval=FALSE; u=FALSE;
				break;
			case LIST_HookEdit:
				LD->ld_HookEdit=(struct Hook *)tag->ti_Data;
				retval=TRUE; u=FALSE;
				break;
			case LIST_Labels:
				LD->ld_Labels = (struct LVList *)tag->ti_Data;
				LD->ld_Top = 0; LD->ld_OldTop=0;
				retval = FALSE; u=FALSE;
				break;
			case LIST_Top:
				LD->ld_OldTop=LD->ld_Top;
				LD->ld_Top = tag->ti_Data;
				if(LD->ld_Top<0) LD->ld_Top=0;
				retval = FALSE; u=TRUE;
				break;
			case LIST_MakeVisible:
				LD->ld_MakeVisible = tag->ti_Data;
				retval = FALSE; u=TRUE;
				break;
			case LIST_Columns:
				LD->ld_Columns = tag->ti_Data;
				retval = FALSE; u=FALSE;
				break;
			case LIST_ShowTreeLines:
				LD->ld_ShowTreeLines = tag->ti_Data;
				retval = FALSE; u=FALSE;
				break;
			case LIST_IsTree:
				LD->ld_IsTree = tag->ti_Data;
				retval = FALSE; u=FALSE;
				break;
			case LIST_Selected:
				for(node=LD->ld_Labels->lh_Head;Tree_NextNode(node);node=Tree_NextNode(node))
				{
					if(node==(struct LVNode *)tag->ti_Data);
					{
						LD->ld_ActualNode = (struct LVNode *)tag->ti_Data;
						break;
					}
				}
				retval = FALSE; u=TRUE;
				break;
		}
	}
	LIST_SetTagArg( notify, TAG_END, NULL );
	DoMethod( o, OM_NOTIFY, &notify, ops->ops_GInfo, 0 );

	if(!retval)
	{
		if(rp=ObtainGIRPort( ops->ops_GInfo ))
		{
			DoMethod(o,GM_RENDER,ops->ops_GInfo, rp, u?GREDRAW_UPDATE:GREDRAW_REDRAW);
			ReleaseGIRPort(rp);
		}
	}

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS LIST_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval;
	struct LISTData *LD = INST_DATA( cl, o );

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	switch( opg->opg_AttrID )
	{
		case LIST_ColumnsPos:
			*opg->opg_Storage = (ULONG)LD->ld_ColPos;
			break;
		case LIST_HookSub:
			*opg->opg_Storage = (ULONG)LD->ld_HookSub;
			retval = TRUE;
			break;
		case LIST_HookEdit:
			*opg->opg_Storage = (ULONG)LD->ld_HookEdit;
			retval = TRUE;
			break;
		case LIST_HookDraw:
			*opg->opg_Storage = (ULONG)LD->ld_HookDraw;
			retval = TRUE;
			break;
		case LIST_Top:
			*opg->opg_Storage = (ULONG)LD->ld_Top;
			retval = TRUE;
			break;
		case LIST_Visible:
			*opg->opg_Storage = (ULONG)LD->ld_Visible-(LD->ld_Columns>1?1:0);
			retval = TRUE;
			break;
		case LIST_Labels:
			*opg->opg_Storage = (ULONG)LD->ld_Labels;
			retval = TRUE;
			break;
		case LIST_TextFont:
			*opg->opg_Storage = (ULONG)LD->ld_TextFont;
			retval = TRUE;
			break;
		case LIST_Border:
			*opg->opg_Storage = (ULONG)LD->ld_Border;
			retval = TRUE;
			break;
		case LIST_ReadOnly:
			*opg->opg_Storage = (ULONG)LD->ld_ReadOnly;
			retval = TRUE;
			break;
		case LIST_Columns:
			*opg->opg_Storage = (ULONG)LD->ld_Columns;
			retval = TRUE;
			break;
		case LIST_MultiSelect:
			*opg->opg_Storage = (ULONG)LD->ld_MultiSelect;
			retval = TRUE;
			break;
		case LIST_ShowTreeLines:
			*opg->opg_Storage = (ULONG)LD->ld_ShowTreeLines;
			retval = TRUE;
			break;
		case LIST_IsTree:
			*opg->opg_Storage = (ULONG)LD->ld_IsTree;
			retval = TRUE;
			break;
		case LIST_TabChar:
			*opg->opg_Storage = (ULONG)LD->ld_TabChar;
			retval = TRUE;
			break;
		case LIST_Selected:
			*opg->opg_Storage = (ULONG)LD->ld_ActualNode;
			retval = TRUE;
			break;
		default:
			retval = DoSuperMethodA(cl, o, (Msg)opg);
			break;
	}
	return( retval );
}

/*
** OM_UPDATE
*/

ULONG SAVEDS LIST_UPDATE(Class *cl,Object *o,struct opUpdate *opu )
{
	ULONG retval;
	struct LISTData *LD = INST_DATA( cl, o );
	struct TagItem *tag, notify;
	struct RastPort *rp;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA(cl, o, (Msg) opu);

	if( opu->opu_AttrList )
	{
		if(tag = FindTagItem( LIST_Top, opu->opu_AttrList ))
		{
			if( tag->ti_Data != LD->ld_Top)
			{
				LD->ld_OldTop = LD->ld_Top;
				LD->ld_Top = tag->ti_Data;

				rp = ObtainGIRPort( opu->opu_GInfo );
				if( rp )
				{
					DoMethod( o, GM_RENDER, opu->opu_GInfo, rp, GREDRAW_UPDATE );
					ReleaseGIRPort( rp );
				}

				/* Notify the change. */
				LIST_SetTagArg( notify, TAG_END, NULL );
				(void)DoMethod( o, OM_NOTIFY, &notify, opu->opu_GInfo, 0 );
			}
		}
		if(tag = FindTagItem( LIST_MakeVisible, opu->opu_AttrList ))
		{
			if( tag->ti_Data != LD->ld_MakeVisible)
			{
				LD->ld_OldTop = LD->ld_Top;
				LD->ld_Top = tag->ti_Data;

				rp = ObtainGIRPort( opu->opu_GInfo );
				if( rp )
				{
					DoMethod( o, GM_RENDER, opu->opu_GInfo, rp, GREDRAW_UPDATE );
					ReleaseGIRPort( rp );
				}

				/* Notify the change. */
				LIST_SetTagArg( notify, TAG_END, NULL );
				(void)DoMethod( o, OM_NOTIFY, &notify, opu->opu_GInfo, 0 );
			}
		}
	}
	return( retval );
}

ULONG SAVEDS LIST_NOTIFY(Class *cl,Object *o,struct opUpdate *opu )
{
	struct TagItem tags[4];
	struct LISTData *LD = INST_DATA( cl, o );

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	LIST_SetTagArg(tags[0], GA_ID, ((struct Gadget *)o)->GadgetID);
	LIST_SetTagArg(tags[1], LIST_Top, LD->ld_Top);
	LIST_SetTagArg(tags[2], LIST_MakeVisible, LD->ld_MakeVisible);

	if( opu->opu_AttrList == NULL )
	{
		LIST_SetTagArg(tags[3], TAG_END, NULL);
	}
	else LIST_SetTagArg(tags[3], TAG_MORE, opu->opu_AttrList );

	return( DoSuperMethod(cl, o, OM_NOTIFY, tags, opu->opu_GInfo, opu->opu_Flags) );
}

ULONG SAVEDS LIST_RENDER(Class *cl,Object *o,struct gpRender *gpr )
{
	ULONG retval,State,a,c,b,w,len;
	struct Gadget *gad = (struct Gadget *)o;
	struct Rectangle rect;
	struct DrawInfo *dri;
	struct IBox container;
	struct TextExtent temp_te;
	struct RastPort *RP = gpr->gpr_RPort;
	struct LISTData *LD = INST_DATA( cl, o );
	struct Region *region1=NULL,*oldregion;
	struct Rectangle rectc;
	UWORD patterndata[2] = { 0x2222, 0x8888 };
	ULONG TextPen, FillPen, BackgroundPen, HLTextPen;
	struct LVNode *node;
	struct IBox ibox;
	STRPTR copytext,columnstring,ts;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA(cl, o, (Msg) gpr);

	LIST_GetGadgetRect( o, gpr->gpr_GInfo, &rect );

	container.Left = rect.MinX; container.Top = rect.MinY;
	container.Width = 1 + rect.MaxX - rect.MinX;
	container.Height = 1 + rect.MaxY - rect.MinY;

	dri = gpr->gpr_GInfo->gi_DrInfo;

	if(gad->Flags & GFLG_DISABLED)
		State = IDS_DISABLED;
	else
    State = IDS_NORMAL;

	if(LD->ld_Border)
	{
		SetAttrs( LD->ld_FrameImage,
			IA_Left,    container.Left,
			IA_Top,     container.Top,
			IA_Width,   container.Width,
			IA_Height,  container.Height,
			TAG_END);

		DrawImageState( RP, (struct Image *)LD->ld_FrameImage, 0, 0, IDS_NORMAL, dri);
	}

	region1=NewRegion();
	rectc.MinX=container.Left+2;
	rectc.MinY=container.Top+2;
	rectc.MaxX=container.Left+container.Width-2;
	rectc.MaxY=container.Top+container.Height-2;
	OrRectRegion(region1,&rectc);
	oldregion=InstallClipRegion(gpr->gpr_GInfo->gi_Window->WLayer,region1);

	if( dri )
	{
		BackgroundPen = dri->dri_Pens[BACKGROUNDPEN];
		TextPen = dri->dri_Pens[TEXTPEN];
		HLTextPen = dri->dri_Pens[HIGHLIGHTTEXTPEN];
		FillPen = dri->dri_Pens[FILLPEN];
	}
	else
	{
		BackgroundPen = 0;
 		TextPen = 1;
		HLTextPen = 2;
		FillPen = 3;
	}

	/* Do we have a proper font. */
	if(LD->ld_TextFont) SetFont(RP,LD->ld_TextFont);
	else LD->ld_TextFont=RP->Font;

	if(LD->ld_Init)
	{
		for(a=0;a<LD->ld_Columns;a++) LD->ld_ColPos[a]=a*(container.Width/LD->ld_Columns);
		LD->ld_Init=FALSE;
	}
	LD->ld_ColPos[LD->ld_Columns]=container.Width;

	if(LD->ld_Labels)
	{
		for(node=LD->ld_Labels->lh_Head,a=0;Tree_NextNode(node)&&(a<LD->ld_Top);node=Tree_NextNode(node),a++);
		LD->ld_TopNode=node;

		LD->ld_Visible=((container.Height-4)/LD->ld_TextFont->tf_YSize);

		switch(gpr->gpr_Redraw)
		{
			case GREDRAW_UPDATE:
				if(LD->ld_OldTop>LD->ld_Top)	// text moves down
				{
					ScrollRaster(RP,0,(LD->ld_Top-LD->ld_OldTop)*LD->ld_TextFont->tf_YSize,
													container.Left+3, container.Top+2+(LD->ld_Columns>1?LD->ld_TextFont->tf_YSize:0),
													container.Left+container.Width-3, container.Top+1+LD->ld_Visible*LD->ld_TextFont->tf_YSize);
					for(a=0+(LD->ld_Columns>1?1:0);Tree_NextNode(node)&&(a<(LD->ld_OldTop-LD->ld_Top)+(LD->ld_Columns>1?1:0))&&(a<LD->ld_Visible);node=Tree_NextNode(node),a++)
					{
						if(LD->ld_MultiSelect && node==LD->ld_ActualNode) node->ln_Flags|=LV_SELECT;
						LineOut(cl,o,RP,node,&container,a,dri);
						if(LD->ld_MultiSelect) node->ln_Flags&=~LV_SELECT;
					}
					for(a=0;a<LD->ld_Columns-1;a++)
					{
						w=LD->ld_ColPos[a+1];
						SetAPen(RP,dri->dri_Pens[SHADOWPEN]);
						Move(RP,container.Left+w,container.Top+2);
						Draw(RP,container.Left+w,container.Top+(LD->ld_Border?container.Height:LD->ld_TextFont->tf_YSize)-3);
						SetAPen(RP,dri->dri_Pens[SHINEPEN]);
						Move(RP,container.Left+w+1,container.Top+2);
						Draw(RP,container.Left+w+1,container.Top+(LD->ld_Border?container.Height:LD->ld_TextFont->tf_YSize)-3);
					}
				}
				if(LD->ld_Top>LD->ld_OldTop)	// text moves up
				{
					ScrollRaster(RP,0,(LD->ld_Top-LD->ld_OldTop)*LD->ld_TextFont->tf_YSize,
													container.Left+3, container.Top+2+(LD->ld_Columns>1?LD->ld_TextFont->tf_YSize:0),
													container.Left+container.Width-3, container.Top+container.Height-2);
					if(LD->ld_Top-LD->ld_OldTop<LD->ld_Visible) for(a=0+(LD->ld_Columns>1?1:0);Tree_NextNode(node)&&(a<LD->ld_Visible-(LD->ld_Top-LD->ld_OldTop));node=Tree_NextNode(node),a++);
					else a=0+(LD->ld_Columns>1?1:0);
					for(;Tree_NextNode(node)&&(a<LD->ld_Visible);node=Tree_NextNode(node),a++)
					{
						if(LD->ld_MultiSelect && node==LD->ld_ActualNode) node->ln_Flags|=LV_SELECT;
						LineOut(cl,o,RP,node,&container,a,dri);
						if(LD->ld_MultiSelect) node->ln_Flags&=~LV_SELECT;
					}
					for(a=0;a<LD->ld_Columns-1;a++)
					{
						w=LD->ld_ColPos[a+1];
						SetAPen(RP,dri->dri_Pens[SHADOWPEN]);
						Move(RP,container.Left+w,container.Top+2);
						Draw(RP,container.Left+w,container.Top+(LD->ld_Border?container.Height:LD->ld_TextFont->tf_YSize)-3);
						SetAPen(RP,dri->dri_Pens[SHINEPEN]);
						Move(RP,container.Left+w+1,container.Top+2);
						Draw(RP,container.Left+w+1,container.Top+(LD->ld_Border?container.Height:LD->ld_TextFont->tf_YSize)-3);
					}
				}
				break;
			case GREDRAW_REDRAW:
				ts=LD->ld_Format;
	
				for(a=0+(LD->ld_Columns>1?1:0);Tree_NextNode(node)&&(a<LD->ld_Visible);node=Tree_NextNode(node),a++)
				{
					if(LD->ld_MultiSelect && node==LD->ld_ActualNode) node->ln_Flags|=LV_SELECT;
					LineOut(cl,o,RP,node,&container,a,dri);
					if(LD->ld_MultiSelect) node->ln_Flags&=~LV_SELECT;
				}
				SetAPen(RP,BackgroundPen);
				RectFill(RP,container.Left+3,container.Top+2+a*LD->ld_TextFont->tf_YSize,
										container.Left+container.Width-3,container.Top+container.Height-2);
	
				for(a=0;a<LD->ld_Columns-1;a++)
				{
					w=LD->ld_ColPos[a+1];
					SetAPen(RP,dri->dri_Pens[SHADOWPEN]);
					Move(RP,container.Left+w,container.Top+2);
					Draw(RP,container.Left+w,container.Top+(LD->ld_Border?container.Height:LD->ld_TextFont->tf_YSize)-3);
					SetAPen(RP,dri->dri_Pens[SHINEPEN]);
					Move(RP,container.Left+w+1,container.Top+2);
					Draw(RP,container.Left+w+1,container.Top+(LD->ld_Border?container.Height:LD->ld_TextFont->tf_YSize)-3);
				}
				if(ts)
				{
					if(copytext=AllocMem(strlen(LD->ld_Format)+100,MEMF_ANY))
					{
						if(columnstring=AllocMem(strlen(LD->ld_Format)+100,MEMF_ANY))
						{
							for(a=0;a<LD->ld_Columns;a++)
							{
								ts=stptok(ts,columnstring,strlen(LD->ld_Format)+100,"|");
								for(c=0,b=0;c<(strlen(columnstring)+1);c++) if(columnstring[c]!='') copytext[b++]=columnstring[c]; else c++;
								len=TextFit(RP,copytext,strlen(copytext),&temp_te,NULL,1,LD->ld_ColPos[a+1]-LD->ld_ColPos[a]-12, 1+LD->ld_TextFont->tf_YSize);
								SetAPen(RP,BackgroundPen);
								RectFill(RP,container.Left+LD->ld_ColPos[a]+2,container.Top+2,container.Left+LD->ld_ColPos[a+1]-2,container.Top+2+LD->ld_TextFont->tf_YSize-1);

								ibox.Left=container.Left+LD->ld_ColPos[a]+6;
								ibox.Top=container.Top+2;
								ibox.Width=LD->ld_ColPos[a+1]-LD->ld_ColPos[a]-12;
								ibox.Height=LD->ld_TextFont->tf_YSize+1;
								TextColor(cl,o,RP,columnstring,len,dri,&ibox);
//								SetAPen(RP,HLTextPen);
//								Move(RP,container.Left+LD->ld_ColPos[a]+6,container.Top+2+LD->ld_TextFont->tf_Baseline);
//								Text(RP,copytext,len);
								if(*ts=='\0') break;
								ts=stpblk(++ts);
							}
							FreeMem(columnstring,strlen(LD->ld_Format)+100);
						}
						FreeMem(copytext,strlen(LD->ld_Format)+100);
					}
				}
				break;
			case GREDRAW_TOGGLE:
				if(LD->ld_MultiSelect)
				{
					for(a=0+(LD->ld_Columns>1?1:0);(node!=LD->ld_ActualNode)&&(a<LD->ld_Visible);node=Tree_NextNode(node),a++);
					if(node==LD->ld_ActualNode)
						LineOut(cl,o,RP,node,&container,a,dri);
				}
				else
				{
					for(a=0+(LD->ld_Columns>1?1:0);Tree_NextNode(node)&&(a<LD->ld_Visible);node=Tree_NextNode(node),a++)
					{
						if((node==LD->ld_PrevSelected) && (node!=LD->ld_ActualNode)) LineOut(cl,o,RP,node,&container,a,dri);
						if((node!=LD->ld_PrevSelected) && (node==LD->ld_ActualNode))
						{
							node->ln_Flags|=LV_SELECT;
							LineOut(cl,o,RP,node,&container,a,dri);
							node->ln_Flags&=~LV_SELECT;
						}
					}
				}
				break;
		}
	}

	if(State==IDS_DISABLED)
	{
		container.Width = MAX( 1, container.Width );
		container.Height = MAX( 1, container.Height );

		SetDrMd(RP,JAM1);
		SetAfPt(RP, patterndata, 1);
		SetAPen(RP,dri->dri_Pens[SHADOWPEN]);

		RectFill(RP,  (LONG)container.Left, (LONG)container.Top,
			container.Left + container.Width-1,
			container.Top + container.Height-1);
		SetAfPt(RP, NULL, 0 );
	}

	InstallClipRegion(gpr->gpr_GInfo->gi_Window->WLayer,oldregion);
	if(region1) ClearRegion(region1);

	return( retval );
}

UWORD SelectedCol(struct LISTData *LD,UWORD x)
{
	UWORD a;
	for(a=1;a<LD->ld_Columns;a++)
	{
		if((x>=(LD->ld_ColPos[a]-2)) && (x<=(LD->ld_ColPos[a]+2))) return a;
	}
	return 0;
}

ULONG SAVEDS LIST_GOACTIVE(Class *cl,Object *o,struct gpInput *gpi )
{
	ULONG retval = GMR_MEACTIVE, Left, Top,SY,a,es,em,plustl;
	struct RastPort *rp;
	struct LISTData *LD = INST_DATA( cl, o );
	struct GadgetInfo *gi = gpi->gpi_GInfo;
	struct Gadget *gad = (struct Gadget *)o;
	struct Rectangle rect;
	struct IBox container;
	struct LVNode *node;
	STRPTR ts;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	/* Test if we are disabled. */
	if((gad->Flags & GFLG_DISABLED) || LD->ld_ReadOnly) return( GMR_NOREUSE );

	/* Call first our parent class. */
	DoSuperMethodA(cl, o, (Msg)gpi);

	LIST_GetGadgetRect( o, gpi->gpi_GInfo, &rect );
	container.Left = rect.MinX; container.Top = rect.MinY;
	container.Width = 1 + rect.MaxX - rect.MinX;
	container.Height = 1 + rect.MaxY - rect.MinY;

	/* Chech whether we were activated from mouse or keyboard. */
	LD->ld_ActiveFromMouse = (gpi->gpi_IEvent != NULL);

	/* Select this gadget. */
	gad->Flags |= GFLG_SELECTED;

	if(rp=ObtainGIRPort(gi))
	{
		Left = gi->gi_Domain.Left;
		Top = gi->gi_Domain.Top;

		SY=((gpi->gpi_Mouse.Y-Top-2)/(LD->ld_TextFont->tf_YSize));

		if(((LD->ld_Columns>1)&&(SY==0)) || ((LD->ld_Visible-(LD->ld_Columns>1?1:0))<SY))
		{
			if(((LD->ld_Columns>1)&&(SY==0)) && SelectedCol(LD,gpi->gpi_Mouse.X-Left))
			{
				LD->ld_EditCol=TRUE;
				LD->ld_PrevColPos=LD->ld_SelColPos=LD->ld_ColPos[SelectedCol(LD,gpi->gpi_Mouse.X-Left)];
				LD->ld_SelCol=SelectedCol(LD,gpi->gpi_Mouse.X-Left);
				SetDrMd(rp,COMPLEMENT);
				Move(rp,container.Left+LD->ld_SelColPos+Left,container.Top+2);
				Draw(rp,container.Left+LD->ld_SelColPos+Left,container.Top+container.Height-2);
				Move(rp,container.Left+LD->ld_SelColPos+1+Left,container.Top+2);
				Draw(rp,container.Left+LD->ld_SelColPos+1+Left,container.Top+container.Height-2);
				SetDrMd(rp,JAM1);
				ReleaseGIRPort( rp );
			}
			else
			{
				ReleaseGIRPort( rp );
				retval=GMR_NOREUSE;
			}
		}
		else
		{
			for(a=0+(LD->ld_Columns>1?1:0),node=LD->ld_TopNode;Tree_NextNode(node)&&(a<SY);node=Tree_NextNode(node),a++);
			if(node->ln_Succ)
			{
				if(LD->ld_MultiSelect)
				{
					LD->ld_Do=!(node->ln_Flags&LV_SELECT);
					if(LD->ld_Do) node->ln_Flags|=LV_SELECT;
					else node->ln_Flags&=~LV_SELECT;
				}
				LD->ld_PrevSelected=LD->ld_ActualNode;
				LD->ld_ActualNode=node;
				CurrentTime(&es,&em);
				if(LD->ld_IsTree || LD->ld_ShowTreeLines) plustl=LD->ld_TextFont->tf_YSize+4; else plustl=0;
				if((gpi->gpi_Mouse.X-Left>(plustl*node->ln_Indent+2) && gpi->gpi_Mouse.X-Left<(plustl*(node->ln_Indent+1)-2)) && node->ln_Sub)
				{
					if(node->ln_Flags&LV_OPEN) node->ln_Flags&=~LV_OPEN; else node->ln_Flags|=LV_OPEN;
					if(LD->ld_HookSub)
					{
						if(node->ln_Flags&LV_OPEN) CallHook(LD->ld_HookSub,o,TLV_OPENSUB,node);
						else CallHook(LD->ld_HookSub,o,TLV_CLOSESUB,node);
					}
					DoMethod( o, GM_RENDER, gi, rp, GREDRAW_REDRAW);
				}
				else
				{
					LD->ld_StringRedraw=FALSE;
					DoMethod( o, GM_RENDER, gi, rp, GREDRAW_TOGGLE);
					if(LD->ld_HookEdit)
					{
						if(LD->ld_StringObj)
						{
							RemoveGadget(LD->ld_StringWnd,(struct Gadget *)LD->ld_StringObj);
							DisposeObject(LD->ld_StringObj);
							LD->ld_StringObj=NULL;
						}
						LD->ld_StringRedraw=TRUE;
						if((LD->ld_PrevSelected==LD->ld_ActualNode) && (!DoubleClick(LD->ld_Secs,LD->ld_Mics,es,em)) && ((gpi->gpi_Mouse.X-Left)<LD->ld_ColPos[1]))
						{
							ts=node->ln_Name;
							for(a=0;a<LD->ld_Columns;a++)
							{
								ts=stptok(ts,LD->ld_StringBuffer,strlen(node->ln_Name)+1,LD->ld_Columns>1?(char *)LD->ld_TabChar:"\0");
								if(((gpi->gpi_Mouse.X-Left)>LD->ld_ColPos[a]) && ((gpi->gpi_Mouse.X-Left)<LD->ld_ColPos[a+1]))
									break;
								ts=stpblk(++ts);
							}
							LD->ld_StringColumn=a;
							if(CallHook(LD->ld_HookEdit,o,TLV_DOEDIT,LD->ld_ActualNode,LD->ld_StringBuffer,LD->ld_StringColumn)!=TLVCB_OK)
							{
								if(a!=0) plustl=0;
								LD->ld_Secs=LD->ld_Mics=0L;
								LD->ld_StringObj=NewObject(LD->ld_StringClass,NULL,
									GA_Left,LD->ld_ColPos[LD->ld_StringColumn]+container.Left+6+plustl*(node->ln_Indent+1),
									GA_Top,container.Top+SY*(LD->ld_TextFont->tf_YSize)+2,
									GA_Width,LD->ld_ColPos[LD->ld_StringColumn+1]-LD->ld_ColPos[a]-(plustl*(node->ln_Indent+1))-10,
									GA_Height,(LD->ld_TextFont->tf_YSize),
									STRINGA_Buffer,LD->ld_StringBuffer,
									STRINGA_TextVal,LD->ld_StringBuffer,
									STRINGA_MaxChars,256,
									STRINGA_WorkBuffer,LD->ld_StringBuffer,
									STRINGA_ExitHelp,TRUE,
									STRINGA_Pens,gi->gi_DrInfo->dri_Pens[HIGHLIGHTTEXTPEN],
									STRINGA_ActivePens,gi->gi_DrInfo->dri_Pens[HIGHLIGHTTEXTPEN],
									STRINGA_BufferPos,strlen(LD->ld_StringBuffer),
									TAG_USER+1,LD,
									TAG_DONE);
								LD->ld_StringWnd=gi->gi_Window;
								AddGadget(LD->ld_StringWnd,(struct Gadget *)LD->ld_StringObj,NULL);
								RefreshGList((struct Gadget *)LD->ld_StringObj,LD->ld_StringWnd,NULL,1);
								ActivateGadget((struct Gadget *)LD->ld_StringObj,LD->ld_StringWnd,NULL);
							}
						}
						else
							CurrentTime(&LD->ld_Secs,&LD->ld_Mics);
					}
				}
				LD->ld_Cnt=SY;
	
				ReleaseGIRPort( rp );
			}
			else
			{
				ReleaseGIRPort( rp );
				retval=GMR_NOREUSE;
			}
		}
	}
	else retval = GMR_NOREUSE;

	return(retval);
}

ULONG SAVEDS LIST_HANDLEINPUT(Class *cl,Object *o,struct gpInput *gpi )
{
	ULONG retval = GMR_MEACTIVE,Left,Top,SY,a;
	struct InputEvent *ie = gpi->gpi_IEvent;
	struct LISTData *LD = INST_DATA(cl, o);
	struct GadgetInfo *gi = gpi->gpi_GInfo;
	struct Rectangle rect;
	struct IBox container;
	struct RastPort *rp;
	struct LVNode *node;
	UWORD prev;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	if( LD->ld_ActiveFromMouse )
	{
		while( ie && (retval == GMR_MEACTIVE) )
		{
			switch( ie->ie_Class )
			{
				case IECLASS_TIMER: // Scroll listview
					break;
				case IECLASS_RAWMOUSE:
					switch(ie->ie_Code)
					{
						case SELECTUP:
							retval=GMR_NOREUSE;
							break;
						case MENUUP:
							retval=GMR_NOREUSE;
							break;
						case IECODE_NOBUTTON:
							if(rp=ObtainGIRPort(gi))
							{
								Left = gi->gi_Domain.Left;
								Top = gi->gi_Domain.Top;
								SY=((gpi->gpi_Mouse.Y-Top-2)/LD->ld_TextFont->tf_YSize);
								if(LD->ld_EditCol)
								{
									if(SY==0 && LD->ld_EditCol)
									{
										LIST_GetGadgetRect( o, gpi->gpi_GInfo, &rect );
										container.Left = rect.MinX; container.Top = rect.MinY;
										container.Width = 1 + rect.MaxX - rect.MinX;
										container.Height = 1 + rect.MaxY - rect.MinY;

										if(((gpi->gpi_Mouse.X-Left)>(LD->ld_ColPos[LD->ld_SelCol-1]+5)) && ((gpi->gpi_Mouse.X-Left)>5) && ((gpi->gpi_Mouse.X-Left)<container.Width-5)) LD->ld_SelColPos=gpi->gpi_Mouse.X-Left;
										SetDrMd(rp,COMPLEMENT);
										Move(rp,container.Left+LD->ld_PrevColPos+Left,container.Top+2);
										Draw(rp,container.Left+LD->ld_PrevColPos+Left,container.Top+container.Height-2);
										Move(rp,container.Left+LD->ld_PrevColPos+1+Left,container.Top+2);
										Draw(rp,container.Left+LD->ld_PrevColPos+1+Left,container.Top+container.Height-2);
										Move(rp,container.Left+LD->ld_SelColPos+Left,container.Top+2);
										Draw(rp,container.Left+LD->ld_SelColPos+Left,container.Top+container.Height-2);
										Move(rp,container.Left+LD->ld_SelColPos+1+Left,container.Top+2);
										Draw(rp,container.Left+LD->ld_SelColPos+1+Left,container.Top+container.Height-2);
										SetDrMd(rp,JAM1);
										LD->ld_PrevColPos=LD->ld_SelColPos;
									}
								}
								else
								{
									if(!(((LD->ld_Columns>1)&&(SY==0)) || ((LD->ld_Visible-(LD->ld_Columns>1?1:0))<SY)))
									{
										for(a=0+(LD->ld_Columns>1?1:0),node=LD->ld_TopNode;Tree_NextNode(node)&&(a<SY);node=Tree_NextNode(node),a++);
										if(node->ln_Succ)
										{
											prev=node->ln_Flags;
											if(LD->ld_MultiSelect)
											{
												if(LD->ld_Do) node->ln_Flags|=LV_SELECT;
												else node->ln_Flags&=~LV_SELECT;
												LD->ld_ActualNode=node;
												if(prev!=node->ln_Flags)
													DoMethod( o, GM_RENDER, gi, rp, GREDRAW_TOGGLE );
											}
											else
											{
												LD->ld_PrevSelected=LD->ld_ActualNode;
												LD->ld_ActualNode=node;
												DoMethod( o, GM_RENDER, gi, rp, GREDRAW_TOGGLE );
											}
										}
									}
								}
								ReleaseGIRPort( rp );
							}
							break;
					}
					break;
			}
			ie = ie->ie_NextEvent;
		}
	}
	return(retval);
}

ULONG SAVEDS STRG_GOINACTIVE(Class *cl,Object *o,struct gpGoInactive *gpgi )
{
	ULONG retval;
	struct STRGData *SD = INST_DATA( cl, o );
	struct LISTData *LD = SD->sd_LISTData;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA(cl, o, (Msg)gpgi);
	if(LD->ld_HookEdit) CallHook(LD->ld_HookEdit,o,TLV_EDIT,LD->ld_ActualNode,LD->ld_StringBuffer,LD->ld_StringColumn);

	SetAttrs(LD->ld_StringObj,GA_Top,-LD->ld_TextFont->tf_YSize-4,TAG_DONE);
	if(LD->ld_StringRedraw) RefreshGList((struct Gadget *)LD->ld_ListViewObj,LD->ld_StringWnd,NULL,1);
	return(retval);
}

ULONG SAVEDS LIST_GOINACTIVE(Class *cl,Object *o,struct gpGoInactive *gpgi )
{
	ULONG retval,a;
	struct LISTData *LD = INST_DATA( cl, o );
	struct RastPort *rp;
	struct DrawInfo *dri;
	struct Rectangle rect;
	struct IBox container;
	WORD delta;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA(cl, o, (Msg)gpgi);
/*	if(LD->ld_StringObj)
	{
		RemoveGadget(gpgi->gpgi_GInfo->gi_Window,(struct Gadget *)LD->ld_StringObj);
		DisposeObject(LD->ld_StringObj);
		LD->ld_StringObj=NULL;
//		if(LD->ld_HookEdit) CallHook(LD->ld_HookEdit,o,TLV_EDIT,LD->ld_ActualNode,LD->ld_StringBuffer);
		if(rp=ObtainGIRPort(gpgi->gpgi_GInfo))
		{
			DoMethod( o, GM_RENDER, gpgi->gpgi_GInfo, rp, GREDRAW_REDRAW );
			ReleaseGIRPort( rp );
		}
	}
*/
//	if(LD->ld_HookEdit) CallHook(LD->ld_HookEdit,o,TLV_EDIT,LD->ld_ActualNode,LD->ld_StringBuffer);

	if(LD->ld_EditCol)
	{
		LD->ld_EditCol=FALSE;
		delta=LD->ld_SelColPos-LD->ld_ColPos[LD->ld_SelCol];
		LD->ld_ColPos[LD->ld_SelCol]=LD->ld_SelColPos;
		for(a=LD->ld_SelCol+1;a<LD->ld_Columns;a++) LD->ld_ColPos[a]+=delta;
		if(rp=ObtainGIRPort(gpgi->gpgi_GInfo))
		{
			LIST_GetGadgetRect( o, gpgi->gpgi_GInfo, &rect );
			container.Left = rect.MinX; container.Top = rect.MinY;
			container.Width = 1 + rect.MaxX - rect.MinX;
			container.Height = 1 + rect.MaxY - rect.MinY;

			dri = gpgi->gpgi_GInfo->gi_DrInfo;
			SetAPen(rp,dri->dri_Pens[BACKGROUNDPEN]);
			RectFill(rp,  (LONG)container.Left+2, (LONG)container.Top+1,
				container.Left + container.Width-2,
				container.Top + container.Height-2);
			DoMethod( o, GM_RENDER, gpgi->gpgi_GInfo, rp, GREDRAW_REDRAW );
			ReleaseGIRPort( rp );
		}
	}
	((struct Gadget *)o)->Flags &= ~GFLG_SELECTED;

	return(retval);
}


void SAVEDS LIST_GetGadgetRect( Object *o,struct GadgetInfo *gi,struct Rectangle *rect )
{
	struct Gadget *gad = (struct Gadget *)o;
	LONG W, H;

	rect->MinX = gad->LeftEdge;
	rect->MinY = gad->TopEdge;
	W = gad->Width;
	H = gad->Height;

	if( gi )
	{
		if( gad->Flags & GFLG_RELRIGHT ) rect->MinX += gi->gi_Domain.Width - 1;
		if( gad->Flags & GFLG_RELBOTTOM ) rect->MinY += gi->gi_Domain.Height - 1;
		if( gad->Flags & GFLG_RELWIDTH ) W += gi->gi_Domain.Width;
		if( gad->Flags & GFLG_RELHEIGHT ) H += gi->gi_Domain.Height;
	}
	rect->MaxX = rect->MinX + W - (W > 0);
	rect->MaxY = rect->MinY + H - (H > 0);
}

void SAVEDS SPrintf(Class *cl, STRPTR buffer, STRPTR format, ...)
{
	putreg( REG_A6, cl->cl_UserData );
	geta4();

	RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", buffer);
}

void TextColor(Class *cl,Object *o,struct RastPort *RP,STRPTR text,ULONG len,struct DrawInfo *dri,struct IBox *container)
{
	ULONG flg,a,b,skipx;
	UBYTE Justify;

	SetAPen(RP,dri->dri_Pens[TEXTPEN]);
	flg=FS_NORMAL;
	SetSoftStyle(RP,flg,AskSoftStyle(RP));
	Justify=0;

	for(a=0,skipx=0;a<strlen(text);a++)
	{
		if(text[a]!='')
		{
			for(b=0;(text[a+b]!='') && (text[a+b]!='\0') && (b<len);b++);
			switch(Justify)
			{
				case 0: // Left justify
					Move(RP,container->Left+skipx,container->Top+RP->TxBaseline);
					break;
				case 1: // Center
					Move(RP,container->Left+skipx+container->Width/2-TextLength(RP,text+a,b)/2,container->Top+RP->TxBaseline);
					break;
				case 2: // Right justify
					Move(RP,container->Left+skipx+container->Width-TextLength(RP,text+a,b),container->Top+RP->TxBaseline);
					break;
			}
			Text(RP,text+a,b);
			Move(RP,container->Left+skipx,container->Top+RP->TxBaseline);
			skipx+=TextLength(RP,text+a,b);
			for(b=0;(text[a+b]!='') && (text[a+b]!='\0');b++);
			a+=b-1;
		}
		else
		{
			a++;
			switch(text[a])
			{
				case 'p': flg=FS_NORMAL; SetSoftStyle(RP,flg,AskSoftStyle(RP)); break;				// Plain
				case 'i': flg|=FSF_ITALIC; SetSoftStyle(RP,flg,AskSoftStyle(RP)); break;			// Italic
				case 'u': flg|=FSF_UNDERLINED; SetSoftStyle(RP,flg,AskSoftStyle(RP)); break;	// Underline
				case 'b': flg|=FSF_BOLD; SetSoftStyle(RP,flg,AskSoftStyle(RP)); break;				// Bold
	
				case 'l': Justify=0; break;											// Left justify
				case 'c': Justify=1; break;											// Center
				case 'r': Justify=2; break;											// Right justify
	
				case '0': SetAPen(RP,0); break;									// Set color
				case '1': SetAPen(RP,1); break;
				case '2': SetAPen(RP,2); break;
				case '3': SetAPen(RP,3); break;
				case '4': SetAPen(RP,4); break;
				case '5': SetAPen(RP,5); break;
				case '6': SetAPen(RP,6); break;
				case '7': SetAPen(RP,7); break;
				case '8': SetAPen(RP,8); break;
				case '9': SetAPen(RP,9); break;
			}
		}
	}
}

void LineOut(Class *cl,Object *o,struct RastPort *RP,struct LVNode *node,struct IBox *container,ULONG linenum,struct DrawInfo *dri)
{
	ULONG len,plus=0,plustl,treeoffset,columnwidth,textcolumnwidth,a,b,col,w;
	struct LISTData *LD = INST_DATA( cl, o );
	struct TextExtent temp_te;
	STRPTR ts,columnstring,copytext;
	UBYTE TextPen,FillPen,BackgroundPen;
	UWORD x,y;
	struct LVNode *np;
	struct IBox ibox;

	if(LD->ld_HookDraw)
	{
		struct Rectangle rect;
		rect.MinX=container->Left+1;
		rect.MinY=container->Top+2+linenum*LD->ld_TextFont->tf_YSize;
		rect.MaxX=container->Left+container->Width-1;
		rect.MaxY=container->Top+2+(linenum+1)*LD->ld_TextFont->tf_YSize-1;
		if(CallHook(LD->ld_HookDraw,o,TLV_DRAW,node,RP,dri,rect,LD->ld_ColPos)==TLVCB_OK) return;
	}

	if( dri )
	{
		TextPen = dri->dri_Pens[TEXTPEN];
		FillPen = dri->dri_Pens[FILLPEN];
		BackgroundPen = dri->dri_Pens[BACKGROUNDPEN];
	}
	else
	{
 		TextPen = 1;
		FillPen = 3;
		BackgroundPen = 0;
	}

	if(LD->ld_IsTree || LD->ld_ShowTreeLines) plustl=LD->ld_TextFont->tf_YSize+4; else plustl=0;

	ts=node->ln_Name;
	if(columnstring=AllocMem(strlen(node->ln_Name)+1,MEMF_ANY))
	{
		if(copytext=AllocMem(strlen(node->ln_Name)+1,MEMF_ANY))
		{
			col=0;
			while(TRUE)
			{
				treeoffset=((col==0)?(plustl*(node->ln_Indent+1)):0);
				columnwidth=LD->ld_ColPos[col+1]-LD->ld_ColPos[col];
				textcolumnwidth=columnwidth-12-treeoffset;

				ts=stptok(ts,columnstring,strlen(node->ln_Name)+1,LD->ld_Columns>1?(char *)LD->ld_TabChar:"\0");
				for(a=0,b=0;a<(strlen(columnstring)+1);a++) if(columnstring[a]!='') copytext[b++]=columnstring[a]; else a++;
				len=TextFit(RP,copytext,strlen(copytext),&temp_te,NULL,1,textcolumnwidth, 1+LD->ld_TextFont->tf_YSize);
				if((LD->ld_MultiSelect)?(node->ln_Flags&LV_SELECT):(LD->ld_ActualNode==node)) SetAPen(RP,FillPen); else SetAPen(RP,BackgroundPen);
				RectFill(RP,container->Left+plus+2,container->Top+2+linenum*LD->ld_TextFont->tf_YSize,container->Left+plus+columnwidth-1,container->Top+2+(linenum+1)*LD->ld_TextFont->tf_YSize-1);

				ibox.Left=container->Left+6+plus+treeoffset;
				ibox.Top=container->Top+2+linenum*LD->ld_TextFont->tf_YSize;
				ibox.Width=textcolumnwidth;
				ibox.Height=LD->ld_TextFont->tf_YSize+1;
				TextColor(cl,o,RP,columnstring,len,dri,&ibox);

				if(*ts=='\0') break;
				ts=stpblk(++ts);
				plus+=columnwidth;
				col++;
			}
			FreeMem(copytext,strlen(node->ln_Name)+1);
		}
		FreeMem(columnstring,strlen(node->ln_Name)+1);
	}

	SetDrMd(RP,JAM1);
	if(LD->ld_ShowTreeLines && LD->ld_ColPos[1]>(2*(LD->ld_TextFont->tf_YSize-5)))
	{
		SetAPen(RP,dri->dri_Pens[SHINEPEN]);
		if(LD->ld_TextFont->tf_YSize>8)
		{
			y=container->Top+2+linenum*LD->ld_TextFont->tf_YSize+2;
			w=(LD->ld_TextFont->tf_YSize-4)-1;
		}
		else
		{
			y=container->Top+2+linenum*LD->ld_TextFont->tf_YSize+1;
			w=(LD->ld_TextFont->tf_YSize-2)-1;
		}
		for(b=0;b<node->ln_Indent;b++)
		{
			x=container->Left+6+(b*plustl);
			for(col=0,np=node;col<node->ln_Indent-b;col++,np=np->ln_Parent);
			if(np->ln_Parent)
			{
				if(np->ln_Parent!=np->ln_Succ)
				{
					Move(RP,x+w/2,y-2);					// Draw '|'
					Draw(RP,x+w/2,y+w+2);
				}
			}
			else
			{
				if(np->ln_Succ->ln_Succ)
				{
					Move(RP,x+w/2,y-2);					// Draw '|'
					Draw(RP,x+w/2,y+w+2);
				}
			}
		}
		x=container->Left+6+(node->ln_Indent*plustl);
		if(node->ln_Sub)
		{
			Move(RP,x,y);									// Draw rectangle
			Draw(RP,x+w,y);
			Draw(RP,x+w,y+w);
			Draw(RP,x,y+w);
			Draw(RP,x,y);
			Move(RP,x+w/2,y-2);						// Draw '|' above rectangle
			Draw(RP,x+w/2,y);
			if((node->ln_Parent && node->ln_Succ!=node->ln_Parent)||(!node->ln_Parent && node->ln_Succ->ln_Succ))
			{															// Draw '|' below rectangle
				Move(RP,x+w/2,y+w);
				Draw(RP,x+w/2,y+w+2);
			}
			Move(RP,x+w,y+w/2);						// Draw '-' at right of rectangle
			Draw(RP,x+w+2,y+w/2);
			if(node->ln_Flags&LV_OPEN)
			{															// Draw '-'
				Move(RP,x+3,y+w/2);
				Draw(RP,x+w-3,y+w/2);
			}
			else
			{															// Draw '+'
				Move(RP,x+3,y+w/2);
				Draw(RP,x+w-3,y+w/2);
				Move(RP,x+w/2,y+3);
				Draw(RP,x+w/2,y+w-3);
			}
		}
		else
		{
			if((node->ln_Parent && node->ln_Succ==node->ln_Parent)||(!node->ln_Parent && !node->ln_Succ->ln_Succ))
			{															// Draw '|'
				Move(RP,x+w/2,y-2);
				Draw(RP,x+w/2,y+w/2);
			}
			else
			{															// Draw half '|'
				Move(RP,x+w/2,y-2);
				Draw(RP,x+w/2,y+w+2);
			}
			Move(RP,x+w/2,y+w/2);					// Draw '-'
			Draw(RP,x+w+2,y+w/2);
		}
	}
}
