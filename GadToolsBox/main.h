#ifndef GTB_PROTOTYPES_H
#define GTB_PROTOTYPES_H
/*
** GadToolsBox v3 - Prototypes
*/

#include"defines.h"

/*
** Prototypes
*/

// main.c

void *MyAllocVec(ULONG,ULONG);
void MyFreeVec(void *);
void main(void);
void InitGUI(void);
Object *NewImageObject(ULONG);
int SysISize(void);
BOOL HandleWorkIDCMP(void);
int EditCloseWindow(void);
void SetWndSignals(void);
struct Node *FindNodeNum(struct List *,ULONG);
ULONG GetNumNode(struct List *,struct Node *);
void MoveNodeUp(struct List *,struct Node *);
void MoveNodeDown(struct List *,struct Node *);
void ShowGrid(piObject *);
UWORD GridX(piObject *,UWORD);
UWORD GridY(piObject *,UWORD);
void FreeProject(void);
struct TagItem *AllocTagItems(ULONG);
void MyFreeTagItems(struct TagItem *);
void CopyTagItems(struct TagItem *,struct TagItem *,ULONG);
void InitClasses(void);
void FreeClasses(void);
UWORD OpenWindow_Props(void);
void CloseWindow_Props(void);
void RefreshPropertiesWindow(STRPTR);
int CreatePopupMenus(void);
void FreePopupMenus(void);
void MenuCheck(struct Menu *,UWORD,BOOL);
int RequestOk(STRPTR,STRPTR,STRPTR, int, ...);
STRPTR GetErrorString(UWORD);
extern BOOL CheckRegistration(void);
void LoadRegistration(void);
void FreeRegistration(void);

// guifunc.c

void SetStatusWindow(int,STRPTR);
int OpenStatusWindow(void);
void CloseStatusWindow(void);
void HandleMouseMove(piObject *,piObject *);
piObject *HandleSelectDown(piObject *,piObject *);
int FN_ActiveWindow_Edit(void);
void AlignObjects(struct MinList *,ULONG);
int HowManySelected(struct MinList *);
void RefreshProperties(void);

// toolbar.c

void ApplySelectionWindow(void);

/*
** Externs
*/

extern struct MsgPort *WndPort;
extern struct GUI GUI;
extern struct DrawInfo *DrawInfo;
extern struct Library *TapeDeckBase,*PageBase,*TextFieldBase,*ProgressBase,*ColorWheelBase,*GradientSliderBase;
extern Class *TextFieldClass,*ProgressClass,*PopUpMenuClass,*GadgetImageClass,*PageClass,*DrawClass;
extern ULONG ActiveGadget;
extern struct FileRequester *OpenReq,*SaveReq,*GenReq;
extern struct ScreenModeRequester *ScreenModeReq;
extern struct FontRequester *FontReq;
extern struct GTBSemaphore GTBSem;
extern struct PopupMenu *PM_Object;
extern struct PopupMenu *PM_ObjGadget;
extern struct PopupMenu *PM_ObjSelGadget;
extern struct PopupMenu *PM_ObjWindow;
extern struct PopupMenu *PM_ObjMenu;
extern struct PopupMenu *PM_ObjImage;

extern struct ClipboardHandle *ClipHandle;
extern struct ClipboardHandle *UndoHandle;

extern struct Node_Window *CurrentWindowNode;
extern piObject *CurrentWindow;
extern piObject *CurrentGadget;
extern piObject *CurrentFrame;
extern piObject *CurrentMenu;
extern piObject *CurrentScreen;
extern piObject *CurrentString;
extern piObject *CurrentVersion;
extern piObject *CurrentImage;
extern piClass *piWindowClass,*piGadgetClass,*piFrameClass,*piMenuClass,*piScreenClass,*piImageClass,*piStringClass,*piVersionClass;
extern Object *gui_LeftImage,*gui_RightImage,*gui_UpImage,*gui_DownImage,*gui_SizeImage;
extern ULONG PropertiesMode,ActiveObject,LastCommand;
extern BOOL PropertiesChanged;
extern struct TextAttr EditFontTA;
extern struct TextFont *EditFontTF;

#define SetWorkingProgress(prg) SetGadgetAttrs(GTBSem.gs_Progress,GTBSem.gs_Window,NULL,PROG_CurVal,prg,TAG_DONE)

extern TEXT _FakeUserName[];
extern TEXT _FakeAddress[];
extern TEXT _FakeSerial[];

#endif /* GTB_PROTOTYPES_H */
