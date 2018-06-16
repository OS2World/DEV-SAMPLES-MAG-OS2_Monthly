/* TERRAIN.C - hex map terrain support

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
#include "header.h"
#include "errors.h"
#include "resource.h"
#include "terrain.h"
#include "hexes.h"
#include "target.h"
#include "bitmap.h"
#include "mech.h"
#include "files.h"
#include "window.h"

int TerrainIdFromMenu(int iMenuID) {
/* Given an IDM_TER_xxx value, this function returns the index into ater[] that corresponds to that
   terrain.  It returns -1 if it can't find a correspondence.
*/
  int i;

  for (i=0; i<NUM_TERRAINS; i++)
    if (ater[i].iMenuID==iMenuID) return i;

  return -1;
}

ERROR TerrainInit(void) {
/* Initialize the terrain data.  This routine will eventually read the values from a configuration
   file.  In the meantime, they are hard-coded
*/
  int i;
  int c,r;
  MAP map;      // default MAP type

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
  if (ErrorBox(BitmapLoad(IDB_TER_BLDGLIGHT,&ater[7].hbm))) return ERR_TERR_INIT;
  if (ErrorBox(BitmapLoad(IDB_TER_BLDGMEDIUM,&ater[8].hbm))) return ERR_TERR_INIT;
  if (ErrorBox(BitmapLoad(IDB_TER_BLDGHEAVY,&ater[9].hbm))) return ERR_TERR_INIT;
  if (ErrorBox(BitmapLoad(IDB_TER_BLDGHARD,&ater[10].hbm))) return ERR_TERR_INIT;

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

// Initialize the default map type
  map.iTerrain=TerrainIdFromMenu(IDM_TER_CLEAR_GROUND);
  map.iHeight=0;

// Initialize some variables
  if (ErrorBox(BitmapLoad(IDB_HEX_MASK,&hbmHexMask))) return ERR_TERR_INIT;

  for (c=0; c<NUM_COLUMNS; c++)
    for (r=c & 1; r<NUM_ROWS-(c & 1); r+=2)
       amap[c][r]=map;

  return ERR_NOERROR;
}

// -------- Drawing the map

void TerrainDrawMap(HWND hwnd) {
/* Draws the combat map.
   Future enhancement: Draw all hexagons of a given terrain first
*/
  RECTL rcl;
  HPS hps=WinBeginPaint(hwnd,0UL,NULL);
  HEXINDEX hi;

  target.fActive=FALSE;                             // Cancel any targetting
  WinQueryWindowRect(hwnd,&rcl);
  WinFillRect(hps,&rcl,HEX_COLOR);

  WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,FALSE));
  for (hi.c=0;hi.c<NUM_COLUMNS;hi.c++)
    for (hi.r=hi.c & 1;hi.r<NUM_ROWS-(hi.c & 1);hi.r+=2)
      HexFillDraw(hps,hi);
  WinSetPointer(HWND_DESKTOP,WinQuerySysPointer(HWND_DESKTOP,SPTR_ARROW,FALSE));
  WinEndPaint(hps);

  MechDraw();
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

static char szCurrentMap[128]="";

void TerrainNewMap(void) {
/* This function clears the current map
*/
  *szCurrentMap=0;
  WindowSetTitle(NULL);
}

static APIRET OpenMap(char *szName) {
/* This function loads the map specified by the filename szName.
*/
  HFILE hfile;
  ULONG ulAction,ulBytesRead;
  FILESTATUS3 fs3;

  RETURN(DosQueryPathInfo(szName,1,&fs3,sizeof(fs3)));
  RETURN(DosOpen(szName,&hfile,&ulAction,0,FILE_NORMAL,LOAD_ACTION,READ_ATTRS,NULL));
  RETURN(DosRead(hfile,amap,sizeof(amap),&ulBytesRead));
  return DosClose(hfile);
}

static APIRET SaveMap(char *szName) {
/* This function saves the map specified under the filename szName
*/
  HFILE hfile;
  ULONG ulAction,ulBytesWritten;

  RETURN(DosOpen(szName,&hfile,&ulAction,sizeof(amap),FILE_NORMAL,SAVE_ACTION,WRITE_ATTRS,NULL));
  RETURN(DosWrite(hfile,amap,sizeof(amap),&ulBytesWritten));
  return DosClose(hfile);
}

void TerrainOpenMap(void) {
/* This function prompts the user for a filename, and then loads that file as a map
   Returns FALSE if the open request was cancelled by the user.
*/
  gec=FileOpen("Open Map",szCurrentMap);                  // Here, the user selects the file
  if (gec) {
    if (gec != ERR_FILES_CANCEL)
      ErrorBox(gec);
    return;
  }

  if (ErrorDosError(OpenMap(szCurrentMap))) return;       // Open and read the map, exit if error

  WinInvalidateRect(hwndClient,NULL,FALSE);               // Make sure the old map is erased
  WindowSetTitle(szCurrentMap);                           // Change the window title to reflect it
  WinPostMsg(hwndClient,WM_PAINT,0,0);                    // Draw the new map
  return;
}

void TerrainSaveMap(void) {
/* This routine saves the current map under the current filename
*/
  ErrorDosError(SaveMap(szCurrentMap));
}

void TerrainSaveMapAs(void) {
/* This function prompts the user for a new filename as which to save the current map.
*/
  gec=FileSave("Save Map",szCurrentMap);
  if (gec) {
    if (gec != ERR_FILES_CANCEL) ErrorBox(gec);
    return;
  }

  if (ErrorDosError(SaveMap(szCurrentMap))) return;
  WindowSetTitle(szCurrentMap);
}
