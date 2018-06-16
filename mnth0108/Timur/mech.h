/* MECH.H

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

Must be #included after hexes.h
*/

typedef struct {
  HEXINDEX hi;
  int iDirection;
  HBITMAP hbm[6];         // One bitmap for each direction
} MECH;

#ifdef MECH_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN MECH mech;

#undef EXTERN

void MechInit(void);
// Initialize this module

void MechErase(void);
// Erase the current 'Mech

void MechDraw(void);
// Draws the current 'Mech

void MechMove(HEXINDEX);
// Moves the current 'Mech, if possible
