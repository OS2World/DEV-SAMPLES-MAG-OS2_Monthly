/* TARGET.C - Targeting routines

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#define TARGET_C
#define INCL_DOS
#define INCL_GPIPRIMITIVES
#define INCL_WINWINDOWMGR
#define INCL_GPIBITMAPS
#include <os2.h>
#include <math.h>
#include <stdlib.h>
#include "header.h"
#include "target.h"
#include "dialog.h"
#include "terrain.h"

void TgtInitialize(HWND hwnd) {
/* Initializes variables that remain the same for all targeting functions.
   Future enhancement: improve positioning algorithm of info box.
*/
  target.fActive=FALSE;
  target.hpsLine=WinGetPS(hwnd);
  target.hpsHighlight=WinGetPS(hwnd);
  target.hpsPath=WinGetPS(hwnd);
  GpiSetColor(target.hpsLine,CLR_WHITE);
  GpiSetMix(target.hpsLine,FM_XOR);
  GpiSetColor(target.hpsPath,CLR_RED);
  GpiSetMix(target.hpsPath,FM_XOR);
}

void TgtShutdown(void) {
/* This function releases the presentation-space handles when the program
   terminates.
*/
  WinReleasePS(target.hpsLine);
  WinReleasePS(target.hpsHighlight);
  WinReleasePS(target.hpsPath);
}

void TgtStart(HEXINDEX hi) {
/* This function activates the targeting mechanism.  It also spanws the thread that
   performs the color cycling of the source hexagon
*/
  target.fActive=TRUE;
  target.hiStart=hi;
  target.hiEnd=hi;
  target.ptlStart=HexMidpoint(hi);
  target.ptlEnd=target.ptlStart;

  DosCreateThread(&target.tid,HexHighlight,0UL,0UL,4096UL);
}

int GetAngle(void) {
/* Returns the targeting angle
*/
  const double pi=3.1415926535897932384;                // Is this already declared anywhere?

  return (int) (atan2(target.ptlEnd.y-target.ptlStart.y,target.ptlEnd.x-target.ptlStart.x)*180.0/pi);
}

int GetRange(void) {
/* returns the range between target.hiStart and target.hiEnd
*/
  int dx=target.hiEnd.c-target.hiStart.c;
  int dy=target.hiEnd.r-target.hiStart.r;             // always even
  int d=abs(dy)-abs(dx);                              // always even if d>0

  if (d <= 0)
    return abs(dx);
  else
    return abs(dx)+d/2;
}

BOOL Intersect(POINTL ptl1, POINTL ptl2) {
/* This function retruns TRUE if the line segment from ptl1 to ptl2 intersects
   with the targeting line.  Let s1 be the segment and let s2 be the targeting
   line itself.  If we extend s1 to a line, then that
   line is called l1.  Similarly, l2 is the line of s2.

   The math is quite simple.  Calculate the slopes of l1 and l2.
   Find the x coordinate of the intersection of l1 and l2.  If the x coordinate
   is between ptl1.x and ptl2.x, then the two seconds intersect.

   This function assumes that l1 and l2 do not _both_ have infinite slopes.

   Future enhancement: possible speed increase with matrix math
*/
  float m,b;                                    // Slopes & intercepts
  int x;                                        // Intersection x-coordinate
  int dx;                                       // x-delta for l1
  float f;                                      // Temp variable

// Is the targeting line vertical?
  if (target.dx == 0)
    return (BOOL) BETWEEN(target.ptlStart.x,ptl1.x,ptl2.x);

  dx=ptl2.x-ptl1.x;
  m=(float) (ptl2.y - ptl1.y) / dx;
  b=ptl1.y-m*ptl1.x;

  f=(target.b-b) / (m-target.m);                // Calculate the floating-point first
  x=(int) f;                                    //  To avoid round-off errors

// Now just test the intersection point
  return (BOOL) ( BETWEEN(x,ptl1.x,ptl2.x) && BETWEEN(x,target.ptlStart.x,target.ptlEnd.x) );
}

int NextHexSide(HEXINDEX hi, int iSide) {
/* This function is used to find the intersection of hexagon hi with
   the targetting line.  It assumes that the line enters
   the hexagon at side iSide.  It checks the next four sides until
   it finds one that also intersects with the line.  If it can't find one,
   then it assumes that the fifth (last) side must be the one.

   The first side (iSide == 0) is the at the lower-right side of the hexagon.

   Future enhancement: correction factor if targeting line is close to a vertex.
*/
  int i;
  POINTL ptl1, ptl2;

  for (i=0; i<4; i++) {
    iSide=(++iSide) % 6;                       // Get the next side #
    HexPointFromSide(hi,iSide,&ptl1,&ptl2);
    if (Intersect(ptl1, ptl2)) return iSide;
  }
  return (iSide+1) % 6;                       // It has to be the last one
}

void TgtInitPath(void) {
/* This function is used to initialize the m, b, and dx fields of structure 'target'.
   This makes sure that we don't waste any time calculating the targeting path.
*/
  target.dx=target.ptlEnd.x-target.ptlStart.x;    // x-delta for targeting line

  if (target.dx != 0) {
    target.m=(float) (target.ptlEnd.y - target.ptlStart.y) / target.dx;
    target.b=target.ptlStart.y-target.m*target.ptlStart.x;
  }
}

void TgtShowPath(void) {
/* This function a series of line segments that connect the midpoints of the
   targeting path.  Since it uses the FM_XOR mix-mode, it erases the line
   every other time it's called.
   Assumes that target.hiStart != target.hiEnd

   Future enhancement: support for vertex angles.
*/
  HEXINDEX hi;
  POINTL ptl;
  int iSide;

// Draw the first segment
  hi=target.hiStart;                                  // Start at the beginning
  iSide=HexFirstSide(target.hiStart,target.hiEnd);    // Which way first?
  if (iSide<0) return;                                // Don't draw a line if it's through a vertex
  hi=target.hiStart;                                  // Start at the beginning
  ptl=HexMidpoint(hi);
  GpiMove(target.hpsPath,&ptl);

  hi=HexFromSide(hi,iSide);                           // Update to the next hex
  ptl=HexMidpoint(hi);
  GpiLine(target.hpsPath,&ptl);                       // Draw the first segment

// If there are any more segments, draw them too
// Note: If hex1 connects to hex2 at side 'x', then hex2 connects to hex1 at side '3+x', modulo 6

  while (!HI_EQUAL(hi,target.hiEnd)) {
    iSide=NextHexSide(hi,(3+iSide) % 6);
    hi=HexFromSide(hi,iSide);
    ptl=HexMidpoint(hi);
    GpiLine(target.hpsPath,&ptl);
    if (HI_EQUAL(hi,target.hiStart))        // Infinite loop?
      return;                               //  Then get out of here!
  }
}

int GetVisibility(void) {
/* Returns the visibility of the target hex, as seen from the source hex.
   The current implementation is a far cry from the true calculations.

   Future enhancement: support for vertex angles.
*/
  HEXINDEX hi;
  int iVis,iSide;

  if (HI_EQUAL(target.hiStart,target.hiEnd)) return 0;  // Visibility in the same hex is always zero

  iSide=HexFirstSide(target.hiStart,target.hiEnd);
  if (iSide<0) return -1;                               // We'll deal with this one later
  hi=HexFromSide(target.hiStart,iSide);
  hi=target.hiStart;
  iVis=0;

  while (!HI_EQUAL(hi,target.hiEnd)) {
    iSide=NextHexSide(hi,(3+iSide) % 6);
    hi=HexFromSide(hi,iSide);
    iVis+=ater[amap[hi.c][hi.r].iTerrain].iVisibility;
    if (HI_EQUAL(hi,target.hiStart)) break;          // Inifinite loop? Get out of here!
  }
  return iVis;
}

void TgtMove(HEXINDEX hi) {
/* Performs all the necessary updates whenever the targeting line is moved.
   Called every time target.fActive is TRUE, and a WM_MOUSEMOVE message is received.
   First determines if the pointer has moved to a new hexagon.  If not, it
   simply exists.
   Otherwise, it erases the existing targeting line and targeting path, draws
   the new ones, and updates the info box.

   Future enhancement: capturing the mouse.
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

  WinSetDlgItemText(hwndInfoBox,IDD_ANGLE,_itoa(GetAngle(),sz,10));
  WinSetDlgItemText(hwndInfoBox,IDD_RANGE,_itoa(GetRange(),sz,10));
  WinSetDlgItemText(hwndInfoBox,IDD_VISIBILITY,_itoa(GetVisibility(),sz,10));
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
}
