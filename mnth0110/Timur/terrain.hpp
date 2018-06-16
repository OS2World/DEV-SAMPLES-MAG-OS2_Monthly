/* TERRAIN.HPP

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

needs: bitmap.hpp
*/

#define NUM_TERRAINS  11        // We'll make this configurable in the future, somehow

typedef struct {
  int iMenuID;                  // The menu ID
  BITMAP *pbmp;                 // each terrain has one bitmap
  int iVisibility;              // A measure of the terrains transparency
} TERRAIN;

class TERRAINS {
public:
  int iCurTer;                            // The current terrain ID (NOT THE MENU ID!!!!)
  TERRAIN ater[NUM_TERRAINS];
  TERRAINS(void);
};

#ifdef TERRAIN_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN TERRAINS terrain;

#undef EXTERN

int TerrainIdFromMenu(int iMenuID);
// Given a Menu ID, this function determines the terrain ID.  It returns -1 if it can't find one
