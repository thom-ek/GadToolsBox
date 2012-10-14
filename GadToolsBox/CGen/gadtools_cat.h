#ifndef gadtools_cat_H
#define gadtools_cat_H
/*
** gadtools_cat.h Catalog Header File
*/

#include<exec/types.h>

struct FC_Type
{
	LONG ID;
	STRPTR Str;
};

extern const struct FC_Type _MSG_SCREENTITLE_SCR;
#define MSG_SCREENTITLE_SCR ((APTR) &_MSG_SCREENTITLE_SCR)
extern const struct FC_Type _MSG_Prefs_WND;
#define MSG_Prefs_WND ((APTR) &_MSG_Prefs_WND)
extern const struct FC_Type _MSG_TEMPLATE_GAD;
#define MSG_TEMPLATE_GAD ((APTR) &_MSG_TEMPLATE_GAD)
extern const struct FC_Type _MSG_GUIFUNCS_GAD;
#define MSG_GUIFUNCS_GAD ((APTR) &_MSG_GUIFUNCS_GAD)
extern const struct FC_Type _MSG_LOCALIZE_GAD;
#define MSG_LOCALIZE_GAD ((APTR) &_MSG_LOCALIZE_GAD)
extern const struct FC_Type _MSG_SAVE_GAD;
#define MSG_SAVE_GAD ((APTR) &_MSG_SAVE_GAD)
extern const struct FC_Type _MSG_CANCEL_GAD;
#define MSG_CANCEL_GAD ((APTR) &_MSG_CANCEL_GAD)
extern const struct FC_Type _MSG_USE_GAD;
#define MSG_USE_GAD ((APTR) &_MSG_USE_GAD)
extern const struct FC_Type _MSG_SCREEN_GAD;
#define MSG_SCREEN_GAD ((APTR) &_MSG_SCREEN_GAD)


#endif /* gadtools_cat_H */
