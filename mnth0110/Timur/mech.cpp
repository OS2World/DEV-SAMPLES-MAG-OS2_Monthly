/* MECH.CPP - routines to control all the BattleMechs

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

#include "header.hpp"
#include "resource.h"
#include "dialog.h"
#include "hexes.hpp"
#include "target.hpp"
#include "bitmap.hpp"
#include "mech.hpp"
#include "window.hpp"

void MECH::ShowPosition(void) {
/* This function updates the "position" field in the Info Box
*/
  char sz[20];

  sprintf(sz,"(%i,%i)",c,r);
  WinSetDlgItemText(hwndInfoBox,IDD_POSITION,sz);
}

void MECH::draw(void) {
  pbmp[iDirection]->ptl=coord();
  pbmp[iDirection]->ptl.x-=HEX_EXT;
  pbmp[iDirection]->draw();
  ShowPosition();
}

MECH::MECH(void) : HEX(0,0) {
/* This function initializes the MECH module and loads all the mech bitmaps.
*/
  iDirection=HEXDIR_NORTH;
  for (int i=0; i<6; i++)
    pbmp[i]=new BITMAP(IDB_MECH_SE+i,IDB_HEX_MASK);
}

MECH::~MECH(void) {
  HEX::draw();
}

void MECH::move(HEX hex) {
/* This routine moves the 'Mech to position 'hi', if that position is adjacent
   to the 'Mech's current position.
   Future enhancement: allow the mech to change direction only by 60 degrees each turn.
*/
  if (!this)
    return;
  int dx=hex.c-c;
  int dy=hex.r-r;
  int iDir;                            // Direction from mech.hi to hi

  if (abs(dx) > 1) return;             // +/- one column?
  if (abs(dy) > 2) return;             // +/- two rows?
  if (*this==hex) return;               // same row/column?

// Calculate direction based on dx and dy
  switch (dy) {
    case -2: iDir=5; break;
    case -1: iDir= dx>0 ? 0 : 4; break;
    case 1:  iDir= dx>0 ? 1 : 3; break;
    case 2:  iDir=2; break;
  }

  if (iDir==iDirection) {         // Are we already facing the way we want to go?
    HEX::draw();                            // Yes, we can move that way
    c=hex.c;
    r=hex.r;
    ShowPosition();
  } else
    iDirection=iDir;              // No, let's turn to that direction instead
  draw();
}
