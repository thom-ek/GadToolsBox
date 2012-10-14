#ifndef GTB_STRING_H
#define GTB_STRING_H
/*
** String
*/

#define ID_LSTR MAKE_ID('L','S','T','R')  // String Data

struct Node_String
{
	struct LVList ns_Strings;
	struct Page *ns_Pages;
	struct Node_StringData *ns_Selected;
};

struct Node_StringData
{
	struct LVNode ns_Node;
	STRPTR ns_GadString;
	STRPTR ns_Label;
	STRPTR ns_String;
};

ULONG Dispatcher_String(piClass *,piObject *,Msg);
ULONG CUT_String(piClass *,piObject *,Msg);
ULONG COPY_String(piClass *,piObject *,struct piCopy *Msg);
ULONG NEW_String(piClass *,piObject *,struct piSet *);
ULONG DISPOSE_String(piClass *,piObject *,Msg );
ULONG CREATE_String(piClass *,piObject *,Msg );
ULONG DELETE_String(piClass *,piObject *,Msg );
ULONG RENDER_String(piClass *,piObject *,Msg );
ULONG GET_String(piClass *,piObject *,struct piGet *);
ULONG READ_String(piClass *,piObject *,struct piIO *);
ULONG WRITE_String(piClass *,piObject *,struct piIO *);
ULONG INIT_String(piClass *,piObject *,Msg );
ULONG APPLY_String(piClass *,piObject *,Msg );

#endif /* GTB_STRING_H */
