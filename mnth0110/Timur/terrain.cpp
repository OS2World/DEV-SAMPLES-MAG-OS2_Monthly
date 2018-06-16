/* TERRAIN.CPP

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#define INCL_GPIPRIMITIVES
#define INCL_GPIBITMAPS
#define INCL_WINPOINTERS
#include <os2.h>

#define TERRAIN_C
#include "header.hpp"
#include "resource.h"
#include "bitmap.hpp"
#include "terrain.hpp"
#include "hexes.hpp"
#include "target.hpp"
#include "mech.hpp"
#include "files.hpp"
#include "window.hpp"

int TerrainIdFromMenu(int iMenuID) {
/* Given an IDM_TER_xxx value, this function returns the index into ater[] that corresponds to that
   terrain.  It returns -1 if it can't find a correspondence.
*/
  int i;

  for (i=0; i<NUM_TERRAINS; i++)
    if (terrain.ater[i].iMenuID==iMenuID) return i;

  return -1;
}

TERRAINS::TERRAINS(void) {
/* Initialize the terrain data.  This routine will eventually read the values from a configuration
   file.  In the meantime, they are hard-coded
*/
  int i;
  int c,r;

  terrain.ater[0].iMenuID=IDM_TER_CLEAR_GROUND;
  terrain.ater[1].iMenuID=IDM_TER_ROUGH_GROUND;
  terrain.ater[2].iMenuID=IDM_TER_WATER;
  terrain.ater[3].iMenuID=IDM_TER_LIGHT_WOODS;
  terrain.ater[4].iMenuID=IDM_TER_HEAVY_WOODS;
  terrain.ater[5].iMenuID=IDM_TER_PAVEMENT;
  terrain.ater[6].iMenuID=IDM_TER_BRIDGE;
  terrain.ater[7].iMenuID=IDM_TER_LIGHT_BLDG;
  terrain.ater[8].iMenuID=IDM_TER_MEDIUM_BLDG;
  terrain.ater[9].iMenuID=IDM_TER_HEAVY_BLDG;
  terrain.ater[10].iMenuID=IDM_TER_HARD_BLDG;

  terrain.ater[0].pbmp=new BITMAP(IDB_TER_CLEAR_GROUND,IDB_HEX_MASK);
  terrain.ater[1].pbmp=new BITMAP(IDB_TER_ROUGH_GROUND,IDB_HEX_MASK);
  terrain.ater[2].pbmp=new BITMAP(IDB_TER_WATER,IDB_HEX_MASK);
  terrain.ater[3].pbmp=new BITMAP(IDB_TER_LIGHT_WOODS,IDB_HEX_MASK);
  terrain.ater[4].pbmp=new BITMAP(IDB_TER_HEAVY_WOODS,IDB_HEX_MASK);
  terrain.ater[5].pbmp=new BITMAP(IDB_TER_PAVEMENT,IDB_HEX_MASK);
  terrain.ater[6].pbmp=new BITMAP(IDB_TER_BRIDGE,IDB_HEX_MASK);
  terrain.ater[7].pbmp=new BITMAP(IDB_TER_BLDGLIGHT,IDB_HEX_MASK);
  terrain.ater[8].pbmp=new BITMAP(IDB_TER_BLDGMEDIUM,IDB_HEX_MASK);
  terrain.ater[9].pbmp=new BITMAP(IDB_TER_BLDGHEAVY,IDB_HEX_MASK);
  terrain.ater[10].pbmp=new BITMAP(IDB_TER_BLDGHARD,IDB_HEX_MASK);

  terrain.ater[0].iVisibility=0;
  terrain.ater[1].iVisibility=0;
  terrain.ater[2].iVisibility=0;
  terrain.ater[3].iVisibility=3;
  terrain.ater[4].iVisibility=2;
  terrain.ater[5].iVisibility=0;
  terrain.ater[6].iVisibility=0;
  terrain.ater[7].iVisibility=1;
  terrain.ater[8].iVisibility=1;
  terrain.ater[9].iVisibility=1;
  terrain.ater[10].iVisibility=1;
}
