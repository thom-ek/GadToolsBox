#ifndef GTB_TOOLBAR_H
#define GTB_TOOLBAR_H
/*
** Menu
*/

#define ID_TBAR MAKE_ID('T','B','A','R') // Toolbar Chunk

struct Node_Toolbar
{
	ULONG np_ID;
	struct Window *nt_Window;
	struct Page *nt_Pages;
	Object *nt_Draw,*nt_Preview,*nt_UpButton,*nt_DownButton,*nt_VertProp,*nt_LeftButton,*nt_RightButton,*nt_HorizProp;
	LONG nt_Top,nt_Left;
	struct ColorPalette nt_Palette[16];
	struct BitMap *nt_BitMap,*nt_TmpBitMap;

	UBYTE np_Label[40];
	UBYTE np_PointerData[20*20/8*4];
};

ULONG Dispatcher_Toolbar(piClass *,piObject *,Msg);
ULONG INIT_Toolbar(piClass *,piObject *,Msg);
ULONG APPLY_Toolbar(piClass *,piObject *,Msg);
ULONG CREATE_Toolbar(piClass *,piObject *,Msg);
ULONG DELETE_Toolbar(piClass *,piObject *,Msg);
ULONG NEW_Toolbar(piClass *,piObject *,struct piSet *);
ULONG DISPOSE_Toolbar(piClass *,piObject *,Msg);
ULONG GET_Toolbar(piClass *,piObject *,struct piGet *);
ULONG READ_Toolbar(piClass *,piObject *,struct piIO *);
ULONG WRITE_Toolbar(piClass *,piObject *,struct piIO *);

#endif
