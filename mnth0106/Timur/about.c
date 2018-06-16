/* ABOUT.C - About Dialog Box routines

Copyright (c) 1992 Timur Tabi
Copyright (c) 1992 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.
*/

#define INCL_WIN
#include <os2.h>
#include <string.h>
#include "dialog.h"

MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
  switch (msg) {
    case WM_COMMAND:
      if (SHORT1FROMMP(mp1)==DID_OK) {
        WinDismissDlg(hwnd,TRUE);
        return 0;
      }
  }
  return WinDefDlgProc(hwnd,msg,mp1,mp2);
}
