#ifndef GTB_SCREEN_H
#define GTB_SCREEN_H
/*
** Screen
*/

#define ID_SCRN MAKE_ID('S','C','R','N')  // Screen Chunk

#define TYPE_WORKBENCH 0
#define TYPE_CUSTOMSCREEN 1
#define TYPE_PUBLICSCREEN 2

#define MYSC_MakePublic (1<<0)
#define MYSC_ShowTitle (1<<3)
#define MYSC_Behind (1<<4)
#define MYSC_Quiet (1<<5)
#define MYSC_AutoScroll (1<<6)
#define MYSC_FullPalette (1<<7)
#define MYSC_Draggable (1<<8)
#define MYSC_Exclusive (1<<9)
#define MYSC_SharePens (1<<10)
#define MYSC_Interleaved (1<<11)
#define MYSC_LikeWB (1<<12)
#define MYSC_MinimizeISG (1<<13)
#define MYSC_DefaultID (1<<14)

struct Node_Screen
{
	UWORD ns_Width,ns_Height,ns_Depth;
	ULONG ns_DisplayID;
	UWORD ns_Type;
	TEXT ns_Title[256];
	TEXT ns_Public[30];
	TEXT ns_FontName[30];
	UWORD ns_FontSize;
	ULONG ns_MyFlags;
};

ULONG Dispatcher_Screen(piClass *,piObject *,Msg);
ULONG NEW_Screen(piClass *,piObject *,struct piSet *);
ULONG DISPOSE_Screen(piClass *,piObject *,Msg );
ULONG OPEN_Screen(piClass *,piObject *,Msg );
ULONG CLOSE_Screen(piClass *,piObject *,Msg );
ULONG SET_Screen(piClass *,piObject *,struct piSet *);
ULONG GET_Screen(piClass *,piObject *,struct piGet *);
ULONG READ_Screen(piClass *,piObject *,struct piIO *);
ULONG WRITE_Screen(piClass *,piObject *,struct piIO *);
ULONG INIT_Screen(piClass *,piObject *,Msg );
ULONG APPLY_Screen(piClass *,piObject *,Msg );

#endif
