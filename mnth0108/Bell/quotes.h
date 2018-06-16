/* QUOTES.H */

#define MAXSTR	    128

#define ID_QUOTES   0x0100
#define IDM_FILE    0x0101
#define IDM_NEW     0x0102
#define IDM_OPEN    0x0103
#define IDM_EXIT    0x0104
#define IDM_OPTIONS 0x0105
#define IDM_TEXT    0x0106
#define IDM_TWAIN   0x0107
#define IDM_IBSEN   0x0108
#define IDM_GATES   0x0109

#define IDS_TWAIN   IDM_TWAIN + 256
#define IDS_IBSEN   IDM_IBSEN + 256
#define IDS_GATES   IDM_GATES + 256

#define IDM_COLOR   0x0300
#define IDM_FOREGROUND	0x0301
#define IDMF_RED    0x0302
#define IDMF_GREEN  0x0303
#define IDMF_BLUE   0x0304
#define IDM_BACKGROUND 0x0305
#define IDMB_RED    0x0306
#define IDMB_GREEN  0x0307
#define IDMB_BLUE   0x0308

#define UM_INITSIZE WM_USER

typedef struct _STRCLR {
    ULONG ulFColor;
    ULONG ulBColor;
    char quote[MAXSTR];
} STRCLR;

typedef STRCLR *PSTRCLR;


#define PSCFROMHWND(hwnd) ((PSTRCLR)WinQueryWindowPtr(hwnd,0))
