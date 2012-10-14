/*
** Gadgets code generator
*/

#include<stdio.h>
#include<string.h>
#include<proto/dos.h>
#include<proto/gadtools.h>
#include<proto/intuition.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<intuition/gadgetclass.h>
#include<gadgets/colorwheel.h>
#include<gadgets/gradientslider.h>
#include<gadgets/page.h>
#include<gadgets/popupmenu.h>
#include<gadgets/progress.h>
#include<gadgets/tapedeck.h>
#include<gadgets/textfield.h>
#include"main.h"

STRPTR ConvertString(STRPTR,BOOL);

extern struct GUI *GUI;

void MENU_GenerateMenu(BPTR handle,piObject *mo)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;
	UBYTE prevtype=10;

	nm=(struct Node_Menu *)OBJECT_DATA(mo);
	FPrintf(handle,"MN_%s MENU DISCARDABLE\nBEGIN\n",nm->nm_Label);
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
	{
		switch(ni->nm_Type)
		{
			case NM_TITLE:
				if(prevtype!=NM_TITLE) FPuts(handle,"END\n");
				FPrintf(handle,"POPUP \"%s\"\nBEGIN\n",ConvertString(ni->nm_Title,TRUE));
				break;
			case NM_ITEM:
				if(prevtype!=NM_ITEM) FPuts(handle,"END\n");
				if(ni->nm_Flags&MYMN_BarLabel)
					FPrintf(handle,"MENUITEM SEPARATOR\n");
				else
					FPrintf(handle,"MENUITEM \"%s\" MD_%s\n",ConvertString(ni->nm_Title,TRUE),ni->nm_Label);
				GetFlag(ni->nm_Flags,MYMN_Disabled) FPuts(handle,"GRAYED");
				GetFlag(ni->nm_Flags,MYMN_Checked) FPuts(handle,"CHECKED");
				break;
			case NM_SUB:
				if(prevtype!=NM_SUB) FPuts(handle,"END\n");
				if(ni->nm_Flags&MYMN_BarLabel)
					FPrintf(handle,"MENUITEM SEPARATOR\n");
				else
					FPrintf(handle,"MENUITEM \"%s\" MD_%s\n",ConvertString(ni->nm_Title,TRUE),ni->nm_Label);
				GetFlag(ni->nm_Flags,MYMN_Disabled) FPuts(handle,"GRAYED");
				GetFlag(ni->nm_Flags,MYMN_Checked) FPuts(handle,"CHECKED");
				break;
		}
		prevtype=ni->nm_Type;
	}
	FPuts(handle,"END\n");
}
