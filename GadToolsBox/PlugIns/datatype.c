/*
** DataType PlugIn
*/

#include"includes.h"
#include"datatype.h"
#include<datatypes/pictureclass.h>

/*
** Defines
*/

/*
** OwnTag definitions
*/

#define OWNTAG_Object (TAG_USER+1)

/*
** Default Tags
*/

struct TagItem DataTypeTags[]=
{
	GA_Disabled,FALSE,
	DTA_SourceType,DTST_FILE,
	OBP_Precision,PRECISION_EXACT,
	PDTA_Screen,NULL,
	PDTA_Remap,TRUE,
	//DTA_Handle,NULL,
	TAG_DONE,0
};

UBYTE DataTypeTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NONE,TAGTYPE_NUMBER,TAGTYPE_DONE
};

struct TagItem DataTypeOwnTags[]=
{
	OWNTAG_Object,NULL,
	TAG_DONE,0
};

UBYTE DataTypeOwnTagsTypes[]=
{
	TAGTYPE_TEXT,TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 5
#define GSIZE_NUMOWNTAGS 1

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGBO_General[];

struct Page PAGES_PROPERTIES_PG_DTYP_PAGES[]=
{
	&PAGES_PROPERTIES_PG_DTYP_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG DATATYPE_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=DATATYPE_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=DATATYPE_APPLY_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_DTYP_Flags();
			PAGES_PROPERTIES_PG_DTYP_PAGES[0].FirstGadget=PGBO_GeneralGList;
			PAGES_PROPERTIES_PG_DTYP_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L0_GAD);
			PAGES_PROPERTIES_PG_DTYP_PAGES[1].FirstGadget=PG_DTYP_FlagsGList;
			PAGES_PROPERTIES_PG_DTYP_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L6_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_DTYP_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_DTYP_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_DTYP_Flags();
			break;
		case OM_NEW:
			retval=DATATYPE_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_CREATE:
			retval=DATATYPE_CREATE_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_DELETE:
			retval=DATATYPE_DELETE_Gadget(cl,o,msg);
			break;
		case OM_RENDER:
			retval=DATATYPE_RENDER_Gadget(cl,o,msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG DATATYPE_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PG_DTYP_FlagsGadgets[GD_PG_DTYP_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetString(PropsWnd,PG_DTYP_FlagsGadgets[GD_PG_DTYP_FLAGS_OBJECT],GetTagData(OWNTAG_Object,0,ng->ng_OwnTags));
	return TRUE;
}

ULONG DATATYPE_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);

	piDoSuperMethodA(cl,o,msg);

	if(FindTagItem(OWNTAG_Object,ng->ng_OwnTags)->ti_Data) MyFreeVec((APTR)FindTagItem(OWNTAG_Object,ng->ng_OwnTags)->ti_Data);
	FindTagItem(OWNTAG_Object,ng->ng_OwnTags)->ti_Data=NULL;
	if(FindTagItem(OWNTAG_Object,ng->ng_OwnTags)->ti_Data=(ULONG)MyAllocVec(strlen(GetString(PG_DTYP_FlagsGadgets[GD_PG_DTYP_FLAGS_OBJECT]))+1,MEMF_PUBLIC))
		strcpy((char *)FindTagItem(OWNTAG_Object,ng->ng_OwnTags)->ti_Data,GetString(PG_DTYP_FlagsGadgets[GD_PG_DTYP_FLAGS_OBJECT]));
	return TRUE;
}

ULONG DATATYPE_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_BOOPSI,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)DataTypeTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)DataTypeOwnTagsTypes,
		GAOBJ_ID,DATATYPE_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_DTYP_PAGES,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);

		CopyTagItems(DataTypeTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(DataTypeOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);

		return (ULONG)obj;
	}
	return FALSE;
}

ULONG DATATYPE_CREATE_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
{
	struct TagItem *ti,*oti;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	WORD x,y,w,h;

	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
	piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&oti);
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

	FindTagItem(PDTA_Screen,ti)->ti_Data=(ULONG)Scr;

	return (ULONG) (ng->ng_Gadget=(struct Gadget *)NewDTObject((APTR)GetTagData(OWNTAG_Object,0,oti),
									ng->ng_RelX?GA_RelRight:GA_Left,x,
									ng->ng_RelY?GA_RelBottom:GA_Top,y,
									ng->ng_RelWidth?GA_RelWidth:GA_Width,w,
									ng->ng_RelHeight?GA_RelHeight:GA_Height,h,
									(msg->Window) ? GA_Previous : TAG_IGNORE, msg->Window,
									GA_ID, 0,
									TAG_MORE,ti));
}

ULONG DATATYPE_DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	if(ng->ng_Gadget && ng->ng_Window) RemoveDTObject(((struct Node_Window *)OBJECT_DATA(ng->ng_Window))->nw_Window,(Object *)ng->ng_Gadget);
	if(ng->ng_Gadget) DisposeDTObject((Object *)ng->ng_Gadget);
	ng->ng_Gadget=NULL;
	return piDoSuperMethodA(cl,o,msg);
}

extern DrawRect(struct RastPort *,UWORD,UWORD,BOOL);

ULONG DATATYPE_RENDER_Gadget(piClass *cl,piObject *o,Msg msg)
{
	UWORD x,y,w,h;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	BOOL fill=FALSE;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);

	x=piCX(o);
	y=piCY(o);
	w=piCW(o);
	h=piCH(o);

	SetAPen(nw->nw_Window->RPort,DrawInfo->dri_Pens[BACKGROUNDPEN]);
	SetDrMd(nw->nw_Window->RPort,JAM1);
	RectFill(nw->nw_Window->RPort,x,y,x+w,y+h);

	SetAPen(nw->nw_Window->RPort,DrawInfo->dri_Pens[SHADOWPEN]);
	SetDrMd(nw->nw_Window->RPort,JAM1);
	Move(nw->nw_Window->RPort,x,y);
	Draw(nw->nw_Window->RPort,x+w,y);
	Draw(nw->nw_Window->RPort,x+w,y+h);
	Draw(nw->nw_Window->RPort,x,y+h);
	Draw(nw->nw_Window->RPort,x,y);
	Draw(nw->nw_Window->RPort,x+w,y+h);
	Move(nw->nw_Window->RPort,x+w,y);
	Draw(nw->nw_Window->RPort,x,y+h);
	Move(nw->nw_Window->RPort,x+10,y+h-10);
	Text(nw->nw_Window->RPort,"DT Obj",5);

	if(ng->ng_Selected && GUI.gui_Mode==MODE_GADGET)
	{
		SetAPen(nw->nw_Window->RPort,1);
		SetDrMd(nw->nw_Window->RPort,COMPLEMENT);
		if(CurrentGadget==o) fill=TRUE;
		DrawRect(nw->nw_Window->RPort,x,y,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x+(w/2)-2,y,fill);
		DrawRect(nw->nw_Window->RPort,x+(w/2)-2,y+h-5,fill);
		DrawRect(nw->nw_Window->RPort,x,y+(h/2)-2,fill);
		DrawRect(nw->nw_Window->RPort,x+w-5,y+(h/2)-2,fill);
	}
	return TRUE;
}

int FN_PG_DTYP_FLAGS_OBJECT_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_DTYP_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

/*
int FN_PG_DTYP_FLAGS_TYPE_Clicked( void )
{
	struct TagItem *ti;

	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	switch(PropertiesMsg.Code)
	{
		case 0: FindTagItem(DTA_SourceType,ti)->ti_Data=DTST_FILE; break;
		case 1: FindTagItem(DTA_SourceType,ti)->ti_Data=DTST_CLIPBOARD; break;
		case 2: FindTagItem(DTA_SourceType,ti)->ti_Data=DTST_RAM; break;
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
*/

int FN_PG_DTYP_FLAGS_OK_Clicked( void )
{
	struct TagItem *ti;
	struct Node_Gadget *ng;

	piGetAttr(CurrentGadget,GAOBJ_OwnTags,(ULONG *)&ti);
	piGetAttr(CurrentGadget,OBJ_Node,(ULONG *)&ng);
//	strcpy(ng->ng_Label,GetString(PropertiesGadgets[GD_PG_DTYP_FLAGS_LABEL]));
//	if(FindTagItem(OWNTAG_Object,ti)->ti_Data) MyFreeVec((APTR)FindTagItem(OWNTAG_Object,ti)->ti_Data);
//	FindTagItem(OWNTAG_Object,ti)->ti_Data=NULL;
//	if(FindTagItem(OWNTAG_Object,ti)->ti_Data=(ULONG)MyAllocVec(strlen(GetString(PropertiesGadgets[GD_PG_DTYP_FLAGS_OBJECT]))+1,MEMF_PUBLIC))
//		strcpy((char *)FindTagItem(OWNTAG_Object,ti)->ti_Data,GetString(PropertiesGadgets[GD_PG_DTYP_FLAGS_OBJECT]));
	return 2;
}
