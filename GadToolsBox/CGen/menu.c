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

extern struct GUI *GUI;

void MENU_GenerateDefs(BPTR handle,piObject *mo)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;
	TEXT Title[256],Label[256],flagsstr[256];
	UBYTE orit;

	nm=(struct Node_Menu *)OBJECT_DATA(mo);
	FPrintf(handle,"/* %s Menu defintion */\n",nm->nm_Label);
	FPrintf(handle,"struct NewMenu %sNewMenu[]=\n{\n",nm->nm_Label);
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
	{
		orit=FALSE;
		if(ni->nm_Flags&MYMN_BarLabel) sprintf(Title,"NM_BARLABEL");
		else sprintf(Title,"MSG_%s%s_",nm->nm_Label,ni->nm_Label);
		if((ni->nm_Flags&MYMN_PopUp) || (ni->nm_Flags&MYMN_BarLabel)) sprintf(Label,"NULL");
		else sprintf(Label,"FN_%s%s_Selected",nm->nm_Label,ni->nm_Label);
		flagsstr[0]='\0';
		switch(ni->nm_Type)
		{
			case NM_TITLE:
				if(!(ni->nm_Flags&MYMN_BarLabel)) strcat(Title,"MTITLE");
				GetFlag(ni->nm_Flags,MYMN_Disabled) { strcat(flagsstr,"MN_MENUDISABLED"); orit=TRUE; }
				FPuts(handle,"	NM_TITLE");
				break;
			case NM_ITEM:
				if(!(ni->nm_Flags&MYMN_BarLabel)) strcat(Title,"MITEM");
				GetFlag(ni->nm_Flags,MYMN_Disabled) { strcat(flagsstr,"MN_ITEMDISABLED"); orit=TRUE; }
				FPuts(handle,"	NM_ITEM");
				break;
			case NM_SUB:
				if(!(ni->nm_Flags&MYMN_BarLabel)) strcat(Title,"MSUB");
				GetFlag(ni->nm_Flags,MYMN_Disabled) { strcat(flagsstr,"MN_ITEMDISABLED"); orit=TRUE; }
				FPuts(handle,"	NM_SUB");
				break;
		}

		GetFlag(ni->nm_Flags,MYMN_Toggle) { if(orit) strcat(flagsstr,"|"); strcat(flagsstr,"CHECKIT"); orit=TRUE; }
		GetFlag(ni->nm_Flags,MYMN_Checked) { if(orit) strcat(flagsstr,"|"); strcat(flagsstr,"CHECKED"); orit=TRUE; }
		if(!orit) strcpy(flagsstr,"NULL");

		FPrintf(handle,", %s, NULL, %s, NULL, %s,\n",Title,flagsstr,Label);
	}
	FPuts(handle,"	NM_END, NULL, NULL, NULL, NULL, NULL\n};\n");
}

void MENU_GenerateProtos(BPTR handle,piObject *mo)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;

	nm=(struct Node_Menu *)OBJECT_DATA(mo);
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
	{
		if(ni->nm_Label[0]) FPrintf(handle,"int FN_%s%s_Selected(void);\n",nm->nm_Label,ni->nm_Label);
	}
}

void MENU_GenerateTemplate(BPTR handle,piObject *mo)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;

	nm=(struct Node_Menu *)OBJECT_DATA(mo);
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
	{
		if(ni->nm_Label[0]) FPrintf(handle,"int FN_%s%s_Selected()\n{\n	return 1;\n}\n\n",nm->nm_Label,ni->nm_Label);
	}
}

void MENU_GenerateCatalogSource(BPTR handle,piObject *mo,LONG *index)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;
	TEXT CommKey[10];

	nm=(struct Node_Menu *)OBJECT_DATA(mo);
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
	{
		if(ni->nm_Key[0]==' ' || ni->nm_Key[0]=='\0') sprintf(CommKey,"\\000");
		else sprintf(CommKey,"%c",ni->nm_Key[0]);
		switch(ni->nm_Type)
		{
			case NM_TITLE:
				if(!(ni->nm_Flags&MYMN_BarLabel))
				{
					FPrintf(handle,"const struct FC_Type _MSG_%s%s_MTITLE = { %ld, \"%s\\000%s\" };\n",nm->nm_Label,ni->nm_Label,*index,CommKey,ni->nm_Title);
					(*index)++;
				}
				break;
			case NM_ITEM:
				if(!(ni->nm_Flags&MYMN_BarLabel))
				{
					FPrintf(handle,"const struct FC_Type _MSG_%s%s_MITEM= { %ld, \"%s\\000%s\" };\n",nm->nm_Label,ni->nm_Label,*index,CommKey,ni->nm_Title);
					(*index)++;
				}
				break;
			case NM_SUB:
				if(!(ni->nm_Flags&MYMN_BarLabel))
				{
					FPrintf(handle,"const struct FC_Type _MSG_%s%s_MSUB = { %ld, \"%s\\000%s\" };\n",nm->nm_Label,ni->nm_Label,*index,CommKey,ni->nm_Title);
					(*index)++;
				}
				break;
		}
	}
}

void MENU_GenerateCatalogHeader(BPTR handle,piObject *mo)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;

	nm=(struct Node_Menu *)OBJECT_DATA(mo);
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
	{
		switch(ni->nm_Type)
		{
			case NM_TITLE:
				if(!(ni->nm_Flags&MYMN_BarLabel))
				{
					FPrintf(handle,"extern const struct FC_Type _MSG_%s%s_MTITLE;\n",nm->nm_Label,ni->nm_Label);
					FPrintf(handle,"#define MSG_%s%s_MTITLE ((APTR) &_MSG_%s%s_MTITLE)\n",nm->nm_Label,ni->nm_Label,nm->nm_Label,ni->nm_Label);
				}
				break;
			case NM_ITEM:
				if(!(ni->nm_Flags&MYMN_BarLabel))
				{
					FPrintf(handle,"extern const struct FC_Type _MSG_%s%s_MITEM;\n",nm->nm_Label,ni->nm_Label);
					FPrintf(handle,"#define MSG_%s%s_MITEM ((APTR) &_MSG_%s%s_MITEM)\n",nm->nm_Label,ni->nm_Label,nm->nm_Label,ni->nm_Label);
				}
				break;
			case NM_SUB:
				if(!(ni->nm_Flags&MYMN_BarLabel))
				{
					FPrintf(handle,"extern const struct FC_Type _MSG_%s%s_MSUB;\n",nm->nm_Label,ni->nm_Label);
					FPrintf(handle,"#define MSG_%s%s_MSUB ((APTR) &_MSG_%s%s_MSUB)\n",nm->nm_Label,ni->nm_Label,nm->nm_Label,ni->nm_Label);
				}
				break;
		}
	}
}

void MENU_GenerateCatalogDescription(BPTR handle,piObject *mo)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;
	TEXT CommKey[10];

	nm=(struct Node_Menu *)OBJECT_DATA(mo);
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
	{
		if(ni->nm_Key[0]==' ' || ni->nm_Key[0]=='\0') sprintf(CommKey,"\\000");
		else sprintf(CommKey,"%c",ni->nm_Key[0]);
		switch(ni->nm_Type)
		{
			case NM_TITLE:
				if(!(ni->nm_Flags&MYMN_BarLabel))
					FPrintf(handle,"MSG_%s%s_MTITLE (//)\n%s\\000%s\n;\n",nm->nm_Label,ni->nm_Label,CommKey,ni->nm_Title);
				break;
			case NM_ITEM:
				if(!(ni->nm_Flags&MYMN_BarLabel))
					FPrintf(handle,"MSG_%s%s_MITEM (//)\n%s\\000%s\n;\n",nm->nm_Label,ni->nm_Label,CommKey,ni->nm_Title);
				break;
			case NM_SUB:
				if(!(ni->nm_Flags&MYMN_BarLabel))
					FPrintf(handle,"MSG_%s%s_MSUB (//)\n%s\\000%s\n;\n",nm->nm_Label,ni->nm_Label,CommKey,ni->nm_Title);
				break;
		}
	}
}

void MENU_GenerateCatalogTranslation(BPTR handle,piObject *mo)
{
	struct Node_Menu *nm;
	struct Node_MenuItem *ni;
	TEXT CommKey[10];

	nm=(struct Node_Menu *)OBJECT_DATA(mo);
	for(ni=(struct Node_MenuItem *)GetHead(List(&nm->nm_MenuItem));ni->nm_Node.ln_Succ;ni=(struct Node_MenuItem *)ni->nm_Node.ln_Succ)
	{
		if(ni->nm_Key[0]==' ' || ni->nm_Key[0]=='\0') sprintf(CommKey,"\\000");
		else sprintf(CommKey,"%c",ni->nm_Key[0]);
		switch(ni->nm_Type)
		{
			case NM_TITLE:
				if(!(ni->nm_Flags&MYMN_BarLabel))
					FPrintf(handle,"MSG_%s%s_MTITLE\n\n;%s\\000%s\n;\n",nm->nm_Label,ni->nm_Label,CommKey,ni->nm_Title);
				break;
			case NM_ITEM:
				if(!(ni->nm_Flags&MYMN_BarLabel))
					FPrintf(handle,"MSG_%s%s_MITEM\n\n;%s\\000%s\n;\n",nm->nm_Label,ni->nm_Label,CommKey,ni->nm_Title);
				break;
			case NM_SUB:
				if(!(ni->nm_Flags&MYMN_BarLabel))
					FPrintf(handle,"MSG_%s%s_MSUB\n\n;%s\\000%s\n;\n",nm->nm_Label,ni->nm_Label,CommKey,ni->nm_Title);
				break;
		}
	}
}
