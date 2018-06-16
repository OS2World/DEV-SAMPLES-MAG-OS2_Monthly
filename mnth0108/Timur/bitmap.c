/* BITMAP.C - Global Bitmap resource manager

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#define INCL_DEV
#define INCL_GPICONTROL
#define INCL_GPIBITMAPS
#define INCL_WINWINDOWMGR
#define INCL_WINDIALOGS
#include <os2.h>
#include <stdio.h>

#include "header.h"
#include "window.h"

static HDC hdcMemory=NULLHANDLE;
static HPS hpsMemory;

void BitmapShutdown(void) {
/* Releases the resouces allocated by BitmapInit()
*/
  GpiDestroyPS(hpsMemory);
  DevCloseDC(hdcMemory);
}

HBITMAP BitmapLoad(ULONG ulBitmapID) {
/* Loads a bitmap into the Bitmap Memory PS.  Also initializes the bitmap routine if needed.
*/
  SIZEL sizel={0,0};
  HBITMAP hbm;
  char sz[60];

// Is this the first call to BitmapLoad() ?
  if (hdcMemory==NULLHANDLE) {
// Yes, so init the module.  First, create the memory device context
    hdcMemory=DevOpenDC(hab,OD_MEMORY,"*",0,NULL,NULLHANDLE);
    if (hdcMemory==NULLHANDLE) {
      sprintf(sz,"Cannot open memory DC for bitmap %lu",ulBitmapID);
      WinMessageBox(HWND_DESKTOP,hwndClient,sz,NULL,0,MB_ENTER|MB_ICONEXCLAMATION);
      return NULLHANDLE;
    }
// Then create the accompanying presentation space
    hpsMemory=GpiCreatePS(hab,hdcMemory,&sizel,PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
    if (hpsMemory==NULLHANDLE) {
      sprintf(sz,"Cannot create memory presentation space for bitmap %lu",ulBitmapID);
      WinMessageBox(HWND_DESKTOP,hwndClient,sz,NULL,0,MB_ENTER|MB_ICONEXCLAMATION);
      return NULLHANDLE;
    }
  }

  hbm=GpiLoadBitmap(hpsMemory,NULLHANDLE,ulBitmapID,0,0);
  if (hbm==NULLHANDLE) {
    sprintf(sz,"Cannot load bitmap %lu",ulBitmapID);
    WinMessageBox(HWND_DESKTOP,hwndClient,sz,NULL,0,MB_ENTER|MB_ICONEXCLAMATION);
    return NULLHANDLE;
  }
  return hbm;
}

void BitmapDraw(HBITMAP hbm, HBITMAP hbmMask, POINTL ptl) {
  POINTL aptl[3];
  HPS hps=WinGetPS(hwndClient);

// First, mask out the background
  GpiSetBitmap(hpsMemory,hbmMask);
  aptl[0]=ptl;
  aptl[1].x=aptl[0].x+29;
  aptl[1].y=aptl[0].y+25;
  aptl[2].x=0;
  aptl[2].y=0;
  GpiBitBlt(hps,hpsMemory,3L,aptl,ROP_SRCAND,0);

// Now, draw the bitmap
  GpiSetBitmap(hpsMemory,hbm);
  GpiBitBlt(hps,hpsMemory,3L,aptl,ROP_SRCPAINT,0);
  WinReleasePS(hps);
}
