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
#define INCL_DOS
#define INCL_GPIPRIMITIVES
#define INCL_DEV
#include <os2.h>
#include "hexes.h"

// The width and the heigh of the client window, in pixels
#define WINDOW_WIDTH (1+HEX_DIAM+(NUM_COLUMNS-1)*(HEX_DIAM+HEX_SIDE)/2)
#define WINDOW_HEIGHT (1+HEX_HEIGHT*(NUM_ROWS+1)/2)

HAB hab;
HMQ hmq;
QMSG qmsg;
HWND hwndFrame,hwndClient,hwndTitleBar;
ULONG flStyle = (ULONG) (FCF_TITLEBAR|FCF_SYSMENU|FCF_TASKLIST|FCF_MINBUTTON|FCF_ICON);

extern TARGET target;

RECTL rclTitleBar;                       // For determining the title bar height
LONG lNumColors;
char *szClassName="TIMUR";               // What?  You don't like it?
char *szWinTitle="The Ultimate OS/2 Game";

static MRESULT EXPENTRY WinProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 ) {
  HPS hps;
  RECTL rcl;
  POINTL ptl;
  HEXINDEX hi;
  static HDC hdc;   // We don't need this on the stack

  switch (msg) {
    case WM_CREATE:
      hdc=WinOpenWindowDC(hwnd);
      DevQueryCaps(hdc,CAPS_COLORS,1L,&lNumColors);
      return 0;
    case WM_PAINT:
      hps=WinBeginPaint(hwnd,0UL,NULL);
      WinQueryWindowRect(hwnd,&rcl);
      WinFillRect(hps,&rcl,CLR_BLACK);
      GpiSetColor(hps,HEX_COLOR);

// Draw the map
      for (hi.c=1;hi.c<=NUM_COLUMNS;hi.c++)                   
        for (hi.r=2-(hi.c%2); hi.r<NUM_ROWS+(hi.c%2); hi.r+=2)
          HexDraw(hps,hi);

      if (target.fActive) {                       // Cancel any targetting
        target.fActive=FALSE;
        WinReleasePS(target.hpsLine);
      }
      WinEndPaint(hps);
      return 0L;
   case WM_BUTTON1DOWN:
      if (target.fActive) break;
      ptl.x=SHORT1FROMMP(mp1);
      ptl.y=SHORT2FROMMP(mp1);
      if (!HexLocate(ptl,&hi)) break;                  // hi = origin hex

      target.fActive=TRUE;                             // Initalize 'target'
      target.hiStart=hi;
      target.hiEnd=hi;
      target.ptlStart=HexMidpoint(hi);
      target.ptlEnd=target.ptlStart;
      target.hpsLine=WinGetPS(hwnd);
      GpiSetColor(target.hpsLine,CLR_BLUE);
      GpiMove(target.hpsLine,&target.ptlStart);
      GpiSetMix(target.hpsLine,FM_XOR);

// Initialize paramters for HexHighlight and start the thread
      target.hpsHighlight=WinGetPS(hwnd);
      DosCreateThread(&target.tid,HexHighlight,0UL,0UL,4096UL);

      break;
  case WM_BUTTON1UP:
    if (target.fActive) {
       target.fActive=FALSE;                 // Automatically terminates HexHighlight
       if (!HI_EQUAL(target.hiStart,target.hiEnd)) {  // Erase the line if it exists
         GpiMove(target.hpsLine,&target.ptlStart);
         GpiLine(target.hpsLine,&target.ptlEnd);
       }
       WinReleasePS(target.hpsLine);
    }
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
  }
  return WinDefWindowProc(hwnd,msg,mp1,mp2);
}

int main(void) {
  hab=WinInitialize(0);
  hmq=WinCreateMsgQueue(hab,0);
  
  if (!WinRegisterClass(hab,szClassName,WinProc,CS_SIZEREDRAW,0UL)) DosExit(EXIT_PROCESS,0);
  hwndFrame=WinCreateStdWindow(HWND_DESKTOP,0,&flStyle,szClassName,"",CS_SIZEREDRAW,0UL,1,&hwndClient);
  if (!hwndFrame) DosExit(EXIT_PROCESS,0);

  WinSetWindowText(hwndFrame,szWinTitle);
  hwndTitleBar=WinWindowFromID(hwndFrame,FID_TITLEBAR);
// We must display the window before we can get the height of the title bar
  WinSetWindowPos(hwndFrame,0,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,SWP_ACTIVATE|SWP_MOVE|SWP_SHOW|SWP_SIZE);
  if (!WinQueryWindowRect(hwndTitleBar,&rclTitleBar)) DosExit(EXIT_PROCESS,0);
// Now that we have the title bar height, re-size the window again.
  WinSetWindowPos(hwndFrame,0,0,0,WINDOW_WIDTH,WINDOW_HEIGHT+rclTitleBar.yTop,SWP_SIZE);
 
  while (WinGetMsg(hab,&qmsg,0,0,0)) WinDispatchMsg(hab,&qmsg);

  WinDestroyWindow(hwndFrame);
  WinDestroyMsgQueue(hmq);
  WinTerminate(hab);
  return 0;
}
