/*
** Page PlugIn
*/

#include"includes.h"
#include"page.h"

/*
** Defines
*/

/*
** OwnTag definitions
*/

#define OWNTAG_Pages (TAG_USER+1)
#define OWNTAG_ClearBBox (TAG_USER+2)

/*
** Default Tags
*/

struct TagItem PageTags[]=
{
	TAG_IGNORE,NULL, //PAGE_Underscore,'_',
	GA_Disabled,FALSE,
	PAGE_Layout,PGLYT_SIMPLE,
	PAGE_ActiveStyle,PGAST_NORMAL,
	PAGE_Active,0,
	PAGE_Pages,NULL,
	PAGE_ClearBBox, NULL,
	TAG_DONE,0
};

UBYTE PageTagsTypes[]=
{
	TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NUMBER,TAGTYPE_NONE,TAGTYPE_NONE,TAGTYPE_DONE
};

struct TagItem PageOwnTags[]=
{
	OWNTAG_Pages,NULL,
	TAG_DONE,0
};

UBYTE PageOwnTagsTypes[]=
{
	TAGTYPE_LIST,TAGTYPE_DONE
};

#define GSIZE_NUMTAGS 7
#define GSIZE_NUMOWNTAGS 1

struct Node_Gadget *PageGadgetGen=NULL;

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGBO_General[];

struct Page PAGES_PROPERTIES_PG_PAGE_PAGES[]=
{
	&PAGES_PROPERTIES_PG_PAGE_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	&PAGES_PROPERTIES_PG_PAGE_PAGES[2], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Create PlugIn
*/

ULONG PAGE_Dispatcher_Gadget(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=PAGE_INIT_Gadget(cl,o,msg);
			break;
		case OM_APPLY:
			retval=PAGE_APPLY_Gadget(cl,o,msg);
			break;
		case OM_DEFAULT:
			retval=PAGE_DEFAULT_Gadget(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PG_PAGE_Flags();
			OpenWindow_PG_PAGE_Lists();
			PAGES_PROPERTIES_PG_PAGE_PAGES[0].FirstGadget=PGBO_GeneralGList;
			PAGES_PROPERTIES_PG_PAGE_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L0_GAD);
			PAGES_PROPERTIES_PG_PAGE_PAGES[1].FirstGadget=PG_PAGE_FlagsGList;
			PAGES_PROPERTIES_PG_PAGE_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L14_GAD);
			PAGES_PROPERTIES_PG_PAGE_PAGES[2].FirstGadget=PG_PAGE_ListsGList;
			PAGES_PROPERTIES_PG_PAGE_PAGES[2].Name=GetCatString(MSG_PROPERTIES_PAGES_L15_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PG_PAGE_PAGES[2].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_PAGE_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PG_PAGE_PAGES[0].FirstGadget=NULL;
			CloseWindow_PG_PAGE_Lists();
			CloseWindow_PG_PAGE_Flags();
			break;
		case OM_NEW:
			retval=PAGE_NEW_Gadget(cl,o,(struct piSet *)msg);
			break;
		case OM_DISPOSE:
			retval=PAGE_DISPOSE_Gadget(cl,o,msg);
			break;
		case OM_CREATE:
			retval=PAGE_CREATE_Gadget(cl,o,(struct piWindow *)msg);
			break;
		case OM_DELETE:
			retval=PAGE_DELETE_Gadget(cl,o,msg);
			break;
		case OM_ERASE:
			retval=PAGE_ERASE_Gadget(cl,o,msg);
			break;
		default:
			retval=piDoSuperMethodA(cl,o,msg);
			break;
	}
	return retval;
}

ULONG PAGE_INIT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetCheckMark(PropsWnd,PG_PAGE_FlagsGadgets[GD_PG_PAGE_FLAGS_DISABLED],GetTagData(GA_Disabled,0,ng->ng_Tags));
	SetCheckMark(PropsWnd,PG_PAGE_FlagsGadgets[GD_PG_PAGE_FLAGS_UNDERSCORE],GetTagData(PAGE_Underscore,0,ng->ng_Tags));
	SetCycle(PropsWnd,PG_PAGE_FlagsGadgets[GD_PG_PAGE_FLAGS_LAYOUT],GetTagData(PAGE_Layout,0,ng->ng_Tags));
	SetCycle(PropsWnd,PG_PAGE_FlagsGadgets[GD_PG_PAGE_FLAGS_STYLE],GetTagData(PAGE_ActiveStyle,0,ng->ng_Tags));

	if(active==2) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetListView(PropsWnd,PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_LIST],GetTagData(OWNTAG_Pages,0,ng->ng_OwnTags));
	SetListView(PropsWnd,PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_WINDOWS],&GUI.gui_List_Window);
	SetInteger(PropsWnd,PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_ACTIVE],GetTagData(PAGE_Active,0,ng->ng_Tags));
	DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_UP],PropsWnd,TRUE);
	DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_DOWN],PropsWnd,TRUE);
	DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_DEL],PropsWnd,TRUE);
	return TRUE;
}

ULONG PAGE_APPLY_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Gadget *ng;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);

	FindTagItem(PAGE_Active,ng->ng_Tags)->ti_Data=GetInteger(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_ACTIVE]);
	return TRUE;
}

ULONG PAGE_DEFAULT_Gadget(piClass *cl,piObject *o,Msg msg)
{
	piObject *wo;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	struct Node *n;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piDoSuperMethodA(cl,o,msg);
	
	ng->ng_Width=200;

	if(n=MyAllocVec(sizeof(struct Node),MEMF_PUBLIC))
	{
		wo=(piObject *)FindNodeNum(List(&GUI.gui_List_Window),0);
		piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
		n->ln_Name=MyAllocVec(strlen(nw->nw_Label)+1,MEMF_PUBLIC|MEMF_CLEAR);
		strcpy(n->ln_Name,nw->nw_Label);
		nw->nw_PageGadget++;
		AddTail(List(FindTagItem(OWNTAG_Pages,ng->ng_OwnTags)->ti_Data),n);
	}
	return TRUE;
}

ULONG PAGE_NEW_Gadget(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Gadget *ng;
	piObject *obj;
	struct TagItem ti[]={
		GAOBJ_Type,GTYPE_BOOPSI,
		GAOBJ_NumTags,GSIZE_NUMTAGS,
		GAOBJ_NumOwnTags,GSIZE_NUMOWNTAGS,
		GAOBJ_TagTypes,(ULONG)PageTagsTypes,
		GAOBJ_OwnTagTypes,(ULONG)PageOwnTagsTypes,
		GAOBJ_ID,PAGE_ID,
		OBJ_Properties,(ULONG)PAGES_PROPERTIES_PG_PAGE_PAGES,
		GAOBJLIST_List,0,
		GAOBJLIST_Active,0,
		GAOBJLIST_Tags,1,
		TAG_DONE};

	if(obj=(piObject *)piDoSuperMethod(cl,o,OM_NEW,ti))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(obj);

		CopyTagItems(PageTags,ng->ng_Tags,ng->ng_NumTags);
		CopyTagItems(PageOwnTags,ng->ng_OwnTags,ng->ng_NumOwnTags);

		FindTagItem(OWNTAG_Pages,ng->ng_OwnTags)->ti_Data=(ULONG)MyAllocVec(sizeof(struct List),MEMF_ANY);
		NewList(List(FindTagItem(OWNTAG_Pages,ng->ng_OwnTags)->ti_Data));

		return (ULONG)obj;
	}
	return FALSE;
}

ULONG PAGE_DISPOSE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	struct TagItem *ti;
	struct List *l;
	struct Node *n;
	piObject *wo;
	struct Node_Window *nw;

	piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&ti);
	if(l=List(FindTagItem(OWNTAG_Pages,ti)->ti_Data))
	{
		while(n=RemTail(l))
		{
			if(wo=(piObject *)FindName(List(&GUI.gui_List_Window),n->ln_Name))
			{
				piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
				nw->nw_PageGadget--;
			}
			MyFreeVec(n);
		}
		MyFreeVec(l);
	}
	return piDoSuperMethodA(cl,o,msg);
}

ULONG PAGE_CREATE_Gadget(piClass *cl,piObject *o,struct piWindow *msg)
{
	piObject *go,*wo,*fo,*fwo=NULL;
	struct List *l;
	struct Node_Gadget *ng,*tng;
	struct Node_Frame *nf;
	struct Page *page=NULL,*prevpage=NULL;
	struct Node *n=NULL;
	struct Node_Window *nw=NULL;
	struct Gadget *g=NULL;
	struct BevelBox *cbbox=NULL,*bbox=NULL,*prevbbox=NULL;
	struct TagItem *ti,*oti;
	WORD x,y,w,h;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&oti);
	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);

	cbbox=MyAllocVec(sizeof(struct BevelBox),MEMF_PUBLIC|MEMF_CLEAR);
	FindTagItem(PAGE_ClearBBox,ti)->ti_Data=(ULONG)cbbox;
	
	for(n=GetHead(List(FindTagItem(OWNTAG_Pages,oti)->ti_Data));GetSucc(n);n=GetSucc(n))
	{
		wo=(piObject *)FindName(List(&GUI.gui_List_Window),n->ln_Name);
		if(!wo) continue;
		if(!fwo) fwo=wo;

		piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
		page=MyAllocVec(sizeof(struct Page),MEMF_PUBLIC|MEMF_CLEAR);
		if(prevpage) prevpage->NextPage=page;
		else FindTagItem(PAGE_Pages,ti)->ti_Data=(ULONG)page;
		prevpage=page;
		page->NextPage=NULL;
		page->Name=nw->nw_WindowTitle;
		page->NameStyle=FS_NORMAL;
		page->FirstGadget=NULL;
		page->FirstIText=NULL;
		page->FirstBBox=NULL;
		page->FirstImage=NULL;
		page->ThisPageHook=NULL;

		if(GUI.gui_Mode==MODE_TEST)
		{
			piGetAttr(wo,WNOBJ_GadgetList,(ULONG *)&l);
			if(!IsListEmpty(l))
			{
				g=CreateContext(&page->FirstGadget);
				for(go=(piObject *)GetHead(l);GetSucc(go);go=(piObject *)GetSucc(go))
				{
					piGetAttr(go,OBJ_Node,(ULONG *)&tng);
					tng->ng_Parent=o;
					g=(struct Gadget *)piDoMethod(go,OM_CREATE,(ULONG)g);
					tng->ng_Parent=NULL;
				}
			}
			piGetAttr(wo,WNOBJ_FrameList,(ULONG *)&l);
			prevbbox=NULL;
			for(fo=(piObject *)GetHead(l);GetSucc(fo);fo=(piObject *)GetSucc(fo))
			{
				piGetAttr(fo,OBJ_Node,(ULONG *)&nf);
				if(bbox=MyAllocVec(sizeof(struct BevelBox),MEMF_ANY|MEMF_CLEAR))
				{
					if(prevbbox) prevbbox->NextBBox=bbox;
					else { prevbbox=bbox; page->FirstBBox=bbox; };
					bbox->NextBBox=NULL;
					bbox->Left=nf->nf_X+piCX(o);
					bbox->Top=nf->nf_Y+piCY(o)+piCH(o);
					bbox->Width=piCW(fo);
					bbox->Height=piCH(fo);
					bbox->BoxShine=DrawInfo->dri_Pens[SHINEPEN];
					bbox->BoxShadow=DrawInfo->dri_Pens[SHADOWPEN];
					GetFlag(nf->nf_Flags,MYFR_Recessed) SetFlag(bbox->Flags,BBFLG_RECESSED);
					switch(nf->nf_FrameType)
					{
						case FTYPE_BUTTON: SetFlag(bbox->Flags,BBFLG_FRAMEBUTTON); break;
						case FTYPE_ICONDROPBOX: SetFlag(bbox->Flags,BBFLG_FRAMEBUTTON); break;
						case FTYPE_RIDGE: SetFlag(bbox->Flags,BBFLG_FRAMERIDGE); break;
						case FTYPE_HORIZONTALLINE: SetFlag(bbox->Flags,BBFLG_FRAMEBUTTON);  break;
						case FTYPE_VERTICALLINE: SetFlag(bbox->Flags,BBFLG_FRAMEBUTTON); break;
					}
				}
			}
		}
	}

	if(cbbox)
	{
		piGetAttr(fwo,OBJ_Node,(ULONG *)&nw);
		cbbox->Left=piCX(o)-5;
		cbbox->Top=piCY(o)+piCH(o);
		if(nw->nw_Window)
		{
			cbbox->Width=WinWidth(fwo)-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight+10;
			cbbox->Height=WinHeight(fwo)-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom+2;
		}
		else
		{
			cbbox->Width=WinWidth(fwo)-Scr->WBorLeft-Scr->WBorRight;
			cbbox->Height=WinHeight(fwo)-(Scr->RastPort.TxHeight + Scr->WBorTop + 1-2+2);
		}
		cbbox->Flags=BBFLG_FRAMEBUTTON|BBFLG_BACKFILL;
		cbbox->BoxShine=DrawInfo->dri_Pens[SHINEPEN];
		cbbox->BoxShadow=DrawInfo->dri_Pens[SHADOWPEN];
	}

	if(ng->ng_RelX) x=-(nw->nw_Window->Width-nw->nw_Window->BorderLeft-ng->ng_X-1);
	else x=piCX(o);
	if(ng->ng_RelY) y=-(nw->nw_Window->Height-nw->nw_Window->BorderTop-ng->ng_Y-1);
	else y=piCY(o);
	if(ng->ng_RelWidth) w=-(nw->nw_Window->BorderLeft+nw->nw_Window->BorderRight+(nw->nw_Window->Width-nw->nw_Window->BorderLeft-nw->nw_Window->BorderRight-ng->ng_Width)-1);
	else w=piCW(o)+1;
	if(ng->ng_RelHeight) h=-(nw->nw_Window->BorderTop+nw->nw_Window->BorderBottom+(nw->nw_Window->Height-nw->nw_Window->BorderTop-nw->nw_Window->BorderBottom-ng->ng_Height)-1);
	else h=piCH(o)+1;

	return (ULONG) (ng->ng_Gadget=NewObject(PageClass, NULL,
									ng->ng_RelX?GA_RelRight:GA_Left,x,
									ng->ng_RelY?GA_RelBottom:GA_Top,y,
									ng->ng_RelWidth?GA_RelWidth:GA_Width,w,
									ng->ng_RelHeight?GA_RelHeight:GA_Height,h,
									(msg->Window) ? GA_Previous : TAG_IGNORE, msg->Window,
									GA_ID, 0,
									TAG_MORE,ti));
}

ULONG PAGE_DELETE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	piObject *wo,*obj;
	struct List *l;
	struct TagItem *ti,*oti;
	struct Node_Gadget *ng=NULL;
	struct Node *n=NULL;
	struct Page *page=NULL,*np=NULL;
	struct BevelBox *bbox=NULL,*nbbox=NULL;

	piGetAttr(o,GAOBJ_Tags,(ULONG *)&ti);
	piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&oti);
	piGetAttr(o,OBJ_Node,(ULONG *)&ng);

	if(ng->ng_Gadget)
	{
		if(ng->ng_Window) RemoveGadget(((struct Node_Window *)OBJECT_DATA(ng->ng_Window))->nw_Window,ng->ng_Gadget);
		DisposeObject(ng->ng_Gadget);
		ng->ng_Gadget=NULL;

		page=(struct Page *)FindTagItem(PAGE_Pages,ti)->ti_Data;
		while(page)
		{
			np=page->NextPage;
			if(page->FirstGadget) FreeGadgets(page->FirstGadget);
			bbox=page->FirstBBox;
			while(bbox)
			{
				nbbox=bbox->NextBBox;
				MyFreeVec(bbox);
				bbox=nbbox;
			}
			MyFreeVec((APTR)page);
			page=np;
		}

		for(n=GetHead(List(FindTagItem(OWNTAG_Pages,oti)->ti_Data));GetSucc(n);n=GetSucc(n))
		{
			wo=(piObject *)FindName(List(&GUI.gui_List_Window),n->ln_Name);
			if(wo)
			{
				piGetAttr(wo,WNOBJ_GadgetList,(ULONG *)&l);
				for(obj=(piObject *)GetHead(l);GetSucc(obj);obj=(piObject *)GetSucc(obj)) piDoMethod(obj,OM_DELETE);
			}
		}
		MyFreeVec((APTR)GetTagData(PAGE_ClearBBox,0,ti));
		FindTagItem(PAGE_ClearBBox,ti)->ti_Data=NULL;
	}
	return piDoSuperMethodA(cl,o,msg);
}

ULONG PAGE_ERASE_Gadget(piClass *cl,piObject *o,Msg msg)
{
	piObject *wo;
	struct TagItem *oti;
	struct Node_Window *nw,*nw2;
	struct Node_Gadget *ng;
	struct Node *n;
	UWORD x,y,w,h;

	piGetAttr(o,OBJ_Node,(ULONG *)&ng);
	piGetAttr(ng->ng_Window,OBJ_Node,(ULONG *)&nw);
	if(nw->nw_Window)
	{
		piGetAttr(o,GAOBJ_OwnTags,(ULONG *)&oti);
		n=GetHead(List(FindTagItem(OWNTAG_Pages,oti)->ti_Data));
		wo=(piObject *)FindName(List(&GUI.gui_List_Window),n->ln_Name);
		if(wo)
		{
			piGetAttr(wo,OBJ_Node,(ULONG *)&nw2);

			x=piCX(o)-5;
			y=piCY(o)+piCH(o);
			if(nw2->nw_Window)
			{
				w=WinWidth(wo)-nw2->nw_Window->BorderLeft-nw2->nw_Window->BorderRight+10;
				h=WinHeight(wo)-nw2->nw_Window->BorderTop-nw2->nw_Window->BorderBottom+2;
			}
			else
			{
				w=WinWidth(wo)-Scr->WBorLeft-Scr->WBorRight;
				h=WinHeight(wo)-(Scr->RastPort.TxHeight + Scr->WBorTop + 1-2+2);
			}
			SetAPen(nw->nw_Window->RPort,DrawInfo->dri_Pens[BACKGROUNDPEN]);
			SetDrMd(nw->nw_Window->RPort,JAM1);
			RectFill(nw->nw_Window->RPort,x,y,x+w,y+h);
		}
	}
	return piDoSuperMethodA(cl,o,msg);
}

int FN_PG_PAGE_FLAGS_STYLE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(PAGE_ActiveStyle,ti)->ti_Data=PropertiesMsg.Code;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PAGE_FLAGS_LAYOUT_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(PAGE_Layout,ti)->ti_Data=PropertiesMsg.Code;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PAGE_FLAGS_DISABLED_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	FindTagItem(GA_Disabled,ti)->ti_Data=PropertiesMsg.Code?TRUE:FALSE;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PAGE_FLAGS_UNDERSCORE_Clicked( void )
{
	struct TagItem *ti;
	piGetAttr(CurrentGadget,GAOBJ_Tags,(ULONG *)&ti);
	ti[0].ti_Tag=PropertiesMsg.Code?PAGE_Underscore:TAG_IGNORE;
	ti[0].ti_Data='_';
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

/********** Lists **********************************************************/

extern ULONG ListSelectedLabel;
LONG AddNum=1;

int FN_PG_PAGE_LISTS_WINDOWS_Clicked( void )
{
	AddNum=PropertiesMsg.Code;
	return 1;
}

int FN_PG_PAGE_LISTS_LIST_Clicked( void )
{
	struct EditList *el;

	piGetAttr(CurrentGadget,GAOBJ_EditList,(ULONG *)&el);
	ListSelectedLabel=PropertiesMsg.Code;
	if(!GetSucc(GetSucc(FindNodeNum(*(el->List),ListSelectedLabel))))
		DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_DOWN],PropertiesWnd,TRUE);
	else
		DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_DOWN],PropertiesWnd,FALSE);
	if(ListSelectedLabel==0) DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_UP],PropertiesWnd,TRUE);
		else DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_UP],PropertiesWnd,FALSE);
	DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_DEL],PropertiesWnd,FALSE);
	return 1;
}

int FN_PG_PAGE_LISTS_ADD_Clicked( void )
{
	piObject *wo;
	struct Node_Window *nw;
	struct Node *n;
	struct EditList *el;

	piGetAttr(CurrentGadget,GAOBJ_EditList,(ULONG *)&el);
	SetListView(PropertiesWnd,PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_LIST],NULL);

	if(AddNum!=-1)
	{
		if(n=MyAllocVec(sizeof(struct Node),MEMF_PUBLIC))
		{
			wo=(piObject *)FindNodeNum(List(&GUI.gui_List_Window),AddNum);
			piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
			n->ln_Name=MyAllocVec(strlen(nw->nw_Label)+1,MEMF_PUBLIC|MEMF_CLEAR);
			strcpy(n->ln_Name,nw->nw_Label);
			nw->nw_PageGadget++;
			AddTail(*(el->List),n);
			SetListView(PropertiesWnd,PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_LIST],NULL);
			SetListView(PropertiesWnd,PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_LIST],*(el->List));
	
			ListSelectedLabel=GetNumNode(*(el->List),n);
			GT_SetGadgetAttrs(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_LIST],PropertiesWnd,NULL,GTLV_Selected,ListSelectedLabel,GTLV_MakeVisible,ListSelectedLabel,TAG_DONE);
			if(!GetSucc(GetSucc(FindNodeNum(*(el->List),ListSelectedLabel))))
				DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_DOWN],PropertiesWnd,TRUE);
			else
				DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_DOWN],PropertiesWnd,FALSE);
			if(ListSelectedLabel==0) DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_UP],PropertiesWnd,TRUE);
			else DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_UP],PropertiesWnd,FALSE);
			DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_DEL],PropertiesWnd,FALSE);
		}
	}
	AddNum=-1;
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PAGE_LISTS_DEL_Clicked( void )
{
	piObject *wo;
	struct Node *page;
	struct Node_Window *nw;
	struct EditList *el;

	piGetAttr(CurrentGadget,GAOBJ_EditList,(ULONG *)&el);
	if(ListSelectedLabel!=-1)
	{
		if(page=FindNodeNum(*(el->List),ListSelectedLabel))
		{
			SetListView(PropertiesWnd,PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_LIST],NULL);
			Remove(page);
			wo=(piObject *)FindName(List(&GUI.gui_List_Window),page->ln_Name);
			piGetAttr(wo,OBJ_Node,(ULONG *)&nw);
			nw->nw_PageGadget--;
			MyFreeVec((APTR)page);
			SetListView(PropertiesWnd,PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_LIST],*(el->List));
			ListSelectedLabel=-1;
			DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_UP],PropertiesWnd,TRUE);
			DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_DOWN],PropertiesWnd,TRUE);
			DisableGadget(PG_PAGE_ListsGadgets[GD_PG_PAGE_LISTS_DEL],PropertiesWnd,TRUE);
		}	
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PG_PAGE_LISTS_UP_Clicked( void )
{
	return FN_PG_LISTS_UP_Clicked();
}

int FN_PG_PAGE_LISTS_DOWN_Clicked( void )
{
	return FN_PG_LISTS_DOWN_Clicked();
}

int FN_PG_PAGE_LISTS_ACTIVE_Clicked( void )
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}
