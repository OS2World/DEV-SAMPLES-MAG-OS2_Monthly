/* MENU.C - Menu control functions

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

This module handles all the menu-related functions.
*/

#define INCL_WINDIALOGS
#define INCL_WINMESSAGEMGR
#define INCL_WINMENUS
#include <os2.h>

#define MENU_C

#include "header.h"
#include "window.h"
#include "errors.h"
#include "resource.h"
#include "dialog.h"
#include "menu.h"
#include "files.h"
#include "terrain.h"

MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
  if (msg == WM_COMMAND && SHORT1FROMMP(mp1) == DID_OK) {
    WinDismissDlg(hwnd,TRUE);
    return 0;
  }
  return WinDefDlgProc(hwnd,msg,mp1,mp2);
}

void MainCommand(int iCmd) {
/* Processes all the pull-down menu options.
*/
  int i;
  static USHORT usCurTer=IDM_TER_CLEAR_GROUND;

// First check the Terrain menu
  if ((iCmd>=IDM_TERRAIN) && iCmd<(IDM_TERRAIN+100) ) {
    for (i=0; i<NUM_TERRAINS; i++)
      if (ater[i].iMenuID==iCmd) {
        iCurTer=i;
        WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(usCurTer,TRUE),MPFROM2SHORT(MIA_CHECKED,0));
        usCurTer=(USHORT) iCmd;
        WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(usCurTer,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
        }
    return;
  }

// Check the other menus
  switch (iCmd) {
    case IDM_ABOUT:
      WinDlgBox(HWND_DESKTOP,hwndFrame,AboutDlgProc,NULLHANDLE,IDD_ABOUT,NULL);
      break;
    case IDM_FILE_OPEN:
      TerrainOpenMap();
      break;
    case IDM_FILE_SAVE:
      TerrainSaveMap();
      break;
    case IDM_FILE_SAVE_AS:
      TerrainSaveMapAs();
      break;
    case IDM_FILE_QUIT:
      WinSendMsg(hwndClient,WM_CLOSE,0L,0L);
      break;
    case IDM_MODE_MOVE:
    case IDM_MODE_EDIT:
      WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(iMode,TRUE),MPFROM2SHORT(MIA_CHECKED,0));
      iMode=iCmd;
      WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(iMode,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
      break;
  }
}
