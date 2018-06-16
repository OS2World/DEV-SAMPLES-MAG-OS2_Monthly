/* TERRAIN.H

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

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

typedef struct {
  int iTerrain;                 // Terrain ID
  int iHeight;                  // The height, where ground level is zero
} MAP;

#define NUM_TERRAINS  11        // We'll make this configurable in the future, somehow

// The next two values must be odd
#define NUM_COLUMNS 21           // The number of columns on the map
#define NUM_ROWS 29              // The number of rows.


#ifdef TERRAIN_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN MAP amap[NUM_COLUMNS][NUM_ROWS];        // Data for each hex on the map.
EXTERN TERRAIN ater[NUM_TERRAINS];             // One for each terrain;
EXTERN int iCurTer;                            // The current terrain ID (NOT THE MENU ID!!!!)

#undef EXTERN

ERROR TerrainInit(void);
// Initializes all of the terrain- and map-related data.

void TerrainDrawMap(HWND);
// Draws the playing field

int TerrainIdFromMenu(int iMenuID);
// Given a Menu ID, this function determines the terrain ID.  It returns -1 if it can't find one

void TerrainNewMap(void);
// Erases the current map.

void TerrainOpenMap(void);
// Loads a new map

void TerrainSaveMap(void);
// Saves the current map under the same filename

void TerrainSaveMapAs(void);
// Prompts the user for a new map name, and then saves the map under that name.
