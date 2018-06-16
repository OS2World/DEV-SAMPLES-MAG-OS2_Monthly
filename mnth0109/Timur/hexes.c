/* HEXES.C - Hex map routines

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

This module contains all the code pertaining to the hexagonal grid of the
combat map.  This includes drawing and interpreting mouse input.  Hexes are
identified by a column/row index passed as two integers.  X,Y coordinates
are identified with a POINTL structure.
*/

#define INCL_DOSPROCESS
#define INCL_GPIPRIMITIVES
#define INCL_GPIBITMAPS
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define HEXES_C

#include "header.h"
#include "errors.h"
#include "resource.h"
#include "hexes.h"
#include "terrain.h"
#include "bitmap.h"
#include "window.h"

// Array of each vertex in a hexagon, ending with the lower-left corner at relative position (0,0)
static POINTL aptlHex[]={ {HEX_SIDE,0},
  {HEX_SIDE+HEX_EXT,HEX_HEIGHT/2},
  {HEX_SIDE,HEX_HEIGHT},
  {0,HEX_HEIGHT},
  {-HEX_EXT,HEX_HEIGHT/2},
  {0,0}                         };

POINTL HexCoord(HEXINDEX hi) {
/* This function returns the X,Y coordinate of the lower-left vertex for a given hex
   index.
*/
   POINTL ptl;

// Odd numbered columns are drawn a little to the right of even columns
// HEX_SIDE+HEX_EXT is the X-coordinate of column #1
   if (hi.c & 1)
      ptl.x=XLAG+HEX_DIAM+(2*XLAG+HEX_SIDE+HEX_DIAM)*(hi.c-1)/2;
   else
      ptl.x=HEX_EXT+(2*XLAG+HEX_SIDE+HEX_DIAM)*hi.c/2;
   ptl.y=hi.r*(YLAG-1+HEX_HEIGHT/2);
   return ptl;
}

POINTL HexMidpoint(HEXINDEX hi) {
/* This function is identical to HexCoord(), except that it returns the coordinates of
   the midpoint (centerpoint) of the hexagon.
*/
   POINTL ptl;

   if (hi.c & 1)
      ptl.x=(HEX_SIDE/2)+XLAG+HEX_DIAM+(2*XLAG+HEX_SIDE+HEX_DIAM)*(hi.c-1)/2;
   else
      ptl.x=(HEX_SIDE/2)+HEX_EXT+(2*XLAG+HEX_SIDE+HEX_DIAM)*hi.c/2;
   ptl.y=(HEX_HEIGHT/2)+hi.r*(YLAG-1+HEX_HEIGHT/2);
   return ptl;
}

void HexDraw(HPS hps, HEXINDEX hi) {
/* This function draws the hexagon at index 'hi'.
   Future enhancement: instead of calculating all six x,y coordinates, just relocate origin.
*/
  int i;
  POINTL aptl[6];

  aptl[5]=HexCoord(hi);                             // Move to the last vertex
  GpiMove(hps,&aptl[5]);
  for (i=0; i<5; i++) {
    aptl[i].x=aptl[5].x+aptlHex[i].x;
    aptl[i].y=aptl[5].y+aptlHex[i].y;
  }
  GpiPolyLine(hps,6L,aptl);           // Draw all six lines at once
}

void HexFillDraw(HPS hps, HEXINDEX hi) {
/* This function is identical to HexDraw() except that it draws the terrain inside the hexagon.
*/
  int iTerrain=amap[hi.c][hi.r].iTerrain;
  POINTL ptl;

  if (ater[iTerrain].hbm) {
    ptl=HexCoord(hi);
    ptl.x-=HEX_EXT;
    BitmapDraw(ater[iTerrain].hbm,hbmHexMask,ptl);

  } else {

    GpiSetBackMix(hps,BM_OVERPAINT);
    GpiSetColor(hps,ater[iTerrain].bColor);
    GpiSetPattern(hps,ater[iTerrain].bPattern);
    GpiBeginArea(hps,BA_NOBOUNDARY);
    HexDraw(hps,hi);
    GpiEndArea(hps);
  }
}

void HexOutline(HPS hps, HEXINDEX hi) {
/* This function draws an outline around the hexagon hi.  It is assume that the calling function
   has already set the desired color with a GpiSetColor(hps,...) call
*/
  static const POINTL aptlOutline[]={
    {HEX_SIDE+1,-1},
    {HEX_SIDE+HEX_EXT+1,HEX_HEIGHT/2},
    {HEX_SIDE+1,HEX_HEIGHT+1},
    {-1,HEX_HEIGHT+1},
    {-HEX_EXT-1,HEX_HEIGHT/2},
    {-1,-1}
  };

  int i;
  POINTL aptl[6];

  aptl[5]=HexCoord(hi);                             // Move to the last vertex
  GpiMove(hps,&aptl[5]);
  for (i=0; i<5; i++) {
    aptl[i].x=aptl[5].x+aptlHex[i].x;
    aptl[i].y=aptl[5].y+aptlHex[i].y;
  }

  GpiPolyLine(hps,6L,aptl);                         // Draw all six lines at once
}

// -----------------------------------------------------------------------
//   Hex Locator routines
// -----------------------------------------------------------------------

static unsigned * HexInitLimits(void) {
/* Contributed by: BCL
   modified by: TT
   This functions initializes the integer array of hexagonal x-deltas.
*/
  static unsigned int auiLimits[HEX_HEIGHT];
  unsigned u;

  for (u=0;u <= HEX_HEIGHT/2; u++) {
    auiLimits[u] = HEX_EXT*u;               // Make sure it does the multiplication first
    auiLimits[u] /= HEX_HEIGHT/2;
    auiLimits[HEX_HEIGHT - u] = auiLimits[u];
  }
  return auiLimits;
}

BOOL HexInPoint(POINTL ptl, HEXINDEX hi) {
/* Contributed by: BCL
   Modified by: TT
   This function returns TRUE if the point 'ptl' is inside hex 'hi'.
*/
  static unsigned int * auiLimits = NULL;
  POINTL ptlHex;                                  // lower-left corner of hi
  int dy;                                         // The y-delta within the hexagon

// Test if hi is a valid hex index
  if (hi.c<0 || hi.r<0) return FALSE;
  if (hi.c&1 && hi.r<1) return FALSE;

  ptlHex=HexCoord(hi);
  if (ptl.y < ptlHex.y) return FALSE;
  if (ptl.y > ptlHex.y+HEX_HEIGHT) return FALSE;

// The point is definitely not within the hexagon's inner rectangle.
//  Let's try the side triangles.

// First, Initialize the limit array if necessary
  if (auiLimits == NULL) auiLimits = HexInitLimits();

  dy = ptl.y - ptlHex.y;
  if (ptl.x < ptlHex.x - auiLimits[dy]) return FALSE;
  if (ptl.x > ptlHex.x+HEX_SIDE + auiLimits[dy]) return FALSE;
  return TRUE;
}

BOOL HexLocate(POINTL ptl, HEXINDEX *phi) {
/* Original by: BCL
   Redesigned by: TT
   This routine identifies the hexagon underneath point ptl.  It returns
   TRUE if it found one, FALSE otherwise.  *phi is modified only if the
   function returns TRUE.
*/
  HEXINDEX hi;
  int GuessC, GuessR;

  if (ptl.x < HEX_SIDE+HEX_EXT)
    GuessC = 0;
  else
    GuessC = (ptl.x-HEX_EXT)/(3*HEX_EXT+XLAG);

  if (GuessC & 1) {
    GuessR = (ptl.y-(HEX_HEIGHT/2)-YLAG)/(HEX_HEIGHT+YLAG);
    GuessR = 1+2*GuessR;                                      // Force the multiplication last
  } else {
    GuessR = ptl.y/(HEX_HEIGHT+YLAG);
    GuessR *= 2;
  }

  hi.c=GuessC;
  hi.r=GuessR;
  if (HexInPoint(ptl,hi)) {
    *phi=hi;
    return TRUE;
  }

  hi.c=GuessC+1;
  hi.r=GuessR+1;
  if (HexInPoint(ptl,hi)) {
    *phi=hi;
    return TRUE;
  }

  hi.r=GuessR-1;
  if (HexInPoint(ptl,hi)) {
    *phi=hi;
    return TRUE;
  }

  return FALSE;
}

HEXINDEX HexFromSide(HEXINDEX hi, int iSide) {
/* This function returns the hex index of the hexagon that is bordering on
   side iSide of hexagon hi.
*/
  switch (iSide) {
    case 0: hi.c++;         // S.E.
            hi.r--;
            break;
    case 1: hi.c++;         // N.E.
            hi.r++;
            break;
    case 2: hi.r+=2;        // North
            break;
    case 3: hi.c--;         // N.W.
            hi.r++;
            break;
    case 4: hi.c--;         // S.W.
            hi.r--;
            break;
    default:hi.r-=2;        // South
  }
  return hi;
}

void HexPointsFromSide(HEXINDEX hi, int iSide, PPOINTL pptl1, PPOINTL pptl2) {
/* This function returns the x,y coordinates of the two endpoints of side
   iSide of hexagon hi.  Two two points are returned in pptl1 and pptl2.
   It actually returns the endpoints of the figure that outlines the hexagon.
*/
  static POINTL aptlOutline[]={
    {HEX_SIDE+1,-1},
    {HEX_SIDE+HEX_EXT+1,HEX_HEIGHT/2},
    {HEX_SIDE+1,HEX_HEIGHT+1},
    {-1,HEX_HEIGHT+1},
    {-HEX_EXT-1,HEX_HEIGHT/2},
    {-1,-1}
  };

  POINTL ptl=HexCoord(hi);      // All coordinates are offsets from this point

  pptl1->x=ptl.x+aptlOutline[iSide].x;             // Calculate the two endpoints of that side
  pptl1->y=ptl.y+aptlOutline[iSide].y;
  pptl2->x=ptl.x+aptlOutline[(iSide+1) % 6].x;
  pptl2->y=ptl.y+aptlOutline[(iSide+1) % 6].y;
}
