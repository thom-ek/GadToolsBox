#include<proto/utility.h>
#include<exec/types.h>
#include<gadtoolsbox/gadtoolsbox.h>

#define SCRN_Dummy (TAG_USER)
#define SCRN_Private (SCRN_Dummy+0)
#define SCRN_LockPubName (SCRN_Dummy+1)
#define SCRN_Handler (SCRN_Dummy+31)

#define SCRN_Screen (SCRN_Dummy+100)
#define SCRN_DrawInfo (SCRN_Dummy+101)
#define SCRN_DefaultTitle (SCRN_Dummy+102)
#define SCRN_VisualInfo (SCRN_Dummy+103)
#define SCRN_SigBit (SCRN_Dummy+106)

#define SCRN_Left                SA_Left
#define SCRN_Top                 SA_Top
#define SCRN_Width               SA_Width
#define SCRN_Height              SA_Height
#define SCRN_Depth               SA_Depth
#define SCRN_DetailPen           SA_DetailPen
#define SCRN_BlockPen            SA_BlockPen
#define SCRN_Title               SA_Title
#define SCRN_Font                SA_Font
#define SCRN_BitMap              SA_BitMap
#define SCRN_ShowTitle           SA_ShowTitle
#define SCRN_Behind              SA_Behind
#define SCRN_Quiet               SA_Quiet
#define SCRN_Type                SA_Type
#define SCRN_DisplayID           SA_DisplayID
#define SCRN_Overscan            SA_Overscan
#define SCRN_DClip               SA_DClip
#define SCRN_AutoScroll          SA_AutoScroll
#define SCRN_PubName             SA_PubName
#define SCRN_Pens                SA_Pens
#define SCRN_Colors              SA_Colors
#define SCRN_FullPalette         SA_FullPalette
#define SCRN_ErrorCode           SA_ErrorCode
#define SCRN_SysFont             SA_SysFont
#define SCRN_Parent              SA_Parent
#define SCRN_FrontChild          SA_FrontChild
#define SCRN_BackChild           SA_BackChild
#define SCRN_BackFill            SA_BackFill
#define SCRN_Draggable           SA_Draggable
#define SCRN_Exclusive           SA_Exclusive
#define SCRN_SharePens           SA_SharePens
#define SCRN_Colors32            SA_Colors32
#define SCRN_Interleaved         SA_Interleaved
#define SCRN_VideoControl        SA_VideoControl
#define SCRN_ColorMapEntries     SA_ColorMapEntries
#define SCRN_LikeWorkbench       SA_LikeWorkbench
#define SCRN_MinimizeISG         SA_MinimizeISG
#define SCRN_PubTask             SA_PubTask
#define SCRN_PubSig              SA_PubSig
#define SCRN_ID                  GA_ID
#define SCRN_UserData            GA_UserData
