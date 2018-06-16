/* TARGET.C - Targeting routines

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

#include "header.h"
#include "errors.h"
#include "hexes.h"
#include "target.h"
#include "target0.h"
#include "terrain.h"
#include "dialog.h"
#include "window.h"

TARGET target={FALSE,NULLHANDLE,NULLHANDLE};

void TgtInit(void) {
/* Loads, displays, and positions the targetting box.  Then sets the focus back to the main window.
*/
  RECTL rclBox,rclFrame;
  LONG lx,ly;

/*  target.fActive=FALSE;
  target.hpsLine=NULLHANDLE;
  target.hpsPath=NULLHANDLE;
*/
  hwndInfoBox=WinLoadDlg(HWND_DESKTOP,HWND_DESKTOP,NULL,NULLHANDLE,IDD_TARGETTING,NULL);
  WinQueryWindowRect(hwndFrame,&rclFrame);
  WinQueryWindowRect(hwndInfoBox,&rclBox);
  lx=rclFrame.xRight+10;
  ly=rclFrame.yTop-(rclBox.yTop-rclBox.yBottom);
  WinSetWindowPos(hwndInfoBox,HWND_BOTTOM,lx,ly,0,0,SWP_MOVE | SWP_SHOW);
  WinSetFocus(HWND_DESKTOP,hwndFrame);
}

void TgtStart(HEXINDEX hi) {
/* This function activates the targeting mechanism.  It also spanws the thread that
   performs the color cycling of the source hexagon
*/
  TID tid;

  target.fActive=TRUE;
  target.hiStart=hi;
  target.hiEnd=hi;
  target.ptlStart=HexMidpoint(hi);
  target.ptlEnd=target.ptlStart;

  target.hpsLine=WinGetPS(hwndClient);
  target.hpsPath=WinGetPS(hwndClient);
  GpiSetColor(target.hpsLine,CLR_WHITE);
  GpiSetMix(target.hpsLine,FM_XOR);
  GpiSetColor(target.hpsPath,CLR_RED);
  GpiSetMix(target.hpsPath,FM_XOR);

  WinSetCapture(HWND_DESKTOP,hwndClient);
  DosCreateThread(&tid,Highlight,0UL,0UL,4096UL);
}

static void TgtInitPath(void) {
/* This function is used to initialize various field in 'target' that are constant for a given
   targetting line.  This makes sure that we don't waste any time calculating the targeting path.
*/
  int dx=target.hiEnd.c-target.hiStart.c;
  int dy=target.hiEnd.r-target.hiStart.r;             // always even
  int d=abs(dy)-abs(dx);                              // always even if d>0

  target.range = (d <= 0) ? abs(dx) : abs(dx)+d/2;
  target.angle = (int) (0.5+atan2(target.ptlEnd.y-target.ptlStart.y,target.ptlEnd.x-target.ptlStart.x)*180.0/pi);
  target.iVis = 0;

  target.dx=target.ptlEnd.x-target.ptlStart.x;    // x-delta for targeting line

  if (target.dx != 0) {
    target.m=(float) (target.ptlEnd.y - target.ptlStart.y) / target.dx;
    target.b=target.ptlStart.y-target.m*target.ptlStart.x;
  }
}

static void TgtShowPath(void) {
/* This function a series of line segments that connect the midpoints of the
   targeting path.  Since it uses the FM_XOR mix-mode, it erases the line
   every other time it's called.
   Assumes that target.hiStart != target.hiEnd

   iTrueSide is the alternate exit side.  For each iteration, iTrueSide equals the side that
   Future enhancement: support for vertex angles.
*/
  HEXINDEX hi;
  POINTL ptl;
  int iSide;

// Draw the first segment
  hi=target.hiStart;                                  // Start at the beginning
  iSide=FirstSide(hi,target.hiEnd);                   // Which way first?
  if (iSide<0) return;                                // Don't draw a line if it's through a vertex
  ptl=HexMidpoint(hi);
  GpiMove(target.hpsPath,&ptl);

  hi=HexFromSide(hi,iSide);                           // Update to the next hex
  ptl=HexMidpoint(hi);
  GpiLine(target.hpsPath,&ptl);                       // Draw the first segment

// If there are any more segments, draw them too

  while (!HI_EQUAL(hi,target.hiEnd)) {                // while we're not at the end
    iSide=ExitSide(hi);                               // Find the exit side
    if (iSide<0) return;                              // Couldn't find one? Just exit

    hi=HexFromSide(hi,iSide);
    ptl=HexMidpoint(hi);
    target.iVis+=ater[amap[hi.c][hi.r].iTerrain].iVisibility;
    GpiLine(target.hpsPath,&ptl);
    if (HI_EQUAL(hi,target.hiStart))        // Infinite loop?
      return;                               //  Then get out of here!
  }
}

void TgtMove(HEXINDEX hi) {
/* Performs all the necessary updates whenever the targeting line is moved.
   Called every time target.fActive is TRUE, and a WM_MOUSEMOVE message is received.
   First determines if the pointer has moved to a new hexagon.  If not, it
   simply exists.
   Otherwise, it erases the existing targeting line and targeting path, draws
   the new ones, and updates the info box.
*/
  char sz[33];                                          // temp string

// If the target hex hasn't moved, just exit
  if HI_EQUAL(target.hiEnd,hi) return;

// Erase any existing line
  if (!HI_EQUAL(target.hiStart,target.hiEnd)) {
    GpiMove(target.hpsLine,&target.ptlStart);
    GpiLine(target.hpsLine,&target.ptlEnd);
    TgtShowPath();
  }

// Set the new endpoint
  target.ptlEnd=HexMidpoint(target.hiEnd=hi);
  TgtInitPath();

// Draw the new line if it exists
  if (!HI_EQUAL(target.hiStart,target.hiEnd)) {
    GpiMove(target.hpsLine,&target.ptlStart);
    GpiLine(target.hpsLine,&target.ptlEnd);
    TgtShowPath();
  }

  WinSetDlgItemText(hwndInfoBox,IDD_ANGLE,_itoa(target.angle,sz,10));
  WinSetDlgItemText(hwndInfoBox,IDD_RANGE,_itoa(target.range,sz,10));
  WinSetDlgItemText(hwndInfoBox,IDD_VISIBILITY,_itoa(target.iVis,sz,10));
  sprintf(sz,"(%i,%i)",target.hiEnd.c,target.hiEnd.r);
  WinSetDlgItemText(hwndInfoBox,IDD_TARGETPOS,sz);
}

void TgtEnd(void) {
/* Cancels the current targeting session
*/
  target.fActive=FALSE;                           // Automatically terminates HexHighlight
  if (!HI_EQUAL(target.hiStart,target.hiEnd)) {   // Erase the line if it exists
    GpiMove(target.hpsLine,&target.ptlStart);
    GpiLine(target.hpsLine,&target.ptlEnd);
    TgtShowPath();
  }

  WinSetDlgItemText(hwndInfoBox,IDD_TARGETPOS,"");
  WinSetCapture(HWND_DESKTOP,NULLHANDLE);
  WinReleasePS(target.hpsLine);
  WinReleasePS(target.hpsPath);
}
