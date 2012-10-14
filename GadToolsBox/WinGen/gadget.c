/*
** Gadgets code generator
*/

#include<string.h>
#include<stdio.h>
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
#include<gadgets/listview.h>
#include"main.h"

STRPTR ConvertString(STRPTR,BOOL);

extern struct GUI *GUI;
extern struct Screen *Scr;

void WIND_GenerateWindow(BPTR handle,piObject *wo)
{
	struct Node_Window *nw;

	nw=(struct Node_Window *)OBJECT_DATA(wo);

	FPrintf(handle,"WND_%s DIALOGEX DISCARDABLE %ld, %ld, %ld, %ld\n",nw->nw_Label,nw->nw_X/4,nw->nw_Y/4,nw->nw_Width/4,nw->nw_Height/4);
	FPrintf(handle,"STYLE WS_POPUP | WS_CAPTION");
	if(nw->nw_Flags&WFLG_SIZEGADGET) FPuts(handle,"| WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME");
	else FPuts(handle,"| DS_MODALFRAME");
	if(nw->nw_Flags&WFLG_CLOSEGADGET) FPuts(handle,"| WS_SYSMENU");
	FPuts(handle,"\n");
	if((nw->nw_MyFlags&MYWA_DragToolHoriz) || (nw->nw_MyFlags&MYWA_DragToolVert))
	{
		FPrintf(handle,"EXSTYLE WS_EX_TOOLWINDOW\n");
	}
	if(nw->nw_MenuTitle[0]) FPrintf(handle,"MENU MN_%s",nw->nw_MenuTitle);
	FPrintf(handle,"CAPTION \"%s\"\n",ConvertString(nw->nw_WindowTitle,FALSE));
	FPrintf(handle,"FONT 8, \"MS Sans Serif\", 0, 0, 0x1\n");
}

void FRAME_GenerateFrame(BPTR handle,piObject *fo)
{
	struct Node_Frame *nf;
	struct Node_Window *nw;

	nf=(struct Node_Frame *)OBJECT_DATA(fo);

	FPuts(handle,"    ");
	switch(nf->nf_FrameType)
	{
		default:
			FPrintf(handle,"GROUPBOX \"\",-1,%ld,%ld,%ld,%ld",nf->nf_X/4,nf->nf_Y/4,nf->nf_Width/4,nf->nf_Height/4);
			break;
		case FTYPE_HORIZONTALLINE:
			FPrintf(handle,"GROUPBOX \"\",-1,%ld,%ld,%ld,%ld",nf->nf_X/4,nf->nf_Y/4,nf->nf_Width/4+1,2);
			break;
		case FTYPE_VERTICALLINE:
			FPrintf(handle,"GROUPBOX \"\",-1,%ld,%ld,%ld,%ld",nf->nf_X/4,nf->nf_Y/4,2,nf->nf_Height/4+1);
			break;
	}
	FPuts(handle,"\n");
}

void WIND_GenerateInit(BPTR handle,piObject *wo)
{
	piObject *go;
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	struct GadToolsData *gtd;
	struct TagItem *ti;
	struct Node *n;
	BOOL doinit=FALSE;

	nw=(struct Node_Window *)OBJECT_DATA(wo);

	for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
	{
		ng=(struct Node_Gadget *)OBJECT_DATA(go);
		gtd=(struct GadToolsData *)GadgetData(ng);
		switch(ng->ng_PlugInID)
		{
			case POPUPMENU_ID:
				if(ti=FindTagItem(PUMG_Labels,ng->ng_Tags))
					if(!IsListEmpty(List(ti->ti_Data)))
						doinit=TRUE;
				break;
			case CYCLE_ID:
				if(ti=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
					if(!IsListEmpty(List(ti->ti_Data)))
						doinit=TRUE;
				break;
		}
	}

	if(doinit)
	{
		FPrintf(handle,"WND_%s DLGINIT\nBEGIN\n",nw->nw_Label);

		for(go=(piObject *)GetHead(List(&nw->nw_List_Gadget));GetSucc(go);go=(piObject *)GetSucc(go))
		{
			ng=(struct Node_Gadget *)OBJECT_DATA(go);
			gtd=(struct GadToolsData *)GadgetData(ng);
			switch(ng->ng_PlugInID)
			{
				case POPUPMENU_ID:
					if(ti=FindTagItem(PUMG_Labels,ng->ng_Tags))
						if(!IsListEmpty(List(ti->ti_Data)))
							for(n=GetHead(ti->ti_Data);GetSucc(n);n=GetSucc(n))
								FPrintf(handle,"    GD_%s, 0x403, %ld, 0, \"%s\"\n",ng->ng_Label,strlen(n->ln_Name)+1,n->ln_Name);
					break;
				case CYCLE_ID:
					if(ti=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
						if(!IsListEmpty(List(ti->ti_Data)))
							for(n=GetHead(ti->ti_Data);GetSucc(n);n=GetSucc(n))
								FPrintf(handle,"    GD_%s, 0x403, %ld, 0, \"%s\"\n",ng->ng_Label,strlen(n->ln_Name)+1,n->ln_Name);
					break;
			}
		}
		FPuts(handle,"END\n");
	}
}

void GADGET_GenerateGadget(BPTR handle,piObject *go)
{
	struct Node_Gadget *ng;
	struct Node_Window *nw;
	struct GadToolsData *gtd;
	struct TagItem *t,*ti;
	struct Node *n;
	int a;

	ng=(struct Node_Gadget *)OBJECT_DATA(go);
	nw=(struct Node_Window *)OBJECT_DATA(ng->ng_Window);
	gtd=(struct GadToolsData *)GadgetData(ng);

	FPuts(handle,"    ");

	switch(ng->ng_PlugInID)
	{
		case BUTTON_ID:
			if(t=FindTagItem(TAG_USER+2,ng->ng_OwnTags))
			{
				FPrintf(handle,"CONTROL \"%s\",GD_%s,\"Button\",BS_AUTOCHECKBOX|BS_PUSHALIKE|WS_TABSTOP",ConvertString(gtd->g_Text,TRUE),ng->ng_Label);
				if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"|WS_DISABLED");
				FPrintf(handle,",%ld,%ld,%ld,%ld",ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			}
			else
			{
				FPrintf(handle,"%s \"%s\",GD_%s,%ld,%ld,%ld,%ld",FindTagItem(GA_Immediate,ng->ng_Tags)->ti_Data?"DEFPUSHBUTTON":"PUSHBUTTON",ConvertString(gtd->g_Text,TRUE),ng->ng_Label,ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
				if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,",WS_DISABLED");
			}
			break;
		case CHECKBOX_ID:
			FPrintf(handle,"CONTROL \"%s\",GD_%s,\"Button\",BS_AUTOCHECKBOX|WS_TABSTOP",ConvertString(gtd->g_Text,TRUE),ng->ng_Label);
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"|WS_DISABLED");
			if(gtd->g_Flags&PLACETEXT_RIGHT) FPuts(handle,"|BS_LEFTTEXT");
//			if(t=FindTagItem(GTCB_Checked,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"GTCB_Checked, TRUE, ");
			FPrintf(handle,",%ld,%ld,%ld,%ld",ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			if(gtd->g_Flags&PLACETEXT_RIGHT) FPuts(handle,",WS_EX_RIGHT");
			break;
		case COLORWHEEL_ID:
			break;
		case CYCLE_ID:
			FPrintf(handle,"COMBOBOX GD_%s,%ld,%ld,%ld,%ld,CBD_DROPDOWN|WS_VSCROLL|WS_TABSTOP",ng->ng_Label,ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			break;
		case DATATYPE_ID:
			break;
		case GRADIENTSLIDER_ID:
			break;
		case INTEGER_ID:
			FPrintf(handle,"EDITTEXT GD_%s,%ld,%ld,%ld,%ld,ES_AUTOHSCROLL|ES_NUMBER|WS_TABSTOP",ng->ng_Label,ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			if(t=FindTagItem(STRINGA_Justification,ng->ng_Tags))
			{
				switch(t->ti_Data)
				{
					case STRINGLEFT: break;
					case STRINGCENTER: FPuts(handle,"|ES_CENTER"); break;
					case STRINGRIGHT: FPuts(handle,"|ES_RIGHT"); break;
				}
			}
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"|WS_DISABLED");
			break;
		case LISTVIEW_ID:
			FPrintf(handle,"LISTBOX GD_%s,%ld,%ld,%ld,%ld,LBS_NOINTEGRALHEIGHT|LBS_DISABLENOSCROLL|WS_VSCROLL|WS_TABSTOP",ng->ng_Label,ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"|WS_DISABLED");
			break;
		case MX_ID:
			if(ti=FindTagItem(TAG_USER+1,ng->ng_OwnTags))
			{
				for(n=GetHead(ti->ti_Data),a=0;GetSucc(n);n=GetSucc(n),a++)
				{
					FPrintf(handle,"CONTROL \"%s\",GD_%s_%ld,\"Button\",BS_AUTORADIOBUTTON",ConvertString(n->ln_Name,TRUE),ng->ng_Label,a);
					if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"|WS_DISABLED");
					if(a==0) FPuts(handle,"|WS_GROUP|WS_TABSTOP");
					if(gtd->g_Flags&PLACETEXT_RIGHT) FPuts(handle,"|BS_LEFTTEXT");
					FPrintf(handle,",%ld,%ld,%ld,%ld",ng->ng_X/4,ng->ng_Y/4+a*ng->ng_Height/4+2,ng->ng_Width/4,ng->ng_Height/4);
					if(gtd->g_Flags&PLACETEXT_RIGHT) FPuts(handle,",WS_EX_RIGHT");
				}
			}
			break;
		case NUMBER_ID:
			FPrintf(handle,"EDITTEXT GD_%s,%ld,%ld,%ld,%ld,ED_NUMBER|ES_READONLY",ng->ng_Label,ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			break;
		case PAGE_ID:
			FPrintf(handle,"CONTROL \"\",GD_%s,\"SysTabControl32\",0x0,%ld,%ld,%ld,%ld",ng->ng_Label,ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			break;
		case PALETTE_ID:
			break;
		case POPUPMENU_ID:
			FPrintf(handle,"COMBOBOX GD_%s,%ld,%ld,%ld,%ld,CBD_DROPDOWN|WS_VSCROLL|WS_TABSTOP",ng->ng_Label,ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			break;
		case PROGRESS_ID:
			FPrintf(handle,"CONTROL \"\",GD_%s,\"msctls_progress32\",",ng->ng_Label);
			if(t=FindTagItem(PROG_Border,ng->ng_Tags)) FPuts(handle,"WS_BORDER,");
			else FPuts(handle,"0x0,");
			FPrintf(handle,"%ld,%ld,%ld,%ld",ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			break;
		case SCROLLER_ID:
			FPrintf(handle,"SCROLLBAR GD_%s,%ld,%ld,%ld,%ld",ng->ng_Label,ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			if(t=FindTagItem(PGA_Freedom,ng->ng_Tags))
			{
				switch(t->ti_Data)
				{
					case LORIENT_HORIZ: FPuts(handle,"SBS_HORIZ"); break;
					case LORIENT_VERT: FPuts(handle,"SBS_VERT"); break;
				}
			}
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"|WS_DISABLED");
			break;
		case SLIDER_ID:
			FPrintf(handle,"CONTROL \"%s\",GD_%s,\"msctls_trackbar32\",TBS_BOTH|TBS_AUTOTICKS|WS_TABSTOP",ConvertString(gtd->g_Text,TRUE),ng->ng_Label);
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"|WS_DISABLED");
			if(t=FindTagItem(PGA_Freedom,ng->ng_Tags))
			{
				FPuts(handle,"PGA_Freedom, ");
				switch(t->ti_Data)
				{
					case LORIENT_HORIZ: break;
					case LORIENT_VERT: FPuts(handle,"|SBS_VERT"); break;
				}
				FPuts(handle,", ");
			}
			FPrintf(handle,",%ld,%ld,%ld,%ld",ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			break;
		case STRING_ID:
			FPrintf(handle,"EDITTEXT GD_%s,%ld,%ld,%ld,%ld,ES_AUTOHSCROLL",ng->ng_Label,ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"|WS_DISABLED");
			break;
		case TAPEDECK_ID:
			break;
		case TEXT_ID:
			FPrintf(handle,"EDITTEXT GD_%s,%ld,%ld,%ld,%ld,ES_READONLY",ng->ng_Label,ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			break;
		case TEXTFIELD_ID:
			FPrintf(handle,"CONTROL \"\",GD_%s,\"RICHEDIT\",ES_AUTOHSCROLL|ES_MULTILINE|ED_AUTOVSCROLL|ES_WANTRETURN|WS_VSCROLL|WS_HSCROLL|WS_TABSTOP",ng->ng_Label);
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"|WS_DISABLED");
			if(t=FindTagItem(TEXTFIELD_ReadOnly,ng->ng_Tags)) FPuts(handle,"|ES_READONLY");
			if(t=FindTagItem(TEXTFIELD_Border,ng->ng_Tags))
			{
				switch(t->ti_Data)
				{
					case TEXTFIELD_BORDER_NONE: FPuts(handle,""); break;
					case TEXTFIELD_BORDER_BEVEL: FPuts(handle,"|WS_BORDER"); break;
					case TEXTFIELD_BORDER_DOUBLEBEVEL: FPuts(handle,"|WS_BORDER"); break;
				}
				FPuts(handle,", ");
			}
			FPrintf(handle,"WS_TABSTOP,%ld,%ld,%ld,%ld",ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			break;
		case TREEVIEW_ID:
			if(FindTagItem(LIST_IsTree,ng->ng_Tags)->ti_Data)
			{
				FPrintf(handle,"CONTROL \"\",GD_%s,\"SysTreeView32\",TVS_HASBUTTONS|TVS_LINESATROOT|TVS_DISABLEDRAGDROP|TVS_SHOWSELALWAYS|WS_TABSTOP",ng->ng_Label);
				if(t=FindTagItem(LIST_ShowTreeLines,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"|TVS_HASLINES");
			}
			else
			{
				FPrintf(handle,"CONTROL \"\",GD_%s,\"SysListView32\",LVS_REPORT|LVS_SHOWSELALWAYS|LVS_NOSORTHEADER|WS_TABSTOP",ng->ng_Label);
				if(t=FindTagItem(LIST_MultiSelect,ng->ng_Tags)) if(!t->ti_Data) FPuts(handle,"|LVS_SINGLESEL");
			}
			if(t=FindTagItem(LIST_Border,ng->ng_Tags)) FPuts(handle,"|WS_BORDER");
			if(t=FindTagItem(GA_Disabled,ng->ng_Tags)) if(t->ti_Data) FPuts(handle,"|WS_DISABLED");
			FPrintf(handle,",%ld,%ld,%ld,%ld",ng->ng_X/4,ng->ng_Y/4,ng->ng_Width/4,ng->ng_Height/4);
			break;
	}
	FPuts(handle,"\n");
}
