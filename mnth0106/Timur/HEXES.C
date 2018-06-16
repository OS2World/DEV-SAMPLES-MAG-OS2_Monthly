/* HEXES.C - Hex map routines

Copyright (c) 1992 Timur Tabi                                                     
Copyright (c) 1992 Fasa Corporation                                               
                                                                                  
The following trademarks are the property of Fasa Corporation:                    
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.        
The use of these trademarks should not be construed as a challenge to these marks.
                                                                                  
This module contains all the code pertaining to the hexagonal grid of the 
combat map.  This includes drawing and interpreting mouse input.  Hexes are
identified by a column/row index passed as two integers.  X,Y coordinates
are identified with a POINTL structure.
*/

#define HEXES_C
#define INCL_DOSPROCESS
#define INCL_GPILOGCOLORTABLE
#define INCL_GPIPRIMITIVES
#include <os2.h>
#include <string.h>
#include "game.h"
#include "hexes.h"

POINTL HexCoord(HEXINDEX hi) {
/* This function returns the X,Y coordinate of the lower-left vertex for a given hex
   index.
*/
   POINTL ptl;

// Odd numbered columns are drawn a little to the right of even columns
// HEX_SIDE+HEX_EXT is the X-coordinate of column #1
   if (hi.c & 1)                      
      ptl.x=(LONG) HEX_EXT+(HEX_SIDE+HEX_EXT)+(HEX_SIDE+HEX_DIAM)*(hi.c-1)/2;
   else
      ptl.x=(LONG) HEX_EXT+(HEX_SIDE+HEX_DIAM)*hi.c/2;
   ptl.y=(LONG) hi.r*(HEX_HEIGHT/2);
   return ptl;
}

POINTL HexMidpoint(HEXINDEX hi) {
/* This function is identical to HexCoord(), except that it returns the coordinates of
   the midpoint (centerpoint) of the hexagon.
*/
   POINTL ptl;

   if (hi.c & 1)
      ptl.x=(LONG) (HEX_SIDE/2)+HEX_EXT+(HEX_SIDE+HEX_EXT)+(HEX_SIDE+HEX_DIAM)*(hi.c-1)/2;
   else
      ptl.x=(LONG) (HEX_SIDE/2)+HEX_EXT+(HEX_SIDE+HEX_DIAM)*hi.c/2;
   ptl.y=(LONG) (HEX_HEIGHT/2)+hi.r*(HEX_HEIGHT/2);
   return ptl;
}

void HexDraw(HPS hps, HEXINDEX hi) {
/* This function draws a the hexagon at index 'hi'.  'ptlHex' is a series of
   X,Y positions of the vertices, relative to the lower-left vertex.  The
   actual X,Y coordinate of the lower-left vertex is calculated and stored
   in the last element of 'ptlHex'.  This value is then added to the remaining
   5 elements in the array.
*/
  POINTL ptlHex[]={ {HEX_SIDE,0},
                    {HEX_SIDE+HEX_EXT,HEX_HEIGHT/2},
                    {HEX_SIDE,HEX_HEIGHT},
                    {0,HEX_HEIGHT},
                    {-HEX_EXT,HEX_HEIGHT/2},
                    {0,0}                         };
  int i=0;

  ptlHex[5]=HexCoord(hi);
  GpiMove(hps,&ptlHex[5]);
  for (;i<5;i++) {
    ptlHex[i].x+=ptlHex[5].x;
    ptlHex[i].y+=ptlHex[5].y;
  }
  GpiPolyLine(hps,6L,&ptlHex[0]);
}

void HexFillDraw(HEXINDEX hi) {
/* This function is identical to HexDraw() except that it draws the terrain inside the
   hexagon and always uses hpsHex.
*/
  GpiSetColor(hpsHex,HexTerrainColor(amap[hi.c][hi.r].bTerrain));
  GpiSetPattern(hpsHex,HexTerrainPattern(amap[hi.c][hi.r].bTerrain));
  GpiBeginArea(hpsHex,BA_NOBOUNDARY);
  HexDraw(hpsHex,hi);
  GpiEndArea(hpsHex);
  GpiSetColor(hpsHex,HEX_COLOR);
  HexDraw(hpsHex,hi);
}

BYTE HexTerrainColor(USHORT usTerrain) {
/* This routine returns the color for a given terrain type.  The value returned is 
   a parameter to the GpiSetColor() function.
*/
  switch (usTerrain) {
    case IDM_TER_CLEAR_GROUND:  return CLR_BROWN;
    case IDM_TER_ROUGH_GROUND:  return CLR_DARKGRAY;
    case IDM_TER_WATER:         return CLR_DARKBLUE;
    case IDM_TER_LIGHT_WOODS:   return CLR_GREEN;
    case IDM_TER_HEAVY_WOODS:   return CLR_DARKGREEN;
    case IDM_TER_PAVEMENT:      return CLR_PALEGRAY;
    case IDM_TER_BRIDGE:        return CLR_BROWN;
    case IDM_TER_LIGHT_BLDG:
    case IDM_TER_MEDIUM_BLDG:
    case IDM_TER_HEAVY_BLDG:
    case IDM_TER_HARD_BLDG:     return CLR_DARKPINK;
  }
  return 0;
}

BYTE HexTerrainPattern(USHORT usTerrain) {
/* This routine returns the pattern for a given terrain type.  The value returned is
   a parameter to the GpiSetPattern() function.
*/
  switch (usTerrain) {
    case IDM_TER_CLEAR_GROUND:  return PATSYM_DENSE5;
    case IDM_TER_ROUGH_GROUND:  return PATSYM_DENSE6;
    case IDM_TER_WATER:         return PATSYM_DEFAULT;
    case IDM_TER_HEAVY_WOODS:   return PATSYM_DENSE3;
    case IDM_TER_LIGHT_WOODS:   return PATSYM_DENSE5;
    case IDM_TER_PAVEMENT:      return PATSYM_DEFAULT;
    case IDM_TER_BRIDGE:        return PATSYM_DIAG4;
    case IDM_TER_LIGHT_BLDG:    return PATSYM_DENSE8;
    case IDM_TER_MEDIUM_BLDG:   return PATSYM_DENSE7;
    case IDM_TER_HEAVY_BLDG:    return PATSYM_DENSE6;
    case IDM_TER_HARD_BLDG:     return PATSYM_DENSE5;
  }
  return 0;
}
void HexInitMap(void) {
/* This routine sets the default terrain type, configures the map presentation space
   paramters, and initializes the 'amap' array.  Assumes that hpsHex has already been
   initialized.
*/
  int c,r;

  mapDefault.bTerrain=IDM_TER_CLEAR_GROUND;
  GpiSetBackMix(hpsHex,BM_OVERPAINT);
  for (c=0;c<NUM_COLUMNS;c++)
    for (r=c & 1;r<NUM_ROWS-(c & 1);r+=2)
       amap[c][r]=mapDefault;
}

BOOL HexInPoint(POINTL ptl, HEXINDEX hi) {
/* This function returns TRUE if the point 'ptl' is inside hex 'hi'.
   Currently it only checks the rectangle bounded by the two upper vertices
   and the two lower vertices.  If anyone knows of a quick and easy algorithm
   that checks the entire hexagon, please let me know.
*/
   POINTL ptlHex=ptlHex=HexCoord(hi);

   if (ptl.x < ptlHex.x) return FALSE;
   if (ptl.x > ptlHex.x+HEX_SIDE) return FALSE;
   if (ptl.y < ptlHex.y) return FALSE;
   if (ptl.y > ptlHex.y+HEX_HEIGHT) return FALSE;
   return TRUE;
}

BOOL HexLocate(POINTL ptl, HEXINDEX *phi) {
/* This function attempts to locate the hex to which 'ptl' points.  It scans
   each hex on the map until it finds a match.  A future version will be more
   efficient.
*/
   HEXINDEX hi;

   for (hi.c=0;hi.c<NUM_COLUMNS;hi.c++)
      for (hi.r=hi.c & 1;hi.r<NUM_ROWS-(hi.c & 1);hi.r+=2)
         if (HexInPoint(ptl,hi)) {
            phi->c=hi.c;
            phi->r=hi.r;
            return TRUE;
         }
   return FALSE;
}

extern LONG lNumColors;

VOID APIENTRY HexHighlight(ULONG ulThreadArg) {
/* This function changes the color of the origin hex during targetting.  It 
   is started as a background thread and continues until target.fActive
   becomes FALSE.  If there are more than 16 colors, then a routine which cycles
   through 256 shades of red is chosen.  Otherwise, the hex simply blinks red.
   At termination, the color is set back and the hex is redrawn.

   For the color cycling, 'i' is a byte because the "i++" statement will
   automatically cycle from 0-255.

   At this writing the code for color-cycling has NOT been tested on a
   monitor with 256-colors.  It has been tested on a 16-color monitor and
   looks stupid.
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
