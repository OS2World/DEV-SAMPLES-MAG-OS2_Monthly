/* MAP.CPP

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
#include <string.h>

#define MAP_C

#include "header.hpp"
#include "resource.h"
#include "bitmap.hpp"
#include "terrain.hpp"
#include "map.hpp"
#include "hexes.hpp"
#include "target.hpp"
#include "mech.hpp"
#include "files.hpp"
#include "window.hpp"

MAP::MAP(void) {
/* Creates a new, blank map
*/
  TILE tile;
  int c,r;

  tile.iTerrain=TerrainIdFromMenu(IDM_TER_CLEAR_GROUND);
  tile.iHeight=0;

  for (c=0; c<NUM_COLUMNS; c++)
    for (r=c & 1; r<NUM_ROWS-(c & 1); r+=2)
       map[c][r]=tile;

  WindowSetTitle(szName);                                 // Change the window title to reflect it
  WinInvalidateRect(hwndClient,NULL,FALSE);               // Make sure the old map is erased
  WinPostMsg(hwndClient,WM_PAINT,0,0);                    // Draw the new map

  fModified=FALSE;
}

MAP::MAP(char *sz) {
/* Loads the map specified by the filename 'sz'
*/
  strcpy(szName,sz);
  if (!OpenMap()) return;                                  // Open and read the map, exit if error

  WindowSetTitle(szName);                                 // Change the window title to reflect it
  WinInvalidateRect(hwndClient,NULL,FALSE);               // Make sure the old map is erased
  WinPostMsg(hwndClient,WM_PAINT,0,0);                    // Draw the new map

  fModified=FALSE;
}

MAP::~MAP(void) {
/* if the map has been modified, prompt for saving here
*/
  if (currentTarget) {
    delete currentTarget;
    currentTarget=NULL;
  }
}

void MAP::draw(HWND hwnd) {
/* Draws the combat map.
   Future enhancement: Draw all hexagons of a given terrain first
*/
  WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE));
  for (int c=0;c<NUM_COLUMNS;c++)
    for (int r=c & 1;r<NUM_ROWS-(c & 1);r+=2) {
      HEX hex(c,r);                           // not very efficient, I know.  I'll fix it later
      hex.quickdraw();
    }
  WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,FALSE));

  currentMech->draw();
}

// -------- Loading, saving, and erasing maps ----------------------------------------------------

#define LOAD_ACTION (OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW )
#define SAVE_ACTION (OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS)

#define READ_ATTRS OPEN_FLAGS_NO_CACHE|              /* No need to take up precious cache space */ \
                   OPEN_FLAGS_SEQUENTIAL|            /* One-time read, remember?                */ \
                   OPEN_SHARE_DENYWRITE|             /* We don't want anyone changing it        */ \
                   OPEN_ACCESS_READONLY              // To prevent accidentally writing to it

#define WRITE_ATTRS OPEN_FLAGS_NO_CACHE|              /* No need to take up precious cache space */ \
                    OPEN_FLAGS_SEQUENTIAL|            /* One-time write, remember?               */ \
                    OPEN_SHARE_DENYREADWRITE|         /* We don't want anyone touching it        */ \
                    OPEN_ACCESS_WRITEONLY             // That's how we're gonna do it

int MAP::OpenMap(void) {
/* This function loads the map specified by the filename szName.
*/
  HFILE hfile;
  ULONG ulAction,ulBytesRead;
  FILESTATUS3 fs3;

  if (DosQueryPathInfo(szName,1,&fs3,sizeof(fs3))) return 1;
  if (DosOpen(szName,&hfile,&ulAction,0,FILE_NORMAL,LOAD_ACTION,READ_ATTRS,NULL)) return 1;
  if (DosRead(hfile,map,sizeof(map),&ulBytesRead)) return 1;
  return DosClose(hfile) == 0;
}

int MAP::save(void) {
/* This routine saves the current map under the current filename
*/
  HFILE hfile;
  ULONG ulAction,ulBytesWritten;

  if (DosOpen(szName,&hfile,&ulAction,sizeof(map),FILE_NORMAL,SAVE_ACTION,WRITE_ATTRS,NULL)) return 1;
  if (DosWrite(hfile,map,sizeof(map),&ulBytesWritten)) return 1;
  return DosClose(hfile) == 0;
}

void MAP::saveas(void) {
/* This function prompts the user for a new filename as which to save the current map.
*/
  char sz[128];

  if (!FileSave("Save Map",sz)) return;
  strcpy(szName,sz);
  if (save())
    WindowSetTitle(szName);
}
