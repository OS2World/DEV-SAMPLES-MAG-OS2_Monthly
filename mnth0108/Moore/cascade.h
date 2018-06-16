
/***********\
* CASCADE.H *
\***********/

/* Window id. */
#define ID_WINDOW   100

/* ID of all menu items and sub-menus. */
#define ID_STRINGS   200
#define ID_DISPLAY   205
#define ID_STRING_1  210
#define ID_STRING_2  215
#define ID_STRING_3  220
#define ID_EXIT      300

/* Class name of this window. */
#define CASCADE_WINDOW_CLASS "CascadeWindow"

/* Strings used to be displayed in the window. */
#define STRING_1  "String 1"
#define STRING_2  "String 2"
#define STRING_3  "String 3"

/* Function prototypes. */
MRESULT EXPENTRY CascadeWindowProc (HWND, ULONG, MPARAM, MPARAM);
INT main (VOID);
