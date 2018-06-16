/* TARGET.H

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#include "hexes.h"

typedef struct {          // Data for the targetting mechanism
   volatile BOOL fActive; // TRUE if we are drawing the targetting line
   HEXINDEX hiStart;      // The index of the starting hex
   HEXINDEX hiEnd;        // The index of the ending hex
   POINTL ptlStart;       // The X,Y coordinate of the line's origin
   POINTL ptlEnd;         // The X,Y coordinate of the line's end
   float m;               // The slope of the targetting line
   float b;               // The intercept of the targetting line
   int dx;                // = ptlEnd.x - ptlStart.x
   HPS hpsLine;           // The HPS for target-line drawing
   HPS hpsHighlight;      // The HPS for origin hex highlighting
   HPS hpsPath;
   TID tid;               // Thread ID
} TARGET;

#ifdef TARGET_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN TARGET target;                          // User-controlled targetting
EXTERN HWND hwndInfoBox;                       // Info box shows angle, range, and visibility

#undef EXTERN

void TgtInitialize(HWND);
// Initializes the targetting structure

void TgtShutdown(void);
// Shuts the targetting mechanism down

void TgtStart(HEXINDEX);
// Activates targetting

void TgtEnd(void);
// Terminates targetting

void TgtMove(HEXINDEX);
// Moves the targetting line
