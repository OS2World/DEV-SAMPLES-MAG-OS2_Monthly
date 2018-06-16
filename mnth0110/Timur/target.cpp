/* TARGET.CPP - Targeting routines

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#define TARGET_C
#define INCL_DOS
#define INCL_GPIPRIMITIVES
#define INCL_WINWINDOWMGR
#define INCL_WININPUT
#define INCL_GPIBITMAPS
#include <os2.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "header.hpp"
#include "hexes.hpp"
#include "target.hpp"
#include "bitmap.hpp"
#include "terrain.hpp"
#include "map.hpp"
#include "dialog.h"
#include "window.hpp"

TARGET::TARGET(HEX hex) : hexStart(hex), hexEnd(hex) {
/* This function activates the targeting mechanism.  It also spawns the thread that
   performs the color cycling of the source hexagon
*/
  ptlEnd=ptlStart=hex.midpoint();

  hpsLine=WinGetPS(hwndClient);
  hpsPath=WinGetPS(hwndClient);
  GpiSetColor(hpsLine,CLR_WHITE);
  GpiSetMix(hpsLine,FM_XOR);
  GpiSetColor(hpsPath,CLR_RED);
  GpiSetMix(hpsPath,FM_XOR);

  WinSetCapture(HWND_DESKTOP,hwndClient);
//  DosCreateThread(&tid,Highlight,0UL,0UL,4096UL);       no more highlighting, 'cause there's no border
}

void TARGET::InitPath(void) {
/* This function is used to initialize various field in 'target' that are constant for a given
   targetting line.  This makes sure that we don't waste any time calculating the targeting path.
*/
  int dx=hexEnd.c-hexStart.c;
  int dy=hexEnd.r-hexStart.r;             // always even
  int d=abs(dy)-abs(dx);                              // always even if d>0

  range = (d <= 0) ? abs(dx) : abs(dx)+d/2;
  angle = (int) (0.5+atan2(ptlEnd.y-ptlStart.y,ptlEnd.x-ptlStart.x)*180.0/pi);
  iVis = 0;

  this->dx=ptlEnd.x-ptlStart.x;    // x-delta for targeting line

  if (dx != 0) {
    m=(float) (ptlEnd.y - ptlStart.y) / this->dx;
    b=ptlStart.y-m*ptlStart.x;
  }
}

void TARGET::ShowPath(void) {
/* This function draws a series of line segments that connect the midpoints of the
   targeting path.  Since it uses the FM_XOR mix-mode, it erases the line
   every other time it's called.
   Assumes that hexStart != hexEnd

   iTrueSide is the alternate exit side.  For each iteration, iTrueSide equals the side that
   Future enhancement: support for vertex angles.
*/
  HEX hex=hexStart;
  POINTL ptl;
  int iSide;

// Draw the first segment
  iSide=FirstSide();                             // Which way first?
  if (iSide<0) return;                                // Don't draw a line if it's through a vertex
  ptl=hex.midpoint();
  GpiMove(hpsPath,&ptl);

  hex.jump(iSide);                           // Update to the next hex
  ptl=hex.midpoint();
  GpiLine(hpsPath,&ptl);                       // Draw the first segment

// If there are any more segments, draw them too

  while (hex != hexEnd) {                // while we're not at the end
    iSide=ExitSide(hex);                               // Find the exit side
    if (iSide<0) return;                              // Couldn't find one? Just exit

    hex.jump(iSide);
    ptl=hex.midpoint();
    iVis+=terrain.ater[currentMap->map[hex.c][hex.r].iTerrain].iVisibility;
    GpiLine(hpsPath,&ptl);
    if (hex==hexStart)                      // Infinite loop?
      return;                               //  Then get out of here!
  }
}

void TARGET::move(HEX hex) {
/* Performs all the necessary updates whenever the targeting line is moved.
   Called every time target.fActive is TRUE, and a WM_MOUSEMOVE message is received.
   First determines if the pointer has moved to a new hexagon.  If not, it
   simply exists.
   Otherwise, it erases the existing targeting line and targeting path, draws
   the new ones, and updates the info box.
*/
  char sz[33];                                          // temp string

// If the target hex hasn't moved, just exit
  if (hexStart==hex) return;

// Erase any existing line
  if (hexStart != hexEnd) {
    GpiMove(hpsLine,&ptlStart);
    GpiLine(hpsLine,&ptlEnd);
    ShowPath();
  }

// Set the new endpoint
  hexEnd=hex;
  ptlEnd=hexEnd.midpoint();
  InitPath();

// Draw the new line if it exists
  if (hexStart != hexEnd) {
    GpiMove(hpsLine,&ptlStart);
    GpiLine(hpsLine,&ptlEnd);
    ShowPath();
  }

  WinSetDlgItemText(hwndInfoBox,IDD_ANGLE,_itoa(angle,sz,10));
  WinSetDlgItemText(hwndInfoBox,IDD_RANGE,_itoa(range,sz,10));
  WinSetDlgItemText(hwndInfoBox,IDD_VISIBILITY,_itoa(iVis,sz,10));
  sprintf(sz,"(%i,%i)",hexEnd.c,hexEnd.r);
  WinSetDlgItemText(hwndInfoBox,IDD_TARGETPOS,sz);
}

TARGET::~TARGET(void) {
/* Cancels the current targeting session
*/
  if (hexStart != hexEnd) {                           // Erase the line if it exists
    GpiMove(hpsLine,&ptlStart);
    GpiLine(hpsLine,&ptlEnd);
    ShowPath();
  }

  WinSetDlgItemText(hwndInfoBox,IDD_TARGETPOS,"");
  WinSetCapture(HWND_DESKTOP,NULLHANDLE);
  WinReleasePS(hpsLine);
  WinReleasePS(hpsPath);
}
