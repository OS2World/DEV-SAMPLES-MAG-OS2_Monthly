/* WINDOW.CPP - controls & variables for the main window

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#define INCL_WINWINDOWMGR
#include <os2.h>
#include <stdio.h>

#define WINDOW_C
#include "window.hpp"

void WindowSetTitle(char *szMapName) {
/* This function sets the title of the window.  If szMapName is NULL, then the title says ".Untitled"
*/
  char sz[128];

  if (!szMapName)
    WinSetWindowText(hwndFrame,"The Ultimate OS/2 Game - .Untitled");
  else {
    sprintf(sz,"The Ultimate OS/2 Game - %s",szMapName);
    WinSetWindowText(hwndFrame,sz);
  }
}
