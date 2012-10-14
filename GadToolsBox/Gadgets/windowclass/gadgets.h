#include<proto/utility.h>
#include<exec/types.h>
#include<gadtoolsbox/gadtoolsbox.h>

#define BOOPSI_KIND ((ULONG)-1)

#define GT_PlaceText (TAG_USER+2)
#define GT_Handler (GA_Dummy)
#define GT_ScreenObj (GA_DrawInfo)

#ifndef GA_TextAttr
#define	GA_TextAttr		(GA_Dummy+40)
#endif
