/* MECH.C - routines to control all the BattleMechs

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#define INCL_WINWINDOWMGR
#define INCL_GPIBITMAPS
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>

#define MECH_C

#include "header.h"
#include "errors.h"
#include "resource.h"
#include "dialog.h"
#include "hexes.h"
#include "mech.h"
#include "target.h"
#include "bitmap.h"
#include "window.h"

static void ShowPosition(void) {
/* This function updates the "position" field in the Info Box
*/
  char sz[20];

  sprintf(sz,"(%i,%i)",mech.hi.c,mech.hi.r);
  WinSetDlgItemText(hwndInfoBox,IDD_POSITION,sz);
}

ERROR MechInit(void) {
/* This function initializes the MECH module and loads all the mech bitmaps.
*/
  mech.hi.c=0;
  mech.hi.r=0;
  mech.iDirection=HEXDIR_NORTH;
  RETURN(BitmapLoad(IDB_MECH_SE,&mech.hbm[0]));
  RETURN(BitmapLoad(IDB_MECH_NE,&mech.hbm[1]));
  RETURN(BitmapLoad(IDB_MECH_N,&mech.hbm[2]));
  RETURN(BitmapLoad(IDB_MECH_NW,&mech.hbm[3]));
  RETURN(BitmapLoad(IDB_MECH_SW,&mech.hbm[4]));
  RETURN(BitmapLoad(IDB_MECH_S,&mech.hbm[5]));
  ShowPosition();
  return ERR_NOERROR;
}

void MechErase(void) {
/* Erases the current 'Mech from the screen.
*/
  HPS hps=WinGetPS(hwndClient);

  HexFillDraw(hps,mech.hi);
  WinReleasePS(hps);
}

void MechDraw(void) {
/* Draws the current 'Mech.
*/
  POINTL ptl;

  ptl=HexCoord(mech.hi);
  ptl.x-=HEX_EXT;

  BitmapDraw(mech.hbm[mech.iDirection],hbmHexMask,ptl);
}


void MechMove(HEXINDEX hi) {
/* This routine moves the 'Mech to position 'hi', if that position is adjacent
   to the 'Mech's current position.
   Future enhancement: allow the mech to change direction only by 60 degrees each turn.
*/
  int dx=hi.c-mech.hi.c;
  int dy=hi.r-mech.hi.r;
  int iDir;                            // Direction from mech.hi to hi

  if (abs(dx) > 1) return;             // +/- one column?
  if (abs(dy) > 2) return;             // +/- two rows?
  if HI_EQUAL(hi,mech.hi) return;      // same row/column?

// Calculate direction based on dx and dy
  switch (dy) {
    case -2: iDir=5; break;
    case -1: iDir= dx>0 ? 0 : 4; break;
    case 1:  iDir= dx>0 ? 1 : 3; break;
    case 2:  iDir=2; break;
  }

  if (iDir==mech.iDirection) {         // Are we already facing the way we want to go?
    MechErase();                       // Yes, we can move that way
    mech.hi=hi;
    ShowPosition();
  } else
    mech.iDirection=iDir;              // No, let's turn to that direction instead
  MechDraw();
}
