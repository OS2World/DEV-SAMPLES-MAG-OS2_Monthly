/* TARGET.C - Targetting routines

Copyright (c) 1992 Timur Tabi 
Copyright (c) 1992 Fasa Corporation 

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.  

*/

#define INCL_DOS
#define INCL_GPIPRIMITIVES
#include <os2.h>
#include "hexes.h"

void TgtInitialize(HWND hwnd) {
  target.fActive=FALSE;
  target.hpsLine=WinGetPS(hwnd);
  target.hpsHighlight=WinGetPS(hwnd);
  GpiSetColor(target.hpsLine,CLR_BLUE);
  GpiMove(target.hpsLine,&target.ptlStart);
  GpiSetMix(target.hpsLine,FM_XOR);
}

void TgtShutdown(void) {
  WinReleasePS(target.hpsLine);
  WinReleasePS(target.hpsHighlight);
}

void TgtStart(HEXINDEX hi) {
  target.fActive=TRUE;                             // Initalize 'target'
  target.hiStart=hi;
  target.hiEnd=hi;
  target.ptlStart=HexMidpoint(hi);
  target.ptlEnd=target.ptlStart;

  DosCreateThread(&target.tid,HexHighlight,0UL,0UL,4096UL);
}

void TgtEnd(void) {
  target.fActive=FALSE;                 // Automatically terminates HexHighlight
  if (!HI_EQUAL(target.hiStart,target.hiEnd)) {  // Erase the line if it exists
    GpiMove(target.hpsLine,&target.ptlStart);
    GpiLine(target.hpsLine,&target.ptlEnd);
  }
}

void TgtMove(HEXINDEX hi) {
// Erase the existing line if it exists
  if (!HI_EQUAL(target.hiStart,target.hiEnd)) {  
    GpiMove(target.hpsLine,&target.ptlStart);
    GpiLine(target.hpsLine,&target.ptlEnd);
  }

  target.ptlEnd=HexMidpoint(target.hiEnd=hi);   // Set the new endpoint and draw the line

// Draw the new line if it exists
  if (!HI_EQUAL(target.hiStart,target.hiEnd)) {
    GpiMove(target.hpsLine,&target.ptlStart);
    GpiLine(target.hpsLine,&target.ptlEnd);
  }
}
