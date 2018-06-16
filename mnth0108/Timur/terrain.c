/* TERRAIN.C - hex map terrain support

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#define INCL_GPIBITMAPS
#include <os2.h>

#define TERRAIN_C
#include "header.h"
#include "terrain.h"
#include "bitmap.h"
#include "resource.h"

void TerrainInit(void) {
/* Initialize the terrain data.  This routine will eventually read the values from a configuration
   file.  In the meantime, they are hard-coded
*/
  int i;

  ater[0].iMenuID=IDM_TER_CLEAR_GROUND;
  ater[1].iMenuID=IDM_TER_ROUGH_GROUND;
  ater[2].iMenuID=IDM_TER_WATER;
  ater[3].iMenuID=IDM_TER_LIGHT_WOODS;
  ater[4].iMenuID=IDM_TER_HEAVY_WOODS;
  ater[5].iMenuID=IDM_TER_PAVEMENT;
  ater[6].iMenuID=IDM_TER_BRIDGE;
  ater[7].iMenuID=IDM_TER_LIGHT_BLDG;
  ater[8].iMenuID=IDM_TER_MEDIUM_BLDG;
  ater[9].iMenuID=IDM_TER_HEAVY_BLDG;
  ater[10].iMenuID=IDM_TER_HARD_BLDG;

// Zero out all bitmap handles
  for (i=0; i<NUM_TERRAINS; i++)
    ater[i].hbm=0;

// Now load the bitmaps for terrains that have them
  ater[7].hbm=BitmapLoad(IDB_TER_BLDGLIGHT);
  ater[8].hbm=BitmapLoad(IDB_TER_BLDGMEDIUM);
  ater[9].hbm=BitmapLoad(IDB_TER_BLDGHEAVY);
  ater[10].hbm=BitmapLoad(IDB_TER_BLDGHARD);

// The remaining terrains have colors and patterns
  ater[0].bColor=CLR_BROWN;
  ater[1].bColor=CLR_DARKGRAY;
  ater[2].bColor=CLR_DARKBLUE;
  ater[3].bColor=CLR_GREEN;
  ater[4].bColor=CLR_DARKGREEN;
  ater[5].bColor=CLR_PALEGRAY;
  ater[6].bColor=CLR_BROWN;

  ater[0].bPattern=PATSYM_DENSE5;
  ater[1].bPattern=PATSYM_DENSE6;
  ater[2].bPattern=PATSYM_DEFAULT;
  ater[3].bPattern=PATSYM_DENSE3;
  ater[4].bPattern=PATSYM_DENSE5;
  ater[5].bPattern=PATSYM_DEFAULT;
  ater[6].bPattern=PATSYM_DIAG4;

  ater[0].iVisibility=0;
  ater[1].iVisibility=0;
  ater[2].iVisibility=0;
  ater[3].iVisibility=3;
  ater[4].iVisibility=2;
  ater[5].iVisibility=0;
  ater[6].iVisibility=0;
  ater[7].iVisibility=1;
  ater[8].iVisibility=1;
  ater[9].iVisibility=1;
  ater[10].iVisibility=1;
}

int TerrainIdFromMenu(int iMenuID) {
/* Given an IDM_TER_xxx value, this function returns the index into ater[] that corresponds to that
   terrain.  It returns -1 if it can't find a correspondence.
*/
  int i;

  for (i=0; i<NUM_TERRAINS; i++)
    if (ater[i].iMenuID==iMenuID) return i;

  return -1;
}
