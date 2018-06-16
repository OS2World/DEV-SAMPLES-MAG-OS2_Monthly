/* TARGET.H

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

typedef struct {                    // Data for the targetting mechanism
   volatile BOOL fActive;           // TRUE if we are drawing the targetting line
   HPS hpsLine;                     // The HPS for target-line drawing
   HPS hpsPath;
   HEXINDEX hiStart;                // The index of the starting hex
   HEXINDEX hiEnd;                  // The index of the ending hex
   POINTL ptlStart;                 // The X,Y coordinate of the line's origin
   POINTL ptlEnd;                   // The X,Y coordinate of the line's end
   float m;                         // The slope of the targetting line
   float b;                         // The intercept of the targetting line
   int range;                       // The range, in hexagons, from start to end
   int angle;                       // The angle, in degrees, where 0 is to the right
   int iVis;                        // Visibility
   int dx;                          // = ptlEnd.x - ptlStart.x
} TARGET;

extern TARGET target;               // This in initialed in TARGET.c

#ifdef TARGET_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN long lNumColors;                        // The number of colors, for Highlight()
EXTERN HWND hwndInfoBox;

#undef EXTERN

void TgtInit(void);
// Initializes the targetting structure

void TgtStart(HEXINDEX);
// Activates targetting

void TgtEnd(void);
// Terminates targetting

void TgtMove(HEXINDEX);
// Moves the targetting line
