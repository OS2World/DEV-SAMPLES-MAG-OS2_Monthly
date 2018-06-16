/* TARGET0.CPP - base functions for module TARGET

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

#include "header.hpp"
#include "hexes.hpp"
#include "target.hpp"
#include "bitmap.hpp"
#include "terrain.hpp"                    // for GetVisibility()
#include "window.hpp"

int TARGET::GetRange(void) {
/* returns the range between target.hiStart and target.hiEnd
*/
  int dx=hexEnd.c-hexStart.c;
  int dy=hexEnd.r-hexStart.r;             // always even
  int d=abs(dy)-abs(dx);                              // always even if d>0

  if (d <= 0)
    return abs(dx);
  else
    return abs(dx)+d/2;
}

int TARGET::Intersect(POINTL ptl1, POINTL ptl2) {
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
  if (this->dx == 0)
    return BETWEEN(ptlStart.x,ptl1.x,ptl2.x);

  dx=ptl2.x-ptl1.x;
  m=(float) (ptl2.y - ptl1.y) / dx;
  if (m==this->m) return FALSE;                // If the slopes are equal, the lines won't intersect
  b=ptl1.y-m*ptl1.x;

  f=(this->b-b) / (m-this->m);                // Calculate the floating-point first
  x=(int) f;                                    //  To avoid round-off errors

// Now just test the intersection point
  return BETWEEN(x,ptl1.x,ptl2.x) && BETWEEN(x,ptlStart.x,ptlEnd.x);
}

static int Distance(POINTL ptl1, POINTL ptl2) {
/* Calculates the distance between two points
*/
  int dy=ptl2.y-ptl1.y;
  int dx=ptl2.x-ptl1.x;

  return (int) sqrt(dy*dy+dx*dx);
}

#define CLOSENESS 4   // Anything smaller than this, and the line zig-zags too much

int TARGET::isClose(POINTL ptl) {
/* This function returns true of ptl is very close to the targetting line.
   The definition of "close" depends on the value of CLOSENESS.
   It returns FALSE if the targetting line is vertical, since a vertical line will never be
   near a vertex.
*/
  if (dx == 0) return FALSE;                           // vertical targetting line?

  if (ptlStart.y == ptlEnd.y)                   // horizontal targetting line?
    return (BOOL) (abs(ptl.y - ptlStart.y) <= CLOSENESS);

/* It's neither vertical nor horizontal, so it must be at an angle.  Let 'l' represent the line
   segment from the point to the targetting line, such that 'l' is perpendicular to the targetting
   line.  The slope of 'l' is -1/target.m
   If ax+by+c=0 describes the targetting line, we see that a=-target.m, b=1, and c=-target.b
   If p=ptl.x and q=ptl.y, then the distance is: abs(ap+bq+c)/sqrt(a^2+b^2)
*/
  double d=fabs(ptl.y-m*ptl.x-b) / sqrt(m*m+1.0);

  return d <= CLOSENESS;
}

int TARGET::NearestSlope(int iSide1, int iSide2) {
/* Given a choice of two sides (iSide1 and iSide2), this function returns the side whose
   perpendicular radius (the line from the center of the hexagon to the midpoint of the side)
   has a slope that is nearest to the slope of the targetting line.
*/
  static const int iAngles[6]={-30,30,90,150,-150,-90};     // Angle of all the radii
  int d1=abs(angle-iAngles[iSide1]);
  int d2=abs(angle-iAngles[iSide2]);

  return (d1<d2) ? iSide1 : iSide2;
}

int TARGET::ExitSide(HEX hex) {
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
    hex.endpoints(i,&ptl1,&ptl2);                    // Find the endpoints of side 'i'
    if (Intersect(ptl1, ptl2)) {                            // Does the targetting line intersect it?
      d1=Distance(hex.SideMidpoint(i),ptlEnd);             // Yes, so find the distance from it
      if (iSide == -1 || d1<d) {                            // Is this the first match? Or is it closer
        iSide=i;                                            //  than the previous one?
        d=d1;                                               // If so, then update our current values
      }
    }
  }

  if (iSide == -1) return -1;                               // Couldn't find a side? Return error

// Second, check for vertex redirection

  hex.endpoints(iSide,&ptl1,&ptl2);

  iSide1 = isClose(ptl1) ? NearestSlope((iSide+5) % 6,iSide) : -1;
  iSide2 = isClose(ptl2) ? NearestSlope(iSide,(iSide+1) % 6) : -1;

  if (iSide1==iSide2) return iSide;

  if (iSide1==-1) return iSide2;
  if (iSide2==-1) return iSide1;
  return NearestSlope(iSide1,iSide2);
}

int TARGET::FirstSide(void) {
/* This function returns the side to the first hexagon that follows the trajectory
   from hiFrom to hiTo.  If the targetting line passes through a vertex, this function
   returns a -1
*/
  int dx,dy=hexEnd.r - hexStart.r;
  float m;

  if (dy == 0) return -1;

  dx=hexEnd.c - hexStart.c;
  if (dx == 0) return (dy>0) ? 2 : 5;           // Vertical line?

  m=(float) dy/dx;
  if (fabs(m) == 3.0) return -1;

  if (fabs(m)>3.0) return (dy>0) ? 2 : 5;               // Almost a vertical line?

  if (m>0.0) return (dx>0) ? 1 : 4;

  return (dx>0) ? 0 : 3;
}
