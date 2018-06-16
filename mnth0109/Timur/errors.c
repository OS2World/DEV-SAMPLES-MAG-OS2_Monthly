/* ERRORS.C - Error handler

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#define INCL_WINWINDOWMGR
#define INCL_WINERRORS
#define INCL_WINDIALOGS
#define INCL_GPIERRORS
#define INCL_DOSMISC
#include <os2.h>
#include <stdio.h>

#define ERRORS_C
#include "header.h"
#include "errors.h"
#include "window.h"
#include "errormsg.h"

static char * FindMsg(ERROR ec) {
/* This function takes an error code and returns the error message associated with it.
   It returns a pointer to "UNKNOWN" if it can't find the error code in its databases.
*/
  static char szUnknown[]="UNKNOWN";
  int i=0;

  do
    if (amsg[i].ec == ec) return amsg[i].szMsg;
  while (amsg[++i].ec);

  return szUnknown;
}
ERROR ErrorBox(ERROR ec) {
/* This function displays a message box that corresponds to error code 'ec'
   (ec & 0xFF000000) returns the base error code for the module to which 'ec' belongs
   (ec & 0xFFFF0000) returns the base error code for the function to which 'ec' belongs
*/
  char szMsg[100];

  if (!ec) return 0;

  sprintf(szMsg,"Module: %s\nFunction: %s\n%s",FindMsg(ec & 0xFF000000),FindMsg(ec & 0xFFFF0000),FindMsg(ec));
  WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,szMsg,NULL,0,MB_OK | MB_ICONEXCLAMATION);
  return ec;
}

APIRET ErrorDosError(APIRET rc) {
/* This function displays a message box appropriate to the Dos error code.  If 'rc' equals
   zero, then this function simply exists.  The return code is always the same as the
   parameter.
*/
  ULONG ulClass,ulAction,ulLocus;
  char sz[128];

  if (rc) {
    DosErrClass(rc,&ulClass,&ulAction,&ulLocus);
    sprintf(sz,"Code: %lu\nClass: %lu\nAction: %lu\nLocus: %lu",rc,ulClass,ulAction,ulLocus);
    WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,sz,"DOS Error!",0,MB_OK | MB_ICONEXCLAMATION);
  }
  return rc;
}

void ErrorWinError(void) {
  char sz[64];
  ERROR eid=WinGetLastError(hab);

  if (!eid) return;
  sprintf(sz,"Error ID: %u Severity: %u",ERRORIDERROR(eid),ERRORIDSEV(eid));
  WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,sz,NULL,0,MB_OK | MB_ICONEXCLAMATION);
}

/*
void ErrorWinError(void) {
  PERRINFO perriErrorInfo;
  PSZ       pszOffSet,pszErrMsg;

  perriErrorInfo = WinGetErrorInfo(habMain);

  if (perriErrorInfo != NULL) {
   pszOffSet = ((PSZ) perriErrorInfo) + perriErrorInfo->offaoffszMsg;
   pszErrMsg = ((PSZ) perriErrorInfo) + *((PSHORT)pszOffSet);
   WinMessageBox(HWND_DESKTOP,
                 HWND_DESKTOP,
                 pszErrMsg,
                 "PM Error",
                 0,
                 MB_MOVEABLE | MB_CUACRITICAL | MB_CANCEL);

   WinFreeErrorInfo(perriErrorInfo);
}
*/

