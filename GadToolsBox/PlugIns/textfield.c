/*
** TextField PlugIn
*/

#include"includes.h"
#include"textfield.h"


/*
** OwnTag definitions
*/

#define OWNTAG_Undo (TAG_USER+1)
#define OWNTAG_Clipboard (TAG_USER+2)

/*
** Default Tags
*/

struct TagItem TextFieldTags[]=
{
	GA_Disabled,FALSE,
	GA_TabCycle,FALSE,
	TEXTFIELD_ReadOnly,FALSE,
	TEXTFIELD_Partial,FALSE,
	TEXTFIELD_BlockCursor,FALSE,
	TEXTFIELD_VCenter,FALSE,
	TEXTFIELD_PassCommand,FALSE,
	TEXTFIELD_NonPrintChars,FALSE,
	TEXTFIELD_MaxSizeBeep,FALSE,
	TEXTFIELD_NoGhost,FALSE,
	TEXTFIELD_Modified,FALSE,
	TEXTFIELD_UserAlign,FALSE,
	TEXTFIELD_RuledPaper,FALSE,
	TEXTFIELD_Inverted,FALSE,
	TEXTFIELD_Delimiters,NULL,
	TEXTFIELD_AcceptChars,NULL,
	TEXTFIELD_RejectChars,NULL,
	TEXTFIELD_BlinkRate,0,
	TEXTFIELD_MaxSize,0,
	TEXTFIELD_TabSpaces,0,
	TEXTFIELD_Spacing,0,
	TEXTFIELD_Border,TEXTFIELD_BORDER_NONE,
	TEXTFIELD_Alignment,TEXTFIELD_ALIGN_LEFT,
	TAG_DONE,0
};

UBYTE TextFieldTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,
	TAGTYPE_TEXT,TAGTYPE_TEXT,TAGTYPE_TEXT,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem TextFieldOwnTags[]=
{
	OWNTAG_Undo,FALSE,
	OWNTAG_Clipboard,FALSE,
	TAG_DONE,0
};

UBYTE TextFieldOwnTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 23
#define GSIZE_NUMOWNTAGS 4

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGBO_General[];

struct Page PAGES_PROPERTIES_PG_TFLD_PAGES[]=
{
	&PAGES_PROPERTIES_PG_TFLD_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PG_TFLD_PAGES[2], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PG_TFLD_PAGES[3], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG TEXTFIELD_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=TEXTFIELD_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=TEXTFIELD_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_TFLD_Flags();
			OpenWindow_PG_TFLD_Flags2();
			OpenWindow_PG_TFLD_Var();
			PAGES_PROPERTIES_PG_TFLD_PAGES[0].FirstGadget=PGBO_GeneralGList;
			PAGES_PROPERTIES_PG_TFLD_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L0_GAD);
			PAGES_PROPERTIES_PG_TFLD_PAGES[1].FirstGadget=PG_TFLD_FlagsGList;
			PAGES_PROPERTIES_PG_TFLD_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L27_GAD);
			PAGES_PROPERTIES_PG_TFLD_PAGES[2].FirstGadget=PG_TFLD_Flags2GList;
			PAGES_PROPERTIES_PG_TFLD_PAGES[2].Name=GetCatString(MSG_PROPERTIES_PAGES_L28_GAD);
			PAGES_PROPERTIES_PG_TFLD_PAGES[3].FirstGadget=PG_TFLD_VarGList;
			PAGES_PROPERTIES_PG_TFLD_PAGES[3].Name=GetCatString(MSG_PROPERTIES_PAGES_L29_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_TFLD_PAGES[3].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_TFLD_PAGES[2].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_TFLD_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_TFLD_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_TFLD_Var();
			CloseWindow_PG_TFLD_Flags2();
			CloseWindow_PG_TFLD_Flags();
			break;
		case OM_NEW:
			retval=TEXTFIELD_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_CREATE:
			retval=TEXTFIELD_CREATE_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_DELETE:
			retval=TEXTFIELD_DELETE_Gadget(cl,o,msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG TEXTFIELD_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PG_TFLD_FlagsGadgets[GD_PG_TFLD_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_FlagsGadgets[GD_PG_TFLD_FLAGS_TABCYCLE],GetTagData(GA_TabCycle,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_FlagsGadgets[GD_PG_TFLD_FLAGS_READONLY],GetTagData(TEXTFIELD_ReadOnly,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_FlagsGadgets[GD_PG_TFLD_FLAGS_INVERTED],GetTagData(TEXTFIELD_Inverted,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_FlagsGadgets[GD_PG_TFLD_FLAGS_PARTIAL],GetTagData(TEXTFIELD_Partial,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_FlagsGadgets[GD_PG_TFLD_FLAGS_BLOCKCURSOR],GetTagData(TEXTFIELD_BlockCursor,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_FlagsGadgets[GD_PG_TFLD_FLAGS_VCENTER],GetTagData(TEXTFIELD_VCenter,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_FlagsGadgets[GD_PG_TFLD_FLAGS_PASSCOMMAND],GetTagData(TEXTFIELD_PassCommand,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_FlagsGadgets[GD_PG_TFLD_FLAGS_CLIPBOARD],GetTagData(OWNTAG_Clipboard,0,ng->ng_OwnTags));
	SetCheckMark(PropsWnd,PG_TFLD_FlagsGadgets[GD_PG_TFLD_FLAGS_UNDO],GetTagData(OWNTAG_Undo,0,ng->ng_OwnTags));

	if(active==2) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_USERALIGN],GetTagData(TEXTFIELD_UserAlign,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_NOGHOST],GetTagData(TEXTFIELD_NoGhost,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_MODIFIED],GetTagData(TEXTFIELD_Modified,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_RULEDPAPER],GetTagData(TEXTFIELD_RuledPaper,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_NONPRINTCHARS],GetTagData(TEXTFIELD_NonPrintChars,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_MAXSIZEBEEP],GetTagData(TEXTFIELD_MaxSizeBeep,0,ng->ng_Tags));
	switch(GetTagData(TEXTFIELD_Alignment,0,ng->ng_Tags))
	{
		case TEXTFIELD_ALIGN_LEFT: SetCycle(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_ALIGN],0); break;
		case TEXTFIELD_ALIGN_RIGHT: SetCycle(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_ALIGN],1); break;
		case TEXTFIELD_ALIGN_CENTER: SetCycle(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_ALIGN],2); break;
	}
	switch(GetTagData(TEXTFIELD_Border,0,ng->ng_Tags))
	{
		case TEXTFIELD_BORDER_NONE: SetCycle(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_BORDER],0); break;
		case TEXTFIELD_BORDER_BEVEL: SetCycle(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_BORDER],1); break;
		case TEXTFIELD_BORDER_DOUBLEBEVEL: SetCycle(PropsWnd,PG_TFLD_Flags2Gadgets[GD_PG_TFLD_FLAGS_BORDER],2); break;
	}

	if(active==3) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetInteger(PropsWnd,PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_BLINKRATE],GetTagData(TEXTFIELD_BlinkRate,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_MAXSIZE],GetTagData(TEXTFIELD_MaxSize,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_TABSPACES],GetTagData(TEXTFIELD_TabSpaces,0,ng->ng_Tags));
	SetInteger(PropsWnd,PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_SPACING],GetTagData(TEXTFIELD_Spacing,0,ng->ng_Tags));
	SetString(PropsWnd,PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_DELIMITERS],GetTagData(TEXTFIELD_Delimiters,0,ng->ng_Tags));
	SetString(PropsWnd,PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_ACCEPTCHARS],GetTagData(TEXTFIELD_AcceptChars,0,ng->ng_Tags));
	SetString(PropsWnd,PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_REJECTCHARS],GetTagData(TEXTFIELD_RejectChars,0,ng->ng_Tags));
	return TRUE;
}

ULONG TEXTFIELD_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	FindTagItem(TEXTFIELD_BlinkRate,ng->ng_Tags)->ti_Data=GetInteger(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_BLINKRATE]);
	FindTagItem(TEXTFIELD_MaxSize,ng->ng_Tags)->ti_Data=GetInteger(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_MAXSIZE]);
	FindTagItem(TEXTFIELD_TabSpaces,ng->ng_Tags)->ti_Data=GetInteger(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_TABSPACES]);
	FindTagItem(TEXTFIELD_Spacing,ng->ng_Tags)->ti_Data=GetInteger(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_SPACING]);

	if(FindTagItem(TEXTFIELD_Delimiters,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(TEXTFIELD_Delimiters,ng->ng_Tags)->ti_Data);
	FindTagItem(TEXTFIELD_Delimiters,ng->ng_Tags)->ti_Data=NULL;
	if(strlen(GetString(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_DELIMITERS]))>0)
	{
		if(FindTagItem(TEXTFIELD_Delimiters,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(strlen(GetString(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_DELIMITERS]))+1,MEMF_ANY|MEMF_CLEAR))
			strcpy((char *)FindTagItem(TEXTFIELD_Delimiters,ng->ng_Tags)->ti_Data,GetString(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_DELIMITERS]));
	}

	if(FindTagItem(TEXTFIELD_AcceptChars,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(TEXTFIELD_AcceptChars,ng->ng_Tags)->ti_Data);
	FindTagItem(TEXTFIELD_AcceptChars,ng->ng_Tags)->ti_Data=NULL;
	if(strlen(GetString(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_ACCEPTCHARS]))>0)
	{
		if(FindTagItem(TEXTFIELD_AcceptChars,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(strlen(GetString(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_ACCEPTCHARS]))+1,MEMF_ANY|MEMF_CLEAR))
			strcpy((char *)FindTagItem(TEXTFIELD_AcceptChars,ng->ng_Tags)->ti_Data,GetString(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_ACCEPTCHARS]));
	}

	if(FindTagItem(TEXTFIELD_RejectChars,ng->ng_Tags)->ti_Data) MyFreeVec((APTR)FindTagItem(TEXTFIELD_RejectChars,ng->ng_Tags)->ti_Data);
	FindTagItem(TEXTFIELD_RejectChars,ng->ng_Tags)->ti_Data=NULL;
	if(strlen(GetString(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_REJECTCHARS]))>0)
	{
		if(FindTagItem(TEXTFIELD_RejectChars,ng->ng_Tags)->ti_Data=(ULONG)MyAllocVec(strlen(GetString(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_REJECTCHARS]))+1,MEMF_ANY|MEMF_CLEAR))
			strcpy((char *)FindTagItem(TEXTFIELD_RejectChars,ng->ng_Tags)->ti_Data,GetString(PG_TFLD_VarGadgets[GD_PG_TFLD_VAR_REJECTCHARS]));
	}
	return TRUE;
}

ULONG TEXTFIELD_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_BOOPSI,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)TextFieldTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)TextFieldOwnTagsTypes,
		GAOBJ_ID,TEXTFIELD_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_TFLD_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);

		CopyTagItems(TextFieldTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(TextFieldOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);

		return (ULONG)obj;
	}
	return FALSE;
}

ULONG TEXTFIELD_CREATE_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
{
	struct TagItem *ti;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	WORD x,y,w,h;

	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
	
	if(ng->ng_RelX) x=-(nw->nw_Window->Width-nw->nw_Window->BorderLeft-ng->ng_X-1);
	else x=piCX(o);
	if(ng->ng_RelY) y=-(nw->nw_Window->Height-nw->nw_Window->BorderTop-ng->ng_Y-1);
	else y=piCY(o);
	if(ng->ng_RelWidth) w=-(nw->nw_Window->BorderLeft+nw->nw_Window->BorderRight+(nw->nw_Window->Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight-ng->ng_Width)-1);
	else w=piCW(o)+1;
	if(ng->ng_RelHeight) h=-(nw->nw_Window->BorderTop+nw->nw_Window->BorderBottom+(nw->nw_Window->Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom-ng->ng_Height)-1);
	else h=piCH(o)+1;

	return (ULONG) (ng->ng_Gadget=NewObject(TextFieldClass, NULL,
									ng->ng_RelX?GA_RelRight:GA_Left,x,
									ng->ng_RelY?GA_RelBottom:GA_Top,y,
									ng->ng_RelWidth?GA_RelWidth:GA_Width,w,
									ng->ng_RelHeight?GA_RelHeight:GA_Height,h,
									(msg->Window) ? GA_Previous : TAG_IGNORE, msg->Window,
									GA_ID,0,
									TAG_MORE,ti));
}

ULONG TEXTFIELD_DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	if(ng->ng_Gadget && ng->ng_Window) RemoveGadget(((struct Node_Window *)OBJECT_DATA(ng->ng_Window))->nw_Window,ng->ng_Gadget);
	if(ng->ng_Gadget) DisposeObject(ng->ng_Gadget);
	ng->ng_Gadget=NULL;
	return piDoSuperMethodA(cl,o,msg);
}

int FN_PG_TFLD_VAR_DELIMITERS_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_VAR_ACCEPTCHARS_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_VAR_REJECTCHARS_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_VAR_BLINKRATE_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_VAR_MAXSIZE_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_VAR_TABSPACES_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_VAR_SPACING_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_READONLY_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_ReadOnly,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_PARTIAL_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_Partial,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_BLOCKCURSOR_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_BlockCursor,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_VCENTER_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_VCenter,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_PASSCOMMAND_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_PassCommand,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_NONPRINTCHARS_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_NonPrintChars,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_TABCYCLE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_TabCycle,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_MAXSIZEBEEP_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_MaxSizeBeep,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_NOGHOST_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_NoGhost,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_MODIFIED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_Modified,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_USERALIGN_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_UserAlign,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_RULEDPAPER_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_RuledPaper,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_INVERTED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(TEXTFIELD_Inverted,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_UNDO_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_OwnTags,(ULONG *)&ti);
	FindTagItem(OWNTAG_Undo,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_CLIPBOARD_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_OwnTags,(ULONG *)&ti);
	FindTagItem(OWNTAG_Clipboard,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_BORDER_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	switch(PropertiesMsg.Code)
	{
		case 0: FindTagItem(TEXTFIELD_Border,ti)->ti_Data=TEXTFIELD_BORDER_NONE; break;
		case 1: FindTagItem(TEXTFIELD_Border,ti)->ti_Data=TEXTFIELD_BORDER_BEVEL; break;
		case 2: FindTagItem(TEXTFIELD_Border,ti)->ti_Data=TEXTFIELD_BORDER_DOUBLEBEVEL; break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_TFLD_FLAGS_ALIGN_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	switch(PropertiesMsg.Code)
	{
		case 0: FindTagItem(TEXTFIELD_Alignment,ti)->ti_Data=TEXTFIELD_ALIGN_LEFT; break;
		case 1: FindTagItem(TEXTFIELD_Alignment,ti)->ti_Data=TEXTFIELD_ALIGN_RIGHT; break;
		case 2: FindTagItem(TEXTFIELD_Alignment,ti)->ti_Data=TEXTFIELD_ALIGN_CENTER; break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
