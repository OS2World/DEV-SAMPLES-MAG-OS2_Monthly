/* HEXES.CPP - Hex map routines

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

#include "header.hpp"
#include "resource.h"
#include "hexes.hpp"
#include "bitmap.hpp"
#include "terrain.hpp"
#include "map.hpp"
#include "window.hpp"

// Array of each vertex in a hexagon, ending with the lower-left corner at relative position (0,0)
static POINTL aptlHex[]={ {HEX_SIDE-1,0},
  {HEX_SIDE-1+HEX_EXT,HEX_HEIGHT/2-1},
  {HEX_SIDE-1,HEX_HEIGHT-1},
  {0,HEX_HEIGHT-1},
  {-HEX_EXT,HEX_HEIGHT/2-1},
  {0,0}                         };

POINTL HEX::coord(void) {
/* This function returns the X,Y coordinate of the lower-left vertex for a given hex
   index.
*/
   POINTL ptl;

   ptl.x=HEX_EXT+c*(HEX_SIDE+HEX_EXT);
   ptl.y=r*HEX_HEIGHT/2;
   return ptl;
}

POINTL HEX::midpoint(void) {
/* This function is identical to HEX::coord(), except that it returns the coordinates of
   the midpoint (centerpoint) of the hexagon.
*/
   POINTL ptl;

   ptl.x=(HEX_SIDE/2) + HEX_EXT+c*(HEX_SIDE+HEX_EXT-1);
   ptl.y=(HEX_HEIGHT/2) + r*HEX_HEIGHT/2;
   return ptl;
}

void HEX::draw(void) {
/* This function is identical to HEX::draw() except that it draws the terrain inside the hexagon.
   Question: shouldn't this method be a part of class MAP?
*/
  int iTerrain=currentMap->map[c][r].iTerrain;

  terrain.ater[iTerrain].pbmp->ptl=coord();
  terrain.ater[iTerrain].pbmp->ptl.x-=HEX_EXT;
  terrain.ater[iTerrain].pbmp->draw();
}

void HEX::quickdraw(void) {
/* This function is identical to HEX::draw() except that it calls BITMAP::quickdraw()
   Question: shouldn't this method be a part of class MAP?
*/
  int iTerrain=currentMap->map[c][r].iTerrain;

  terrain.ater[iTerrain].pbmp->ptl=coord();
  terrain.ater[iTerrain].pbmp->ptl.x-=HEX_EXT;
  terrain.ater[iTerrain].pbmp->quickdraw();
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

int HEX::inside(POINTL ptl) {
/* Contributed by: BCL
   Modified by: TT
   This function returns TRUE if the point 'ptl' is inside hex 'hi'.
*/
  static unsigned int * auiLimits = NULL;
  POINTL ptlHex;                                  // lower-left corner of hi
  int dy;                                         // The y-delta within the hexagon

// Test if hi is a valid hex index
  if (c<0 || r<0) return FALSE;
  if (c&1 && r<1) return FALSE;

  ptlHex=coord();
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

HEX::HEX(POINTL ptl) {
/* Original by: BCL, as function HexLocate()
   Redesigned by: TT
   This constructor initializes the hex object with its location on the map.  'ptl' points to
   a screen X,Y coordinate.  If there is a hexagon at that location, then this hex object is
   inialized there.  Otherwise, fValid is set to false.
   Future enhancement: find a better way to indicate that the hex couldn't be located.
   Future enhancement: check to see if the 'ptl' outside map boundries.
*/
  int GuessC, GuessR;

  fValid=TRUE;

  if (ptl.x < HEX_SIDE+HEX_EXT)
    GuessC = 0;
  else
    GuessC = (ptl.x-HEX_EXT)/(HEX_SIDE+HEX_EXT);

  if (GuessC & 1) {
    GuessR = (ptl.y-(HEX_HEIGHT/2))/HEX_HEIGHT;     // make sure it's truncated first
    GuessR = 1+2*GuessR;
  } else {
    GuessR = ptl.y/HEX_HEIGHT;
    GuessR *= 2;
  }

  c=GuessC;
  r=GuessR;
  if (inside(ptl)) return;

  c=GuessC+1;
  r=GuessR+1;
  if (inside(ptl)) return;

  r=GuessR-1;
  fValid=inside(ptl);
}

void HEX::jump(int iSide) {
/* This function returns the hex index of the hexagon that is bordering on
   side iSide of hexagon hi.
*/
  switch (iSide) {
    case HEXDIR_SE: 
      c++;
      r--;
      return;
    case HEXDIR_NE: 
      c++;
      r++;
      return;
    case HEXDIR_NORTH: 
      r+=2;
      return;
    case HEXDIR_NW: 
      c--;
      r++;
      return;
    case HEXDIR_SW: 
      c--;
      r--;
      return;
    default:
      r-=2;
  }
}

void HEX::endpoints(int iSide, PPOINTL pptl1, PPOINTL pptl2) {
/* This function returns the x,y coordinates of the two endpoints of side
   iSide of hexagon hi.  Two two points are returned in pptl1 and pptl2.
*/
  POINTL ptl=coord();      // All coordinates are offsets from this point

  pptl1->x=ptl.x+aptlHex[iSide].x;             // Calculate the two endpoints of that side
  pptl1->y=ptl.y+aptlHex[iSide].y;
  pptl2->x=ptl.x+aptlHex[(iSide+1) % 6].x;
  pptl2->y=ptl.y+aptlHex[(iSide+1) % 6].y;
}

POINTL HEX::SideMidpoint(int iSide) {
/* This function returns the coordinate of the midpoint of side iSide of hexagon 'hi'.
*/
  static const POINTL ptlMidpoints[6]={ {HEX_SIDE+HEX_EXT/2,HEX_HEIGHT/4},
    {HEX_SIDE+HEX_EXT/2,3*HEX_HEIGHT/4},
    {HEX_SIDE/2,HEX_HEIGHT},
    {-HEX_EXT/2,3*HEX_HEIGHT/4},
    {-HEX_EXT/2,HEX_HEIGHT/4},
    {HEX_SIDE/2,0},
  };
  POINTL ptl=coord();

  ptl.x+=ptlMidpoints[iSide].x;
  ptl.y+=ptlMidpoints[iSide].y;
  return ptl;
}
