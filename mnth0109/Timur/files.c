/* FILES.C - File I/O routines & dialog boxes window procedures

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#define INCL_DOSFILEMGR
#define INCL_WINSTDFILE
#include <os2.h>
#include <string.h>

#include "header.h"
#include "errors.h"
#include "hexes.h"
#include "window.h"

/* From the PM reference:

typedef struct _FILEDLG {
 ULONG      cbSize;                            // Structure size
 ULONG      fl;                                // FDS_* flags
 ULONG      ulUser;                            // Used by the application
 LONG       lReturn;                           // Result code
 LONG       lSRC;                              // System return code
 PSZ        pszTitle;                          // Dialog title string
 PSZ        pszOKButton;                       // OK push button text
 PFNWP      pfnDlgProc;                        // Custom dialog procedure
 PSZ        pszIType;                          // Extended-attribute type filter
 PAPSZ      papszITypeList;                    // Pointer
 PSZ        pszIDrive;                         // The initial drive
 PAPSZ      papszIDriveList;                   // Pointer
 HMODULE    hMod;                              // Module for custom dialog resources
 CHAR       szFullFile[CCHMAXPATH];            // Character array
 PAPSZ      papszFQFilename;                   // Pointer
 ULONG      ulFQFCount;                        // Number of file names
 USHORT     usDlgId;                           // Custom dialog ID
 SHORT      x;                                 // X-axis dialog position
 SHORT      y;                                 // Y-axis dialog position
 SHORT      sEAType;                           // Selected extended-attribute type
} FILEDLG;

*/

static FILEDLG fdg={
  sizeof(FILEDLG),                                         // Structure size
  FDS_CENTER | FDS_PRELOAD_VOLINFO | FDS_OPEN_DIALOG,      // FDS_* flags
  0,                                                       // Used by the application
  0,                                                       // Result code
  0,                                                       // System return code
  NULL,                                                    // Dialog title string
  NULL,                                                    // OK push button text
  NULL,                                                    // Custom dialog procedure
  NULL,                                                    // Extended-attribute type filter
  NULL,                                                    // Pointer
  NULL,                                                    // The initial drive
  NULL,                                                    // Pointer
  NULLHANDLE,                                              // Module for custom dialog resources
  "*",                                                     // Character array
  NULL,                                                    // Pointer
  0,                                                       // Number of file names
  0,                                                       // Custom dialog ID
  0,                                                       // X-axis dialog position
  0,                                                       // Y-axis dialog position
  0                                                        // Selected extended-attribute type
};

ERROR FileOpen(char *szTitle, char *szFileName) {
/* Creates a file dialog box intended to open an existing file.
*/
  fdg.pszTitle=szTitle;
  fdg.pszOKButton="Open";
  strcpy(fdg.szFullFile,"*");

  if (!WinFileDlg(HWND_DESKTOP,hwndClient,&fdg)) return ERR_FILES_DLG;
  if (fdg.lReturn != DID_OK) return ERR_FILES_CANCEL;                            // User cancelled
  strcpy(szFileName,fdg.szFullFile);
  return ERR_NOERROR;
}

ERROR FileSave(char *szTitle, char *szFileName) {
/* Creates a file dialog box intended to save a file under a new name
*/
  fdg.pszTitle=szTitle;
  fdg.pszOKButton="Save";
  strcpy(fdg.szFullFile,"*");

  if (!WinFileDlg(HWND_DESKTOP,hwndClient,&fdg)) return ERR_FILES_DLG;
  if (fdg.lReturn != DID_OK) return ERR_FILES_CANCEL;                            // User cancelled
  strcpy(szFileName,fdg.szFullFile);
  return ERR_NOERROR;
}
