/* HEXES.H

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

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
#define HEX_COLOR CLR_BLACK

// The next two values must be odd
#define NUM_COLUMNS 21           // The number of columns on the map
#define NUM_ROWS 29              // The number of rows.

// The spacing between the hexagons.  A value of 1 (no space) is the minimum
#define XLAG 2
#define YLAG 2


typedef struct {          // The column and row index of a hex
  int c;
  int r;
} HEXINDEX;

typedef struct {
  int iTerrain;        // Terrain ID
  int iHeight;      // The height, where ground level is zero
} MAP;

// Global variables
#ifdef HEXES_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN long lNumColors;                        // The number of colors, for HexHighlight()
EXTERN MAP amap[NUM_COLUMNS][NUM_ROWS];        // Data for each hex on the map
EXTERN HBITMAP hbmHexMask;                     // Hex bitmap mask.  Erases the hexagon before a bitblt

#undef EXTERN

// Determine whether two hex indices are equal
#define HI_EQUAL(hi1,hi2) (hi1.c==hi2.c && hi1.r==hi2.r)

POINTL HexCoord(HEXINDEX);
// Returns the X,Y coordinate of the bottom-left corner of a hex

POINTL HexMidpoint(HEXINDEX);
// Returns the X,Y coordinate of the midpoint of a hex

void HexDraw(HPS, HEXINDEX);
// Draws a single hex outline

void HexFillDraw(HEXINDEX);
// Draws a hex with outline HEX_COLOR and filled with the terrain color

void HexInitMap(HWND);
// Initializes the map array, given the window handle during WM_CREATE

BOOL HexInPoint(POINTL, HEXINDEX);
// Returns true of a given X,Y point is within a given hex

BOOL HexLocate(POINTL, HEXINDEX *);
// Returns the hex index of the given X,Y coordinate

void HexDrawMap(HWND);
// Draws the playing field

void APIENTRY HexHighlight(ULONG);
// Thread which highlights the starting hex

HEXINDEX HexNextShortest(HEXINDEX, HEXINDEX);
// Returns the next hex in a shortest-distance path from hi1 to hi2

int HexNextSide(HEXINDEX hi, int iSide, POINTL ptl1, POINTL ptl2);
// returns the exiting side of hi through which a line from ptl1 to ptl would pass,
//  if it entered at side iSide

HEXINDEX HexFromSide(HEXINDEX, int iSide);
// returns the hex that is across side iSide of hex hi

int HexFirstSide(HEXINDEX, HEXINDEX);
// returns the side of hi1 through which the target line to hi2 passes

void HexPointFromSide(HEXINDEX, int iSide, PPOINTL, PPOINTL);
// Sets pptl1 & pptl2 to the two endpoints of side iSide of the hexagon
