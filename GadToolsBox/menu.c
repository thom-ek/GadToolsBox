/*
** GadToolsBox v3 - Menu Managment
*/

#include"includes.h"

/*
** Defines
*/

extern struct BevelBox BBOX_PROPERTIES_PAGES_PGMN_General[];

struct Page PAGES_PROPERTIES_PGMN_PAGES[]=
{
	&PAGES_PROPERTIES_PGMN_PAGES[1], NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, FS_NORMAL, NULL, NULL, NULL, NULL, NULL,
};

/*
** Functions
*/


struct Node_MenuItem *FindSelectedMenu(piObject *o)
{
	struct Node_MenuItem *ni=NULL,*sni=NULL;
	struct Node_Menu *nm;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
		if(ni->nm_Selected) sni=ni;

	return sni;
}

ULONG GetMenuNum(struct Node_MenuItem *ni)
{
	ULONG xp=0;

	for(;ni->nm_Node.ln_Pred;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Pred)
		if(ni) if(ni->nm_Type==NM_TITLE) xp++;

	return xp-1;
}

struct Node_MenuItem *FindNumMenu(piObject *o,LONG t,LONG i,LONG s)
{
	struct Node_MenuItem *ni=NULL;
	struct Node_Menu *nm;
	LONG ct=0,ci=0,cs=0,nt=0,nj=0;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
	{
		if(ni->nm_Type==NM_TITLE)
		{
			nt=ct;
			ci=0; cs=0;
			if(ct++==t) if(i==-1) return ni;
		}
		if(ni->nm_Type==NM_ITEM)
		{
			nj=ci;
			cs=0;
			if(ci++==i && nt==t) if(s==-1) return ni;
		}
		if(ni->nm_Type==NM_SUB)
		{
			if(cs++==s && nj==i && nt==t) return ni;
		}
	}
	return NULL;
}

void SelectMenu(struct Node_MenuItem *ni)
{
	switch(ni->nm_Type)
	{
		case NM_SUB:
			ni->nm_Selected=TRUE;
			for(;ni->nm_Node.ln_Pred;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Pred)
				if(ni->nm_Type==NM_ITEM)
					break;
		case NM_ITEM:
			ni->nm_Selected=TRUE;
			for(;ni->nm_Node.ln_Pred;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Pred)
				if(ni->nm_Type==NM_TITLE)
					break;
		case NM_TITLE:
			ni->nm_Selected=TRUE;
			break;
	}
}

LONG CountItems(struct Node_MenuItem *ni,UBYTE type)
{
	LONG c=0;

	ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ;
	while(ni->nm_Node.ln_Succ)
	{
		if(ni->nm_Type==type) c++;
		switch(type)
		{
			case NM_ITEM: if(ni->nm_Type==NM_TITLE) return c; break;
			case NM_SUB: if(ni->nm_Type==NM_ITEM || ni->nm_Type==NM_TITLE) return c; break;
		}
		ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ;
	}
	return c;
}

LONG RevCountItems(struct Node_MenuItem *ni,UBYTE type)
{
	LONG c=0;

	switch(type)
	{
		case NM_TITLE:
			while(ni->nm_Node.ln_Pred)
			{
				if(ni->nm_Type==type) c++;
				ni=(struct Node_MenuItem *)ni->nm_Node.ln_Pred;
			}
			return c;
		case NM_ITEM:
			ni=(struct Node_MenuItem *)ni->nm_Node.ln_Pred;
			while(ni->nm_Node.ln_Pred)
			{
				if(ni->nm_Type==type) c++;
				if(ni->nm_Type==NM_TITLE) return c;
				ni=(struct Node_MenuItem *)ni->nm_Node.ln_Pred;
			}
			return c;
		case NM_SUB:
			ni=(struct Node_MenuItem *)ni->nm_Node.ln_Pred;
			while(ni->nm_Node.ln_Pred)
			{
				if(ni->nm_Type==type) c++;
				if(ni->nm_Type==NM_ITEM && ni->nm_Type==NM_TITLE) return c;
				ni=(struct Node_MenuItem *)ni->nm_Node.ln_Pred;
			}
			return c;
	}
}

void UnSelectAllMenus(piObject *o)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
		ni->nm_Selected=FALSE;
}

ULONG Dispatcher_Menu(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval;

	switch(msg->MethodID)
	{
		case OM_NEW:
			retval=NEW_Menu(cl,o,(struct piSet *)msg);
			break;
		case OM_INIT:
			retval=INIT_Menu(cl,o,msg);
			break;
		case OM_APPLY:
			retval=APPLY_Menu(cl,o,msg);
			break;
		case OM_OPEN:
			OpenWindow_PGMN_General();
			OpenWindow_PGMN_Item();
			PAGES_PROPERTIES_PGMN_PAGES[0].FirstGadget=PGMN_GeneralGList;
			PAGES_PROPERTIES_PGMN_PAGES[0].Name=GetCatString(MSG_PROPERTIES_PAGES_L35_GAD);
			PAGES_PROPERTIES_PGMN_PAGES[1].FirstGadget=PGMN_ItemGList;
			PAGES_PROPERTIES_PGMN_PAGES[1].Name=GetCatString(MSG_PROPERTIES_PAGES_L38_GAD);
			break;
		case OM_CLOSE:
			PAGES_PROPERTIES_PGMN_PAGES[1].FirstGadget=NULL;
			PAGES_PROPERTIES_PGMN_PAGES[0].FirstGadget=NULL;
			CloseWindow_PGMN_Item();
			CloseWindow_PGMN_General();
			break;
		case OM_CREATE:
			retval=CREATE_Menu(cl,o,(struct piSet *)msg);
			break;
		case OM_DELETE:
			retval=DELETE_Menu(cl,o,(struct piSet *)msg);
			break;
		case OM_SET:
			retval=SET_Menu(cl,o,(struct piSet *)msg);
			break;
		case OM_GET:
			retval=GET_Menu(cl,o,(struct piGet *)msg);
			break;
		case OM_DISPOSE:
			retval=DISPOSE_Menu(cl,o,msg);
			break;
		case OM_RENDER:
			retval=RENDER_Menu(cl,o,msg);
			break;
		case OM_ADDTAIL:
			AddTail(List(&GUI.gui_List_Menu),Node(o));
			break;
		case OM_REMOVE:
			Remove(Node(o));
			break;
		case OM_READ:
			retval=READ_Menu(cl,o,(struct piIO *)msg);
			break;
		case OM_WRITE:
			retval=WRITE_Menu(cl,o,(struct piIO *)msg);
			break;
		case OM_CUT:
			retval=CUT_Menu(cl,o,msg);
			break;
		case OM_COPY:
			retval=COPY_Menu(cl,o,(struct piCopy *)msg);
			break;
	}
	return retval;
}

ULONG CUT_Menu(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *mi,*nmi;

	if(mi=FindSelectedMenu(o))
	{
		piGetAttr(o,OBJ_Node,(ULONG *)&nm);
		switch(mi->nm_Type)
		{
			case NM_TITLE:
				while(mi->nm_Node.ln_Succ)
				{
					nmi=(struct Node_MenuItem *)mi->nm_Node.ln_Succ;
					Remove(Node(mi));
					FreeVec(mi);
					mi=nmi;
					if(mi->nm_Type==NM_TITLE) break;
					nm->nm_Total--;
					SetGadgetAttrs((struct Gadget *)nm->nm_HorizProp,nm->nm_Window,NULL,
						PGA_Top, nm->nm_Top,
						PGA_Total, nm->nm_Total,
						PGA_Visible, nm->nm_Visible,
						TAG_DONE);
				}
				break;
			case NM_ITEM:
				while(mi->nm_Node.ln_Succ)
				{
					nmi=(struct Node_MenuItem *)mi->nm_Node.ln_Succ;
					Remove(Node(mi));
					FreeVec(mi);
					mi=nmi;
					if(mi->nm_Type!=NM_SUB) break;
				}
				break;
			case NM_SUB:
				Remove(Node(mi));
				FreeVec(mi);
				break;
		}
	}
	else
	{
		piCopyObject(o,COPY_CLIP);
		piCopyObject(o,COPY_UNDO);

		piRemoveObject(o);
		piDisposeObject(o);
//		PropertiesMode=PM_NONE;
//		RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
	}
	return TRUE;
}

ULONG COPY_Menu(piClass *cl,piObject *o,struct piCopy *msg)
{
	struct IFFHandle *iffh;
	struct Node_MenuItem *mi;
	ULONG error=0;

	if(mi=FindSelectedMenu(o))
	{
		// menu selected, copy selected item
	}
	else
	{
		if(!(ClipHandle=OpenClipboard(PRIMARY_CLIP))) return 1;
		if(iffh=AllocIFF())
		{
			iffh->iff_Stream=(ULONG)((msg->Copy==COPY_CLIP)?ClipHandle:UndoHandle);
			InitIFFasClip(iffh);
			if(!OpenIFF(iffh,IFFF_WRITE))
			{
				if(!PushChunk(iffh,ID_GTBX,ID_FORM,IFFSIZE_UNKNOWN))
				{
					piWriteObject(o,iffh);
					if(error=PopChunk(iffh)) goto skip;;
				}
skip:		CloseIFF(iffh);
			}
			FreeIFF(iffh);
		}
		if(ClipHandle) CloseClipboard(ClipHandle); ClipHandle=NULL;
	}
	return TRUE;
}

ULONG INIT_Menu(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *sn;
	ULONG active=0;
	struct Window *PropsWnd=NULL;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);
	GetAttr(PAGE_Active,PropertiesGadgets[GD_PROPERTIES_PAGES], &active);

	if(active==0) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	SetString(PropsWnd,PGMN_GeneralGadgets[GD_PGMN_LABEL],nm->nm_Label);
	SetCheckMark(PropsWnd,PGMN_GeneralGadgets[GD_PGMN_NEWLOOK],(nm->nm_Flags&MYMN_NewLookMenus)?TRUE:FALSE);

	if(active==1) PropsWnd=PropertiesWnd; else PropsWnd=NULL;
	if(sn=FindSelectedMenu(o))
	{
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_LABEL],PropsWnd,(sn->nm_Flags&MYMN_BarLabel)?TRUE:FALSE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_TITLE],PropsWnd,(sn->nm_Flags&MYMN_BarLabel)?TRUE:FALSE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_COMMKEY],PropsWnd,(sn->nm_Flags&MYMN_BarLabel)?TRUE:FALSE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_DISABLED],PropsWnd,(sn->nm_Flags&MYMN_BarLabel)?TRUE:FALSE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_TOGGLE],PropsWnd,(sn->nm_Flags&MYMN_BarLabel)?TRUE:FALSE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_CHECKED],PropsWnd,(sn->nm_Flags&MYMN_BarLabel)?TRUE:FALSE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_POPUP],PropsWnd,((sn->nm_Flags&MYMN_BarLabel)||(sn->nm_Type==NM_SUB))?TRUE:FALSE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_BARLABEL],PropsWnd,FALSE);
		SetString(PropsWnd,PGMN_ItemGadgets[GD_PGMN_ITEM_LABEL],sn->nm_Label);
		SetString(PropsWnd,PGMN_ItemGadgets[GD_PGMN_ITEM_TITLE],sn->nm_Title);
		SetString(PropsWnd,PGMN_ItemGadgets[GD_PGMN_ITEM_COMMKEY],sn->nm_Key);
		SetCheckMark(PropsWnd,PGMN_ItemGadgets[GD_PGMN_ITEM_DISABLED],(sn->nm_Flags&MYMN_Disabled)?TRUE:FALSE);
		SetCheckMark(PropsWnd,PGMN_ItemGadgets[GD_PGMN_ITEM_TOGGLE],(sn->nm_Flags&MYMN_Toggle)?TRUE:FALSE);
		SetCheckMark(PropsWnd,PGMN_ItemGadgets[GD_PGMN_ITEM_CHECKED],(sn->nm_Flags&MYMN_Checked)?TRUE:FALSE);
		SetCheckMark(PropsWnd,PGMN_ItemGadgets[GD_PGMN_ITEM_BARLABEL],(sn->nm_Flags&MYMN_BarLabel)?TRUE:FALSE);
		SetCheckMark(PropsWnd,PGMN_ItemGadgets[GD_PGMN_ITEM_POPUP],(sn->nm_Flags&MYMN_PopUp)?TRUE:FALSE);
	}
	else
	{
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_LABEL],PropsWnd,TRUE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_TITLE],PropsWnd,TRUE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_COMMKEY],PropsWnd,TRUE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_DISABLED],PropsWnd,TRUE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_TOGGLE],PropsWnd,TRUE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_CHECKED],PropsWnd,TRUE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_BARLABEL],PropsWnd,TRUE);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_POPUP],PropsWnd,TRUE);
	}
	return TRUE;
}

ULONG APPLY_Menu(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *mi;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);

	strcpy(nm->nm_Label,GetString(PGMN_GeneralGadgets[GD_PGMN_LABEL]));
	RefreshObjectList();
	if(mi=FindSelectedMenu(o))
	{
		strcpy(mi->nm_Label,GetString(PGMN_ItemGadgets[GD_PGMN_ITEM_LABEL]));
		strcpy(mi->nm_Title,GetString(PGMN_ItemGadgets[GD_PGMN_ITEM_TITLE]));
		strncpy(mi->nm_Key,GetString(PGMN_ItemGadgets[GD_PGMN_ITEM_COMMKEY]),2);
	}
	return TRUE;
}

ULONG RENDER_Menu(piClass *cl,piObject *o,Msg msg)
{
	struct Node_MenuItem *ni;
	struct Node_Menu *nm;
	ULONG xp,yp,syp,a,b,numtitle;
	BOOL renderi=FALSE,renders=FALSE;
	UWORD fh,fb,bl,br,bt,bb,ww,wh;
	struct RastPort *rp;
	struct Region *region1=NULL,*oldregion;
	struct Rectangle rect;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);

	region1=NewRegion();
	rect.MinX=nm->nm_Window->BorderLeft;
	rect.MinY=nm->nm_Window->BorderTop;
	rect.MaxX=nm->nm_Window->Width-nm->nm_Window->BorderRight-1;
	rect.MaxY=nm->nm_Window->Height-nm->nm_Window->BorderBottom-1;
	OrRectRegion(region1,&rect);
	oldregion=InstallClipRegion(nm->nm_Window->WLayer,region1);

	ww=nm->nm_Window->Width;
	wh=nm->nm_Window->Height;
	rp=nm->nm_Window->RPort;
	bl=nm->nm_Window->BorderLeft;
	br=nm->nm_Window->BorderRight;
	bt=nm->nm_Window->BorderTop;
	bb=nm->nm_Window->BorderBottom;
	fh=rp->TxHeight+2;
	fb=fh-rp->TxBaseline-1;

	SetAPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
	RectFill(rp,bl,bt,ww-br-1,wh-bb-1);

	SetAPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
	RectFill(rp,bl,bt,ww-br-1,bt+fh);

	SetAPen(rp,DrawInfo->dri_Pens[BARTRIMPEN]);
	Move(rp,bl,bt+fh);
	Draw(rp,ww-br-1,bt+fh);
	Draw(rp,ww-br-1,bt);

	xp=-150; yp=0;
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem)),numtitle=0;ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
		if(ni->nm_Type==NM_TITLE) numtitle++;
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem)),a=0;ni->nm_Node.ln_Succ && a<nm->nm_Top;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
		if(ni->nm_Type==NM_TITLE) a++;
	if(numtitle==0)
	{
		SetAPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
		RectFill(rp,xp+bl+150,yp+bt,xp+bl+150+150,yp+bt+fh-1);
		SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
		SetBPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
		SetDrPt(rp,0x5555);
		Move(rp,xp+bl+150,yp+bt);
		Draw(rp,xp+bl+150+150,yp+bt);
		Draw(rp,xp+bl+150+150,yp+bt+fh);
		Draw(rp,xp+bl+150,yp+bt+fh);
		Draw(rp,xp+bl+150,yp+bt);
		SetDrPt(rp,0xffff);
		Move(rp,xp+bl+5+150,yp+bt+fh-fb);
		Text(rp,GetCatString(MSG_NEWTITLE),strlen(GetCatString(MSG_NEWTITLE)));
	}
	for(b=0;ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
	{
		switch(ni->nm_Type)
		{
			case NM_TITLE:
				xp+=150; yp=0; b++;
				if(ni->nm_Selected) SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
				else SetAPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
				RectFill(rp,xp+bl,yp+bt,xp+bl+150,yp+bt+fh-1);
				if(ni->nm_Selected)
				{
					SetAPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
					SetBPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
					renderi=TRUE;
				}
				else
				{
					SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
					SetBPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
					renderi=FALSE;
					renders=FALSE;
				}
				if(ni->nm_Flags&MYMN_Toggle)
				{
					Move(rp,xp+bl+3+20,yp+bt+fh-fb);
					if(ni->nm_Flags&MYMN_Checked)
						DrawImageState(rp,DrawInfo->dri_CheckMark,xp+bl+3,yp+bt+(fb/2),ni->nm_Selected?IDS_INACTIVENORMAL:IDS_NORMAL,DrawInfo);
				}
				else
					Move(rp,xp+bl+5,yp+bt+fh-fb);
				if(ni->nm_Flags&MYMN_BarLabel) Text(rp,"~~~~~~~~~~~~~",13);
				else Text(rp,ni->nm_Title,strlen(ni->nm_Title));
				if(ni->nm_Key[0]!='\0')
				{
					DrawImageState(rp,DrawInfo->dri_AmigaKey,xp+bl+150-41-3,yp+bt+(fb/2),ni->nm_Selected?IDS_INACTIVENORMAL:IDS_NORMAL,DrawInfo);
					Move(rp,xp+bl+150-10-3,yp+bt+fh-fb);
					Text(rp,ni->nm_Key,strlen(ni->nm_Key));
				}
				if((b+a)==numtitle)
				{
					SetAPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
					RectFill(rp,xp+bl+150,yp+bt,xp+bl+150+150,yp+bt+fh-1);
					SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
					SetBPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
					SetDrPt(rp,0x5555);
					Move(rp,xp+bl+150,yp+bt);
					Draw(rp,xp+bl+150+150,yp+bt);
					Draw(rp,xp+bl+150+150,yp+bt+fh);
					Draw(rp,xp+bl+150,yp+bt+fh);
					Draw(rp,xp+bl+150,yp+bt);
					SetDrPt(rp,0xffff);
					Move(rp,xp+bl+5+150,yp+bt+fh-fb);
					Text(rp,GetCatString(MSG_NEWTITLE),strlen(GetCatString(MSG_NEWTITLE)));
				}
				if(((struct Node_MenuItem *)ni->nm_Node.ln_Succ)->nm_Type!=NM_ITEM && ni->nm_Selected)
				{
					SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
					SetBPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
					SetDrPt(rp,0x5555);
					Move(rp,xp+bl,yp+bt+fh);
					Draw(rp,xp+bl+150,yp+bt+fh);
					Draw(rp,xp+bl+150,yp+bt+fh+fh);
					Draw(rp,xp+bl,yp+bt+fh+fh);
					Draw(rp,xp+bl,yp+bt+fh);
					SetDrPt(rp,0xffff);
					Move(rp,xp+bl+5,yp+bt+fh-fb+fh);
					Text(rp,GetCatString(MSG_NEWITEM),strlen(GetCatString(MSG_NEWITEM)));
				}
				break;
			case NM_ITEM:
				yp+=fh;
				syp=yp-fh;
				if(renderi)
				{
					SetAPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
					RectFill(rp,xp+bl,yp+bt+1,xp+bl+150,yp+bt+fh+1);
					SetAPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
					if(yp==fh)
					{
						Move(rp,xp+bl+2,yp+bt);
						Draw(rp,xp+bl+148,yp+bt);
						SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
						Move(rp,xp+bl,yp+bt-1);
						Draw(rp,xp+bl+150,yp+bt-1);
					}
					else
						Move(rp,xp+bl+150,yp+bt);
					SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
					Draw(rp,xp+bl+150,yp+bt+1+fh);
					Draw(rp,xp+bl,yp+bt+1+fh);
					Draw(rp,xp+bl,yp+bt+1);
					Move(rp,xp+bl+1,yp+bt+1);
					Draw(rp,xp+bl+1,yp+bt+1+fh);
					Move(rp,xp+bl+149,yp+bt+1);
					Draw(rp,xp+bl+149,yp+bt+1+fh);
					if(ni->nm_Selected) SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
					else SetAPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
					RectFill(rp,xp+bl+4,yp+bt+1,xp+bl+146,yp+bt+fh-2+1);
					if(ni->nm_Selected)
					{
						SetAPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
						SetBPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
						renders=TRUE;
					}
					else
					{
						SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
						SetBPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
						renders=FALSE;
					}
					if(ni->nm_Flags&MYMN_Toggle)
					{
						Move(rp,xp+bl+3+20,yp+bt+fh-fb+1);
						if(ni->nm_Flags&MYMN_Checked)
							DrawImageState(rp,DrawInfo->dri_CheckMark,xp+bl+3,yp+bt+1+(fb/2),ni->nm_Selected?IDS_INACTIVENORMAL:IDS_NORMAL,DrawInfo);
					}
					else
						Move(rp,xp+bl+5,yp+bt+fh-fb+1);
					if(ni->nm_Flags&MYMN_BarLabel) Text(rp,"~~~~~~~~~~~~~",13);
					else Text(rp,ni->nm_Title,strlen(ni->nm_Title));
					if(ni->nm_Key[0]!='\0')
					{
						DrawImageState(rp,DrawInfo->dri_AmigaKey,xp+bl+150-41-3,yp+bt+1+(fb/2),ni->nm_Selected?IDS_INACTIVENORMAL:IDS_NORMAL,DrawInfo);
						Move(rp,xp+bl+150-10-3,yp+bt+1+fh-fb);
						Text(rp,ni->nm_Key,strlen(ni->nm_Key));
					}
					if(((struct Node_MenuItem *)ni->nm_Node.ln_Succ)->nm_Type!=NM_SUB && ni->nm_Selected && ni->nm_Flags&MYMN_PopUp)
					{
						SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
						SetBPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
						SetDrPt(rp,0x5555);
						Move(rp,xp+bl+1+150,yp+bt);
						Draw(rp,xp+bl+1+150+150,yp+bt);
						Draw(rp,xp+bl+1+150+150,yp+bt+fh);
						Draw(rp,xp+bl+1+150,yp+bt+fh);
						Draw(rp,xp+bl+1+150,yp+bt);
						SetDrPt(rp,0xffff);
						Move(rp,xp+bl+5+150,yp+bt+fh-fb);
						Text(rp,GetCatString(MSG_NEWSUB),strlen(GetCatString(MSG_NEWSUB)));
					}
					SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
					SetBPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
					SetDrPt(rp,0x5555);
					Move(rp,xp+bl,yp+bt+fh+2);
					Draw(rp,xp+bl+150,yp+bt+fh+2);
					Draw(rp,xp+bl+150,yp+bt+fh+fh+2);
					Draw(rp,xp+bl,yp+bt+fh+fh+2);
					Draw(rp,xp+bl,yp+bt+fh+2);
					SetDrPt(rp,0xffff);
					Move(rp,xp+bl+1+5,yp+bt+fh-fb+fh+2);
					Text(rp,GetCatString(MSG_NEWITEM),strlen(GetCatString(MSG_NEWITEM)));
				}
				break;
			case NM_SUB:
				xp+=150;
				syp+=fh;
				if(renders)
				{
					SetAPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
					RectFill(rp,xp+bl,syp+bt+1,xp+bl+150,syp+bt+fh+1);
					SetAPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
					if(syp==yp)
					{
						Move(rp,xp+bl+2,syp+bt);
						Draw(rp,xp+bl+148,syp+bt);
						SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
						Move(rp,xp+bl,syp+bt-1);
						Draw(rp,xp+bl+150,syp+bt-1);
					}
					else
						Move(rp,xp+bl+150,syp+bt);
					SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
					Draw(rp,xp+bl+150,syp+bt+1+fh);
					Draw(rp,xp+bl,syp+bt+1+fh);
					Draw(rp,xp+bl,syp+bt+1);
					Move(rp,xp+bl+1,syp+bt+1);
					Draw(rp,xp+bl+1,syp+bt+1+fh);
					Move(rp,xp+bl+149,syp+bt+1);
					Draw(rp,xp+bl+149,syp+bt+1+fh);
					if(ni->nm_Selected) SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
					else SetAPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
					RectFill(rp,xp+bl+4,syp+bt+1,xp+bl+146,syp+bt+fh-2+1);
					if(ni->nm_Selected)
					{
						SetAPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
						SetBPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
					}
					else
					{
						SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
						SetBPen(rp,DrawInfo->dri_Pens[BARBLOCKPEN]);
					}
					if(ni->nm_Flags&MYMN_Toggle)
					{
						Move(rp,xp+bl+3+20,syp+bt+fh-fb+1);
						if(ni->nm_Flags&MYMN_Checked)
							DrawImageState(rp,DrawInfo->dri_CheckMark,xp+bl+3,syp+bt+1+(fb/2),ni->nm_Selected?IDS_INACTIVENORMAL:IDS_NORMAL,DrawInfo);
					}
					else
						Move(rp,xp+bl+5,syp+bt+fh-fb+1);
					if(ni->nm_Flags&MYMN_BarLabel) Text(rp,"~~~~~~~~~~~~~",13);
					else Text(rp,ni->nm_Title,strlen(ni->nm_Title));
					if(ni->nm_Key[0]!='\0')
					{
						DrawImageState(rp,DrawInfo->dri_AmigaKey,xp+bl+150-41-3,syp+bt+1+(fb/2),ni->nm_Selected?IDS_INACTIVENORMAL:IDS_NORMAL,DrawInfo);
						Move(rp,xp+bl+150-10-3,syp+bt+1+fh-fb);
						Text(rp,ni->nm_Key,strlen(ni->nm_Key));
					}
					SetAPen(rp,DrawInfo->dri_Pens[BARDETAILPEN]);
					SetBPen(rp,DrawInfo->dri_Pens[BACKGROUNDPEN]);
					SetDrPt(rp,0x5555);
					Move(rp,xp+bl,syp+bt+fh+2);
					Draw(rp,xp+bl+150,syp+bt+fh+2);
					Draw(rp,xp+bl+150,syp+bt+fh+fh+2);
					Draw(rp,xp+bl,syp+bt+fh+fh+2);
					Draw(rp,xp+bl,syp+bt+fh+2);
					SetDrPt(rp,0xffff);
					Move(rp,xp+bl+5,syp+bt+fh-fb+fh+2);
					Text(rp,GetCatString(MSG_NEWSUB),strlen(GetCatString(MSG_NEWSUB)));
				}
				xp-=150;
				break;
		}
	}

	InstallClipRegion(nm->nm_Window->WLayer,oldregion);
	if(region1) ClearRegion(region1);

	return TRUE;
}

ULONG NEW_Menu(piClass *cl,piObject *o,struct piSet *msg)
{
	piObject *obj;
	struct Node_Menu *nm;

	if(!CheckRegistration())
		if(!ListEmpty(&GUI.gui_List_Menu)) return NULL;
	if(obj=MyAllocVec(sizeof(piObject)+cl->Size,MEMF_PUBLIC|MEMF_CLEAR))
	{
		nm=(struct Node_Menu *)OBJECT_DATA(obj);
		nm->nm_ID=ID_MENU;
		nm->nm_Pages=PAGES_PROPERTIES_PGMN_PAGES,
		obj->ln_Name=nm->nm_Label;
		NewList(List(&nm->nm_MenuItem));
		nm->nm_Menu=NULL;
		nm->nm_NewMenu=NULL;
		nm->nm_LeftButton=nm->nm_RightButton=nm->nm_HorizProp=NULL;
		nm->nm_Top=nm->nm_Visible=nm->nm_Total=0;
		return (ULONG) obj;
	}
	return NULL;
}

ULONG DISPOSE_Menu(piClass *cl,piObject *o,Msg msg)
{
	struct Node_MenuItem *ni;
	struct Node_Menu *nm;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);
	piDoMethod(o,OM_DELETE);
	while(ni=(struct Node_MenuItem *)RemTail(List(&nm->nm_MenuItem)))
		MyFreeVec((APTR)ni);

	MyFreeVec(o);
	return TRUE;
}

#define IM_WIDTH(o) ((struct Image *) o)->Width
#define IM_HEIGHT(o) ((struct Image *) o)->Height

int MENUFN_Left(void)
{
	struct Node_Menu *nm;
	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);

	nm->nm_Top--;
	if(nm->nm_Top<0L) nm->nm_Top=0L;
	SetGadgetAttrs((struct Gadget *)nm->nm_HorizProp,nm->nm_Window,NULL,PGA_Top,nm->nm_Top,TAG_DONE);
	piRenderObject(CurrentMenu);
	return 1;
}

int MENUFN_Right(void)
{
	struct Node_Menu *nm;
	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);

	nm->nm_Top++;
	if(nm->nm_Visible<nm->nm_Total)
		if(nm->nm_Top>nm->nm_Total-nm->nm_Visible) nm->nm_Top=nm->nm_Total-nm->nm_Visible;
	if(nm->nm_Top>=nm->nm_Total) nm->nm_Top=nm->nm_Total-1;
	SetGadgetAttrs((struct Gadget *)nm->nm_HorizProp,nm->nm_Window,NULL,PGA_Top,nm->nm_Top,TAG_DONE);
	piRenderObject(CurrentMenu);
	return 1;
}

int MENUFN_Horiz(void)
{
	struct Node_Menu *nm;
	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);

	GetAttr(PGA_Top,nm->nm_HorizProp,(ULONG *)&nm->nm_Top);
	piRenderObject(CurrentMenu);
	return 1;
}

ULONG CREATE_Menu(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);
	if(!nm->nm_Window)
	{
		unless(nm->nm_RightButton=NewObject(NULL, BUTTONGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			GA_Image, gui_RightImage,
			GA_RelRight, 1-(IM_WIDTH(gui_RightImage)+IM_WIDTH(gui_SizeImage)),
			GA_RelBottom, 1-(IM_HEIGHT(gui_RightImage)),
			GA_RightBorder,TRUE,
			GA_UserData,MENUFN_Right, 
			GA_ID, 2,
			TAG_DONE)) return 1;
		unless(nm->nm_LeftButton=NewObject(NULL, BUTTONGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			GA_Image, gui_LeftImage,
			GA_RelRight, 1-(IM_WIDTH(gui_LeftImage)+IM_WIDTH(gui_SizeImage)+IM_WIDTH(gui_RightImage)),
			GA_RelBottom, 1-(IM_HEIGHT(gui_LeftImage)),
			GA_RightBorder,TRUE,
			GA_Previous,nm->nm_RightButton,
			GA_UserData, MENUFN_Left,
			GA_ID, 3,
			TAG_DONE)) return 1;
		unless(nm->nm_HorizProp=NewObject(NULL, PROPGCLASS,
			ICA_TARGET, ICTARGET_IDCMP,
			PGA_Freedom, FREEHORIZ,
			PGA_NewLook, TRUE,
			PGA_Borderless, ((DrawInfo->dri_Flags & DRIF_NEWLOOK) && DrawInfo->dri_Depth != 1),
			GA_RelBottom, -(IM_HEIGHT(gui_SizeImage)-3),
			GA_RelWidth, -(4+IM_WIDTH(gui_LeftImage)+IM_WIDTH(gui_RightImage)+IM_WIDTH(gui_SizeImage)+1),
			GA_Left, 3,
			GA_Height, IM_HEIGHT(gui_SizeImage)-4,
			GA_RightBorder, TRUE,
			GA_Previous,nm->nm_LeftButton,
			GA_UserData, MENUFN_Horiz,
			GA_ID, 4,
			PGA_Top, nm->nm_Top,
			PGA_Total, nm->nm_Total,
			PGA_Visible, nm->nm_Visible,
			TAG_DONE)) return 1;
	
		if(!(nm->nm_Window=OpenWindowTags(NULL,
					WA_Title, nm->nm_Label,
					WA_CustomScreen, Scr,
					TAG_MORE, EditMenuWTags ))) return GTBERR_OPENWINDOW;
	
		for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem)),nm->nm_Total=0;ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
			if(ni->nm_Type==NM_TITLE) nm->nm_Total++;
		nm->nm_Visible=nm->nm_Window->Width/150;
		SetGadgetAttrs((struct Gadget *)nm->nm_HorizProp,nm->nm_Window,NULL,
			PGA_Top, nm->nm_Top,
			PGA_Total, nm->nm_Total,
			PGA_Visible, nm->nm_Visible,
			TAG_DONE);
	
		AddGList(nm->nm_Window,(struct Gadget *)nm->nm_RightButton,-1,-1,NULL);
		RefreshGList((struct Gadget *)nm->nm_RightButton,nm->nm_Window,NULL,-1);
	
		nm->nm_Window->UserPort=ProjectMsgPort;
		nm->nm_Window->UserData=(BYTE *)o;
		ModifyIDCMP(nm->nm_Window,IDCMP_CLOSEWINDOW|IDCMP_MOUSEMOVE|IDCMP_GADGETUP|IDCMP_MOUSEBUTTONS|IDCMP_MENUPICK|IDCMP_NEWSIZE|IDCMP_ACTIVEWINDOW|IDCMP_MENUVERIFY|IDCMP_IDCMPUPDATE);
		SetMenuStrip(nm->nm_Window,MainMenus);
		SetFont(nm->nm_Window->RPort,DrawInfo->dri_Font);
		piRenderObject(o);
	}
	else
		ActivateWindow(nm->nm_Window);
	return GTBERR_OK;
}

ULONG DELETE_Menu(piClass *cl,piObject *o,struct piSet *msg)
{
	struct Node_Menu *nm;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);

	if(nm->nm_Window)
	{
		ClearMenuStrip(nm->nm_Window);
		RemoveGList(nm->nm_Window,(struct Gadget *)nm->nm_RightButton,-1);
		DisposeObject(nm->nm_HorizProp);
		DisposeObject(nm->nm_RightButton);
		DisposeObject(nm->nm_LeftButton);
		if(nm->nm_Window) CloseWindowSafely(nm->nm_Window); nm->nm_Window=NULL;
	}
	return TRUE;
}

ULONG SET_Menu(piClass *cl,piObject *o,struct piSet *msg)
{
//	struct TagItem *t;
	struct Node_Menu *nm;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);

//	if(t=FindTagItem(OBJ_Select,msg->Tags)) nf->nf_Selected=t->ti_Data;
	return TRUE;
}

ULONG GET_Menu(piClass *cl,piObject *o,struct piGet *msg)
{
	struct Node_Menu *nm=(struct Node_Menu *)OBJECT_DATA(o);

	switch(msg->TagID)
	{
		case OBJ_Node: *(msg->TagData)=(ULONG)nm; return TRUE;
		case OBJ_Properties: *(msg->TagData)=(ULONG)nm->nm_Pages; return TRUE;
		default: return FALSE;
	}
}

ULONG READ_Menu(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0;
	struct Node_Menu *nm;
	struct Node_MenuItem *ni,n;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);
	ReadChunkBytes(msg->IFFh,(APTR)nm->nm_Label,SIZE(Node_Menu,nm_Label));
	while(ReadChunkBytes(msg->IFFh,(APTR)&n.nm_Label,SIZE(Node_MenuItem,nm_Label)))
	{
		if(ni=(struct Node_MenuItem *)MyAllocVec(sizeof(struct Node_MenuItem),MEMF_ANY|MEMF_CLEAR))
		{
			CopyMem(&n.nm_Label,&ni->nm_Label,SIZE(Node_MenuItem,nm_Label));
			ni->nm_Node.ln_Name=ni->nm_Title;
			AddTail(List(&nm->nm_MenuItem),Node(ni));
		}
	}
	return (ULONG)error;
}

ULONG WRITE_Menu(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0;
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;

	piGetAttr(o,OBJ_Node,(ULONG *)&nm);
	if(error=PushChunk(msg->IFFh,NULL,ID_MENU,IFFSIZE_UNKNOWN)) return (ULONG)error;
	WriteChunkBytes(msg->IFFh,(APTR)nm->nm_Label,SIZE(Node_Menu,nm_Label));
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
		WriteChunkBytes(msg->IFFh,(APTR)&ni->nm_Label,SIZE(Node_MenuItem,nm_Label));
	if(error=PopChunk(msg->IFFh)) return (ULONG)error;
	return (ULONG)error;
}

/****************************************************************************/

void DisposeAllMenus()
{
	piObject *obj;	

	while(obj=(piObject *)RemTail(List(&GUI.gui_List_Menu))) piDisposeObject(obj);
}

void DeleteAllMenus()
{
	piObject *obj;	

	for(obj=(piObject *)GetHead(&GUI.gui_List_Menu);GetSucc(obj);obj=(piObject *)GetSucc(obj)) piCloseWindow(obj);
}

/****************************************************************************/

int FN_PGMN_LABEL_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGMN_NEWLOOK_Clicked(void)
{
	struct Node_Menu *nm;
	ULONG code;

	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);
	code=PropertiesMsg.Code;
	DoFlag(nm->nm_Flags,MYMN_NewLookMenus);
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGMN_ITEM_LABEL_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGMN_ITEM_TITLE_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGMN_ITEM_COMMKEY_Clicked(void)
{
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGMN_ITEM_BARLABEL_Clicked(void)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *mi;
	ULONG code;

	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);
	if(mi=FindSelectedMenu(CurrentMenu))
	{
		code=PropertiesMsg.Code;
		DoFlag(mi->nm_Flags,MYMN_BarLabel);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_LABEL],PropertiesWnd,code);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_TITLE],PropertiesWnd,code);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_COMMKEY],PropertiesWnd,code);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_DISABLED],PropertiesWnd,code);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_TOGGLE],PropertiesWnd,code);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_CHECKED],PropertiesWnd,code);
		DisableGadget(PGMN_ItemGadgets[GD_PGMN_ITEM_POPUP],PropertiesWnd,code);
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGMN_ITEM_DISABLED_Clicked(void)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *mi;
	ULONG code;

	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);
	if(mi=FindSelectedMenu(CurrentMenu))
	{
		code=PropertiesMsg.Code;
		DoFlag(mi->nm_Flags,MYMN_Disabled);
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGMN_ITEM_TOGGLE_Clicked(void)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *mi;
	ULONG code;

	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);
	if(mi=FindSelectedMenu(CurrentMenu))
	{
		code=PropertiesMsg.Code;
		DoFlag(mi->nm_Flags,MYMN_Toggle);
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGMN_ITEM_CHECKED_Clicked(void)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *mi;
	ULONG code;

	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);
	if(mi=FindSelectedMenu(CurrentMenu))
	{
		code=PropertiesMsg.Code;
		DoFlag(mi->nm_Flags,MYMN_Checked);
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}

int FN_PGMN_ITEM_POPUP_Clicked(void)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *mi;
	ULONG code;

	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);
	if(mi=FindSelectedMenu(CurrentMenu))
	{
		code=PropertiesMsg.Code;
		DoFlag(mi->nm_Flags,MYMN_PopUp);
	}
	PropertiesChanged=TRUE;
	FN_PROPERTIES_APPLY_Clicked();
	return 1;
}


struct Node_MenuItem *selectmi=NULL,*SelMenuItem=NULL;

int FN_MouseButtons_EditMenu(void)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni,*pni,*ppni;
	LONG xp,yp,i,i2;
	UWORD fh,fb,bl,br,bt,bb,ww,wh;
	int (*func)();
	BOOL running=TRUE;

	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);

	ww=nm->nm_Window->Width;
	wh=nm->nm_Window->Height;
	bl=nm->nm_Window->BorderLeft;
	br=nm->nm_Window->BorderRight;
	bt=nm->nm_Window->BorderTop;
	bb=nm->nm_Window->BorderBottom;
	fh=nm->nm_Window->RPort->TxHeight+2;
	fb=fh-nm->nm_Window->RPort->TxBaseline-1;

	switch(EditMsg.Code)
	{
		case MENUDOWN:
			func=(void *) PM_OpenPopupMenu(nm->nm_Window,PM_Menu,PM_ObjMenu,PM_Code, EditMsg.Code, TAG_DONE);
			if(func)
			{
				running = func();
				if(running > 1) return running;
			}
			break;
		case SELECTDOWN:
			xp=(EditMsg.MouseX-bl)/150;
			yp=(EditMsg.MouseY-bt)/fh;
			selectmi=NULL;
			xp+=nm->nm_Top;
			if(yp==0)			// Title selected
			{
				selectmi=FindNumMenu(CurrentMenu,xp,-1,-1);
			}
			else
			{
				yp--;
				if(pni=ni=FindNumMenu(CurrentMenu,xp,-1,-1))
					if(ni->nm_Selected)
						selectmi=FindNumMenu(CurrentMenu,xp,yp,-1);
			}
			break;
		case SELECTUP:
			xp=(EditMsg.MouseX-bl)/150;
			yp=(EditMsg.MouseY-bt)/fh;
			xp+=nm->nm_Top;
			{
				if(yp==0)			// Title selected
				{
					SelMenuItem=NULL;
					UnSelectAllMenus(CurrentMenu);
					if(ni=FindNumMenu(CurrentMenu,xp,-1,-1))
						SelectMenu(ni);
					else
					{
						if(xp==(CountItems((struct Node_MenuItem *)&nm->nm_MenuItem,NM_TITLE)))
						{
							if(ni=(struct Node_MenuItem *)MyAllocVec(sizeof(struct Node_MenuItem),MEMF_ANY|MEMF_CLEAR))
							{
								ni->nm_Node.ln_Name=ni->nm_Title;
								ni->nm_Type=NM_TITLE;
								strcpy(ni->nm_Title,GetCatString(MSG_NEWTITLE));
								SetFlag(ni->nm_Flags,MYMN_PopUp);
								AddTail(List(&nm->nm_MenuItem),Node(ni));
								SelectMenu(ni);
								nm->nm_Total++;
								SetGadgetAttrs((struct Gadget *)nm->nm_HorizProp,nm->nm_Window,NULL,
									PGA_Top, nm->nm_Top,
									PGA_Total, nm->nm_Total,
									PGA_Visible, nm->nm_Visible,
									TAG_DONE);
								if(nm->nm_Total==1)
								{
									PropertiesMode=PM_MENU;
									RefreshProperties();
								}
							}
						}
					}
				}
				else
				{
					yp--;
					if(xp>0)
					{
						if(SelMenuItem) if(SelMenuItem->nm_Selected && SelMenuItem->nm_Flags&MYMN_PopUp)
						{
							i=RevCountItems(SelMenuItem,NM_ITEM);
							if(GetMenuNum(FindNumMenu(CurrentMenu,xp-1,i,-1))==GetMenuNum(FindSelectedMenu(CurrentMenu)))
							{																								// have i selected sub menu?
								if(ni=FindNumMenu(CurrentMenu,xp-1,i,yp-i))
								{
									UnSelectAllMenus(CurrentMenu);
									SelectMenu(ni);
									goto skip;
								}
								else
								{
									UnSelectAllMenus(CurrentMenu);
									if((yp-i)==(i2=CountItems(SelMenuItem,NM_SUB)))
									{
										if(ni=(struct Node_MenuItem *)MyAllocVec(sizeof(struct Node_MenuItem),MEMF_ANY|MEMF_CLEAR))
										{
											ni->nm_Node.ln_Name=ni->nm_Title;
											ni->nm_Type=NM_SUB;
											strcpy(ni->nm_Title,GetCatString(MSG_NEWSUB));
											Insert(List(&nm->nm_MenuItem),Node(ni),Node(FindNumMenu(CurrentMenu,xp-1,i,yp-i-1)));
											SelectMenu(ni);
											goto skip;
										}
									}
								}
							}
						}
					}
					if(pni=ni=FindNumMenu(CurrentMenu,xp,-1,-1))
					{
						if(ni->nm_Selected)
						{
							if(ni=FindNumMenu(CurrentMenu,xp,yp,-1))
							{
								UnSelectAllMenus(CurrentMenu);
								SelectMenu(ni);
								SelMenuItem=ni;
							}
							else
							{
								UnSelectAllMenus(CurrentMenu);
								if(yp==(i=CountItems(pni,NM_ITEM)))
								{
									if(ni=(struct Node_MenuItem *)MyAllocVec(sizeof(struct Node_MenuItem),MEMF_ANY|MEMF_CLEAR))
									{
										ni->nm_Node.ln_Name=ni->nm_Title;
										ni->nm_Type=NM_ITEM;
										strcpy(ni->nm_Title,GetCatString(MSG_NEWITEM));
										ppni=pni=(struct Node_MenuItem *)FindNumMenu(CurrentMenu,xp,yp-1,-1);
										if(pni)
										{
											pni=(struct Node_MenuItem *)pni->nm_Node.ln_Succ;
											if(pni->nm_Type==NM_SUB)
											{
												while(pni->nm_Node.ln_Succ)
												{
													if(pni->nm_Type!=NM_SUB) { pni=ppni; break; }
													ppni=pni;
													pni=(struct Node_MenuItem *)pni->nm_Node.ln_Succ;
												}
											}
											else
												pni=FindNumMenu(CurrentMenu,xp,yp-1,-1);
										}
										Insert(List(&nm->nm_MenuItem),Node(ni),Node(pni));
										SelectMenu(ni);
										SelMenuItem=ni;
									}
								}
							}
						}
						else
							UnSelectAllMenus(CurrentMenu);
					}
				}
			}
skip:
			piRenderObject(CurrentMenu);
			if(PropertiesMode==PM_MENU) if(PropertiesWnd) piInitObjectGUI(CurrentMenu);
			PropertiesMode=PM_MENU;
			break;
	}
	return 1;
}

int FN_MouseMove_EditMenu(void)
{
	return 1;
}

int FN_NewSize_EditMenu(void)
{
	struct Node_Menu *nm;

	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);
	nm->nm_Visible=nm->nm_Window->Width/150;
	SetGadgetAttrs((struct Gadget *)nm->nm_HorizProp,nm->nm_Window,NULL,
		PGA_Top, nm->nm_Top,
		PGA_Total, nm->nm_Total,
		PGA_Visible, nm->nm_Visible,
		TAG_DONE);
	piRenderObject(CurrentMenu);
	return 1;
}

int FN_MenuVerify_EditMenu(void)
{
	return 1;
}

int FN_CloseWindow_EditMenu(void)
{
	piCloseWindow(CurrentMenu);
	PropertiesMode=PM_NONE;
	RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
	return 0;
}

int FN_ActiveWindow_EditMenu(void)
{
	struct Node_Menu *nm;

	piGetAttr(CurrentMenu,OBJ_Node,(ULONG *)&nm);
	PropertiesMode=PM_MENU;
	RefreshProperties();
	if(TB_WindowWnd) CloseWindow_TB_Window();
	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	SetWndSignals();
	return 1;
}

int FN_IDCMPUpdate_EditMenu()
{
	switch(GetTagData(GA_ID,0,(struct TagItem *)EditMsg.IAddress))
	{
		case 2: MENUFN_Right(); break;
		case 3: MENUFN_Left(); break;
		case 4: MENUFN_Horiz(); break;
	}
	return 1;
}
