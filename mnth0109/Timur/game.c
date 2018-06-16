/* The Ultimate OS/2 Game.

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

The final and all interim code, in whole or in part, may only be for personal use
and may not be used as a basis for any shareware or commercial software.

The program accompanies Timur Tabi's column "The Ultimate OS/2 Game", which
appears in "OS/2 Monthly".  It is a computer representation of the BattleTech and
Mechwarrior board games, as produced and distributed by the Fasa Coporation.

Developed on:

IBM PS/2 70-A21 with 16MB, standard VGA, 80386DX-25 w/ 80387-25

OS/2 2.1 March Beta
IBM C/Set 2 compiler, with CSD #44 applied
EPM 5.51 enhanced editor
IBM WorkFrame/2 1.1
IBM OS/2 2.1 Toolkit

Special thanx go to:

Scott Cherkofsky, for great ideas and cool icons.
Erin Sasaki, for proofreading my articles, even during finals.
Sam Lewis, president of Fasa, for giving me permission.

Contributions:
Brian C. Ladd (BCL) - Hex map routines
Chris A. Masters (CAM) - Targetting algorithms

*/

#define INCL_WIN
#define INCL_GPIPRIMITIVES
#define INCL_DEV
#define INCL_GPIBITMAPS                               // for terrain.h
#include <os2.h>

#include "header.h"
#include "errors.h"
#include "window.h"
#include "resource.h"
#include "hexes.h"
#include "target.h"
#include "dialog.h"
#include "files.h"
#include "menu.h"
#include "mech.h"
#include "bitmap.h"
#include "terrain.h"

MRESULT EXPENTRY WinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 ) {
  POINTL ptl;
  HEXINDEX hi;
  HPS hps;
  static HDC hdc;             // We don't need this on the stack

  switch (msg) {
    case WM_CREATE:
      hdc=WinOpenWindowDC(hwnd);
      DevQueryCaps(hdc,CAPS_COLORS,1L,&lNumColors);
      break;
    case WM_PAINT:
      TerrainDrawMap(hwnd);
      break;
    case WM_MINMAXFRAME:                               // To hide/show infobox
      if ( ((PSWP) mp1)->fl & SWP_MINIMIZE )
        WinSetWindowPos(hwndInfoBox,NULLHANDLE,0,0,0,0,SWP_HIDE);
      if ( ((PSWP) mp1)->fl & SWP_RESTORE )
        WinSetWindowPos(hwndInfoBox,NULLHANDLE,0,0,0,0,SWP_SHOW);
      break;
    case WM_BUTTON1DOWN:
    case WM_BUTTON2DOWN:
      if (target.fActive) break;
      ptl.x=SHORT1FROMMP(mp1);
      ptl.y=SHORT2FROMMP(mp1);
      if (!HexLocate(ptl,&hi)) break;                  // hi = origin hex

      switch (iMode) {
        case IDM_MODE_MOVE:
          if (msg==WM_BUTTON1DOWN)
            MechMove(hi);                              // Button 1 moves
          else
            TgtStart(mech.hi);                         // Button 2 targets
          break;
        case IDM_MODE_EDIT:                            // Make this a separate function
          amap[hi.c][hi.r].iTerrain=iCurTer;
          hps=WinGetPS(hwnd);
          HexFillDraw(hps,hi);
          WinReleasePS(hps);
          if HI_EQUAL(hi,mech.hi) MechDraw();          // In case we drew over our 'Mech
          break;
       }
       break;
    case WM_BUTTON2UP:
      if (target.fActive) TgtEnd();
      break;
    case WM_MOUSEMOVE:
      if (!target.fActive) break;
      ptl.x=SHORT1FROMMP(mp1);                 // get the X,Y coordinates
      ptl.y=SHORT2FROMMP(mp1);
      if (!HexLocate(ptl,&hi)) break;          // Find out which hex it is
      TgtMove(hi);
      break;
    case WM_COMMAND:
      MainCommand(SHORT1FROMMP(mp1));
      break;
  } // end switch (msg)
  return WinDefWindowProc(hwnd,msg,mp1,mp2);
}

// The width and the height of the client window, in pixels
#define WINDOW_WIDTH (XLAG+HEX_DIAM+(NUM_COLUMNS-1)*(HEX_SIDE+HEX_EXT+XLAG))
#define WINDOW_HEIGHT ((HEX_HEIGHT+YLAG)*(NUM_ROWS+1)/2)

int main(void) {
  HMQ hmq;
  QMSG qmsg;
  ULONG flStyle = (ULONG) (FCF_TITLEBAR|FCF_SYSMENU|FCF_TASKLIST|FCF_MINBUTTON|FCF_MENU|FCF_ICON|FCF_ACCELTABLE);
  RECTL rclTitleBar,rclMenu;

// Standard variables for PM programs
  static char *szClassName="TIMUR";                 // What?  You don't like it?

  hab=WinInitialize(0);
  if (hab==NULLHANDLE) DosExit(EXIT_PROCESS,1);     // if we can't even get an anchor block, then just give up

  hmq=WinCreateMsgQueue(hab,0);

  if (!WinRegisterClass(hab,szClassName,WinProc,CS_SIZEREDRAW,0UL)) DosExit(EXIT_PROCESS,1);
  hwndFrame=WinCreateStdWindow(HWND_DESKTOP,0,&flStyle,szClassName,"",CS_SIZEREDRAW,0UL,ID_RESOURCE,&hwndClient);
  if (!hwndFrame) DosExit(EXIT_PROCESS,1);

// We must display the window before we can get the height of the title and menu bars
  WinSetWindowPos(hwndFrame,0,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,SWP_ACTIVATE|SWP_MOVE|SWP_SHOW|SWP_SIZE);

// Get the sizes of the titlebar and menu
  if (!WinQueryWindowRect(WinWindowFromID(hwndFrame,FID_TITLEBAR),&rclTitleBar)) DosExit(EXIT_PROCESS,1);
  hwndMenu=WinWindowFromID(hwndFrame,FID_MENU);
  if (!WinQueryWindowRect(hwndMenu,&rclMenu)) DosExit(EXIT_PROCESS,1);

// Now that we have the bar heights, re-size the window again.
  WinSetWindowPos(hwndFrame,0,0,0,WINDOW_WIDTH,WINDOW_HEIGHT+rclTitleBar.yTop+rclMenu.yTop,SWP_SIZE);

// The window has been displayed.  Let's initialize all the modules.  Some of them depend on
// hwndFrame and hwndClient.
  TgtInit();
  TerrainInit();
  MechInit();
  WindowSetTitle(NULL);

// You know what this does
  while (WinGetMsg(hab,&qmsg,0,0,0)) WinDispatchMsg(hab,&qmsg);

  BitmapShutdown();
  WinDestroyWindow(hwndFrame);
  WinDestroyMsgQueue(hmq);
  WinTerminate(hab);
  return 0;
}
