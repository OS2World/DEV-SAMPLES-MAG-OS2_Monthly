#include <os2.h>

/* resource types */
#define ID_EDIT             10

#define IDM_FILE           100
#define IDM_NEW            101
#define IDM_OPEN           102
#define IDM_SAVE           103
#define IDM_SAVEAS         104
#define IDM_EXIT           105

#define IDM_EDIT           200
#define IDM_UNDO           201
#define IDM_CUT            202
#define IDM_COPY           203
#define IDM_PASTE          204
#define IDM_DELETE         205
#define IDM_FIND           206
#define IDM_REPLACE        207
#define IDM_GO             208

#define IDM_HELP           300
#define IDM_HELPFORHELP    301
#define IDM_EXTENDEDHELP   302
#define IDM_KEYSHELP       303
#define IDM_HELPINDEX      304
#define IDM_ABOUT          305
#define IDH_KEYSHELP       310

#define IDD_ABOUT          400

#define IDD_OPEN           500
#define DID_PATH           501
#define DID_DIRLIST        502
#define DID_FILELIST       503
#define DID_FILEEDIT       504

#define IDD_SAVEAS         600

#define IDD_FIND           700
#define DID_FINDTEXT       701

#define IDD_REPLACE        800
#define DID_OLDTEXT        801
#define DID_NEWTEXT        802
#define DID_DOREPLACE      803
#define DID_REPLACEALL     804
#define DID_REPLACEMSG     805

#define IDD_GOLINE         900
#define DID_LINENBR        901


/* global variables -- program scope */
extern char szFileName[80];   /* current filename */
extern char szFind[60];   /* target string for search */
extern char szReplace[60];   /* replacement for search/replace */
extern char szLine[20];   /* line number to go to */
