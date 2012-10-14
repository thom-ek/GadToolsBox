#ifndef GTB_INOUT_H
#define GTB_IBOUT_H
/*
** InOut
*/

#include<gadtoolsbox/forms.h>		// Import for GadToolsBox v2.x

#define ID_VERS MAKE_ID('V','E','R','S')  // Image Chunk

struct GTB_Version
{
	UWORD vr_Version;
	UWORD vr_Revision;
	ULONG vr_Flags;
	ULONG vr_Reserved[ 4 ];
};

#define TAGTYPE_NONE	0
#define TAGTYPE_NUMBER	1
#define TAGTYPE_LIST	2
#define TAGTYPE_TEXT	3
#define TAGTYPE_DONE	TAGTYPE_NONE

// Size of structure 'sn' from node 'se'
#define	SIZE(sn, se) (sizeof(struct sn)-(ULONG)(&(((struct sn *) 0)->se)))

void SaveGUI(STRPTR);
void LoadGUI(STRPTR);
LONG WriteTagItem(struct IFFHandle *,struct TagItem *,UBYTE);
LONG ReadTagItem(struct IFFHandle *,struct TagItem *);
LONG piReadGadTools(struct IFFHandle *,struct Node *);
LONG piReadBOOPSI(struct IFFHandle *,struct Node *);
LONG piWriteGadTools(struct IFFHandle *,piObject *,UBYTE *,UBYTE *);
LONG piWriteBOOPSI(struct IFFHandle *,piObject *,UBYTE *,UBYTE *);
LONG WriteGUI(struct IFFHandle *);
LONG ReadGUI(struct IFFHandle *);
LONG gtb2_ReadGUI(struct IFFHandle *);
LONG piReadWindow(struct IFFHandle *,piObject **);
LONG gtb2_piReadWindow(struct IFFHandle *,piObject **);
LONG piWriteWindow(struct IFFHandle *);
LONG piReadGadget(struct IFFHandle *,piObject *);
LONG gtb2_piReadGadget(struct IFFHandle *,piObject *);
LONG piReadFrame(struct IFFHandle *,piObject *);
LONG gtb2_piReadFrame(struct IFFHandle *,piObject *);
LONG piReadMenu(struct IFFHandle *);
LONG gtb2_piReadMenu(struct IFFHandle *,piObject *,piObject *,piObject **);
LONG piWriteMenu(struct IFFHandle *);
LONG piReadImage(struct IFFHandle *);
LONG piReadPointer(struct IFFHandle *);
LONG piWriteImage(struct IFFHandle *);
void SavePrefs(STRPTR);
void LoadPrefs(STRPTR);

#endif /* GTB_INOUT_H */
