#ifndef INCL_STDAPP_U
#define INCL_STDAPP_U


#ifndef API_SUCCESS
#define API_SUCCESS   1
#endif

#ifndef API_FAILED
#define API_FAILED    0
#endif


#define STDAPP_PTR_INDEX                  4
#define STDAPP_IDR_WIN                 9000      // resource identifier
#define STDAPP_IDR_WIN_BITMAP          9000      // resource identifier

// Services Rendered
#define STDAPP_SERVICE_HELP          0x00000001L
#define STDAPP_SERVICE_HPS           0x00000002L
#define STDAPP_SERVICE_RGB           0x00000004L
#define STDAPP_SERVICE_DEVRES        0x00000008L
#define STDAPP_SERVICE_CONFIRM_EXIT  0x00000010L


// *  Standard Help Menu item ids                                               *

#define STDAPP_HELP_TABLE                          9999
#define STDAPP_SUBTABLE                            9998

#define STDAPP_IDM_HELP                            9997
#define STDAPP_IDM_HELPINDEX                       9996
#define STDAPP_IDM_HELPGENERAL                     9995
#define STDAPP_IDM_HELPKEYS                        9994
#define STDAPP_IDM_HELPUSINGHELP                   9993

// * Standard Help menu panels
#define STDAPP_PANEL_HELP                          9992
#define STDAPP_PANEL_HELPHELPFORHELP               9991
#define STDAPP_PANEL_HELPEXTENDED                  9990
#define STDAPP_PANEL_HELPKEYS                      9989
#define STDAPP_PANEL_HELPINDEX                     9988

// *  Main window help panels
#define STDAPP_PANEL_MAIN                          9987

// Main window exit menu item
#define STDAPP_IDM_EXIT                            9986



//  Stringtable ids
#define STDAPP_IDS_WIN_APPNAME       1
#define STDAPP_IDS_WIN_CLASSNAME     2
#define STDAPP_IDS_WIN_TITLE         3
#define STDAPP_IDS_HELPLIBRARYNAME   4
#define STDAPP_IDS_HELPWINDOWTITLE   5
#define STDAPP_IDS_TITLEBARSEPARATOR 6


typedef struct tag_PUBSTDAPP * PPUBSTDAPP;
typedef struct tag_PUBSTDAPP
  {
  PVOID      pv;
  HPS        ( * APIENTRY pfnQueryHps      )  ( PPUBSTDAPP pPubStdApp );
  HAB        ( * APIENTRY pfnQueryHab      )  ( PPUBSTDAPP pPubStdApp );
  HWND       ( * APIENTRY pfnQueryFrame    )  ( PPUBSTDAPP pPubStdApp );
  HWND       ( * APIENTRY pfnQueryClient   )  ( PPUBSTDAPP pPubStdApp );
  BOOL       ( * APIENTRY pfnMsgLoop       )  ( PPUBSTDAPP pPubStdApp );
  BOOL       ( * APIENTRY pfnQueryDevColor )  ( PPUBSTDAPP pPubStdApp, PULONG pulColorPlanes, PULONG pulColorBitcount );
  BOOL       ( * APIENTRY pfnQueryDevRes   )  ( PPUBSTDAPP pPubStdApp, PULONG pulXRes, PULONG pulYRes );
  PPUBSTDAPP ( * APIENTRY pfnDelete        )  ( PPUBSTDAPP pPubStdApp );
  }PUBSTDAPP;


PPUBSTDAPP  APIENTRY NewStdApp( PVOID pvUserProc, ULONG flExclude,
                                ULONG flServices );

#endif
