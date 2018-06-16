/* HEXES.H: Header file for hexes.c

Copyright (c) 1992 Timur Tabi
Copyright (c) 1992 Fasa Corporation                                               
                                                                                  
The following trademarks are the property of Fasa Corporation:                    
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

This file assumes that it is included after os2.h

The file HEX.PCX is a diagram describing the dimensions of a hexagon.
HEX_SIDE is given a value, the others are all derived from it.

The hex map is indexed as shown:
             __      __      __      __
            /  \    /  \    /  \    /  \  
         __/ 26 \__/ 46 \__/ 66 \__/ 86 \__
        /  \    /  \    /  \    /  \    /  \   
       / 15 \__/ 35 \__/ 55 \__/ 75 \__/ 95 \
       \    /  \    /  \    /  \    /  \    /
        \__/ 24 \__/ 44 \__/ 64 \__/ 84 \__/ 
        /  \    /  \    /  \    /  \    /  \ 
       / 13 \__/ 33 \__/ 53 \__/ 73 \__/ 93 \
       \    /  \    /  \    /  \    /  \    /
        \__/ 22 \__/ 42 \__/ 62 \__/ 82 \__/ 
        /  \    /  \    /  \    /  \    /  \ 
       / 11 \__/ 31 \__/ 51 \__/ 71 \__/ 91 \
       \    /  \    /  \    /  \    /  \    /
        \__/    \__/    \__/    \__/    \__/ 

*/

#define HEX_SIDE 26              // Must be even
#define HEX_HEIGHT 44            // 2 * sin(60) * HEX_SIDE, must be be even
#define HEX_EXT (HEX_SIDE/2)     // HEX_SIDE * cos(60)
#define HEX_DIAM (HEX_SIDE*2)    // The long diameter (width)
#define HEX_COLOR CLR_DARKGRAY

// The next two values must be odd
#define NUM_COLUMNS 15           // The number of columns on the map
#define NUM_ROWS 15              // The number of rows.

// Determine whether two hex indices are equal
#define HI_EQUAL(hi1,hi2) (hi1.c==hi2.c && hi1.r==hi2.r)

typedef struct {          // The column and row index of a hex
  int c;
  int r;
} HEXINDEX;
  
typedef struct _TARGET {  // Data for the targetting mechanism
   BOOL fActive;          // TRUE if we are drawing the targetting line   
   HEXINDEX hiStart;      // The index of the starting hex
   HEXINDEX hiEnd;        // The index of the ending hex
   POINTL ptlStart;       // The X,Y coordinate of the line's origin          
   POINTL ptlEnd;         // The X,Y coordinate of the line's end
   HPS hpsLine;           // The HPS for target-line drawing
   HPS hpsHighlight;      // The HPS for origin hex highlighting
   TID tid;               // Thread ID
} TARGET;

POINTL HexCoord(HEXINDEX);
// Returns the X,Y coordinate of the bottom-left corner of a hex

POINTL HexMidpoint(HEXINDEX);
// Returns the X,Y coordinate of the midpoint of a hex

void HexDraw(HPS, HEXINDEX);
// Draws a single hex

BOOL HexInPoint(POINTL, HEXINDEX);
// Returns true of a given X,Y point is within a given hex

BOOL HexLocate(POINTL, HEXINDEX *);
// Returns the hex index of the given X,Y coordinate

VOID APIENTRY HexHighlight(ULONG);
// Thread which highlights the starting hex
