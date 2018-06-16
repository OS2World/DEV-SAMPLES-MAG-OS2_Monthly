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

#define INCL_DOSPROCESS
#define INCL_GPILOGCOLORTABLE
#include <os2.h>
#include <string.h>
#include "hexes.h"

TARGET target;                   // User-controlled targetting

POINTL HexCoord(HEXINDEX hi) {
   POINTL ptl;

   if (hi.c%2 == 1)
      ptl.x=(LONG) HEX_EXT+(HEX_SIDE+HEX_DIAM)*(hi.c-1)/2;
   else
      ptl.x=(LONG) HEX_EXT+(HEX_SIDE+HEX_EXT)+(HEX_SIDE+HEX_DIAM)*(hi.c-2)/2;
   ptl.y=(LONG) (hi.r-1)*(HEX_HEIGHT/2);
   return ptl;
}

POINTL HexMidpoint(HEXINDEX hi) {
   POINTL ptl;

   if (hi.c%2 == 1)
      ptl.x=(LONG) (HEX_SIDE/2)+HEX_EXT+(HEX_SIDE+HEX_DIAM)*(hi.c-1)/2;
   else
      ptl.x=(LONG) (HEX_SIDE/2)+HEX_EXT+(HEX_SIDE+HEX_EXT)+(HEX_SIDE+HEX_DIAM)*(hi.c-2)/2;
   ptl.y=(LONG) (HEX_HEIGHT/2)+(hi.r-1)*(HEX_HEIGHT/2);
   return ptl;
}

void HexDraw(HPS hps,HEXINDEX hi) {
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
   int i;

   ptlHex[5]=HexCoord(hi);
   GpiMove(hps,&ptlHex[5]);
   for (i=0;i<5;i++) {
      ptlHex[i].x+=ptlHex[5].x;
      ptlHex[i].y+=ptlHex[5].y;
   }
   GpiPolyLine(hps,6L,&ptlHex[0]);
}

BOOL HexInPoint(POINTL ptl, HEXINDEX hi) {
/* This function returns TRUE if the point 'ptl' is inside hex 'hi'.
   Currently it only checks the rectangle bounded by the two two vertices
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

   for (hi.c=1;hi.c<=NUM_COLUMNS;hi.c++)
      for (hi.r=2-(hi.c%2);hi.r<=NUM_ROWS+(hi.c%2);hi.r+=2)
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
   becomes FALSE.  If there are more than 16 colors, then a routine which
   through 256 shades of red is chosen.  Otherwise, the hex simply blinks red.
   At termination, the color is set back to white and the hex is redrawn.

   For the color cycling, 'i' is a byte because the "i++" statement will
   automatically cycle from 0-255.  'i' is used for a delay loop since
   DosSleep() never returns, even when the parameter is 1 millisecond.

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
       GpiSetColor(target.hpsHighlight,HEX_COLOR);
       HexDraw(target.hpsHighlight,target.hiStart);
       DosSleep(500L);
       if (!target.fActive) break;
       GpiSetColor(target.hpsHighlight,CLR_RED);
       HexDraw(target.hpsHighlight,target.hiStart);
       DosSleep(500L);
     }
 

// Redraw the starting hex before exiting
   GpiSetColor(target.hpsHighlight,HEX_COLOR);
   HexDraw(target.hpsHighlight,target.hiStart);
   WinReleasePS(target.hpsHighlight);
}
