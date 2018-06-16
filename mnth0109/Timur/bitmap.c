/* BITMAP.C - Global Bitmap resource manager

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

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
#include "errors.h"

static HDC hdcMemory=NULLHANDLE;
static HPS hpsMemory=NULLHANDLE;
static HBITMAP hbmCurrent=NULLHANDLE;   // The current bitmap selected.  Used to avoid GpiSetBitmap() errors

ERROR BitmapShutdown(void) {
/* Releases the resouces allocated by BitmapLoad() during initialization.  OS/2 automatically deletes
   all bitmaps when the process termintes.  Therefore, after BitmapShutdown() is called, you can only
   call BitmapLoad() if you delete all bitmaps manually.
   This function may be called even if this module has never been initialized.
*/
// First check if the module was initialized in the first place
  if (hdcMemory==NULLHANDLE) return ERR_NOERROR;

  if (!GpiDestroyPS(hpsMemory)) return ERR_BITMAP_SHUT_PS;
  if (DevCloseDC(hdcMemory) != DEV_OK) return ERR_BITMAP_SHUT_DC;

  hdcMemory=NULLHANDLE;
  hpsMemory=NULLHANDLE;
  hbmCurrent=NULLHANDLE;
  return ERR_NOERROR;
}

ERROR BitmapLoad(ULONG ulBitmapID, PHBITMAP phbm) {
/* Loads a bitmap into the Bitmap Memory PS.  Also initializes this module when first called.
*/
  SIZEL sizel={0,0};
  HBITMAP hbm;

// Is this the first call to BitmapLoad() ?
  if (hdcMemory==NULLHANDLE) {
    hdcMemory=DevOpenDC(hab,OD_MEMORY,"*",0,NULL,NULLHANDLE);
    if (hdcMemory==NULLHANDLE) return ERR_BITMAP_LOAD_DC;

    hpsMemory=GpiCreatePS(hab,hdcMemory,&sizel,PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
    if (hpsMemory==NULLHANDLE) {
      DevCloseDC(hdcMemory);
      hdcMemory=NULLHANDLE;
      return ERR_BITMAP_LOAD_PS;
    }
    hbmCurrent=NULLHANDLE;
  }

  hbm=GpiLoadBitmap(hpsMemory,NULLHANDLE,ulBitmapID,0,0);
  if (hbm==NULLHANDLE) return ERR_BITMAP_LOAD_HBM;

  *phbm=hbm;
  return ERR_NOERROR;
}

void BitmapDraw(HBITMAP hbm, HBITMAP hbmMask, POINTL ptl) {
  POINTL aptl[3];
  HPS hps=WinGetPS(hwndClient);

  aptl[0]=ptl;
  aptl[1].x=aptl[0].x+29;
  aptl[1].y=aptl[0].y+25;
  aptl[2].x=0;
  aptl[2].y=0;

// First, mask out the background if requested
  if (hbmMask != NULLHANDLE) {
    if (hbmCurrent != hbmMask) {
      GpiSetBitmap(hpsMemory,hbmMask);
      hbmCurrent=hbmMask;
      }

    GpiBitBlt(hps,hpsMemory,3L,aptl,ROP_SRCAND,0);
  }

// Now, draw the bitmap
  if (hbmCurrent != hbm) {
    GpiSetBitmap(hpsMemory,hbm);
    hbmCurrent=hbm;
  }
  GpiBitBlt(hps,hpsMemory,3L,aptl,ROP_SRCPAINT,0);
  WinReleasePS(hps);
}
