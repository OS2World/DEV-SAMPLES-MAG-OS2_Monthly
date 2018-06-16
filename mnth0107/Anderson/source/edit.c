/*
 *  EDIT.C -- A Simple Programmer's Editor Using MLE
 *  
 *  Programmer: Brian R. Anderson
 *  Date: January 1992
 *
 */

#define INCL_WINHELP
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "edit.h"
#include "efile.h"
#include "edlg.h"

/* custom messages, etc. */
#define WM_ARGS (WM_USER + 0)
#define WM_CLEANFILE (WM_USER + 1)
#define ID_MLE 13   /* my lucky number! */
#define ID_UPDATE 14
#define TAB 3

/* local function prototypes */
MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM);
MRESULT EXPENTRY TabWndProc (HWND, USHORT, MPARAM, MPARAM);
VOID SetPtrArrow (VOID);
VOID SetPtrWait (VOID);

/* global variables */
HAB hab;   /* anchor block handle */
HELPINIT hmiHelpData;   /* Help initialization structure*/
HWND hwndHelpInstance;   /* Handle to Help window        */
char szFileName[80];   /* current filename */
BOOL hasName = FALSE;   /* TRUE if current file has a name */ 
USHORT NeedToSave = FALSE;   /* TRUE if current file is 'Dirty' */
char szFind[60];   /* target string for search */
char szReplace[60];   /* replacement for search/replace */
char szLine[20];   /* line number to go to */
PFNWP pfMLE;   /* original MLE window procedure: subclassed to TabWndProc */


int main (int argc, char *argv[])
{
   static CHAR  szClientClass[] = "Edit";
   static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
                               FCF_SIZEBORDER    | FCF_MINMAX  |
                               FCF_SHELLPOSITION | FCF_TASKLIST |
                               FCF_MENU | FCF_ACCELTABLE | FCF_ICON;
   HMQ hmq;
   HWND hwndFrame, hwndClient;
   QMSG qmsg;
   USHORT res;
      
   hab = WinInitialize (0);
   hmq = WinCreateMsgQueue (hab, 0);

   WinRegisterClass (
                  hab,                /* Anchor block handle            */
                  szClientClass,      /* Name of class being registered */
                  ClientWndProc,      /* Window procedure for class     */
                  CS_SIZEREDRAW,      /* Class style                    */
                  0);                 /* Extra bytes to reserve         */

   /* Initialization IPF structure and create help instance */
   hmiHelpData.cb = sizeof (HELPINIT);
   hmiHelpData.ulReturnCode = 0L;   /* store HM return code from init. */
   hmiHelpData.pszTutorialName = NULL;   /* no tutorial program */
   hmiHelpData.phtHelpTable = (PVOID)(0xffff0000 | ID_EDIT);  /* table in RC */
   hmiHelpData.hmodAccelActionBarModule = 0L;   /* normal action bar */
   hmiHelpData.idAccelTable = 0;
   hmiHelpData.idActionBar = 0;
   hmiHelpData.pszHelpWindowTitle = "Edit Help";
   hmiHelpData.hmodHelpTableModule = 0L;   /* help not in DLL */
   hmiHelpData.usShowPanelId = 0;   /* don't display help panel IDs */
   hmiHelpData.pszHelpLibraryName = "EDIT.HLP";

   hwndHelpInstance = WinCreateHelpInstance (hab, &hmiHelpData);
   if (!hwndHelpInstance) {
      WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
         "Help Not Available", "Help Creation Error", 0,
          MB_OK | MB_ICONEXCLAMATION | MB_MOVEABLE);
   }
   else {
      if (hmiHelpData.ulReturnCode) {
         WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
            "Help Terminated Due to Error", "Help Creation Error", 0,
            MB_OK | MB_ICONEXCLAMATION | MB_MOVEABLE);
         WinDestroyHelpInstance(hwndHelpInstance);
      }
   }

   hwndFrame = WinCreateStdWindow (
                  HWND_DESKTOP,       /* Parent window handle            */
                  WS_VISIBLE,         /* Style of frame window           */
                  &flFrameFlags,      /* Pointer to control data         */
                  szClientClass,      /* Client window class name        */
                  NULL,               /* Title bar text                  */
                  0L,                 /* Style of client window          */
                  0,                  /* Module handle for resources     */
                  ID_EDIT,            /* ID of resources                 */
                  &hwndClient);       /* Pointer to client window handle */

   WinStartTimer (hab, hwndClient, ID_UPDATE, 200);   /* 1/5 second */
   
   if (argc > 1) {
      strcpy (szFileName, argv[1]);
      WinSendMsg (hwndClient, WM_ARGS, (MPARAM) 0L, (MPARAM) 0L);
   }

   if (hwndHelpInstance) 
      WinAssociateHelpInstance (hwndHelpInstance, hwndFrame);

   for (;;) {   
      while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
         WinDispatchMsg (hab, &qmsg);

      if (NeedToSave) {
         res = WinMessageBox (HWND_DESKTOP, hwndClient,
                              "Save before exit?", "End Session", 0,
                              MB_YESNOCANCEL | MB_ICONQUESTION | MB_MOVEABLE);
                        
         if (res == MBID_YES) {
            WinSendMsg (hwndClient, WM_COMMAND, 
               MPFROM2SHORT (IDM_SAVE, 0), (MPARAM) 0L);
            break;
         }
         else if (res == MBID_NO)
            break;
         else   /* res == MBID_CANCEL */
            WinCancelShutdown (hmq, TRUE);
      }
      else
         break;   /* terminate */
   }                                     

   if (hwndHelpInstance) 
      WinDestroyHelpInstance (hwndHelpInstance);
   WinStopTimer (hab, hwndClient, ID_UPDATE);
   WinDestroyWindow (hwndFrame);
   WinDestroyMsgQueue (hmq);
   WinTerminate (hab);
   return 0;
}


/* main window procedure for application */
MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   HDC hdc;   /* device context */
   LONG lHRes, lVRes;   /* device resolution */
   HPS hps;   /* presentation space */
   RECTL rcl;   /* rectangle drawing coordinates */
   PSWP pswp;   /* set window position structure */
   static HWND hwndFrame, hwndClient, hwndMenu, hwndMLE;   /* window handles */
   static USHORT cyClient, cxClient;   /* size of edit window */
   LONG selmin, selmax;   /* text selected for deletion */
   SHORT undoable;   /* TRUE if last operation can be undone */
   static BOOL undone = FALSE;   /* allows for redo immediately after undo */
   CHAR str[60];   /* character string for output messages */
   USHORT newline, newcolumn;   /* latest cursor position -- changed? */
   static USHORT line, column;   /* current cursor position */
   USHORT usfInfo;   /* flags for query clipboard information */
   static USHORT menuheight;   /* height of standard window */
   PCHAR buffer;   /* buffer for file I/O */
   LONG bufferlen;   /* number of characters read into buffer */
   FATTRS fattrs;   /* font attributes -- try to change to monospaced */
   FONTMETRICS *pfm;   /* find a monospaced system font */
   LONG lNumberFonts, lRequestFonts;   /* look for 10 point font */
   USHORT res;   /* response from dialog or message box */
   static USHORT resNew;   /* result of IDM_NEW (allows Cancel) */
   BOOL first;   /* set to TRUE if replace dialog called for first time */
   MLE_SEARCHDATA SearchData;   /* used is MLM_SEARCH message */
   LONG goline, gochar;   /* go to line specified by user */
   POINTS ps;   /* simulate mouse click to place cursor on specified line */
   int i;   /* simple loop counter */
            
   switch (msg) {
      case WM_CREATE:
         hwndClient = hwnd;
         hwndFrame = WinQueryWindow (hwnd, QW_PARENT, FALSE);
         hwndMenu = WinWindowFromID (hwndFrame, FID_MENU);
         menuheight = (USHORT) WinQuerySysValue (HWND_DESKTOP, SV_CYMENU);
         WinSetWindowText (hwndFrame, "EDIT");
            
         hwndMLE = WinCreateWindow (
            hwndClient,
            WC_MLE, 
            "", 
            WS_VISIBLE | MLS_HSCROLL | MLS_VSCROLL | MLS_BORDER,
            0, 0, 0, 0,   /* will set size & position later */
            hwndClient,
            HWND_TOP,
            ID_MLE,
            NULL, NULL);

         /* subclass to intercept tabs (convert to spaces) */
         pfMLE = WinSubclassWindow (hwndMLE, TabWndProc);
         
         /* override v2.x MLE colors */
         WinSendMsg (hwndMLE, MLM_SETTEXTCOLOR, 
            MPFROMLONG (CLR_DEFAULT), 0L);
         WinSendMsg (hwndMLE, MLM_SETBACKCOLOR, 
            MPFROMLONG (CLR_BACKGROUND), 0L);
   
         /* try to switch to System Monospaced 10-point font */                     
         hps = WinGetPS (hwndMLE);
         hdc = GpiQueryDevice (hps);
         DevQueryCaps (hdc, CAPS_HORIZONTAL_FONT_RES, 1L, &lHRes);
         DevQueryCaps (hdc, CAPS_VERTICAL_FONT_RES, 1L, &lVRes);
         lRequestFonts = 0L;
         lNumberFonts = GpiQueryFonts (hps, QF_PUBLIC, "System Monospaced",
                           &lRequestFonts, 0L, NULL);
         pfm = malloc ((SHORT) lNumberFonts * sizeof (FONTMETRICS));
         GpiQueryFonts (hps, QF_PUBLIC, "System Monospaced",  
            &lNumberFonts, (LONG) sizeof (FONTMETRICS), pfm);

         for (i = 0; i < (int)lNumberFonts; i++) {
            if (pfm[i].sXDeviceRes == (SHORT)lHRes &&   /* does font... */
                pfm[i].sYDeviceRes == (SHORT)lVRes &&   /* match device? */
                pfm[i].sNominalPointSize == 100) {   /* 10 point? */
               WinSendMsg (hwndMLE, MLM_QUERYFONT, 
                  MPFROMP (&fattrs), (MPARAM) 0L);
               fattrs.lMatch = pfm[i].lMatch;
               strcpy (fattrs.szFacename, "System Monospaced"); 
               WinSendMsg (hwndMLE, MLM_SETFONT, 
                  MPFROMP (&fattrs), (MPARAM) 0L);
               break;   /* exit for loop */
            }
         }
         free (pfm);
         WinReleasePS (hps);                     
         
         /* set up some MLE parameters */
         WinSendMsg (hwndMLE, MLM_SETTEXTLIMIT, 
            MPFROMLONG (65535L), (MPARAM) 0L);
                     
         WinSendMsg (hwndMLE, MLM_SETTABSTOP, 
            MPFROMSHORT (64), (MPARAM) 0L);
                     
         WinSendMsg (hwndMLE, MLM_FORMAT, 
            MPFROMSHORT (MLFIE_CFTEXT), (MPARAM) 0L);

         WinSendMsg (hwndMLE, MLM_RESETUNDO, (MPARAM) 0L, (MPARAM) 0L);
         WinSetFocus (HWND_DESKTOP, hwndMLE);
         return 0;

      case WM_MINMAXFRAME:
         pswp = PVOIDFROMMP (mp1);
         if (pswp->fs & SWP_MINIMIZE) 
            WinSetWindowText (hwndFrame, "EDIT");
         else {
            if (hasName) {
               sprintf (str, "EDIT -- %s", szFileName);			
               WinSetWindowText (hwndFrame, str);
            }
         }
         return 0;
         
      case WM_INITMENU:
         if (SHORT1FROMMP (mp1) == IDM_EDIT) {
            /* enable Cut, Copy, or Delete only if text selected */
            selmin = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYSEL, 
                                    (MPARAM) MLFQS_MINSEL, (MPARAM) 0L));
            selmax = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYSEL, 
                                    (MPARAM) MLFQS_MAXSEL, (MPARAM) 0L));
            WinSendMsg (hwndMenu, MM_SETITEMATTR,
               MPFROM2SHORT (IDM_CUT, TRUE),
               MPFROM2SHORT (MIA_DISABLED, 
               (selmin == selmax) ? MIA_DISABLED : 0));
            WinSendMsg (hwndMenu, MM_SETITEMATTR,
               MPFROM2SHORT (IDM_COPY, TRUE),
               MPFROM2SHORT (MIA_DISABLED, 
               (selmin == selmax) ? MIA_DISABLED : 0));
            WinSendMsg (hwndMenu, MM_SETITEMATTR,
               MPFROM2SHORT (IDM_DELETE, TRUE),
               MPFROM2SHORT (MIA_DISABLED, 
               (selmin == selmax) ? MIA_DISABLED : 0));
                  
            /* enable Paste only if data available in Clipboard */
            WinSendMsg (hwndMenu, MM_SETITEMATTR,
               MPFROM2SHORT (IDM_PASTE, TRUE),
               MPFROM2SHORT (MIA_DISABLED,
               WinQueryClipbrdFmtInfo (hab, CF_TEXT, &usfInfo)
               ? 0 : MIA_DISABLED));
                     
            /* enable Undo only if operation may be undone */
            undoable = SHORT1FROMMR (WinSendMsg (hwndMLE, MLM_QUERYUNDO, 
                                        (MPARAM) 0L, (MPARAM) 0L));
            WinSendMsg (hwndMenu, MM_SETITEMATTR,
               MPFROM2SHORT (IDM_UNDO, TRUE),
               MPFROM2SHORT (MIA_DISABLED, (undoable || undone) 
                                           ? 0 : MIA_DISABLED)); 
         }
         return 0;

      case WM_TIMER:
         if (SHORT1FROMMP (mp1) == ID_UPDATE) {
            /* determine position (line/column) of text cursor (caret) */
            selmin = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYSEL, 
                                    (MPARAM) MLFQS_MINSEL, (MPARAM) 0L));
            newline = (int) LONGFROMMR (WinSendMsg (hwndMLE, 
                                           MLM_LINEFROMCHAR,
                                           MPFROMLONG (selmin), 
                                           (MPARAM) 0L));
            newcolumn = (int) (selmin - LONGFROMMR (WinSendMsg (hwndMLE, 
                                           MLM_CHARFROMLINE,
                                           MPFROMLONG ((long) newline), 
                                           (MPARAM) 0L)));
         
            /* update on screen only if line or column changed */
            if (newline != line || newcolumn != column) {
               line = newline;   column = newcolumn;
               WinInvalidateRect (hwnd, NULL, FALSE);
            }
         }
         return 0;

      case WM_CONTROL:
         switch (SHORT2FROMMP (mp1)) {
            case MLN_OVERFLOW:
               if (SHORT1FROMMP (mp1) == ID_MLE) {
                  WinMessageBox (HWND_DESKTOP, hwnd, 
                     "File too large -- 64K limit exceeded.",
                     "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
               }
               return 0;

            case MLN_CHANGE:
               if (SHORT1FROMMP (mp1) == ID_MLE) {
                  NeedToSave = TRUE;
                  undone = FALSE;
               }
               return 0;
            
            default:
               break;
         }
         break;
    
      case WM_CLEANFILE:
         WinSendMsg (hwndMLE, MLM_RESETUNDO, (MPARAM) 0L, (MPARAM) 0L);
         NeedToSave = FALSE;
         return 0;
      
      case WM_ARGS:
         /* read file into buffer */			
         bufferlen = ReadFile (szFileName, &buffer);			 
         if (bufferlen == CANTREAD) {			 
            WinMessageBox (HWND_DESKTOP, hwnd, 
               "Specified file does not exist.",
               "New File", 0, MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
            hasName = TRUE;
            sprintf (str, "EDIT -- %s", szFileName);			
            WinSetWindowText (hwndFrame, str);			 
            NeedToSave = TRUE;
         }			 
         else if (bufferlen == TOOLONG) {			  
            WinMessageBox (HWND_DESKTOP, hwnd, 
               "File too large -- 64K limit exceeded.",
               "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
            WinSetWindowText (hwndFrame, "EDIT");
         }			 
         else if (bufferlen == NOMEMORY) {			
            WinMessageBox (HWND_DESKTOP, hwnd, 
               "Cannot allocate memory.",
               "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
            WinSendMsg (hwnd, WM_QUIT, (MPARAM) 0L, (MPARAM) 0L);
         }			 
         else {	/* normal */
            /* transfer buffer to MLE */
            WinSendMsg (hwndMLE, MLM_SETIMPORTEXPORT,
               MPFROMP (buffer),
               MPFROMSHORT ((USHORT)bufferlen));			
				selmin = 0L;
            WinSendMsg (hwndMLE, MLM_IMPORT,
               MPFROMP (&selmin), MPFROMLONG (bufferlen));			
				/* free buffer */
            Release (buffer);	
            hasName = TRUE;
            sprintf (str, "EDIT -- %s", szFileName);			
            WinSetWindowText (hwndFrame, str);			 
            WinPostMsg (hwnd, WM_CLEANFILE, (MPARAM) 0L, (MPARAM) 0L);
         }			 
         return 0;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1)) {
            case IDM_NEW:
               if (NeedToSave) {
                   res = WinMessageBox (HWND_DESKTOP, hwnd,
                            "Save current file?", "New", 0,
                             MB_YESNOCANCEL | MB_ICONQUESTION | MB_MOVEABLE);
                                
                  if (res == MBID_YES) {
                     WinSendMsg (hwnd, WM_COMMAND, 
                        MPFROM2SHORT (IDM_SAVE, 0), (MPARAM) 0L);
                  }
                  else if (res == MBID_CANCEL) {
                     resNew = MBID_CANCEL;
                     return 0;
                  }
               }
               SetPtrWait();
                  
               /* empty MLE */
               selmin = 0L;
               selmax = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYTEXTLENGTH,
                                       (MPARAM) 0L, (MPARAM) 0L));
               WinSendMsg (hwndMLE, MLM_DELETE, 
                  MPFROMLONG (selmin), MPFROMLONG (selmax));
                           
               hasName = FALSE;
               WinSetWindowText (hwndFrame, "EDIT");
               WinPostMsg (hwnd, WM_CLEANFILE, (MPARAM) 0L, (MPARAM) 0L);
               
               SetPtrArrow();
               resNew = MBID_OK;
               return 0;
               
            case IDM_OPEN:
               WinSendMsg (hwnd, WM_COMMAND, 
                  MPFROMSHORT (IDM_NEW), (MPARAM) 0L);
               if (resNew == MBID_CANCEL)
                  return 0;
                  
               if (WinDlgBox (HWND_DESKTOP, hwnd, OpenDlgProc,
                      0, IDD_OPEN, NULL)) {
                              
                  SetPtrWait();
                     
                  /* user selected a (valid) file to open */
                  /* read file into buffer */
                  bufferlen = ReadFile (szFileName, &buffer);
                  
                  SetPtrArrow();
                     
                  if (bufferlen == CANTREAD) {
                     WinMessageBox (HWND_DESKTOP, hwnd, 
                        "Specified file does not exist.",
                        "New File", 0, MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
                     hasName = TRUE;
                     sprintf (str, "EDIT -- %s", szFileName);			
                     WinSetWindowText (hwndFrame, str);			 
                     NeedToSave = TRUE;
                  }
                  else if (bufferlen == TOOLONG) {
                     WinMessageBox (HWND_DESKTOP, hwnd, 
                        "File too large -- 64K limit exceeded.",
                        "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
                     WinSetWindowText (hwndFrame, "EDIT");
                     NeedToSave = FALSE;
                  }
                  else if (bufferlen == NOMEMORY) {
                     WinMessageBox (HWND_DESKTOP, hwnd, 
                        "Cannot allocate memory.",
                        "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
                     WinSendMsg (hwnd, WM_QUIT, (MPARAM) 0L, (MPARAM) 0L);
                  }
                  else {   /* normal */
                     SetPtrWait();
                        
                     /* transfer buffer to MLE */
                     WinSendMsg (hwndMLE, MLM_SETIMPORTEXPORT, 
                        MPFROMP (buffer), MPFROMSHORT ((USHORT)bufferlen));
                     selmin = 0L;            
                     WinSendMsg (hwndMLE, MLM_IMPORT, 
                        MPFROMP (&selmin), MPFROMLONG (bufferlen));
                     /* free buffer */
                     Release (buffer);
                     hasName = TRUE;
                     sprintf (str, "EDIT -- %s", szFileName);
                     WinSetWindowText (hwndFrame, str);
                     WinPostMsg (hwnd, WM_CLEANFILE, (MPARAM) 0L, (MPARAM) 0L);
                     
                     SetPtrArrow();
                  }
               } 
               return 0;
       
            case IDM_SAVE:
               if (hasName) {
                  /* determine amount of text in MLE */
                  bufferlen = LONGFROMMR (WinSendMsg (hwndMLE, 
                                             MLM_QUERYFORMATTEXTLENGTH, 
                                             (MPARAM) 0L, MPFROMLONG (-1L)));
                                    
                  /* allocate space for buffer */
                  if (NOMEMORY == MakeWriteBuffer (bufferlen, &buffer)) {
                     WinMessageBox (HWND_DESKTOP, hwnd, 
                        "Cannot allocate memory.",
                        "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
                     WinSendMsg (hwnd, WM_QUIT, (MPARAM) 0L, (MPARAM) 0L);
                  }
                  SetPtrWait();
                     
                  /* transfer text from MLE to buffer */
                  WinSendMsg (hwndMLE, MLM_SETIMPORTEXPORT, 
                     MPFROMP (buffer), 
                     MPFROMSHORT ((USHORT)bufferlen));
                  selmin = 0L;   selmax = bufferlen;
                  WinSendMsg (hwndMLE, MLM_EXPORT, 
                     MPFROMP (&selmin), MPFROMP (&selmax));

                  /* write to file */
                  if (CANTWRITE == WriteFile (szFileName, bufferlen, buffer)) {
                     WinMessageBox (HWND_DESKTOP, hwnd, 
                        "Unable to write to file.",
                        "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
                  }
                              
                  /* deallocate buffer */
                  Release (buffer);
                  WinPostMsg (hwnd, WM_CLEANFILE, (MPARAM) 0L, (MPARAM) 0L);
                  
                  SetPtrArrow();
               }
               else {
                  WinSendMsg (hwnd, WM_COMMAND, 
                     MPFROM2SHORT (IDM_SAVEAS, 0), (MPARAM) 0L);
               } 
               return 0;
               
            case IDM_SAVEAS:
               if (WinDlgBox (HWND_DESKTOP, hwnd, SaveasDlgProc,
                      0, IDD_SAVEAS, NULL)) {
                  hasName = TRUE;
                  sprintf (str, "EDIT -- %s", szFileName);
                  WinSetWindowText (hwndFrame, str);
                  WinSendMsg (hwnd, WM_COMMAND, 
                     MPFROM2SHORT (IDM_SAVE, 0), (MPARAM) 0L);
               }
               return 0;
            
            case IDM_EXIT:
               WinSendMsg (hwnd, WM_CLOSE, (MPARAM) 0L, (MPARAM) 0L);
               return 0;
               
            case IDM_UNDO:
               WinSendMsg (hwndMLE, MLM_UNDO, (MPARAM) 0L, (MPARAM) 0L);
               undone = TRUE;
               return 0;
               
            case IDM_CUT:
               WinSendMsg (hwndMLE, MLM_CUT, (MPARAM) 0L, (MPARAM) 0L);
               return 0;
            
            case IDM_COPY:
               WinSendMsg (hwndMLE, MLM_COPY, (MPARAM) 0L, (MPARAM) 0L);
               return 0;
               
            case IDM_PASTE:
               WinSendMsg (hwndMLE, MLM_PASTE, (MPARAM) 0L, (MPARAM) 0L);
               return 0;
               
            case IDM_DELETE:
               WinSendMsg (hwndMLE, MLM_CLEAR, (MPARAM) 0L, (MPARAM) 0L);
               return 0;  
            
            case IDM_FIND:
               if (DID_OK == WinDlgBox (HWND_DESKTOP, hwnd, FindDlgProc,
                                0, IDD_FIND, NULL)) {
                  SetPtrWait();
      
                  SearchData.cb = sizeof (MLE_SEARCHDATA);
                  SearchData.pchFind = szFind;
                  SearchData.cchFind = strlen (szFind);
                  SearchData.iptStart = (-1L);
                  SearchData.iptStop = (-1L);
      
                  res = SHORT1FROMMR (WinSendMsg (hwndMLE, MLM_SEARCH, 
                                          MPFROMLONG (MLFSEARCH_SELECTMATCH),
                                          MPFROMP (&SearchData)));
      
                  SetPtrArrow();
      
                  if (!res) {
                     WinMessageBox (HWND_DESKTOP, hwnd, 
                        "Search string not found.", "Find", 0, 
                        MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
                  }
               }
               return 0;
               
            case IDM_REPLACE:
               first = TRUE;
               res = WinDlgBox (HWND_DESKTOP, hwnd, ReplaceDlgProc,
                        0, IDD_REPLACE, (PVOID)(&first));
               first = FALSE;
               for (;;) {
                  if (res == DID_CANCEL)
                     break;
                  else if (res == DID_OK) {                                   
                     SetPtrWait();
                        
                     SearchData.cb = sizeof (MLE_SEARCHDATA);
                     SearchData.pchFind = szFind;
                     SearchData.cchFind = strlen (szFind);
                     SearchData.iptStart = (-1L);
                     SearchData.iptStop = (-1L);
                     
                     res = SHORT1FROMMR (WinSendMsg (hwndMLE, MLM_SEARCH, 
                                            MPFROMLONG (MLFSEARCH_SELECTMATCH),
                                            MPFROMP (&SearchData)));
                                       
                     SetPtrArrow();
                        
                     if (!res) {                  
                        WinMessageBox (HWND_DESKTOP, hwnd, 
                           "Search string not found.", "Replace", 0,
                            MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
                        break;   /* exit search/replace */
                     }
                  }
                  else if (res == DID_DOREPLACE)
                     WinSendMsg (hwndMLE, MLM_INSERT, 
                        MPFROMP (szReplace), (MPARAM) 0L);
                  else if (res == DID_REPLACEALL) {
                     SetPtrWait();
                        
                     WinSendMsg (hwndMLE, MLM_INSERT, 
                        MPFROMP (szReplace), (MPARAM) 0L);
                                 
                     SearchData.cb = sizeof (MLE_SEARCHDATA);
                     SearchData.pchFind = szFind;
                     SearchData.cchFind = strlen (szFind);
                     SearchData.pchReplace = szReplace;
                     SearchData.cchReplace = strlen (szReplace);
                     SearchData.iptStart = (-1L);
                     SearchData.iptStop = (-1L);
                     WinSendMsg (hwndMLE, MLM_SEARCH, 
                        MPFROMLONG (MLFSEARCH_CHANGEALL), 
                        MPFROMP (&SearchData));
                                 
                     SetPtrArrow();
                        
                     WinMessageBox (HWND_DESKTOP, hwnd, 
                        "All occurrences replaced.", "Replace", 0,
                        MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
                     break;   /* exit search/replace */
                  }
                  else
                     break;   /* exit search/replace */
                     
                  res = WinDlgBox (HWND_DESKTOP, hwnd, ReplaceDlgProc,
                           0, IDD_REPLACE, (PVOID)(&first));
               }
               return 0;

            case IDM_GO:
                  res = WinDlgBox (HWND_DESKTOP, hwnd, GoLnDlgProc,
                           0, IDD_GOLINE, NULL);
                  if (res == MBID_OK) {
                     if (sscanf (szLine, "%ld", &goline)) {
                        if (goline < 1)
                           goline = 1;
                        gochar = LONGFROMMR (WinSendMsg (hwndMLE, 
                                                 MLM_CHARFROMLINE, 
                                                 MPFROMLONG (--goline), 
                                                 (MPARAM) 0L));
                        WinSendMsg (hwndMLE, MLM_SETFIRSTCHAR, 
                           MPFROMLONG (gochar), (MPARAM) 0L);
                        ps.x = 0;   ps.y = cyClient - (2 * menuheight);
                        WinSendMsg (hwndMLE, WM_BUTTON1DOWN, 
                           MPFROM2SHORT (ps.x, ps.y), (MPARAM) 0L);
                        WinSendMsg (hwndMLE, WM_BUTTON1UP, 
                           MPFROM2SHORT (ps.x, ps.y), (MPARAM) 0L);
                     }
                  }
               return 0;

            case IDM_HELPFORHELP:
               if (hwndHelpInstance)
                  WinSendMsg( hwndHelpInstance, HM_DISPLAY_HELP, 0L, 0L);
               break;

            case IDM_EXTENDEDHELP:
               WinPostMsg (hwndFrame, WM_SYSCOMMAND,
                  MPFROM2SHORT (SC_HELPEXTENDED, 0), (MPARAM) 0L);
               break;
               
            case IDM_KEYSHELP:
               WinPostMsg (hwndFrame, WM_SYSCOMMAND,
                  MPFROM2SHORT (SC_HELPKEYS, 0), (MPARAM) 0L);
               break;
               
            case IDM_HELPINDEX:
               WinPostMsg (hwndFrame, WM_SYSCOMMAND,
                  MPFROM2SHORT (SC_HELPINDEX, 0), (MPARAM) 0L);
               break;
               
            case IDM_ABOUT:
               WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc, 
                  0, IDD_ABOUT, NULL);
               return 0;
               
            default:
               break;
         }
         break;

      case WM_SIZE:
         cxClient = SHORT1FROMMP (mp2);
         cyClient = SHORT2FROMMP (mp2);
         
         WinSetWindowPos (
            hwndMLE, 
            HWND_TOP, 
            0, 0, 
            cxClient, cyClient - menuheight, 
            SWP_MOVE | SWP_SIZE);
         WinSetFocus (HWND_DESKTOP, hwndMLE);
         return 0;

      case WM_PAINT:
         hps = WinBeginPaint (hwnd, NULL, NULL);
         sprintf (str, "  Line: %-5d   Col: %-5d", line + 1, column + 1);
         WinQueryWindowRect (hwnd, &rcl);
         rcl.yBottom = cyClient - menuheight;
         WinDrawText (hps, -1, str, &rcl, CLR_NEUTRAL, CLR_BACKGROUND,
            DT_LEFT | DT_VCENTER | DT_ERASERECT);
         WinEndPaint (hps);
         return 0;
   
      case HM_QUERY_KEYS_HELP:
         return ((MRESULT) IDH_KEYSHELP);
         break;

      case HM_ERROR:
         if ( (hwndHelpInstance && (ULONG) mp1) == HMERR_NO_MEMORY) {
            WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
               "Help Terminated Due to Error", "Help Error", 0,
               MB_OK | MB_ICONEXCLAMATION | MB_MOVEABLE);
            WinDestroyHelpInstance(hwndHelpInstance);
         }
         else {
            WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
               "Help Error Occurred", "Help Error", 0,
               MB_OK | MB_ICONEXCLAMATION | MB_MOVEABLE);
         }
         break;
         
      case WM_DESTROY:
         WinDestroyWindow (hwndMLE);
         return 0;
   }
   return WinDefWindowProc (hwnd, msg, mp1, mp2);
}


MRESULT EXPENTRY TabWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   USHORT flags, vk;   /* WM_CHAR message parameters */
   long curpos;   /* current postion -- relative to top of document */
   int ln, col;   /* current position -- line/column */
   char str[TAB + 1];   /* insert string of spaces for tab */
   int i;   /* loop counter: will insert spaces for tab */
   int ts;   /* number of spaces to insert for tab */

   switch (msg) {
      case WM_CHAR:
         flags = SHORT1FROMMP (mp1);
         vk = SHORT2FROMMP (mp2);
         if (flags & KC_VIRTUALKEY && !(flags & KC_KEYUP) && vk == VK_TAB) {
            /* determine position of text cursor */
            curpos = LONGFROMMR (WinSendMsg (hwnd, MLM_QUERYSEL, 
                                    (MPARAM) MLFQS_MINSEL, (MPARAM) 0L));
            ln = (int) LONGFROMMR (WinSendMsg (hwnd, MLM_LINEFROMCHAR,
                                      MPFROMLONG (curpos), (MPARAM) 0L));
            col = (int) (curpos - LONGFROMMR (WinSendMsg (hwnd, 
                                                 MLM_CHARFROMLINE,
                                                 MPFROMLONG ((long) ln), 
                                                 (MPARAM) 0L)));
            ts = TAB - (col % TAB); 
            for (i = 0; i < ts; i++) 
               str[i] = ' ';
            str[i] = '\0';
            
            WinSendMsg (hwnd, MLM_INSERT, MPFROMP (str), (MPARAM) 0L);
            return 0;
         }
         else
            break;

      default:
         break;
   }
   pfMLE (hwnd, msg, mp1, mp2);
}


VOID SetPtrArrow (VOID)
{
   if (!WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT))
      WinShowPointer (HWND_DESKTOP, FALSE);

   WinSetPointer (HWND_DESKTOP,
      WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, 0));
}


VOID SetPtrWait (VOID)
{
   WinSetPointer (HWND_DESKTOP,
      WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, 0));

   if (!WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT))
      WinShowPointer (HWND_DESKTOP, TRUE);
}
