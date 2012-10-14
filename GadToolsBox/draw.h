#ifndef GTB_DRAW_H
#define GTB_DRAW_H
/*
** Draw GadgetClass
*/

/*****************************************************************************/


#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif


/*****************************************************************************/

struct ColorPalette
{
	ULONG cp_I,cp_R,cp_G,cp_B;
};

#define DRAW_Dummy		(TAG_USER+0x00001000)
#define DRAW_BitMap (DRAW_Dummy+1)
#define DRAW_Width (DRAW_Dummy+2)
#define DRAW_Height (DRAW_Dummy+3)
#define DRAW_Zoom (DRAW_Dummy+4)
#define DRAW_Palette (DRAW_Dummy+5)
#define DRAW_Border (DRAW_Dummy+6)
#define DRAW_ActualPen (DRAW_Dummy+7)
#define DRAW_SetPoint (DRAW_Dummy+8)
#define DRAW_PointX (DRAW_Dummy+9)
#define DRAW_PointY (DRAW_Dummy+10)
#define DRAW_ReadOnly (DRAW_Dummy+11)
#define DRAW_Grid (DRAW_Dummy+12)
#define DRAW_Left (DRAW_Dummy+13)
#define DRAW_Top (DRAW_Dummy+14)
#define DRAW_VisibleX (DRAW_Dummy+15)
#define DRAW_VisibleY (DRAW_Dummy+16)
#define DRAW_Depth (DRAW_Dummy+17)
#define DRAW_RastPort (DRAW_Dummy+18)
#define DRAW_ShowPoint (DRAW_Dummy+19)

/*****************************************************************************/

Class *CreateDrawClass(void);
void DisposeDrawClass(Class *);

#endif /* GTB_DRAW_H */
