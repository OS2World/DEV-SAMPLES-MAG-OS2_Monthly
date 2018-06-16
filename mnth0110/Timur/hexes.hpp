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

#define HEX_SIDE 17
#define HEX_HEIGHT 24                    // 2 * sin(60) * HEX_SIDE, must be be even
#define HEX_EXT 5                        // HEX_SIDE * cos(60)
#define HEX_DIAM (HEX_SIDE+2*HEX_EXT)    // The long diameter (width)

// Names for each of the six directions of a hexagon
#define HEXDIR_SE       0
#define HEXDIR_NE       1
#define HEXDIR_NORTH    2
#define HEXDIR_NW       3
#define HEXDIR_SW       4
#define HEXDIR_SOUTH    5

class HEX {
  public:
    int c,r;
    int fValid;
    inline HEX(int cc, int rr) {
      c=cc;
      r=rr;
      fValid=TRUE;
    }
    HEX(POINTL);                // initialize with screen coordinates.  Sets c=r=0 if not found
    inline int operator==(const HEX &hex) {
      return c==hex.c && r==hex.r;
    }
    inline int operator!=(const HEX &hex) {
      return c!=hex.c || r!=hex.r;
    }
    inline HEX& operator=(const HEX &hex1) {
      c=hex1.c;
      r=hex1.r;
      return *this;
    }
    POINTL coord(void);         // returns the X,Y of the lower-left vertex
    POINTL midpoint(void);      // returns the X,Y of the midpoint
    virtual void draw(void);            // draws a filled hexagon, makes its own HPS
    void quickdraw(void);            // draws a filled hexagon, makes its own HPS
    int inside(POINTL);        // returns TRUE if the point is inside the current hex;
    void jump(int);             // jumps this hexagon to the one on the other side
    void endpoints(int, PPOINTL, PPOINTL);
    POINTL SideMidpoint(int iSide);
};
