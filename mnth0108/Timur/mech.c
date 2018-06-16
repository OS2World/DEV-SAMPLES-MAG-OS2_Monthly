/* MECH.C - routines to control all the BattleMechs

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#define INCL_WINWINDOWMGR
#define INCL_GPIBITMAPS
#include <os2.h>
#include <stdlib.h>

#define MECH_C
#include "header.h"
#include "resource.h"
#include "hexes.h"
#include "mech.h"
#include "bitmap.h"

void MechInit(void) {
  mech.hi.c=0;
  mech.hi.r=0;
  mech.iDirection=2;                         // Default is facing North
  mech.hbm[0]=BitmapLoad(IDB_MECH_SE);
  mech.hbm[1]=BitmapLoad(IDB_MECH_NE);
  mech.hbm[2]=BitmapLoad(IDB_MECH_N);
  mech.hbm[3]=BitmapLoad(IDB_MECH_NW);
  mech.hbm[4]=BitmapLoad(IDB_MECH_SW);
  mech.hbm[5]=BitmapLoad(IDB_MECH_S);
}

void MechErase(void) {
/* Erases the current 'Mech from the screen.
*/
  HexFillDraw(mech.hi);
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
    MechErase();
    mech.hi=hi;                        // Yes, we can move that way
  } else
    mech.iDirection=iDir;              // No, let's turn to that direction;
  MechDraw();
}
