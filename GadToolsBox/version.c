/*
** GadToolsBox v3 - Version Management
*/

#include"includes.h"

/*
** Defines
*/

/*
** Functions
*/

ULONG Dispatcher_Version(piClass *cl,piObject *o,Msg msg)
{
	ULONG retval=NULL;

	switch(msg->MethodID)
	{
		case OM_INIT:
			retval=INIT_Version(cl,o,msg);
			break;
		case OM_APPLY:
			retval=APPLY_Version(cl,o,msg);
			break;
		case OM_NEW:
			retval=NEW_Version(cl,o,(struct piSet *)msg);
			break;
		case OM_GET:
			retval=GET_Version(cl,o,(struct piGet *)msg);
			break;
		case OM_DISPOSE:
			retval=DISPOSE_Version(cl,o,(struct piSet *)msg);
			break;
		case OM_CREATE:
			retval=OPEN_Version(cl,o,msg);
			break;
		case OM_DELETE:
			retval=CLOSE_Version(cl,o,msg);
			break;
		case OM_READ:
			retval=READ_Version(cl,o,(struct piIO *)msg);
			break;
		case OM_WRITE:
			retval=WRITE_Version(cl,o,(struct piIO *)msg);
			break;
	}
	return retval;
}

ULONG NEW_Version(piClass *cl,piObject *o,struct piSet *msg)
{
	piObject *obj;
	struct Node_Version *nv;

	if(obj=MyAllocVec(sizeof(piObject)+cl->Size,MEMF_PUBLIC|MEMF_CLEAR))
	{
		nv=(struct Node_Version *)OBJECT_DATA(obj);
		obj->ln_Name=nv->nv_ProgramName;
		nv->nv_Version=1;
		nv->nv_Revision=0;
		strcpy(nv->nv_ProgramName,GetCatString(MSG_UNTITLED));
		strcpy(nv->nv_Author,GetCatString(MSG_UNKNOWN));
	}
	return (ULONG) obj;
}

ULONG DISPOSE_Version(piClass *cl,piObject *o,Msg msg)
{
	piDoMethod(o,OM_DELETE);
	MyFreeVec(o);
	return TRUE;
}

ULONG OPEN_Version(piClass *cl,piObject *o,Msg msg)
{
	if(!EditVersionWnd)
	{
		OpenWindow_EditVersion();
		piInitObjectGUI(o);
		SetWndSignals();
	}
	else
		ActivateWindow(EditVersionWnd);
	return TRUE;
}

ULONG CLOSE_Version(piClass *cl,piObject *o,Msg msg)
{
	if(EditVersionWnd)
	{
		piApplyObject(o);
		CloseWindow_EditVersion();
		SetWndSignals();
	}
	return TRUE;
}

ULONG GET_Version(piClass *cl,piObject *o,struct piGet *msg)
{
	struct Node_Version *nv=(struct Node_Version *)OBJECT_DATA(o);

	switch(msg->TagID)
	{
		case OBJ_Node: *(msg->TagData)=(ULONG)nv; return TRUE;
		default: *(msg->TagData)=NULL; return FALSE;
	}
}

ULONG READ_Version(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0;
	struct Node_Version *nv;

	piGetAttr(o,OBJ_Node,(ULONG *)&nv);
	ReadChunkBytes(msg->IFFh,(APTR)&nv->nv_Version,SIZE(Node_Version,nv_Version));
	return (ULONG)error;
}

ULONG WRITE_Version(piClass *cl,piObject *o,struct piIO *msg)
{
	LONG error=0;
	struct Node_Version *nv;
	
	piGetAttr(o,OBJ_Node,(ULONG *)&nv);
	if(error=PushChunk(msg->IFFh,NULL,ID_SVER,SIZE(Node_Version,nv_Version))) return (ULONG)error;
	WriteChunkBytes(msg->IFFh,(APTR)&nv->nv_Version,SIZE(Node_Version,nv_Version));
	if(error=PopChunk(msg->IFFh)) return (ULONG) error;
	return (ULONG)error;
}

TEXT DateString[15];

ULONG INIT_Version(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Version *nv;
	struct Window *PropsWnd=NULL;
	struct DateTime dt;

	piGetAttr(o,OBJ_Node,(ULONG *)&nv);

	PropsWnd=EditVersionWnd;
	SetString(PropsWnd,EditVersionGadgets[GD_PGVER_PROGRAMNAME],nv->nv_ProgramName);
	SetString(PropsWnd,EditVersionGadgets[GD_PGVER_AUTHOR],nv->nv_Author);
	SetInteger(PropsWnd,EditVersionGadgets[GD_PGVER_VERSION],nv->nv_Version);
	SetInteger(PropsWnd,EditVersionGadgets[GD_PGVER_REVISION],nv->nv_Revision);

	dt.dat_Format=FORMAT_CDN;
	dt.dat_Flags=NULL;
	dt.dat_StrDay=NULL;
	dt.dat_StrDate=DateString;
	dt.dat_StrTime=NULL;
	DateStamp(&dt.dat_Stamp);
	DateToStr(&dt);
	SetText(PropsWnd,EditVersionGadgets[GD_PGVER_DATE],DateString);
	return TRUE;
}

ULONG APPLY_Version(piClass *cl,piObject *o,Msg msg)
{
	struct Node_Version *nv;

	piGetAttr(o,OBJ_Node,(ULONG *)&nv);

	strcpy(nv->nv_ProgramName,GetString(EditVersionGadgets[GD_PGVER_PROGRAMNAME]));
	strcpy(nv->nv_Author,GetString(EditVersionGadgets[GD_PGVER_AUTHOR]));
	nv->nv_Version=GetInteger(EditVersionGadgets[GD_PGVER_VERSION]);
	nv->nv_Revision=GetInteger(EditVersionGadgets[GD_PGVER_REVISION]);

	return TRUE;
}

/******************************************************************************/

int FN_CloseWindow_EditVersion(void)
{
	piApplyObject(CurrentVersion);
	piCloseWindow(CurrentVersion);
	return 2;
}

int FN_PGVER_PROGRAMNAME_Clicked(void)
{
	return 1;
}

int FN_PGVER_AUTHOR_Clicked(void)
{
	return 1;
}

int FN_PGVER_VERSION_Clicked(void)
{
	return 1;
}

int FN_PGVER_REVISION_Clicked(void)
{
	return 1;
}

int FN_ActiveWindow_EditVersion( void )
{
	ActiveObject=ID_SVER;
	PropertiesMode=PM_NONE;
	RefreshPropertiesWindow(GetCatString(MSG_NOOBJECT));
	if(TB_WindowWnd) CloseWindow_TB_Window();
	if(TB_PointerWnd) CloseWindow_TB_Pointer();
	return 1;
}
