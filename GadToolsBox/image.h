#ifndef GTB_IMAGE_H
#define GTB_IMAGE_H
/*
** Image
*/

#define ID_IMGE MAKE_ID('I','M','G','E')  // Image Chunk
#define ID_PNTR MAKE_ID('P','N','T','R')  // Image Chunk

#define IMTYPE_NORMAL 0
#define IMTYPE_TOOLBAR 1
#define IMTYPE_POINTER 2

struct Node_Pointer
{
	ULONG ni_ID;
	struct Window *ni_Window;
	struct Page *ni_Pages;
	Object *ni_Draw,*ni_Preview,*ni_UpButton,*ni_DownButton,*ni_VertProp,*ni_LeftButton,*ni_RightButton,*ni_HorizProp;
	LONG ni_Top,ni_Left;
	struct BitMap *ni_BitMap;

	UBYTE ni_Label[40];
	UWORD ni_PointX,ni_PointY;
	ULONG ni_Flags;
	UBYTE ni_ImageData[32*32/8*2];
};

struct Node_Image
{
	ULONG ni_ID;
	struct Window *ni_Window;
	struct Page *ni_Pages;
	Object *ni_Draw,*ni_Preview,*ni_UpButton,*ni_DownButton,*ni_VertProp,*ni_LeftButton,*ni_RightButton,*ni_HorizProp;
	LONG ni_Top,ni_Left;
	struct BitMap *ni_BitMap;
	struct ColorPalette *ni_CT;
	UBYTE *ni_Pens;

	UBYTE ni_Label[40];
	UBYTE ni_Type,ni_Depth;
	UWORD ni_Width,ni_Height;
	UWORD ni_PointX,ni_PointY;
	ULONG ni_Flags;
};

ULONG Dispatcher_Image(piClass *,piObject *,Msg);
ULONG NEW_Image(piClass *,piObject *,struct piSet *);
ULONG DISPOSE_Image(piClass *,piObject *,Msg );
ULONG CREATE_Image(piClass *,piObject *,Msg );
ULONG DELETE_Image(piClass *,piObject *,Msg );
ULONG RENDER_Image(piClass *,piObject *,Msg );
ULONG SET_Image(piClass *,piObject *,struct piSet *);
ULONG GET_Image(piClass *,piObject *,struct piGet *);
ULONG READ_Image(piClass *,piObject *,struct piIO *);
ULONG WRITE_Image(piClass *,piObject *,struct piIO *);
ULONG INIT_Image(piClass *,piObject *,Msg );
ULONG APPLY_Image(piClass *,piObject *,Msg );
ULONG CUT_Image(piClass *,piObject *,Msg );
ULONG COPY_Image(piClass *,piObject *,Msg );

void DisposeAllImages(void);
void DeleteAllImages(void);
int FN_MouseButtons_EditImage(void);
int FN_MouseMove_EditImage(void);
int FN_CloseWindow_EditImage(void);
int FN_ChangeWindow_EditImage(void);

#endif /* GTB_Image_H */
