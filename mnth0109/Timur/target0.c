/* TARGET0.C - base functions for module TARGET

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/
#define INCL_GPILOGCOLORTABLE
#define INCL_DOSPROCESS
#include <os2.h>
#include <stdlib.h>
#include <math.h>

#include "header.h"
#include "errors.h"
#include "hexes.h"
#include "target.h"
#include "terrain.h"                    // for GetVisibility()
#include "window.h"

void static DoHighlight(HPS hps) {
/* This function does the actual highlighting.  It returns either if an error occurs, or if
   target.fActive becomes false.
   Future enhancement: temporarily disable this thread during target path drawing.
*/
  int i;

  if (lNumColors>16) {
    if (!GpiCreateLogColorTable(hps,0,LCOLF_RGB,0,0,NULL)) return;
    for (i=0; target.fActive; i+=20) {
      if (!GpiSetColor(hps,(LONG) ((i & 255) << 16))) return;
      HexOutline(hps,target.hiStart);
      if (DosSleep(30)) return;                               // Allows the main thread to run better
    }
  } else
    while (target.fActive) {
      if (!GpiSetColor(hps,CLR_RED)) return;
      HexOutline(hps,target.hiStart);
      if (DosSleep(300L)) return;
      if (!target.fActive) return;
      if (!GpiSetColor(hps,HEX_COLOR)) return;
      HexOutline(hps,target.hiStart);
      if (DosSleep(300L)) return;
    }
}

void APIENTRY Highlight(ULONG ul) {
/* This function changes the color of the outline of the origin hex during targetting.  It is
   started as a background thread and continues until target.fActive becomes FALSE.  If there
   are more than 16 colors, then a routine which cycles through 256 shades of red is chosen.
   Otherwise, the outline simply blinks red.  At termination, the outline is erased.

   Can someone tell me if it even works on a 256-color monitor?  I haven't even seen it yet.

   At this writing the code for color-cycling has NOT been tested on a monitor with 256-colors.
   It has been tested on a 16-color monitor and looks stupid.

   Once it creates a presentation space, it calls DoHighlight() to do the actual work.  When
   DoHighlight() exits, the presentation space handle is released.  This ensures that the
   presentation space handle is always released.
*/

// Array of vertices of the outline of a hexagon
  HPS hps=WinGetPS(hwndClient);

  if (hps != NULLHANDLE) {
    DoHighlight(hps);
    GpiSetColor(hps,HEX_COLOR);                 // Erase the outline before exiting
    HexOutline(hps,target.hiStart);
    WinReleasePS(hps);
  }
}

static int GetRange(void) {
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

static BOOL Intersect(POINTL ptl1, POINTL ptl2) {
/* This function retruns TRUE if the line segment from ptl1 to ptl2 intersects
   with the targeting line.  Let s1 be the segment and let s2 be the targeting
   line itself.  If we extend s1 to a line, then that
   line is called l1.  Similarly, l2 is the line of s2.

   The math is quite simple.  Calculate the slopes of l1 and l2.
   Find the x coordinate of the intersection of l1 and l2.  If the x coordinate
   is between ptl1.x and ptl2.x, then the two seconds intersect.

   This function assumes that l1 does not have an infinite slope.

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
  if (m==target.m) return FALSE;                // If the slopes are equal, the lines won't intersect
  b=ptl1.y-m*ptl1.x;

  f=(target.b-b) / (m-target.m);                // Calculate the floating-point first
  x=(int) f;                                    //  To avoid round-off errors

// Now just test the intersection point
  return (BOOL) ( BETWEEN(x,ptl1.x,ptl2.x) && BETWEEN(x,target.ptlStart.x,target.ptlEnd.x) );
}

static POINTL SideMidpoint(HEXINDEX hi, int iSide) {
/* This function returns the coordinate of the midpoint of side iSide of hexagon 'hi'.
*/
  static const POINTL ptlMidpoints[6]={ {HEX_SIDE+HEX_EXT/2,HEX_HEIGHT/4},
    {HEX_SIDE+HEX_EXT/2,3*HEX_HEIGHT/4},
    {HEX_SIDE/2,HEX_HEIGHT},
    {-HEX_EXT/2,3*HEX_HEIGHT/4},
    {-HEX_EXT/2,HEX_HEIGHT/4},
    {HEX_SIDE/2,0},
  };
  POINTL ptl=HexCoord(hi);

  ptl.x+=ptlMidpoints[iSide].x;
  ptl.y+=ptlMidpoints[iSide].y;
  return ptl;
}

static int Distance(POINTL ptl1, POINTL ptl2) {
/* Calculates the distance between two points
*/
  int dy=ptl2.y-ptl1.y;
  int dx=ptl2.x-ptl1.x;

  return (int) sqrt(dy*dy+dx*dx);
}

#define CLOSENESS 4   // Anything smaller than this, and the line zig-zags too much

static BOOL Close(POINTL ptl) {
/* This function returns true of ptl is very close to the targetting line.
   The definition of "close" depends on the value of CLOSENESS.
   It returns FALSE if the targetting line is vertical, since a vertical line will never be
   near a vertex.
*/
  double d;

  if (target.dx == 0) return FALSE;                           // vertical targetting line?

  if (target.ptlStart.y == target.ptlEnd.y)                   // horizontal targetting line?
    return (BOOL) (abs(ptl.y - target.ptlStart.y) <= CLOSENESS);

/* It's neither vertical nor horizontal, so it must be at an angle.  Let 'l' represent the line
   segment from the point to the targetting line, such that 'l' is perpendicular to the targetting
   line.  The slope of 'l' is -1/target.m
   If ax+by+c=0 describes the targetting line, we see that a=-target.m, b=1, and c=-target.b
   If p=ptl.x and q=ptl.y, then the distance is: abs(ap+bq+c)/sqrt(a^2+b^2)
*/
  d=fabs(ptl.y-target.m*ptl.x-target.b) / sqrt(target.m*target.m+1.0);

  return (BOOL) (d <= CLOSENESS);
}

static int NearestSlope(int iSide1, int iSide2) {
/* Given a choice of two sides (iSide1 and iSide2), this function returns the side whose
   perpendicular radius (the line from the center of the hexagon to the midpoint of the side)
   has a slope that is nearest to the slope of the targetting line.
*/
  static const int iAngles[6]={-30,30,90,150,-150,-90};     // Angle of all the radii
  int d1=abs(target.angle-iAngles[iSide1]);
  int d2=abs(target.angle-iAngles[iSide2]);

  return (d1<d2) ? iSide1 : iSide2;
}

int ExitSide(HEXINDEX hi) {
/* This function locates the side of 'hi' through which the targetting line exits.  This side is
   called the 'exit side'.

   This function may select a side adjacent to the true exit side, if it determines that this
   side would produce a better targetting path.
*/
  int iSide=-1;            // The true exit side
  int iSide1,iSide2;       // The two sides adjacent to the exit side.  Used in case of closeness
  int i;
  int d,d1;                // d=distance from exit side to target. d1=temp distance
  POINTL ptl1, ptl2;

// First, find the intersecting side that is closest to the target.  This loop checks each of the
// six sides in order.
  for (i=0; i<6; i++) {
    HexPointsFromSide(hi,i,&ptl1,&ptl2);                    // Find the endpoints of side 'i'
    if (Intersect(ptl1, ptl2)) {                            // Does the targetting line intersect it?
      d1=Distance(SideMidpoint(hi,i),target.ptlEnd);        // Yes, so find the distance from it
      if (iSide == -1 || d1<d) {                            // Is this the first match? Or is it closer
        iSide=i;                                            //  than the previous one?
        d=d1;                                               // If so, then update our current values
      }
    }
  }

  if (iSide == -1) return -1;                               // Couldn't find a side? Return error

// Second, check for vertex redirection

  HexPointsFromSide(hi,iSide,&ptl1,&ptl2);

  iSide1 = Close(ptl1) ? NearestSlope((iSide+5) % 6,iSide) : -1;
  iSide2 = Close(ptl2) ? NearestSlope(iSide,(iSide+1) % 6) : -1;

  if (iSide1==iSide2) return iSide;

  if (iSide1==-1) return iSide2;
  if (iSide2==-1) return iSide1;
  return NearestSlope(iSide1,iSide2);
}

int FirstSide(HEXINDEX hiFrom, HEXINDEX hiTo) {
/* This function returns the side to the first hexagon that follows the trajectory
   from hiFrom to hiTo.  If the targetting line passes through a vertex, this function
   returns a -1
*/
  int dx,dy=hiTo.r - hiFrom.r;
  float m;

  if (dy == 0) return -1;

  dx=hiTo.c - hiFrom.c;
  if (dx == 0) return (dy>0) ? 2 : 5;           // Vertical line?

  m=(float) dy/dx;
  if (fabs(m) == 3.0) return -1;

  if (fabs(m)>3.0) return (dy>0) ? 2 : 5;               // Almost a vertical line?

  if (m>0.0) return (dx>0) ? 1 : 4;

  return (dx>0) ? 0 : 3;
}
