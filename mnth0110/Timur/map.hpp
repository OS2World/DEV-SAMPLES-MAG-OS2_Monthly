/* MAP.HPP

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

// The next two values must be odd
#define NUM_COLUMNS 21           // The number of columns on the map
#define NUM_ROWS 29              // The number of rows.

struct TILE {
  int iTerrain;
  int iHeight;
};

class MAP {
  int iNumCol;
  int iNumRow;
  int iColOff;                // column offset
  int iRowOff;                // row offset
  int fModified;
  char szName[128];
  int OpenMap(void);
public:
  TILE map[NUM_COLUMNS][NUM_ROWS];
  MAP(void);                    // create a new, blank map
  MAP(char *sz);                // open a map from disk
  ~MAP(void);
  void draw(HWND);              // Draws the playing field
  int save(void);               // Saves the current map under the same filename
  void saveas(void);            // Prompts for a new map name, and then saves the map under that name.
};

#ifdef MAP_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN MAP *currentMap;

#undef EXTERN
