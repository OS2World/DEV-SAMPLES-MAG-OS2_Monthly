/* HEXES.C - Hex map routines

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

This module contains all the code pertaining to the hexagonal grid of the
combat map.  This includes drawing and interpreting mouse input.  Hexes are
identified by a column/row index passed as two integers.  X,Y coordinates
are identified with a POINTL structure.
*/

#define INCL_DOSPROCESS
#define INCL_GPILOGCOLORTABLE
#define INCL_GPIPRIMITIVES
#define INCL_GPIBITMAPS
#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define HEXES_C

#include "header.h"
#include "resource.h"
#include "target.h"
#include "mech.h"
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

// The PS handle for all hex drawing
static HPS hpsHex;

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

  aptl[5]=HexCoord(hi);                       // Move to the last vertex
  GpiMove(hps,&aptl[5]);
  for (i=0; i<5; i++) {
    aptl[i].x=aptl[5].x+aptlHex[i].x;
    aptl[i].y=aptl[5].y+aptlHex[i].y;
  }
  GpiPolyLine(hps,6L,aptl);                   // Draw all six lines at once
}

void HexFillDraw(HEXINDEX hi) {
/* This function is identical to HexDraw() except that it draws the terrain inside the
   hexagon and always uses hpsHex.
*/
  int iTerrain=amap[hi.c][hi.r].iTerrain;
  POINTL ptl;

  if (ater[iTerrain].hbm) {
    ptl=HexCoord(hi);
    ptl.x-=HEX_EXT;
    BitmapDraw(ater[iTerrain].hbm,hbmHexMask,ptl);
  } else {
    GpiSetColor(hpsHex,ater[iTerrain].bColor);
    GpiSetPattern(hpsHex,ater[iTerrain].bPattern);
    GpiBeginArea(hpsHex,BA_NOBOUNDARY);
    HexDraw(hpsHex,hi);
    GpiEndArea(hpsHex);
  }
}

void HexInitMap(HWND hwnd) {
/* This routine sets the default terrain type, configures the map presentation space
   paramters, and initializes the 'amap' array.  Assumes that hpsHex has already been
   initialized.

   Must be called after TerrainInit().
*/
  int c,r;
  MAP map;      // default MAP type

// Initialize the default map type
  map.iTerrain=TerrainIdFromMenu(IDM_TER_CLEAR_GROUND);
  map.iHeight=0;

// Initialize some variables
  hbmHexMask=BitmapLoad(IDB_HEX_MASK);
  hpsHex=WinGetPS(hwnd);

  GpiSetBackMix(hpsHex,BM_OVERPAINT);
  for (c=0;c<NUM_COLUMNS;c++)
    for (r=c & 1;r<NUM_ROWS-(c & 1);r+=2)
       amap[c][r]=map;
}

void HexDrawMap(HWND hwnd) {
/* Draws the combat map.
   Future enhancement: Draw all hexagons of a given terrain first
*/
  RECTL rcl;
  POINTL ptl={0,0};
  HPS hps;
  HEXINDEX hi;

  target.fActive=FALSE;                            // Cancel any targetting
  hps=WinBeginPaint(hwnd,0UL,NULL);
  WinQueryWindowRect(hwnd,&rcl);
  WinFillRect(hps,&rcl,HEX_COLOR);
  WinEndPaint(hps);

  for (hi.c=0;hi.c<NUM_COLUMNS;hi.c++)
    for (hi.r=hi.c & 1;hi.r<NUM_ROWS-(hi.c & 1);hi.r+=2)
      HexFillDraw(hi);

  MechDraw();
}

// -----------------------------------------------------------------------
//   Hex Locator routines
// -----------------------------------------------------------------------

unsigned int * InitHexLimit(unsigned int uiHeight) {
/* Contributed by: BCL
   This functions initializes the integer array of hexagonal y-deltas.
*/
  unsigned int * HexLimit = (unsigned int *) malloc(uiHeight * sizeof(int));
  unsigned int HalfWay = uiHeight / 2;
  unsigned int ndx;

  for (ndx=0;ndx <= HalfWay; ndx++) {
    HexLimit[ndx] = ((4 * ndx) + 2) / 7;
    HexLimit[uiHeight - 1 - ndx] = HexLimit[ndx];
  }
  return HexLimit;
}

BOOL HexInPoint(POINTL ptl, HEXINDEX hi) {
/* Contributed by: BCL
   This function returns TRUE if the point 'ptl' is inside hex 'hi'.
*/
   static unsigned int * Limits = NULL;
   POINTL ptlHex=HexCoord(hi);
   int dy;

// Initialize the limit array the first time this function is called
   if (Limits == NULL) Limits = InitHexLimit(HEX_HEIGHT);

// Test if hi is a valid hex index
   if (hi.c<0 || hi.r<0) return FALSE;
   if (hi.c&1 && hi.r<1) return FALSE;

   if (ptl.y < ptlHex.y) return FALSE;
   if (ptl.y > ptlHex.y+HEX_HEIGHT) return FALSE;
// The point is definitely not within the hexagon's inner rectangle.
//  Let's try the side triangles.
   dy = ptl.y - ptlHex.y;
   if (ptl.x < ptlHex.x - Limits[dy]) return FALSE;
   if (ptl.x > ptlHex.x+HEX_SIDE + Limits[dy]) return FALSE;
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

VOID APIENTRY HexHighlight(ULONG ulThreadArg) {
/* This function changes the color of the origin hex during targetting.  It
   is started as a background thread and continues until target.fActive
   becomes FALSE.  If there are more than 16 colors, then a routine which cycles
   through 256 shades of red is chosen.  Otherwise, the hex simply blinks red.
   At termination, the color is set back and the hex is redrawn.

   For the color cycling, 'i' is a byte because the "i++" statement will
   automatically cycle from 0-255.  By the way, can someone tell me if it
   even works on a 256-color monitor?  I haven't even seen it yet.

   At this writing the code for color-cycling has NOT been tested on a
   monitor with 256-colors.  It has been tested on a 16-color monitor and
   looks stupid.

   This function belongs in module TARGET.  It will eventually be moved.
*/
  BYTE i;

  if (lNumColors>16) {
    GpiCreateLogColorTable(target.hpsHighlight,0,LCOLF_RGB,0,0,NULL);

    for (i=0; target.fActive; i++) {
      GpiSetColor(target.hpsHighlight,(LONG) i<<16);
      HexDraw(target.hpsHighlight,target.hiStart);
    }
    GpiCreateLogColorTable(target.hpsHighlight,LCOL_RESET,0,0,0,NULL);
  } else
    while (target.fActive) {
      GpiSetColor(target.hpsHighlight,CLR_BLACK);
      HexDraw(target.hpsHighlight,target.hiStart);
      DosSleep(300L);
      if (!target.fActive) break;
        GpiSetColor(target.hpsHighlight,CLR_RED);
      HexDraw(target.hpsHighlight,target.hiStart);
      DosSleep(300L);
    }

// Redraw the starting hex before exiting
  GpiSetColor(target.hpsHighlight,HEX_COLOR);
  HexDraw(target.hpsHighlight,target.hiStart);
}

HEXINDEX HexNextShortest(HEXINDEX hiFrom, HEXINDEX hiTo) {
/* Contributed by: BCL
   This routine, given a starting and finishing hex, will calculate the next
   hex along the shortest (in count of hexes) path between them.
*/
  int i;                                              // The change in column

  if (hiFrom.c < hiTo.c)
    i = 1;                                            // Move from E to W.
  else
    i = -1;                                           // Move from W to E.

  if (hiFrom.c != hiTo.c) {
      hiFrom.c += i;                                  // Advance one col in the appropriate dir.
      if (hiFrom.r < hiTo.r)
        hiFrom.r++;                                   // Target lies N; go N in next column.
      else if (hiFrom.r > hiTo.r)
        hiFrom.r--;                                   // Target lies S; go S in next column.
      else if (hiFrom.r > 0)
        hiFrom.r--;                                   // Target lies E or W; go S in next column
                                                      // since we can (row > 0).
      else
        hiFrom.r++;                                   // Target lies E or W; go N in next column
                                                      // since we can't go S (row = 0).
  } else {

// Now that we have the right column, lets match the rows.

    if (hiFrom.r < hiTo.r)
      hiFrom.r += 2;                                  // Move from S to N.
    if (hiFrom.r > hiTo.r)
      hiFrom.r -= 2;                                  // Move from N to S.
  }

  return hiFrom;
}

// -----------------------------------------------------------------------
//   Targetting-line path routines
// -----------------------------------------------------------------------

/* Algorithms for the targetting-line path contributed by CM

   The problem with this algorithm is that it is _too_ good.  It catches hexagons
   that, although are theoretically under the trajectory, would not normally be
   counted in a real Battletech game.  Try it, and you'll see what I mean.  I'm
   open to suggestions on this one.
*/

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

int HexFirstSide(HEXINDEX hiFrom, HEXINDEX hiTo) {
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

  if (fabs(m)>3) return (dy>0) ? 2 : 5;               // Almost a vertical line?

  if (m>0) return (dx>0) ? 1 : 4;

  return (dx>0) ? 0 : 3;
}

void HexPointFromSide(HEXINDEX hi, int iSide, PPOINTL pptl1, PPOINTL pptl2) {
/* This function returns the x,y coordinates of the two endpoints of side
   iSide of hexagon hi.  Two two points are returned in pptl1 and pptl2
*/
   POINTL ptl=HexCoord(hi);      // All coordinates are offsets from this point

   pptl1->x=ptl.x+aptlHex[iSide].x;             // Calculate the two endpoints of that side
   pptl1->y=ptl.y+aptlHex[iSide].y;
   pptl2->x=ptl.x+aptlHex[(iSide+1) % 6].x;
   pptl2->y=ptl.y+aptlHex[(iSide+1) % 6].y;
}
