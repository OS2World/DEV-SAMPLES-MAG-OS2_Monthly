// slider.h -- Definitions for slider demo

//--------------------------------------------------------------
//  Change the following as required for target system
//--------------------------------------------------------------
#define OS220                           // Define target system
//#define OS213                         // OS/2 1.3 + CUA Lib/2

#ifdef OS220
    #define MSGID   ULONG               // OS/2 2.0
#else
    #define MSGID   USHORT              // OS/2 1.3
    #include <fclsldp.h>                // CUA Library/2
    #define WC_SLIDER CCL_SLIDER        // Window class
#endif                               

//  Defines for dialogs, controls

#define IDLG_SETTIME                100 // Set time dialog
#define IDSL_SETTIME_TIME           101

#define IDLG_PROGRESS               200 // Progress bar dialog
#define IDSL_PROGRESS_BAR           201

#define ID_TIMER                      1 // For WinStartTimer

//  Prototypes for useful functions

VOID InitSlider (                       // Initialize slider
HWND    hwndDlg,                        // Handle of dialog
ULONG   idSlider,                       // ID of slider control
USHORT  usSizeMultiplier,               // Size multiplier
USHORT  usMinorTickSpacing,             // Minor tick spacing
USHORT  usMinorTickSize,                // Size of minor ticks
USHORT  usMajorTickSpacing,             // Major tick spacing
USHORT  usMajorTickSize,                // Size of major ticks
USHORT  usDetentSpacing,                // Detent spacing
USHORT  usTextSpacing,                  // Text label spacing
PSZ     pszFont);                       // Font for text or NULL
