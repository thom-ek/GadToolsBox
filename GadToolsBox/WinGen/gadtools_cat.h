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
extern const struct FC_Type _MSG_SAVE_GAD;
#define MSG_SAVE_GAD ((APTR) &_MSG_SAVE_GAD)
extern const struct FC_Type _MSG_CANCEL_GAD;
#define MSG_CANCEL_GAD ((APTR) &_MSG_CANCEL_GAD)
extern const struct FC_Type _MSG_USE_GAD;
#define MSG_USE_GAD ((APTR) &_MSG_USE_GAD)
extern const struct FC_Type _MSG_LANGUAGE_GAD;
#define MSG_LANGUAGE_GAD ((APTR) &_MSG_LANGUAGE_GAD)
extern const struct FC_Type _MSG_LANGUAGE_L0_GAD;
#define MSG_LANGUAGE_L0_GAD ((APTR) &_MSG_LANGUAGE_L0_GAD)
extern const struct FC_Type _MSG_LANGUAGE_L1_GAD;
#define MSG_LANGUAGE_L1_GAD ((APTR) &_MSG_LANGUAGE_L1_GAD)
extern const struct FC_Type _MSG_LANGUAGE_L2_GAD;
#define MSG_LANGUAGE_L2_GAD ((APTR) &_MSG_LANGUAGE_L2_GAD)
extern const struct FC_Type _MSG_CONVUNDERSCORES_GAD;
#define MSG_CONVUNDERSCORES_GAD ((APTR) &_MSG_CONVUNDERSCORES_GAD)


#endif /* gadtools_cat_H */
