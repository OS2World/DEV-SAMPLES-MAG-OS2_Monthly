/* TERRAIN.H

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

typedef struct {
  int iMenuID;                  // The menu ID
  HBITMAP hbm;                  // If there's a bitmap, then this is non-zero
  BYTE bColor;                  // The color, if hbm==0
  BYTE bPattern;                // The pattern, if hbm==0
  int iVisibility;              // A measure of the terrains transparency
} TERRAIN;

#define NUM_TERRAINS  11        // We'll make this configurable in the future, somehow

#ifdef TERRAIN_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN TERRAIN ater[NUM_TERRAINS];      // One for each terrain;
EXTERN int iCurTer;                     // The current terrain ID (NOT THE MENU ID!!!!)

#undef EXTERN

void TerrainInit(void);
int TerrainIdFromMenu(int iMenuID);
