/* HEXES.H

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

This file assumes that it is included after os2.h

The file HEX.PCX is a diagram describing the dimensions of a hexagon.
HEX_SIDE is given a value, the others are all derived from it.

The hex map is indexed as shown:
             __      __      __      __
            /  \    /  \    /  \    /  \
         __/ 15 \__/ 35 \__/ 55 \__/ 75 \__
        /  \    /  \    /  \    /  \    /  \
       / 04 \__/ 24 \__/ 44 \__/ 64 \__/ 84 \
       \    /  \    /  \    /  \    /  \    /
        \__/ 13 \__/ 33 \__/ 53 \__/ 73 \__/
        /  \    /  \    /  \    /  \    /  \
       / 02 \__/ 22 \__/ 42 \__/ 62 \__/ 82 \
       \    /  \    /  \    /  \    /  \    /
        \__/ 11 \__/ 31 \__/ 51 \__/ 71 \__/
        /  \    /  \    /  \    /  \    /  \
       / 00 \__/ 20 \__/ 40 \__/ 60 \__/ 80 \
       \    /  \    /  \    /  \    /  \    /
        \__/    \__/    \__/    \__/    \__/

*/

#define HEX_SIDE 14              // Must be even
#define HEX_HEIGHT 24            // 2 * sin(60) * HEX_SIDE, must be be even
#define HEX_EXT (HEX_SIDE/2)     // HEX_SIDE * cos(60)
#define HEX_DIAM (HEX_SIDE*2)    // The long diameter (width)
#define HEX_COLOR CLR_BLACK      // The color of the outline of a hex

/* The spacing between the hexagons.  These are included only to improve the readability of the
   code.  They _MUST_ both be set to a value of 2.  Anything else not only causes the targetting
   mechanism to fail, but it doesn't make any sense either.
*/
#define XLAG 2
#define YLAG 2

// Names for each of the six directions of a hexagon
#define HEXDIR_SE       0
#define HEXDIR_NE       1
#define HEXDIR_NORTH    2
#define HEXDIR_NW       3
#define HEXDIR_SW       4
#define HEXDIR_SOUTH    5

typedef struct {          // The column and row index of a hex
  int c;
  int r;
} HEXINDEX;

// Global variables
#ifdef HEXES_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN HBITMAP hbmHexMask;                     // Hex bitmap mask.  Erases the hexagon before a bitblt

#undef EXTERN

// Determine whether two hex indices are equal
#define HI_EQUAL(hi1,hi2) ((hi1).c==(hi2).c && (hi1).r==(hi2).r)

POINTL HexCoord(HEXINDEX);
// Returns the X,Y coordinate of the bottom-left corner of a hex

POINTL HexMidpoint(HEXINDEX);
// Returns the X,Y coordinate of the midpoint of a hex

void HexDraw(HPS, HEXINDEX);
// Draws a single hex outline

void HexFillDraw(HPS, HEXINDEX);
// Draws a hex with outline HEX_COLOR and filled with the terrain color

void HexOutline(HPS, HEXINDEX);
// Outlines a single hexagon

BOOL HexInPoint(POINTL, HEXINDEX);
// Returns TRUE if a given X,Y point is within a given hex

BOOL HexLocate(POINTL, HEXINDEX *);
// Finds the hex index of the given X,Y coordinate.  Returns TRUE if it finds one

HEXINDEX HexNextShortest(HEXINDEX, HEXINDEX);
// Returns the next hex in a shortest-distance path from hi1 to hi2

HEXINDEX HexFromSide(HEXINDEX, int iSide);
// returns the hex that is across side iSide of hex hi

void HexPointsFromSide(HEXINDEX, int iSide, PPOINTL, PPOINTL);
// Sets pptl1 & pptl2 to the two endpoints of side iSide of the hexagon
