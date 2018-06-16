/* The Ultimate OS/2 Game.

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

The program accompanies Timur Tabi's column "The Ultimate OS/2 Game", which
appears in "OS/2 Monthly".  It is a computer representation of the BattleTech and
Mechwarrior board games, as produced and distributed by the Fasa Coporation.

Developed on:

IBM PS/2 70-A21 with 16MB, 640x480x16 VGA

OS/2 2.0 GA w/ various CSD's applied
IBM C/Set 2 compiler, with CSD #22 applied
EPM 5.51 enhanced editor
IBM WorkFrame/2, with CSD #3 applied
IBM NMake with HPFS-support

Special thanx go to:

Scott Cherkofsky, for great ideas since the very beginning.
Erin Sasaki, for proofreading my articles, even during finals.
Sam Lewis, president of Fasa, for giving me permission.

Contributors:
Brian C. Ladd (BCL) - Hex map routines
Chris A. Masters (CAM) - Targetting algorithms

*/

#define INCL_WIN
#define INCL_GPIPRIMITIVES
#define INCL_DEV
#define INCL_GPIBITMAPS                               // for terrain.h
#include <os2.h>

#include "header.h"
#include "window.h"
#include "resource.h"
#include "target.h"
#include "dialog.h"
#include "files.h"
#include "menu.h"
#include "mech.h"
#include "bitmap.h"
#include "terrain.h"

MRESULT EXPENTRY WinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 ) {
  POINTL ptl={0,0};
  HEXINDEX hi;
  static HDC hdc;             // We don't need this on the stack

  switch (msg) {
    case WM_CREATE:
      hdc=WinOpenWindowDC(hwnd);
      DevQueryCaps(hdc,CAPS_COLORS,1L,&lNumColors);
      TgtInitialize(hwnd);
      TerrainInit();
      MechInit();
      HexInitMap(hwnd);
      break;
    case WM_PAINT:
      HexDrawMap(hwnd);
      break;
    case WM_BUTTON1DOWN:
      if (target.fActive) break;
      ptl.x=SHORT1FROMMP(mp1);
      ptl.y=SHORT2FROMMP(mp1);
      if (!HexLocate(ptl,&hi)) break;                  // hi = origin hex

      switch (iMode) {
        case IDM_MODE_MOVE:
          MechMove(hi);
          break;
        case IDM_MODE_TARGET:
          TgtStart(mech.hi);
          break;
        case IDM_MODE_EDIT:
          amap[hi.c][hi.r].iTerrain=iCurTer;
          HexFillDraw(hi);
          if HI_EQUAL(hi,mech.hi) MechDraw();                  // In case we drew over our 'Mech
          break;
       }
       break;
    case WM_BUTTON1UP:
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
    default:
      return WinDefWindowProc(hwnd,msg,mp1,mp2);
  } // end switch (msg)
  return 0;
}

// The width and the height of the client window, in pixels
#define WINDOW_WIDTH (XLAG+HEX_DIAM+(NUM_COLUMNS-1)*(HEX_SIDE+HEX_EXT+XLAG))
#define WINDOW_HEIGHT ((HEX_HEIGHT+YLAG)*(NUM_ROWS+1)/2)

int main(void) {
  HMQ hmq;
  QMSG qmsg;
  ULONG flStyle = (ULONG) (FCF_TITLEBAR|FCF_SYSMENU|FCF_TASKLIST|FCF_MINBUTTON|FCF_MENU|FCF_ICON);

// Standard variables for PM programs
  static char *szClassName="TIMUR";                     // What?  You don't like it?
  static char *szWinTitle="The Ultimate OS/2 Game";

  LONG lx,ly;                                       // For positioning the Info Box

  hab=WinInitialize(0);
  hmq=WinCreateMsgQueue(hab,0);

  if (!WinRegisterClass(hab,szClassName,WinProc,CS_SIZEREDRAW,0UL)) DosExit(EXIT_PROCESS,0);
  hwndFrame=WinCreateStdWindow(HWND_DESKTOP,0,&flStyle,szClassName,"",CS_SIZEREDRAW,0UL,ID_RESOURCE,&hwndClient);
  if (!hwndFrame) DosExit(EXIT_PROCESS,0);

  WinSetWindowText(hwndFrame,szWinTitle);

// We must display the window before we can get the height of the title and menu bars
  WinSetWindowPos(hwndFrame,0,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,SWP_ACTIVATE|SWP_MOVE|SWP_SHOW|SWP_SIZE);

  hwndTitleBar=WinWindowFromID(hwndFrame,FID_TITLEBAR);
  if (!WinQueryWindowRect(hwndTitleBar,&rclTitleBar)) DosExit(EXIT_PROCESS,0);
  hwndMenu=WinWindowFromID(hwndFrame,FID_MENU);
  if (!WinQueryWindowRect(hwndMenu,&rclMenu)) DosExit(EXIT_PROCESS,0);

// Now that we have the bar heights, re-size the window again.
  WinSetWindowPos(hwndFrame,0,0,0,WINDOW_WIDTH,WINDOW_HEIGHT+rclTitleBar.yTop+rclMenu.yTop,SWP_SIZE);

// Load and position the info box
  hwndInfoBox=WinLoadDlg(HWND_DESKTOP,HWND_DESKTOP,NULL,NULLHANDLE,IDD_TARGETTING,NULL);
  WinQueryWindowRect(hwndFrame,&rclFrame);
  WinQueryWindowRect(hwndInfoBox,&rclBox);
  lx=rclFrame.xRight+10;
  ly=rclFrame.yTop-(rclBox.yTop-rclBox.yBottom);
  WinSetWindowPos(hwndInfoBox,HWND_BOTTOM,lx,ly,0,0,SWP_MOVE | SWP_SHOW);

// You know what this does
  while (WinGetMsg(hab,&qmsg,0,0,0)) WinDispatchMsg(hab,&qmsg);

  BitmapShutdown();
  TgtShutdown();
  WinDestroyWindow(hwndFrame);
  WinDestroyMsgQueue(hmq);
  WinTerminate(hab);
  return 0;
}
