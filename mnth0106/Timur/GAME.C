/* The Ultimate OS/2 Game.
Copyright (c) 1992 Timur Tabi 
Copyright (c) 1992 Fasa Corporation 

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.  

The program accompanies Timur Tabi's column "The Ultimate OS/2 Game", which
appears in "OS/2 Monthly".  It is a computer representation of the BattleTech and
Mechwarrior board games, as produced and distributed by the Fasa Coporation.

Special thanx go to:

Scott Cherkofsky, for great ideas since the very beginning.
Erin Sasaki, for proofreading my articles, even during finals.
Sam Lewis, president of Fasa, for giving me permission.
*/

#define INCL_WIN
#define INCL_GPIPRIMITIVES
#define INCL_DEV
#include <os2.h>
#include "game.h"
#include "hexes.h"
#include "target.h"
#include "dialog.h"
#include "files.h"

MRESULT EXPENTRY AboutDlgProc(HWND, ULONG, MPARAM, MPARAM);

// The width and the heigh of the client window, in pixels
#define WINDOW_WIDTH (1+HEX_DIAM+(NUM_COLUMNS-1)*(HEX_DIAM+HEX_SIDE)/2)
#define WINDOW_HEIGHT (1+HEX_HEIGHT*(NUM_ROWS+1)/2)

// Standard variables for PM programs
char *szClassName="TIMUR";                     // What?  You don't like it?
char *szWinTitle="The Ultimate OS/2 Game";
HAB hab;
HMQ hmq;
HPS hps=0;                          // For all drawing functions
QMSG qmsg;
HWND hwndFrame,hwndClient;
ULONG flStyle = (ULONG) (FCF_TITLEBAR|FCF_SYSMENU|FCF_TASKLIST|FCF_MINBUTTON|FCF_MENU|FCF_ICON);

// For determining the title and menu bar heights
HWND hwndTitleBar,hwndMenu;
RECTL rclTitleBar,rclMenu;

// What the user is currently doing
enum {MODE_TARGET, MODE_EDIT} eMode = MODE_TARGET;

// The Menu ID of the currently selected terrain type
USHORT usCheckedTerrain=IDM_TER_CLEAR_GROUND;

MRESULT EXPENTRY WinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 ) {
  RECTL rcl;
  POINTL ptl={0,0};
  HEXINDEX hi;
  static HDC hdc;             // We don't need this on the stack

  switch (msg) {
    case WM_CREATE:
      hdc=WinOpenWindowDC(hwnd);
      DevQueryCaps(hdc,CAPS_COLORS,1L,&lNumColors);
      TgtInitialize(hwnd);
      hpsHex=WinGetPS(hwnd);
      HexInitMap();
      return 0;
    case WM_PAINT:
      target.fActive=FALSE;                            // Cancel any targetting
      hps=WinBeginPaint(hwnd,0UL,NULL);
      WinQueryWindowRect(hwnd,&rcl);
      GpiSetBackMix(hps,BM_OVERPAINT);
      GpiSetPattern(hps,HexTerrainPattern(mapDefault.bTerrain));
      GpiSetColor(hps,HexTerrainColor(mapDefault.bTerrain));
      GpiMove(hps,&ptl);
      ptl.x=rcl.xRight;
      ptl.y=rcl.yTop;
      GpiBox(hps,DRO_OUTLINEFILL,&ptl,0,0);            // WinFillRect() doesn't
      WinEndPaint(hps);                                //  support patterns

      for (hi.c=0;hi.c<NUM_COLUMNS;hi.c++)
        for (hi.r=hi.c & 1;hi.r<NUM_ROWS-(hi.c & 1);hi.r+=2)
          if (amap[hi.c][hi.r].bTerrain==mapDefault.bTerrain) 
            HexDraw(hpsHex,hi);
          else
            HexFillDraw(hi);

      return 0L;
    case WM_BUTTON1DOWN:
      if (target.fActive) break;
      ptl.x=SHORT1FROMMP(mp1);
      ptl.y=SHORT2FROMMP(mp1);
      if (!HexLocate(ptl,&hi)) break;                  // hi = origin hex

      switch (eMode) {
        case MODE_TARGET:
          TgtStart(hi);
          break;
        case MODE_EDIT:
          amap[hi.c][hi.r].bTerrain=(BYTE) usCheckedTerrain;
          HexFillDraw(hi);
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
      if HI_EQUAL(hi,target.hiEnd) break;      // Test for redundancy

      if (!HI_EQUAL(target.hiStart,target.hiEnd)) {  // Erase the existing line if it exists
        GpiMove(target.hpsLine,&target.ptlStart);
        GpiLine(target.hpsLine,&target.ptlEnd);
      }
      target.ptlEnd=HexMidpoint(target.hiEnd=hi);    // Set the new endpoint and draw the line
      if (!HI_EQUAL(target.hiStart,target.hiEnd)) {  // Draw the new line if it exists
        GpiMove(target.hpsLine,&target.ptlStart);
        GpiLine(target.hpsLine,&target.ptlEnd);
      }
      break;
    case WM_COMMAND:
      switch (SHORT1FROMMP(mp1)) {
        case IDM_ABOUT:
          WinDlgBox(HWND_DESKTOP,hwnd,AboutDlgProc,NULLHANDLE,IDD_ABOUT,NULL);
          return 0;
        case IDM_MAP_LOAD:
          WinDlgBox(HWND_DESKTOP,hwnd,LoadDlgProc,NULLHANDLE,IDD_LOAD_MAP,NULL);
          return 0;
        case IDM_MAP_SAVE:
          WinDlgBox(HWND_DESKTOP,hwnd,SaveDlgProc,NULLHANDLE,IDD_SAVE_MAP,NULL);
          return 0;
        case IDM_TER_CLEAR_GROUND:
        case IDM_TER_ROUGH_GROUND:
        case IDM_TER_WATER:
        case IDM_TER_LIGHT_WOODS:
        case IDM_TER_HEAVY_WOODS:
        case IDM_TER_PAVEMENT:
        case IDM_TER_BRIDGE:
        case IDM_TER_LIGHT_BLDG:
        case IDM_TER_MEDIUM_BLDG:
        case IDM_TER_HEAVY_BLDG:
        case IDM_TER_HARD_BLDG:
          WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(usCheckedTerrain,TRUE),MPFROM2SHORT(MIA_CHECKED,0));
          usCheckedTerrain=SHORT1FROMMP(mp1);
          WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(usCheckedTerrain,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
          return 0L;
        case IDM_MAP_EDIT:
          if (eMode==MODE_EDIT) {
            eMode=MODE_TARGET;
            WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_MAP_EDIT,TRUE),MPFROM2SHORT(MIA_CHECKED,0));
          } else {
            eMode=MODE_EDIT;
            WinSendMsg(hwndMenu,MM_SETITEMATTR,MPFROM2SHORT(IDM_MAP_EDIT,TRUE),MPFROM2SHORT(MIA_CHECKED,MIA_CHECKED));
          }
          return 0L;
      } // end switch (SHORT1FROMMP(mp1))

  } // end switch (msg)

  return WinDefWindowProc(hwnd,msg,mp1,mp2);
}

int main(void) {
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

  while (WinGetMsg(hab,&qmsg,0,0,0)) WinDispatchMsg(hab,&qmsg);

  TgtShutdown();
  WinDestroyWindow(hwndFrame);
  WinDestroyMsgQueue(hmq);
  WinTerminate(hab);
  return 0;
}
