/* MENU.C - Menu control functions

Copyright (c) 1992 Timur Tabi
Copyright (c) 1992 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

This module handles all the menu-related functions.
*/

#define MENU_C
#define INCL_WINDIALOGS
#define INCL_WINMESSAGEMGR
#define INCL_WINMENUS
#include <os2.h>
#include "game.h"
#include "dialog.h"
#include "menu.h"
#include "files.h"

void InitMenu(void) {
  usCurTer=IDM_TER_CLEAR_GROUND; 
  eMode=MODE_TARGET;
}

MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
  if (msg == WM_COMMAND && SHORT1FROMMP(mp1) == DID_OK) {
    WinDismissDlg(hwnd,TRUE);
    return 0;
  }
  return WinDefDlgProc(hwnd,msg,mp1,mp2);
}

void MainCommand(USHORT usCmd) {
  extern HWND hwndFrame,hwndClient,hwndMenu;

  switch (usCmd) {
    case IDM_ABOUT:
      WinDlgBox(HWND_DESKTOP,hwndFrame,AboutDlgProc,NULLHANDLE,IDD_ABOUT,NULL);
      break;
    case IDM_FILE_LOAD:
      LoadMap(hwndFrame);
// Now show the new map.
      WinInvalidateRect(hwndClient,NULL,FALSE);   // Make sure the old map is erased
      WinPostMsg(hwndClient,WM_PAINT,0,0);        // Draws the new map
      break;
    case IDM_FILE_SAVE_AS:
      SaveMap(hwndFrame);
      break;
    case IDM_TER_CLEAR_GROUND:
    case IDM_TER_ROUGH_GROUND:
    case IDM_TER_WATER:
    case IDM_TER_LIGHT_WOODS:
    case IDM_TER_HEAVY_WOODS:
    case IDM_TER_PAVEMENT:
    case IDM_TER_BRIDGE:
    case IDM_TER_LIGHT_BLDG:
    case IDM_TER_MEDIUM_BLDG:
    case IDM_TER_HEAVY_BLDG:
    case IDM_TER_HARD_BLDG:
      WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(usCurTer,TRUE),MPFROM2SHORT(MIA_CHECKED,0));
      usCurTer=usCmd;
      WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(usCurTer,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
      break;
    case IDM_MODE_EDIT:
      WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_MODE_TARGET,TRUE),MPFROM2SHORT(MIA_CHECKED,0));
      WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_MODE_EDIT,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
      eMode=MODE_EDIT;
      break;
    case IDM_MODE_TARGET:
      WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_MODE_EDIT,TRUE),MPFROM2SHORT(MIA_CHECKED,0));
      WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_MODE_TARGET,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
      eMode=MODE_TARGET;
      break;
  }
}

