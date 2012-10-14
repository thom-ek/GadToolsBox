/*
** Smart Bitmap Image Class
** $VER: smartbitmap.c 39.1 (10.1.98)
*/

#include<dos.h>
#include<string.h>
#include<clib/macros.h>
#include<proto/dos.h>
#include<proto/exec.h>
#include<proto/utility.h>
#include<proto/graphics.h>
#include<proto/intuition.h>
#include<proto/cybergraphics.h>
#include<images/smartbitmap.h>
#include<graphics/gfxmacros.h>
#include<exec/memory.h>
#include<libraries/gadtools.h>
#include<intuition/imageclass.h>
#include<intuition/gadgetclass.h>
#include<intuition/intuitionbase.h>
#include"smartbitmap.image_rev.h"

TEXT version[]=VERSTAG;

/*
** Defines
*/

#define unless(x) if(!(x))
#define SMBM_SetTagArg(tag,id,data)	{ tag.ti_Tag = (ULONG)(id); tag.ti_Data = (ULONG)(data); }
typedef ULONG (*HookFunction)(void);
#define SAVEDS 
#define ASMFUNC __saveds __asm
#define REG(x) register __## x
#define SMARTBITMAPIMAGE "smartbitmap.image"

#define Bit32(x) ((ULONG)(x|(x<<8)|(x<<16)|(x<<24)))
#define RGB8(r,g,b) ((ULONG)(((r)&0xff0000)|((g)&0xff00)|((b)&0xff)))

/*
** Prototypes
*/

Class * ASMFUNC GetSmartBitMapClass(void);
ULONG ASMFUNC SMBM_Dispatcher(REG(a0) Class *,REG(a2) Object *,REG(a1) Msg);
ULONG SMBM_NEW(Class *,Object *,struct opSet *);
ULONG SMBM_DISPOSE(Class *, Object *, Msg);
ULONG SMBM_SET(Class *, Object *, struct opSet *);
ULONG SMBM_GET(Class *,Object *,struct opGet *);
ULONG SMBM_DRAW(Class *,Object *,struct impDraw *);
void SAVEDS ConvertChunky(struct SMBMData *);

/*
** Variables
*/

struct SMBMData
{
	UBYTE im_Border;
	UBYTE im_SelBorder;
	UBYTE im_SourceDeep;
	UBYTE im_DestDeep;
	UBYTE im_DestCGFX;
	UBYTE im_Recessed;
	struct BitMap *im_SourceBM;
	struct BitMap *im_DestBM;
	struct RastPort im_SourceRP;
	struct RastPort im_DestRP;
	Object *im_FrameImage;
	struct Screen *im_Screen;
	struct SmartBitMapIRGB *im_Palette;
	UWORD *im_Pens;
	UWORD im_Width;
	UWORD im_Height;
	UBYTE im_PensAlloc,im_PaletteAlloc;
	LONG im_Allocated[256];
};

Class *cl=NULL;
struct Library *CyberGfxBase=NULL;

/*
** Create Class
*/

int ASMFUNC __UserLibInit(REG(a6) struct MyLibrary *libbase)
{
	if(IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",36L))
	{
		if(UtilityBase=OpenLibrary("utility.library",36L))
		{
			if(GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",36L))
			{
				CyberGfxBase=OpenLibrary("cybergraphics.library",36L);
				if (cl = MakeClass (NULL, IMAGECLASS, NULL, sizeof(struct SMBMData), 0L))
				{
					cl->cl_Dispatcher.h_Entry = (HookFunction)  SMBM_Dispatcher;
					cl->cl_UserData = (ULONG) libbase;
//					AddClass (cl);
					return(FALSE);
				}
				if(CyberGfxBase) CloseLibrary(CyberGfxBase);
				CloseLibrary((struct Library *)GfxBase);
			}
			CloseLibrary(UtilityBase);
		}
		CloseLibrary((struct Library *)IntuitionBase);
	}
	return(TRUE);
}

void ASMFUNC __UserLibCleanup(REG(a6) struct MyLibrary *libbase)
{
	FreeClass(cl);
	if(IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
	if(UtilityBase) CloseLibrary(UtilityBase);
	if(GfxBase) CloseLibrary((struct Library *)GfxBase);
	if(CyberGfxBase) CloseLibrary(CyberGfxBase);
}


Class * ASMFUNC GetSmartBitMapClass()
{
	return cl;
}

/*
** Dispatcher
*/

ULONG ASMFUNC SMBM_Dispatcher( REG(a0) Class *cl, REG(a2) Object *o, REG(a1) Msg msg)
{
	ULONG retval;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	switch( msg->MethodID )
	{
		case OM_NEW:
			retval = SMBM_NEW(cl, o, (struct opSet *)msg );
			break;
		case OM_DISPOSE:
			retval = SMBM_DISPOSE(cl, o, msg );
			break;
		case OM_SET:
			retval = SMBM_SET(cl, o, (struct opSet *)msg );
			break;
		case OM_GET:
			retval = SMBM_GET(cl, o, (struct opGet *)msg );
			break;
		case IM_DRAW:
			retval = SMBM_DRAW(cl, o, (struct impDraw *)msg );
			break;
		default:
			retval = DoSuperMethodA(cl, o, msg);
			break;
	}
	return(retval);
}

/*
** OM_NEW
*/

ULONG SAVEDS SMBM_NEW(Class *cl,Object *o,struct opSet *ops )
{
	Object *object;
	struct SMBMData *IM;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	object = (Object *)DoSuperMethodA( cl, o, (Msg)ops );
	if(object)
	{
		IM = INST_DATA( cl, object );
		IM->im_PaletteAlloc=FALSE;
		IM->im_PensAlloc=FALSE;
		IM->im_FrameImage=NULL;

		IM->im_Border = GetTagData( SMBM_Border, FALSE, ops->ops_AttrList );
		IM->im_SelBorder = GetTagData( SMBM_SelBorder, IM->im_Border, ops->ops_AttrList );
		IM->im_Recessed = GetTagData( SMBM_Recessed, FALSE, ops->ops_AttrList );
		IM->im_Screen = (struct Screen *)GetTagData( SMBM_Screen, 0, ops->ops_AttrList );
		IM->im_SourceBM = (struct BitMap *)GetTagData( SMBM_BitMap, 0, ops->ops_AttrList );
		IM->im_Palette = (struct SmartBitMapIRGB *)GetTagData( SMBM_Palette, 0, ops->ops_AttrList );
		IM->im_Pens = (UWORD *)GetTagData( SMBM_Pens, 0, ops->ops_AttrList );
		if(!IM->im_Screen || !IM->im_SourceBM)	// We need destination screen and source bitmap
		{
			CoerceMethod(cl, o, OM_DISPOSE);
			return NULL;
		}
		IM->im_Width = (UWORD)GetTagData( IA_Width, GetBitMapAttr(IM->im_SourceBM,BMA_WIDTH), ops->ops_AttrList );
		IM->im_Height = (UWORD)GetTagData( IA_Height, GetBitMapAttr(IM->im_SourceBM,BMA_HEIGHT), ops->ops_AttrList );
		SetAttrs(object,IA_Width,IM->im_Width,IA_Height,IM->im_Height,TAG_DONE);

		if(GetBitMapAttr(IM->im_SourceBM,BMA_DEPTH)>8)
		{
			if(!CyberGfxBase) 										// We cannot handle deep bitmaps without cybergraphx
			{
				CoerceMethod(cl, o, OM_DISPOSE);
				return NULL;
			}
			IM->im_SourceDeep=TRUE;
		}
		else IM->im_SourceDeep=FALSE;

		if(GetBitMapAttr(IM->im_Screen->RastPort.BitMap,BMA_DEPTH)>8) IM->im_DestDeep=TRUE; else IM->im_DestDeep=FALSE;
		if(CyberGfxBase) IM->im_DestCGFX=GetCyberMapAttr(IM->im_Screen->RastPort.BitMap,CYBRMATTR_ISCYBERGFX);
		else IM->im_DestCGFX=FALSE;

		if(!IM->im_SourceDeep)									// We need palette definition for palette based bitmaps
		{
			if(!IM->im_Palette && !IM->im_Pens)
			{
				CoerceMethod(cl, o, OM_DISPOSE);
				return NULL;
			}
		}

		IM->im_PensAlloc=FALSE;
		IM->im_PaletteAlloc=FALSE;

		if(!IM->im_SourceDeep)									// Allocate pens conversion table for palette based bitmaps
		{
			if(!IM->im_Palette)
			{
				IM->im_Palette=(struct SmartBitMapIRGB *)AllocVec((2<<GetBitMapAttr(IM->im_SourceBM,BMA_DEPTH))*sizeof(struct SmartBitMapIRGB),MEMF_ANY);
				IM->im_PaletteAlloc=TRUE;
			}
			if(!IM->im_Pens)
			{
				IM->im_Pens=(UWORD *)AllocVec((2<<GetBitMapAttr(IM->im_SourceBM,BMA_DEPTH))*sizeof(UWORD),MEMF_ANY);
				IM->im_PensAlloc=TRUE;
			}
		}

		InitRastPort(&IM->im_SourceRP);
		IM->im_SourceRP.BitMap=IM->im_SourceBM;

		// Create destination bitmap compatible with screen
		IM->im_DestBM=AllocBitMap(GetBitMapAttr(IM->im_SourceBM,BMA_WIDTH),GetBitMapAttr(IM->im_SourceBM,BMA_HEIGHT),GetBitMapAttr(IM->im_Screen->RastPort.BitMap,BMA_DEPTH),BMF_CLEAR|BMF_MINPLANES,IM->im_Screen->RastPort.BitMap);
		InitRastPort(&IM->im_DestRP);
		IM->im_DestRP.BitMap=IM->im_DestBM;

		unless(IM->im_FrameImage=NewObject(NULL,"frameiclass",
			IA_Recessed,    IM->im_Recessed,
			IA_EdgesOnly,   TRUE,
			IA_FrameType,   FRAME_BUTTON,
			TAG_END))
		{
			CoerceMethod(cl, o, OM_DISPOSE);
			return NULL;
		}

		// Create pens conversion table for palette based bitmaps
		if(!IM->im_DestDeep && !IM->im_SourceDeep)
		{
			int a;
			if(IM->im_Palette)
			{
				for(a=0;a<(2<<GetBitMapAttr(IM->im_SourceBM,BMA_DEPTH));a++)
				{
					if(IM->im_Palette[a].I==((ULONG)-1))
						IM->im_Pens[a]=IM->im_Palette[a].I=IM->im_Allocated[a]=ObtainBestPen(IM->im_Screen->ViewPort.ColorMap,IM->im_Palette[a].R,IM->im_Palette[a].G,IM->im_Palette[a].B,OBP_Precision,PRECISION_IMAGE,TAG_DONE);
					else
					{
						IM->im_Pens[a]=(UWORD)-1;
						IM->im_Allocated[a]=-1;
					}
				}
			}
			else
			{
				for(a=0;a<(2<<GetBitMapAttr(IM->im_SourceBM,BMA_DEPTH));a++)
				{
					IM->im_Palette[a].I=(ULONG)IM->im_Pens[a];
					GetRGB32(IM->im_Screen->ViewPort.ColorMap,IM->im_Palette[a].I,1,&IM->im_Palette[a].R);
				}
			}
		}
		ConvertChunky(IM);			// Convert bitmap to destination
	}
	return( (ULONG)object );
}

/*
** OM_DISPOSE
*/

ULONG SAVEDS SMBM_DISPOSE(Class *cl, Object *o, Msg msg )
{
	struct SMBMData *IM = INST_DATA( cl, o );
	int a;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	if(!IM->im_PaletteAlloc)
	{
		for(a=0;a<(2<<GetBitMapAttr(IM->im_SourceBM,BMA_DEPTH));a++)
		{
			if(IM->im_Allocated[a]!=-1) ReleasePen(IM->im_Screen->ViewPort.ColorMap,IM->im_Allocated[a]);
		}
	}

	if(IM->im_DestBM) FreeBitMap(IM->im_DestBM);
	if(IM->im_PensAlloc) if(IM->im_Pens) FreeVec(IM->im_Pens);
	if(IM->im_PaletteAlloc) if(IM->im_Palette) FreeVec(IM->im_Palette);
	if(IM->im_FrameImage) DisposeObject(IM->im_FrameImage);
	return( DoSuperMethodA(cl, o, msg) );
}

/*
** OM_SET
*/

ULONG SAVEDS SMBM_SET(Class *cl, Object *o, struct opSet *ops)
{
	ULONG retval;
	struct SMBMData *IM = INST_DATA( cl, o );
	struct TagItem *tag, *tstate;
	struct RastPort *rp;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA( cl, o, (Msg)ops );

	tstate=ops->ops_AttrList;
	while(tag = NextTagItem( &tstate ))
	{
		switch(tag->ti_Tag)
		{
			case SMBM_Border:
				IM->im_Border= tag->ti_Data;
				retval = FALSE;
				break;
			case SMBM_SelBorder:
				IM->im_SelBorder= tag->ti_Data;
				retval = FALSE;
				break;
			case SMBM_Recessed:
				IM->im_Recessed= tag->ti_Data;
				retval = FALSE;
				break;
		}
	}
	if(!retval)
	{
		if(rp=ObtainGIRPort( ops->ops_GInfo ))
		{
			DoMethod(o,GM_RENDER,ops->ops_GInfo, rp, GREDRAW_UPDATE);
			ReleaseGIRPort(rp);
		}
	}

	return( retval );
}

/*
** OM_GET
*/

ULONG SAVEDS SMBM_GET(Class *cl,Object *o,struct opGet *opg )
{
	ULONG retval;
	struct SMBMData *IM = INST_DATA( cl, o );

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	switch( opg->opg_AttrID )
	{
		case SMBM_Border:
			*opg->opg_Storage = (ULONG)IM->im_Border;
			retval = TRUE;
			break;
		case SMBM_SelBorder:
			*opg->opg_Storage = (ULONG)IM->im_SelBorder;
			retval = TRUE;
			break;
		case SMBM_Recessed:
			*opg->opg_Storage = (ULONG)IM->im_Recessed;
			retval = TRUE;
			break;
		default:
			retval = DoSuperMethodA(cl, o, (Msg)opg);
			break;
	}
	return( retval );
}

ULONG SAVEDS SMBM_DRAW(Class *cl,Object *o,struct impDraw *imp )
{
	ULONG retval;
	struct DrawInfo *dri;
	struct RastPort *RP = imp->imp_RPort;
	struct SMBMData *IM = INST_DATA( cl, o );
	struct Image *im;
	UWORD patterndata[2] = { 0x2222, 0x8888 };
	BOOL b=FALSE;

	im=(struct Image *)o;

	putreg( REG_A6, cl->cl_UserData );
	geta4();

	retval = DoSuperMethodA(cl, o, (Msg) imp);

	dri = imp->imp_DrInfo;

	if(IM->im_Border || IM->im_SelBorder)
	{
		b=TRUE;
		SetAPen(RP,dri->dri_Pens[BACKGROUNDPEN]);
		RectFill(RP,	im->LeftEdge+imp->imp_Offset.X, im->TopEdge+imp->imp_Offset.Y,
									im->LeftEdge+imp->imp_Offset.X+IM->im_Width+1, im->TopEdge+imp->imp_Offset.Y+IM->im_Height+1);
	}
	if(IM->im_Border && (imp->imp_State!=IDS_SELECTED))
	{
		SetAttrs( IM->im_FrameImage,
			IA_Left,    im->LeftEdge+imp->imp_Offset.X,
			IA_Top,     im->TopEdge+imp->imp_Offset.Y,
			IA_Width,   IM->im_Width+2,
			IA_Height,  IM->im_Height+2,
			IA_Recessed, IM->im_Recessed?TRUE:FALSE,
			TAG_END);
		DrawImageState( RP, (struct Image *)IM->im_FrameImage, 0, 0, IDS_NORMAL, dri);
	}
	
	if(IM->im_SelBorder && (imp->imp_State==IDS_SELECTED))
	{
		SetAttrs( IM->im_FrameImage,
			IA_Left,    im->LeftEdge+imp->imp_Offset.X,
			IA_Top,     im->TopEdge+imp->imp_Offset.Y,
			IA_Width,   IM->im_Width+2,
			IA_Height,  IM->im_Height+2,
			IA_Recessed, IM->im_Recessed?FALSE:TRUE,
			TAG_END);
		DrawImageState( RP, (struct Image *)IM->im_FrameImage, 0, 0, IDS_NORMAL, dri);
	}

	ClipBlit(&IM->im_DestRP,0,0,RP,im->LeftEdge+imp->imp_Offset.X+(b?1:0),im->TopEdge+imp->imp_Offset.Y+(b?1:0),IM->im_Width,IM->im_Height,0xc0);

	if(imp->imp_State==IDS_DISABLED)
	{
		SetDrMd(RP,JAM1);
		SetAfPt(RP, patterndata, 1);

		RectFill(RP,  (LONG)imp->imp_Offset.X, (LONG)imp->imp_Offset.Y,
			imp->imp_Offset.X + IM->im_Width - 1 + (b?2:0),
			imp->imp_Offset.Y + IM->im_Height - 1 + (b?2:0));
		SetAfPt(RP, NULL, 0 );
	}

	return( retval );
}

void SAVEDS ConvertChunky(struct SMBMData *IM)
{
	if(IM->im_SourceDeep && IM->im_DestDeep) // copy 24 to 24bit
	{
		ClipBlit(&IM->im_SourceRP,0,0,&IM->im_DestRP,0,0,IM->im_Width,IM->im_Height,0xc0);
		return;
	}
	if(!IM->im_SourceDeep && IM->im_DestDeep) // copy 8 to 24 bit
	{
		ULONG x,y,p;
		UBYTE *array=NULL;
		struct RastPort temprp;

		InitRastPort(&temprp);
		if(temprp.BitMap=AllocBitMap(IM->im_Width,1,8,BMF_CLEAR,IM->im_SourceRP.BitMap))
		{
			if(array=AllocVec(((IM->im_Width+15)>>4)<<4,MEMF_ANY))
			{
				for(y=0;y<IM->im_Height;y++)
				{
					ReadPixelLine8(&IM->im_SourceRP,0,y,IM->im_Width,array,&temprp);
					for(x=0;x<IM->im_Width;x++)
					{
						p=array[x];
						WriteRGBPixel(&IM->im_DestRP,x,y,RGB8(IM->im_Palette[p].R,IM->im_Palette[p].G,IM->im_Palette[p].B));
					}
				}
				FreeVec(array);
			}
			FreeBitMap(temprp.BitMap);
		}
	}
	if(!IM->im_SourceDeep && !IM->im_DestDeep) // copy 8 to 8 bit
	{
		ULONG x,y;
		UBYTE *array=NULL;
		struct RastPort temprp;

		InitRastPort(&temprp);
		if(temprp.BitMap=AllocBitMap(IM->im_Width,1,8,BMF_CLEAR,IM->im_SourceRP.BitMap))
		{
			if(array=AllocVec(((IM->im_Width+15)>>4)<<4,MEMF_ANY))
			{
				if(IM->im_DestCGFX)
				{
					for(y=0;y<IM->im_Height;y++)
					{
						ReadPixelLine8(&IM->im_SourceRP,0,y,IM->im_Width,array,&temprp);
						for(x=0;x<IM->im_Width;x++) array[x]=IM->im_Palette[array[x]].I;
						WritePixelLine8(&IM->im_DestRP,0,y,IM->im_Width,array,&temprp);
					}
				}
				else
				{
					for(y=0;y<IM->im_Height;y++)
					{
						ReadPixelLine8(&IM->im_SourceRP,0,y,IM->im_Width,array,&temprp);
						for(x=0;x<IM->im_Width;x++)
						{
							SetAPen(&IM->im_DestRP,IM->im_Palette[array[x]].I);
							WritePixel(&IM->im_DestRP,x,y);
						}
					}
				}
				FreeVec(array);
			}
			FreeBitMap(temprp.BitMap);
		}
	}
	if(IM->im_SourceDeep && !IM->im_DestDeep) // copy 24 to 8 bit
	{
	}
}
