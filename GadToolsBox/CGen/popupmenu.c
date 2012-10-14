struct PopupMenu *PM_CreateMenusA(struct NewMenu *menu,struct TagItem *tags)
{
	struct TagItem *ti;
	struct PopupMenu *pm=NULL;
	struct Menu *m;
	ULONG NumMenus,a;

	for(NumMenus=0,n=menu;m->nm_Type!=NM_END;m+=(sizeof(struct Menu)),NumMenus++);
	if(ti=AllocTagItems((NumMenus+10)*5))
	{
		for(n=menu,a=0;m->nm_Type!=NM_END;m+=(sizeof(struct Menu)),NumMenus++)
		{
			switch(m->nm_Type)
			{
				case NM_TITLE:
					ti[a].ti_Tag=PM_Title; ti[a].ti_Data=m->nm_Label; a++;
					ti[a].ti_Tag=PM_NoSelect; ti[a].ti_Data=TRUE; a++;
					ti[a].ti_Tag=PM_ShinePen; ti[a].ti_Data=TRUE; a++;
					ti[a].ti_Tag=PM_Item; ti[a].ti_Data=PM_MakeItem(PM_WideTitleBar,TRUE,TAG_DONE); a++;
					break;
				case NM_ITEM:
					ti[a].ti_Tag=PM_Item;
					if(m->nm_Label==NM_BARLABEL)
					{
						ti[a].ti_Data=PM_MakeItem(PM_TitleBar,TRUE,
																			TAG_DONE);
					}
					else
					{
						ti[a].ti_Data=PM_MakeItem(PM_Title,m->nm_Label,
																			(m->nm_Flags&MENUTOGGLE)?PM_Checkit:TAG_IGNORE,TRUE,
																			(m->nm_Flags&CHECKED)?PM_Checked:TAG_IGNORE,TRUE,
																			(m->nm_Flags&NM_ITEMDISABLED)?PM_Disabled:TAG_IGNORE,TRUE,
																			PM_UserData,m->nm_UserData,
																			TAG_DONE);
					}
					a++;
					break;
				case NM_SUB:
					// Not supported yet
					break;
			}
		}
		ti[a].ti_Tag=TAG_DONE;
		pm=PM_MakeMenuA(ti);
		FreeTagItems(ti);
	}
	return pm;
}

void PM_FreeMenus(struct PopupMenu *pm)
{
	PM_FreePopupMenu(pm);
}

ULONG PM_LayoutMenusA(struct Window *w,struct PopupMenu *pm,ULONG code)
{
	return PM_OpenPopupMenu(w,
		PM_Menu,pm,
		PM_Code,code,
		TAG_DONE);
}
