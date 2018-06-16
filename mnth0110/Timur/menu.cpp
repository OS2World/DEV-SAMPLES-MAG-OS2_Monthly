/* MENU.CPP - Menu control functions

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

#include "header.hpp"
#include "window.hpp"
#include "resource.h"
#include "dialog.h"
#include "menu.hpp"
#include "files.hpp"
#include "bitmap.hpp"
#include "terrain.hpp"
#include "map.hpp"
#include "hexes.hpp"
#include "mech.hpp"

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
      if (terrain.ater[i].iMenuID==iCmd) {
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
    case IDM_FILE_NEW:
      if (currentMap) delete currentMap;
      currentMap=new MAP;
      currentMech=new MECH;
      break;
    case IDM_FILE_OPEN: {
      char sz[128];
      if (currentMap) delete currentMap;
      if (FileOpen("Open Map",sz)) {
        currentMap=new MAP(sz);
        currentMech=new MECH;
      }
      break;
    }
    case IDM_FILE_SAVE:
      currentMap->save();
      break;
    case IDM_FILE_SAVE_AS:
      currentMap->saveas();
      break;
    case IDM_FILE_CLOSE:
      delete currentMech;
      currentMech=NULL;
      delete currentMap;
      currentMap=NULL; 
      WinInvalidateRect(hwndClient,NULL,FALSE);               // Make sure the old map is erased
      WinPostMsg(hwndClient,WM_PAINT,0,0);                    // Draw the new map
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