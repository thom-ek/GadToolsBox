#ifndef GTB_FRAME_H
#define GTB_FRAME_H
/*
** Frame
*/

#define ID_FRME MAKE_ID('F','R','M','E')  // Frame Chunk

#define MYFR_Recessed (1<<0)

struct Node_Frame
{
	piObject *nf_Window;	// Parent Window
	struct Page *nf_Pages;

	UWORD nf_X,nf_Y,nf_Width,nf_Height;
	UBYTE nf_RelX,nf_RelY,nf_RelWidth,nf_RelHeight;
	UBYTE nf_FrameType;
	UBYTE nf_Selected;
	ULONG nf_Flags;
};

#define FTYPE_BUTTON 0
#define FTYPE_ICONDROPBOX 1
#define FTYPE_RIDGE 2
#define FTYPE_HORIZONTALLINE 3
#define FTYPE_VERTICALLINE 4

#define FROBJ_Type (TAG_USER+20+1)

piObject *piFindFrame(piObject *,UWORD,UWORD);
ULONG Dispatcher_Frame(piClass *,piObject *,Msg);
ULONG INIT_Frame(piClass *,piObject *,Msg);
ULONG APPLY_Frame(piClass *,piObject *,Msg);
ULONG DEFAULT_Frame(piClass *,piObject *,Msg);
ULONG COMPUTE_Frame(piClass *,piObject *,struct piCompute *);
ULONG HITTEST_Frame(piClass *,piObject *,struct piHitTest *);
ULONG NEW_Frame(piClass *,piObject *,struct piSet *);
ULONG DISPOSE_Frame(piClass *,piObject *,Msg);
ULONG ADDTAIL_Frame(piClass *,piObject *,struct piWindow *);
ULONG RENDER_Frame(piClass *,piObject *,Msg);
ULONG ERASE_Frame(piClass *,piObject *,Msg);
ULONG SET_Frame(piClass *,piObject *,struct piSet *);
ULONG GET_Frame(piClass *,piObject *,struct piGet *);
ULONG READ_Frame(piClass *,piObject *,struct piIO *);
ULONG WRITE_Frame(piClass *,piObject *,struct piIO *);

#endif
