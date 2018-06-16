#ifndef INCL_STDAPP_I
#define INCL_STDAPP_I

#include <stdlib.h>
#include <string.h>
#include "..\apidll\stdappu.h"
#include "..\apidll\utl.h"


#define SERVICE_DLL_FULLNAME    "API.DLL"
#define SERVICE_DLL_NAME        "API"

#define HELP_MENU_TEXT          "~Help"
#define HELP_INDEX_TEXT         "Help ~index"
#define HELP_GENERAL_TEXT       "~General help"
#define HELP_KEYS_TEXT          "~Keys help"
#define HELP_USING_TEXT         "~Using help"
#define EXIT_MENU_TEXT          "~Exit"


typedef struct tag_PRVSTDAPP * PPRVSTDAPP;
typedef struct tag_PRVSTDAPP
  {
  ULONG   flServices;
  LONG    cxRes;
  LONG    cyRes;
  LONG    cColorPlanes;
  LONG    cColorBitcount;
  PFNWP   pfnUserProc;
  HAB     hab;
  HMQ     hmq;
  HWND    hHelpInstance;
  HWND    hFrame;                         // handle to the  frame window
  HWND    hTitleBar;
  HWND    hMenu;
  HWND    hClient;                        // handle to the main client window
  HDC     hdcClient;
  HPS     hpsClient;
  HDC     hdcBuffer;
  HPS     hpsBuffer;
  HMODULE hmodExe;
  HMODULE hmodDll;
  CHAR    szAppName       [MAXNAMEL+1];
  CHAR    szClassName     [MAXNAMEL+1];
  CHAR    szTitle         [MAXNAMEL+1];
  CHAR    szWindowTitle   [MAXNAMEL+1];
  CHAR    szStdAppService [MAXNAMEL+1];
  CHAR    szLibName       [CCHMAXPATH+1];

#ifndef STDAPP_TEXT_LENGTH
#define STDAPP_TEXT_LENGTH    128
#endif
  CHAR    szText1         [STDAPP_TEXT_LENGTH+1];
  }PRVSTDAPP;


typedef struct _MT
    {
       USHORT cb;
       USHORT version;
       USHORT codepage;
       USHORT iInputsize;
       USHORT cMti;
    }MT;



VOID APIENTRY HlpInitHelp( PPRVSTDAPP pPrvStdApp );
VOID APIENTRY HlpUsingHelp(PPRVSTDAPP pPrvStdApp);
VOID APIENTRY HlpGeneral(PPRVSTDAPP pPrvStdApp);
VOID APIENTRY HlpKeys(PPRVSTDAPP pPrvStdApp);
VOID APIENTRY HlpIndex(PPRVSTDAPP pPrvStdApp);
VOID APIENTRY HlpProdInfo(PPRVSTDAPP pPrvStdApp);
VOID APIENTRY HlpDestroyHelpInstance(PPRVSTDAPP pPrvStdApp);


#endif


