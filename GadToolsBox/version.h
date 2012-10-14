#ifndef GTB_VERSION_H
#define GTB_VERSION_H
/*
** Version
*/

#define ID_SVER MAKE_ID('S','V','E','R')  // Version Chunk

struct Node_Version
{
	ULONG nv_Version;
	ULONG nv_Revision;
	TEXT nv_ProgramName[256];
	TEXT nv_Author[256];
};

ULONG Dispatcher_Version(piClass *,piObject *,Msg);
ULONG NEW_Version(piClass *,piObject *,struct piSet *);
ULONG DISPOSE_Version(piClass *,piObject *,Msg );
ULONG OPEN_Version(piClass *,piObject *,Msg );
ULONG CLOSE_Version(piClass *,piObject *,Msg );
ULONG SET_Version(piClass *,piObject *,struct piSet *);
ULONG GET_Version(piClass *,piObject *,struct piGet *);
ULONG READ_Version(piClass *,piObject *,struct piIO *);
ULONG WRITE_Version(piClass *,piObject *,struct piIO *);
ULONG INIT_Version(piClass *,piObject *,Msg );
ULONG APPLY_Version(piClass *,piObject *,Msg );

#endif /* GTB_VERSION_H */
