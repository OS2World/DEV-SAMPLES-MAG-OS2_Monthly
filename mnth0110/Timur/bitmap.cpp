/* BITMAP.CPP - Global Bitmap resource manager

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#define INCL_DEV
#define INCL_GPICONTROL
#define INCL_GPIBITMAPS
#include <os2.h>
#include <stdio.h>

#include "header.hpp"
#include "window.hpp"
#include "hexes.hpp"
#include "bitmap.hpp"

static HDC hdcMemory=NULLHANDLE;
static HPS hpsMemory=NULLHANDLE;
static int iNumBitmaps=0;

static void setbitmap(HBITMAP hbm) {
/* Sets hbm as the current bitmap.  GpiSetBitmap() normally returns an error if the same bitmap 
  is set twice in a row.  This function eliminates that.  Since this function doesn't check the 
  return code from GpiSetBitmap() anyway, its usefulness is dubious.
*/
  static HBITMAP hbmCurrent=NULLHANDLE;

  if (hbmCurrent != hbm) {
    GpiSetBitmap(hpsMemory,hbm);
    hbmCurrent=hbm;
  }
}

void BITMAP::init(int ID, int maskID, POINTL ptl1) {
/* Initializes the OS/2 bitmap engine if necessary.  Normally, the code in this function would
   be in the constructor BITMAP::BITMAP(), but structures like {0,0} can't be passed as default 
   arguments (i.e. POINTL), so I have to do things the hard way.
*/
  if (hdcMemory==NULLHANDLE) {
    hdcMemory=DevOpenDC(hab,OD_MEMORY,"*",0,NULL,NULLHANDLE);
    if (hdcMemory==NULLHANDLE) return; // ERR_BITMAP_LOAD_DC;

    SIZEL sizel={0,0};
    hpsMemory=GpiCreatePS(hab,hdcMemory,&sizel,PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
    if (hpsMemory==NULLHANDLE) {
      DevCloseDC(hdcMemory);
      hdcMemory=NULLHANDLE;
      return; // ERR_BITMAP_LOAD_PS;
    }
    setbitmap(NULLHANDLE);
  }

  hbm=GpiLoadBitmap(hpsMemory,NULLHANDLE,(ULONG) ID,0,0);
  hbmMask= maskID ? GpiLoadBitmap(hpsMemory,NULLHANDLE,(ULONG) maskID,0,0) : NULLHANDLE;
  ptl=ptl1;
  ++iNumBitmaps;
}

BITMAP::BITMAP(POINTL ptl1, int ID, int maskID) {
  init(ID,maskID,ptl1);
}

BITMAP::BITMAP(int ID, int maskID) {
  POINTL ptl1={0,0};

  init(ID,maskID,ptl1);
}

BITMAP::~BITMAP(void) {
  GpiDeleteBitmap(hbm);

  if (!--iNumBitmaps) {
    GpiDestroyPS(hpsMemory);
    DevCloseDC(hdcMemory);

    hdcMemory=NULLHANDLE;
    hpsMemory=NULLHANDLE;
  }
}

void BITMAP::draw(void) {
/* Draws the bitmap at its current location.  Uses the mask if one has been defined
*/
  POINTL aptl[3];
  HPS hps=WinGetPS(hwndClient);

  aptl[0]=ptl;
  aptl[1].x=aptl[0].x+27;
  aptl[1].y=aptl[0].y+24;
  aptl[2].x=0;
  aptl[2].y=0;

// First, mask out the background if requested
  if (hbmMask != NULLHANDLE) {
    setbitmap(hbmMask);
    GpiBitBlt(hps,hpsMemory,3L,aptl,ROP_SRCAND,0);
  }

// Now, draw the bitmap
  setbitmap(hbm);
  GpiBitBlt(hps,hpsMemory,3L,aptl,ROP_SRCPAINT,0);
  WinReleasePS(hps);
}

void BITMAP::quickdraw(void) {
/* Same as BITMAP::draw() but doesn't use the mask, even if one is defined
*/
  POINTL aptl[3];
  HPS hps=WinGetPS(hwndClient);

  aptl[0]=ptl;
  aptl[1].x=aptl[0].x+27;
  aptl[1].y=aptl[0].y+24;
  aptl[2].x=0;
  aptl[2].y=0;

// Now, draw the bitmap
  setbitmap(hbm);
  GpiBitBlt(hps,hpsMemory,3L,aptl,ROP_SRCPAINT,0);
  WinReleasePS(hps);
}
