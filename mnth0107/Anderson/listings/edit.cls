            Listing for edit.c                                          Page 5

          102|   /*
          103|    *  EDIT.C -- A Simple Programmer's Editor Using MLE
          104|    *  
          105|    *  Programmer: Brian R. Anderson
          106|    *  Date: January 1992
          107|    *
          108|    */
          109|   
          110|   #define INCL_WINHELP
          111|   #define INCL_WIN
          112|   #define INCL_GPI
          113|   #define INCL_DOS
          114|   #include <os2.h>
          115|   #include <stdio.h>
          116|   #include <string.h>
          117|   #include <stdlib.h>
          118|   #include "edit.h"
          119|   #include "efile.h"
          120|   #include "edlg.h"
          121|   
          122|   /* custom messages, etc. */
          123|   #define WM_ARGS (WM_USER + 0)
          124|   #define WM_CLEANFILE (WM_USER + 1)
          125|   #define ID_MLE 13   /* my lucky number! */
          126|   #define ID_UPDATE 14
          127|   #define TAB 3
          128|   
          129|   /* local function prototypes */
          130|   MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM);
          131|   MRESULT EXPENTRY TabWndProc (HWND, USHORT, MPARAM, MPARAM);
          132|   VOID SetPtrArrow (VOID);
          133|   VOID SetPtrWait (VOID);
          134|   
          135|   /* global variables */
          136|   HAB hab;   /* anchor block handle */
          137|   HELPINIT hmiHelpData;   /* Help initialization structure*/
          138|   HWND hwndHelpInstance;   /* Handle to Help window        */
          139|   char szFileName[80];   /* current filename */
          140|   BOOL hasName = FALSE;   /* TRUE if current file has a name */ 
          141|   USHORT NeedToSave = FALSE;   /* TRUE if current file is 'Dirty' */
          142|   char szFind[60];   /* target string for search */
          143|   char szReplace[60];   /* replacement for search/replace */
          144|   char szLine[20];   /* line number to go to */
          145|   PFNWP pfMLE;   /* original MLE window procedure: subclassed to TabWndProc */
          146|   
          147|   
          148|   int main (int argc, char *argv[])
          149|   {
          150|      static CHAR  szClientClass[] = "Edit";
          151|      static ULONG flFrameFlags = FCF_TITLEBAR      | FCF_SYSMENU |
          152|                                  FCF_SIZEBORDER    | FCF_MINMAX  |
          153|                                  FCF_SHELLPOSITION | FCF_TASKLIST |
          154|                                  FCF_MENU | FCF_ACCELTABLE | FCF_ICON;
          155|      HMQ hmq;
          156|      HWND hwndFrame, hwndClient;
edit.c                                                                  Page 6

          157|      QMSG qmsg;
          158|      USHORT res;
          159|         
          160|      hab = WinInitialize (0);
          161|      hmq = WinCreateMsgQueue (hab, 0);
          162|   
          163|      WinRegisterClass (
          164|                     hab,                /* Anchor block handle            */
          165|                     szClientClass,      /* Name of class being registered */
          166|                     ClientWndProc,      /* Window procedure for class     */
          167|                     CS_SIZEREDRAW,      /* Class style                    */
          168|                     0);                 /* Extra bytes to reserve         */
          169|   
          170|      /* Initialization IPF structure and create help instance */
          171|      hmiHelpData.cb = sizeof (HELPINIT);
          172|      hmiHelpData.ulReturnCode = 0L;   /* store HM return code from init. */
          173|      hmiHelpData.pszTutorialName = NULL;   /* no tutorial program */
          174|      hmiHelpData.phtHelpTable = (PVOID)(0xffff0000 | ID_EDIT);  /* table in RC */
          175|      hmiHelpData.hmodAccelActionBarModule = 0L;   /* normal action bar */
          176|      hmiHelpData.idAccelTable = 0;
          177|      hmiHelpData.idActionBar = 0;
          178|      hmiHelpData.pszHelpWindowTitle = "Edit Help";
          179|      hmiHelpData.hmodHelpTableModule = 0L;   /* help not in DLL */
          180|      hmiHelpData.usShowPanelId = 0;   /* don't display help panel IDs */
          181|      hmiHelpData.pszHelpLibraryName = "EDIT.HLP";
          182|   
          183|      hwndHelpInstance = WinCreateHelpInstance (hab, &hmiHelpData);
          184|      if (!hwndHelpInstance) {
          185|         WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
          186|            "Help Not Available", "Help Creation Error", 0,
          187|             MB_OK | MB_ICONEXCLAMATION | MB_MOVEABLE);
          188|      }
          189|      else {
          190|         if (hmiHelpData.ulReturnCode) {
          191|            WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
          192|               "Help Terminated Due to Error", "Help Creation Error", 0,
          193|               MB_OK | MB_ICONEXCLAMATION | MB_MOVEABLE);
          194|            WinDestroyHelpInstance(hwndHelpInstance);
          195|         }
          196|      }
          197|   
          198|      hwndFrame = WinCreateStdWindow (
          199|                     HWND_DESKTOP,       /* Parent window handle            */
          200|                     WS_VISIBLE,         /* Style of frame window           */
          201|                     &flFrameFlags,      /* Pointer to control data         */
          202|                     szClientClass,      /* Client window class name        */
          203|                     NULL,               /* Title bar text                  */
          204|                     0L,                 /* Style of client window          */
          205|                     0,                  /* Module handle for resources     */
          206|                     ID_EDIT,            /* ID of resources                 */
          207|                     &hwndClient);       /* Pointer to client window handle */
          208|   
          209|      WinStartTimer (hab, hwndClient, ID_UPDATE, 200);   /* 1/5 second */
          210|      
          211|      if (argc > 1) {
edit.c                                                                  Page 7

          212|         strcpy (szFileName, argv[1]);
          213|         WinSendMsg (hwndClient, WM_ARGS, (MPARAM) 0L, (MPARAM) 0L);
          214|      }
          215|   
          216|      if (hwndHelpInstance) 
          217|         WinAssociateHelpInstance (hwndHelpInstance, hwndFrame);
          218|   
          219|      for (;;) {   
          220|         while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          221|            WinDispatchMsg (hab, &qmsg);
          222|   
          223|         if (NeedToSave) {
          224|            res = WinMessageBox (HWND_DESKTOP, hwndClient,
          225|                                 "Save before exit?", "End Session", 0,
          226|                                 MB_YESNOCANCEL | MB_ICONQUESTION | MB_MOVEABLE);
          227|                           
          228|            if (res == MBID_YES) {
          229|               WinSendMsg (hwndClient, WM_COMMAND, 
          230|                  MPFROM2SHORT (IDM_SAVE, 0), (MPARAM) 0L);
          231|               break;
          232|            }
          233|            else if (res == MBID_NO)
          234|               break;
          235|            else   /* res == MBID_CANCEL */
          236|               WinCancelShutdown (hmq, TRUE);
          237|         }
          238|         else
          239|            break;   /* terminate */
          240|      }                                     
          241|   
          242|      if (hwndHelpInstance) 
          243|         WinDestroyHelpInstance (hwndHelpInstance);
          244|      WinStopTimer (hab, hwndClient, ID_UPDATE);
          245|      WinDestroyWindow (hwndFrame);
          246|      WinDestroyMsgQueue (hmq);
          247|      WinTerminate (hab);
          248|      return 0;
          249|   }
          250|   
          251|   
          252|   /* main window procedure for application */
          253|   MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
          254|   {
          255|      HDC hdc;   /* device context */
          256|      LONG lHRes, lVRes;   /* device resolution */
          257|      HPS hps;   /* presentation space */
          258|      RECTL rcl;   /* rectangle drawing coordinates */
          259|      PSWP pswp;   /* set window position structure */
          260|      static HWND hwndFrame, hwndClient, hwndMenu, hwndMLE;   /* window handles */
          261|      static USHORT cyClient, cxClient;   /* size of edit window */
          262|      LONG selmin, selmax;   /* text selected for deletion */
          263|      SHORT undoable;   /* TRUE if last operation can be undone */
          264|      static BOOL undone = FALSE;   /* allows for redo immediately after undo */
          265|      CHAR str[60];   /* character string for output messages */
          266|      USHORT newline, newcolumn;   /* latest cursor position -- changed? */
edit.c                                                                  Page 8

          267|      static USHORT line, column;   /* current cursor position */
          268|      USHORT usfInfo;   /* flags for query clipboard information */
          269|      static USHORT menuheight;   /* height of standard window */
          270|      PCHAR buffer;   /* buffer for file I/O */
          271|      LONG bufferlen;   /* number of characters read into buffer */
          272|      FATTRS fattrs;   /* font attributes -- try to change to monospaced */
          273|      FONTMETRICS *pfm;   /* find a monospaced system font */
          274|      LONG lNumberFonts, lRequestFonts;   /* look for 10 point font */
          275|      USHORT res;   /* response from dialog or message box */
          276|      static USHORT resNew;   /* result of IDM_NEW (allows Cancel) */
          277|      BOOL first;   /* set to TRUE if replace dialog called for first time */
          278|      MLE_SEARCHDATA SearchData;   /* used is MLM_SEARCH message */
          279|      LONG goline, gochar;   /* go to line specified by user */
          280|      POINTS ps;   /* simulate mouse click to place cursor on specified line */
          281|      int i;   /* simple loop counter */
          282|               
          283|      switch (msg) {
          284|         case WM_CREATE:
          285|            hwndClient = hwnd;
          286|            hwndFrame = WinQueryWindow (hwnd, QW_PARENT, FALSE);
          287|            hwndMenu = WinWindowFromID (hwndFrame, FID_MENU);
          288|            menuheight = (USHORT) WinQuerySysValue (HWND_DESKTOP, SV_CYMENU);
          289|            WinSetWindowText (hwndFrame, "EDIT");
          290|               
          291|            hwndMLE = WinCreateWindow (
          292|               hwndClient,
          293|               WC_MLE, 
          294|               "", 
          295|               WS_VISIBLE | MLS_HSCROLL | MLS_VSCROLL | MLS_BORDER,
          296|               0, 0, 0, 0,   /* will set size & position later */
          297|               hwndClient,
          298|               HWND_TOP,
          299|               ID_MLE,
          300|               NULL, NULL);
          301|   
          302|            /* subclass to intercept tabs (convert to spaces) */
          303|            pfMLE = WinSubclassWindow (hwndMLE, TabWndProc);
          304|            
          305|            /* override v2.x MLE colors */
          306|            WinSendMsg (hwndMLE, MLM_SETTEXTCOLOR, 
          307|               MPFROMLONG (CLR_DEFAULT), 0L);
          308|            WinSendMsg (hwndMLE, MLM_SETBACKCOLOR, 
          309|               MPFROMLONG (CLR_BACKGROUND), 0L);
          310|      
          311|            /* try to switch to System Monospaced 10-point font */                     
          312|            hps = WinGetPS (hwndMLE);
          313|            hdc = GpiQueryDevice (hps);
          314|            DevQueryCaps (hdc, CAPS_HORIZONTAL_FONT_RES, 1L, &lHRes);
          315|            DevQueryCaps (hdc, CAPS_VERTICAL_FONT_RES, 1L, &lVRes);
          316|            lRequestFonts = 0L;
          317|            lNumberFonts = GpiQueryFonts (hps, QF_PUBLIC, "System Monospaced",
          318|                              &lRequestFonts, 0L, NULL);
          319|            pfm = malloc ((SHORT) lNumberFonts * sizeof (FONTMETRICS));
          320|            GpiQueryFonts (hps, QF_PUBLIC, "System Monospaced",  
          321|               &lNumberFonts, (LONG) sizeof (FONTMETRICS), pfm);
edit.c                                                                  Page 9

          322|   
          323|            for (i = 0; i < (int)lNumberFonts; i++) {
          324|               if (pfm[i].sXDeviceRes == (SHORT)lHRes &&   /* does font... */
          325|                   pfm[i].sYDeviceRes == (SHORT)lVRes &&   /* match device? */
          326|                   pfm[i].sNominalPointSize == 100) {   /* 10 point? */
          327|                  WinSendMsg (hwndMLE, MLM_QUERYFONT, 
          328|                     MPFROMP (&fattrs), (MPARAM) 0L);
          329|                  fattrs.lMatch = pfm[i].lMatch;
          330|                  strcpy (fattrs.szFacename, "System Monospaced"); 
          331|                  WinSendMsg (hwndMLE, MLM_SETFONT, 
          332|                     MPFROMP (&fattrs), (MPARAM) 0L);
          333|                  break;   /* exit for loop */
          334|               }
          335|            }
          336|            free (pfm);
          337|            WinReleasePS (hps);                     
          338|            
          339|            /* set up some MLE parameters */
          340|            WinSendMsg (hwndMLE, MLM_SETTEXTLIMIT, 
          341|               MPFROMLONG (65535L), (MPARAM) 0L);
          342|                        
          343|            WinSendMsg (hwndMLE, MLM_SETTABSTOP, 
          344|               MPFROMSHORT (64), (MPARAM) 0L);
          345|                        
          346|            WinSendMsg (hwndMLE, MLM_FORMAT, 
          347|               MPFROMSHORT (MLFIE_CFTEXT), (MPARAM) 0L);
          348|   
          349|            WinSendMsg (hwndMLE, MLM_RESETUNDO, (MPARAM) 0L, (MPARAM) 0L);
          350|            WinSetFocus (HWND_DESKTOP, hwndMLE);
          351|            return 0;
          352|   
          353|         case WM_MINMAXFRAME:
          354|            pswp = PVOIDFROMMP (mp1);
          355|            if (pswp->fs & SWP_MINIMIZE) 
          356|               WinSetWindowText (hwndFrame, "EDIT");
          357|            else {
          358|               if (hasName) {
          359|                  sprintf (str, "EDIT -- %s", szFileName);			
          360|                  WinSetWindowText (hwndFrame, str);
          361|               }
          362|            }
          363|            return 0;
          364|            
          365|         case WM_INITMENU:
          366|            if (SHORT1FROMMP (mp1) == IDM_EDIT) {
          367|               /* enable Cut, Copy, or Delete only if text selected */
          368|               selmin = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYSEL, 
          369|                                       (MPARAM) MLFQS_MINSEL, (MPARAM) 0L));
          370|               selmax = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYSEL, 
          371|                                       (MPARAM) MLFQS_MAXSEL, (MPARAM) 0L));
          372|               WinSendMsg (hwndMenu, MM_SETITEMATTR,
          373|                  MPFROM2SHORT (IDM_CUT, TRUE),
          374|                  MPFROM2SHORT (MIA_DISABLED, 
          375|                  (selmin == selmax) ? MIA_DISABLED : 0));
          376|               WinSendMsg (hwndMenu, MM_SETITEMATTR,
edit.c                                                                  Page 10

          377|                  MPFROM2SHORT (IDM_COPY, TRUE),
          378|                  MPFROM2SHORT (MIA_DISABLED, 
          379|                  (selmin == selmax) ? MIA_DISABLED : 0));
          380|               WinSendMsg (hwndMenu, MM_SETITEMATTR,
          381|                  MPFROM2SHORT (IDM_DELETE, TRUE),
          382|                  MPFROM2SHORT (MIA_DISABLED, 
          383|                  (selmin == selmax) ? MIA_DISABLED : 0));
          384|                     
          385|               /* enable Paste only if data available in Clipboard */
          386|               WinSendMsg (hwndMenu, MM_SETITEMATTR,
          387|                  MPFROM2SHORT (IDM_PASTE, TRUE),
          388|                  MPFROM2SHORT (MIA_DISABLED,
          389|                  WinQueryClipbrdFmtInfo (hab, CF_TEXT, &usfInfo)
          390|                  ? 0 : MIA_DISABLED));
          391|                        
          392|               /* enable Undo only if operation may be undone */
          393|               undoable = SHORT1FROMMR (WinSendMsg (hwndMLE, MLM_QUERYUNDO, 
          394|                                           (MPARAM) 0L, (MPARAM) 0L));
          395|               WinSendMsg (hwndMenu, MM_SETITEMATTR,
          396|                  MPFROM2SHORT (IDM_UNDO, TRUE),
          397|                  MPFROM2SHORT (MIA_DISABLED, (undoable || undone) 
          398|                                              ? 0 : MIA_DISABLED)); 
          399|            }
          400|            return 0;
          401|   
          402|         case WM_TIMER:
          403|            if (SHORT1FROMMP (mp1) == ID_UPDATE) {
          404|               /* determine position (line/column) of text cursor (caret) */
          405|               selmin = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYSEL, 
          406|                                       (MPARAM) MLFQS_MINSEL, (MPARAM) 0L));
          407|               newline = (int) LONGFROMMR (WinSendMsg (hwndMLE, 
          408|                                              MLM_LINEFROMCHAR,
          409|                                              MPFROMLONG (selmin), 
          410|                                              (MPARAM) 0L));
          411|               newcolumn = (int) (selmin - LONGFROMMR (WinSendMsg (hwndMLE, 
          412|                                              MLM_CHARFROMLINE,
          413|                                              MPFROMLONG ((long) newline), 
          414|                                              (MPARAM) 0L)));
          415|            
          416|               /* update on screen only if line or column changed */
          417|               if (newline != line || newcolumn != column) {
          418|                  line = newline;   column = newcolumn;
          419|                  WinInvalidateRect (hwnd, NULL, FALSE);
          420|               }
          421|            }
          422|            return 0;
          423|   
          424|         case WM_CONTROL:
          425|            switch (SHORT2FROMMP (mp1)) {
          426|               case MLN_OVERFLOW:
          427|                  if (SHORT1FROMMP (mp1) == ID_MLE) {
          428|                     WinMessageBox (HWND_DESKTOP, hwnd, 
          429|                        "File too large -- 64K limit exceeded.",
          430|                        "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
          431|                  }
edit.c                                                                  Page 11

          432|                  return 0;
          433|   
          434|               case MLN_CHANGE:
          435|                  if (SHORT1FROMMP (mp1) == ID_MLE) {
          436|                     NeedToSave = TRUE;
          437|                     undone = FALSE;
          438|                  }
          439|                  return 0;
          440|               
          441|               default:
          442|                  break;
          443|            }
          444|            break;
          445|       
          446|         case WM_CLEANFILE:
          447|            WinSendMsg (hwndMLE, MLM_RESETUNDO, (MPARAM) 0L, (MPARAM) 0L);
          448|            NeedToSave = FALSE;
          449|            return 0;
          450|         
          451|         case WM_ARGS:
          452|            /* read file into buffer */			
          453|            bufferlen = ReadFile (szFileName, &buffer);			 
          454|            if (bufferlen == CANTREAD) {			 
          455|               WinMessageBox (HWND_DESKTOP, hwnd, 
          456|                  "Specified file does not exist.",
          457|                  "New File", 0, MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
          458|               hasName = TRUE;
          459|               sprintf (str, "EDIT -- %s", szFileName);			
          460|               WinSetWindowText (hwndFrame, str);			 
          461|               NeedToSave = TRUE;
          462|            }			 
          463|            else if (bufferlen == TOOLONG) {			  
          464|               WinMessageBox (HWND_DESKTOP, hwnd, 
          465|                  "File too large -- 64K limit exceeded.",
          466|                  "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
          467|               WinSetWindowText (hwndFrame, "EDIT");
          468|            }			 
          469|            else if (bufferlen == NOMEMORY) {			
          470|               WinMessageBox (HWND_DESKTOP, hwnd, 
          471|                  "Cannot allocate memory.",
          472|                  "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
          473|               WinSendMsg (hwnd, WM_QUIT, (MPARAM) 0L, (MPARAM) 0L);
          474|            }			 
          475|            else {	/* normal */
          476|               /* transfer buffer to MLE */
          477|               WinSendMsg (hwndMLE, MLM_SETIMPORTEXPORT,
          478|                  MPFROMP (buffer),
          479|                  MPFROMSHORT ((USHORT)bufferlen));			
          480|   				selmin = 0L;
          481|               WinSendMsg (hwndMLE, MLM_IMPORT,
          482|                  MPFROMP (&selmin), MPFROMLONG (bufferlen));			
          483|   				/* free buffer */
          484|               Release (buffer);	
          485|               hasName = TRUE;
          486|               sprintf (str, "EDIT -- %s", szFileName);			
edit.c                                                                  Page 12

          487|               WinSetWindowText (hwndFrame, str);			 
          488|               WinPostMsg (hwnd, WM_CLEANFILE, (MPARAM) 0L, (MPARAM) 0L);
          489|            }			 
          490|            return 0;
          491|   
          492|         case WM_COMMAND:
          493|            switch (SHORT1FROMMP (mp1)) {
          494|               case IDM_NEW:
          495|                  if (NeedToSave) {
          496|                      res = WinMessageBox (HWND_DESKTOP, hwnd,
          497|                               "Save current file?", "New", 0,
          498|                                MB_YESNOCANCEL | MB_ICONQUESTION | MB_MOVEABLE);
          499|                                   
          500|                     if (res == MBID_YES) {
          501|                        WinSendMsg (hwnd, WM_COMMAND, 
          502|                           MPFROM2SHORT (IDM_SAVE, 0), (MPARAM) 0L);
          503|                     }
          504|                     else if (res == MBID_CANCEL) {
          505|                        resNew = MBID_CANCEL;
          506|                        return 0;
          507|                     }
          508|                  }
          509|                  SetPtrWait();
          510|                     
          511|                  /* empty MLE */
          512|                  selmin = 0L;
          513|                  selmax = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYTEXTLENGTH,
          514|                                          (MPARAM) 0L, (MPARAM) 0L));
          515|                  WinSendMsg (hwndMLE, MLM_DELETE, 
          516|                     MPFROMLONG (selmin), MPFROMLONG (selmax));
          517|                              
          518|                  hasName = FALSE;
          519|                  WinSetWindowText (hwndFrame, "EDIT");
          520|                  WinPostMsg (hwnd, WM_CLEANFILE, (MPARAM) 0L, (MPARAM) 0L);
          521|                  
          522|                  SetPtrArrow();
          523|                  resNew = MBID_OK;
          524|                  return 0;
          525|                  
          526|               case IDM_OPEN:
          527|                  WinSendMsg (hwnd, WM_COMMAND, 
          528|                     MPFROMSHORT (IDM_NEW), (MPARAM) 0L);
          529|                  if (resNew == MBID_CANCEL)
          530|                     return 0;
          531|                     
          532|                  if (WinDlgBox (HWND_DESKTOP, hwnd, OpenDlgProc,
          533|                         0, IDD_OPEN, NULL)) {
          534|                                 
          535|                     SetPtrWait();
          536|                        
          537|                     /* user selected a (valid) file to open */
          538|                     /* read file into buffer */
          539|                     bufferlen = ReadFile (szFileName, &buffer);
          540|                     
          541|                     SetPtrArrow();
edit.c                                                                  Page 13

          542|                        
          543|                     if (bufferlen == CANTREAD) {
          544|                        WinMessageBox (HWND_DESKTOP, hwnd, 
          545|                           "Specified file does not exist.",
          546|                           "New File", 0, MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
          547|                        hasName = TRUE;
          548|                        sprintf (str, "EDIT -- %s", szFileName);			
          549|                        WinSetWindowText (hwndFrame, str);			 
          550|                        NeedToSave = TRUE;
          551|                     }
          552|                     else if (bufferlen == TOOLONG) {
          553|                        WinMessageBox (HWND_DESKTOP, hwnd, 
          554|                           "File too large -- 64K limit exceeded.",
          555|                           "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
          556|                        WinSetWindowText (hwndFrame, "EDIT");
          557|                        NeedToSave = FALSE;
          558|                     }
          559|                     else if (bufferlen == NOMEMORY) {
          560|                        WinMessageBox (HWND_DESKTOP, hwnd, 
          561|                           "Cannot allocate memory.",
          562|                           "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
          563|                        WinSendMsg (hwnd, WM_QUIT, (MPARAM) 0L, (MPARAM) 0L);
          564|                     }
          565|                     else {   /* normal */
          566|                        SetPtrWait();
          567|                           
          568|                        /* transfer buffer to MLE */
          569|                        WinSendMsg (hwndMLE, MLM_SETIMPORTEXPORT, 
          570|                           MPFROMP (buffer), MPFROMSHORT ((USHORT)bufferlen));
          571|                        selmin = 0L;            
          572|                        WinSendMsg (hwndMLE, MLM_IMPORT, 
          573|                           MPFROMP (&selmin), MPFROMLONG (bufferlen));
          574|                        /* free buffer */
          575|                        Release (buffer);
          576|                        hasName = TRUE;
          577|                        sprintf (str, "EDIT -- %s", szFileName);
          578|                        WinSetWindowText (hwndFrame, str);
          579|                        WinPostMsg (hwnd, WM_CLEANFILE, (MPARAM) 0L, (MPARAM) 0L);
          580|                        
          581|                        SetPtrArrow();
          582|                     }
          583|                  } 
          584|                  return 0;
          585|          
          586|               case IDM_SAVE:
          587|                  if (hasName) {
          588|                     /* determine amount of text in MLE */
          589|                     bufferlen = LONGFROMMR (WinSendMsg (hwndMLE, 
          590|                                                MLM_QUERYFORMATTEXTLENGTH, 
          591|                                                (MPARAM) 0L, MPFROMLONG (-1L)));
          592|                                       
          593|                     /* allocate space for buffer */
          594|                     if (NOMEMORY == MakeWriteBuffer (bufferlen, &buffer)) {
          595|                        WinMessageBox (HWND_DESKTOP, hwnd, 
          596|                           "Cannot allocate memory.",
edit.c                                                                  Page 14

          597|                           "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
          598|                        WinSendMsg (hwnd, WM_QUIT, (MPARAM) 0L, (MPARAM) 0L);
          599|                     }
          600|                     SetPtrWait();
          601|                        
          602|                     /* transfer text from MLE to buffer */
          603|                     WinSendMsg (hwndMLE, MLM_SETIMPORTEXPORT, 
          604|                        MPFROMP (buffer), 
          605|                        MPFROMSHORT ((USHORT)bufferlen));
          606|                     selmin = 0L;   selmax = bufferlen;
          607|                     WinSendMsg (hwndMLE, MLM_EXPORT, 
          608|                        MPFROMP (&selmin), MPFROMP (&selmax));
          609|   
          610|                     /* write to file */
          611|                     if (CANTWRITE == WriteFile (szFileName, bufferlen, buffer)) {
          612|                        WinMessageBox (HWND_DESKTOP, hwnd, 
          613|                           "Unable to write to file.",
          614|                           "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
          615|                     }
          616|                                 
          617|                     /* deallocate buffer */
          618|                     Release (buffer);
          619|                     WinPostMsg (hwnd, WM_CLEANFILE, (MPARAM) 0L, (MPARAM) 0L);
          620|                     
          621|                     SetPtrArrow();
          622|                  }
          623|                  else {
          624|                     WinSendMsg (hwnd, WM_COMMAND, 
          625|                        MPFROM2SHORT (IDM_SAVEAS, 0), (MPARAM) 0L);
          626|                  } 
          627|                  return 0;
          628|                  
          629|               case IDM_SAVEAS:
          630|                  if (WinDlgBox (HWND_DESKTOP, hwnd, SaveasDlgProc,
          631|                         0, IDD_SAVEAS, NULL)) {
          632|                     hasName = TRUE;
          633|                     sprintf (str, "EDIT -- %s", szFileName);
          634|                     WinSetWindowText (hwndFrame, str);
          635|                     WinSendMsg (hwnd, WM_COMMAND, 
          636|                        MPFROM2SHORT (IDM_SAVE, 0), (MPARAM) 0L);
          637|                  }
          638|                  return 0;
          639|               
          640|               case IDM_EXIT:
          641|                  WinSendMsg (hwnd, WM_CLOSE, (MPARAM) 0L, (MPARAM) 0L);
          642|                  return 0;
          643|                  
          644|               case IDM_UNDO:
          645|                  WinSendMsg (hwndMLE, MLM_UNDO, (MPARAM) 0L, (MPARAM) 0L);
          646|                  undone = TRUE;
          647|                  return 0;
          648|                  
          649|               case IDM_CUT:
          650|                  WinSendMsg (hwndMLE, MLM_CUT, (MPARAM) 0L, (MPARAM) 0L);
          651|                  return 0;
edit.c                                                                  Page 15

          652|               
          653|               case IDM_COPY:
          654|                  WinSendMsg (hwndMLE, MLM_COPY, (MPARAM) 0L, (MPARAM) 0L);
          655|                  return 0;
          656|                  
          657|               case IDM_PASTE:
          658|                  WinSendMsg (hwndMLE, MLM_PASTE, (MPARAM) 0L, (MPARAM) 0L);
          659|                  return 0;
          660|                  
          661|               case IDM_DELETE:
          662|                  WinSendMsg (hwndMLE, MLM_CLEAR, (MPARAM) 0L, (MPARAM) 0L);
          663|                  return 0;  
          664|               
          665|               case IDM_FIND:
          666|                  if (DID_OK == WinDlgBox (HWND_DESKTOP, hwnd, FindDlgProc,
          667|                                   0, IDD_FIND, NULL)) {
          668|                     SetPtrWait();
          669|         
          670|                     SearchData.cb = sizeof (MLE_SEARCHDATA);
          671|                     SearchData.pchFind = szFind;
          672|                     SearchData.cchFind = strlen (szFind);
          673|                     SearchData.iptStart = (-1L);
          674|                     SearchData.iptStop = (-1L);
          675|         
          676|                     res = SHORT1FROMMR (WinSendMsg (hwndMLE, MLM_SEARCH, 
          677|                                             MPFROMLONG (MLFSEARCH_SELECTMATCH),
          678|                                             MPFROMP (&SearchData)));
          679|         
          680|                     SetPtrArrow();
          681|         
          682|                     if (!res) {
          683|                        WinMessageBox (HWND_DESKTOP, hwnd, 
          684|                           "Search string not found.", "Find", 0, 
          685|                           MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
          686|                     }
          687|                  }
          688|                  return 0;
          689|                  
          690|               case IDM_REPLACE:
          691|                  first = TRUE;
          692|                  res = WinDlgBox (HWND_DESKTOP, hwnd, ReplaceDlgProc,
          693|                           0, IDD_REPLACE, (PVOID)(&first));
          694|                  first = FALSE;
          695|                  for (;;) {
          696|                     if (res == DID_CANCEL)
          697|                        break;
          698|                     else if (res == DID_OK) {                                   
          699|                        SetPtrWait();
          700|                           
          701|                        SearchData.cb = sizeof (MLE_SEARCHDATA);
          702|                        SearchData.pchFind = szFind;
          703|                        SearchData.cchFind = strlen (szFind);
          704|                        SearchData.iptStart = (-1L);
          705|                        SearchData.iptStop = (-1L);
          706|                        
edit.c                                                                  Page 16

          707|                        res = SHORT1FROMMR (WinSendMsg (hwndMLE, MLM_SEARCH, 
          708|                                               MPFROMLONG (MLFSEARCH_SELECTMATCH),
          709|                                               MPFROMP (&SearchData)));
          710|                                          
          711|                        SetPtrArrow();
          712|                           
          713|                        if (!res) {                  
          714|                           WinMessageBox (HWND_DESKTOP, hwnd, 
          715|                              "Search string not found.", "Replace", 0,
          716|                               MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
          717|                           break;   /* exit search/replace */
          718|                        }
          719|                     }
          720|                     else if (res == DID_DOREPLACE)
          721|                        WinSendMsg (hwndMLE, MLM_INSERT, 
          722|                           MPFROMP (szReplace), (MPARAM) 0L);
          723|                     else if (res == DID_REPLACEALL) {
          724|                        SetPtrWait();
          725|                           
          726|                        WinSendMsg (hwndMLE, MLM_INSERT, 
          727|                           MPFROMP (szReplace), (MPARAM) 0L);
          728|                                    
          729|                        SearchData.cb = sizeof (MLE_SEARCHDATA);
          730|                        SearchData.pchFind = szFind;
          731|                        SearchData.cchFind = strlen (szFind);
          732|                        SearchData.pchReplace = szReplace;
          733|                        SearchData.cchReplace = strlen (szReplace);
          734|                        SearchData.iptStart = (-1L);
          735|                        SearchData.iptStop = (-1L);
          736|                        WinSendMsg (hwndMLE, MLM_SEARCH, 
          737|                           MPFROMLONG (MLFSEARCH_CHANGEALL), 
          738|                           MPFROMP (&SearchData));
          739|                                    
          740|                        SetPtrArrow();
          741|                           
          742|                        WinMessageBox (HWND_DESKTOP, hwnd, 
          743|                           "All occurrences replaced.", "Replace", 0,
          744|                           MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
          745|                        break;   /* exit search/replace */
          746|                     }
          747|                     else
          748|                        break;   /* exit search/replace */
          749|                        
          750|                     res = WinDlgBox (HWND_DESKTOP, hwnd, ReplaceDlgProc,
          751|                              0, IDD_REPLACE, (PVOID)(&first));
          752|                  }
          753|                  return 0;
          754|   
          755|               case IDM_GO:
          756|                     res = WinDlgBox (HWND_DESKTOP, hwnd, GoLnDlgProc,
          757|                              0, IDD_GOLINE, NULL);
          758|                     if (res == MBID_OK) {
          759|                        if (sscanf (szLine, "%ld", &goline)) {
          760|                           if (goline < 1)
          761|                              goline = 1;
edit.c                                                                  Page 17

          762|                           gochar = LONGFROMMR (WinSendMsg (hwndMLE, 
          763|                                                    MLM_CHARFROMLINE, 
          764|                                                    MPFROMLONG (--goline), 
          765|                                                    (MPARAM) 0L));
          766|                           WinSendMsg (hwndMLE, MLM_SETFIRSTCHAR, 
          767|                              MPFROMLONG (gochar), (MPARAM) 0L);
          768|                           ps.x = 0;   ps.y = cyClient - (2 * menuheight);
          769|                           WinSendMsg (hwndMLE, WM_BUTTON1DOWN, 
          770|                              MPFROM2SHORT (ps.x, ps.y), (MPARAM) 0L);
          771|                           WinSendMsg (hwndMLE, WM_BUTTON1UP, 
          772|                              MPFROM2SHORT (ps.x, ps.y), (MPARAM) 0L);
          773|                        }
          774|                     }
          775|                  return 0;
          776|   
          777|               case IDM_HELPFORHELP:
          778|                  if (hwndHelpInstance)
          779|                     WinSendMsg( hwndHelpInstance, HM_DISPLAY_HELP, 0L, 0L);
          780|                  break;
          781|   
          782|               case IDM_EXTENDEDHELP:
          783|                  WinPostMsg (hwndFrame, WM_SYSCOMMAND,
          784|                     MPFROM2SHORT (SC_HELPEXTENDED, 0), (MPARAM) 0L);
          785|                  break;
          786|                  
          787|               case IDM_KEYSHELP:
          788|                  WinPostMsg (hwndFrame, WM_SYSCOMMAND,
          789|                     MPFROM2SHORT (SC_HELPKEYS, 0), (MPARAM) 0L);
          790|                  break;
          791|                  
          792|               case IDM_HELPINDEX:
          793|                  WinPostMsg (hwndFrame, WM_SYSCOMMAND,
          794|                     MPFROM2SHORT (SC_HELPINDEX, 0), (MPARAM) 0L);
          795|                  break;
          796|                  
          797|               case IDM_ABOUT:
          798|                  WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc, 
          799|                     0, IDD_ABOUT, NULL);
          800|                  return 0;
          801|                  
          802|               default:
          803|                  break;
          804|            }
          805|            break;
          806|   
          807|         case WM_SIZE:
          808|            cxClient = SHORT1FROMMP (mp2);
          809|            cyClient = SHORT2FROMMP (mp2);
          810|            
          811|            WinSetWindowPos (
          812|               hwndMLE, 
          813|               HWND_TOP, 
          814|               0, 0, 
          815|               cxClient, cyClient - menuheight, 
          816|               SWP_MOVE | SWP_SIZE);
edit.c                                                                  Page 18

          817|            WinSetFocus (HWND_DESKTOP, hwndMLE);
          818|            return 0;
          819|   
          820|         case WM_PAINT:
          821|            hps = WinBeginPaint (hwnd, NULL, NULL);
          822|            sprintf (str, "  Line: %-5d   Col: %-5d", line + 1, column + 1);
          823|            WinQueryWindowRect (hwnd, &rcl);
          824|            rcl.yBottom = cyClient - menuheight;
          825|            WinDrawText (hps, -1, str, &rcl, CLR_NEUTRAL, CLR_BACKGROUND,
          826|               DT_LEFT | DT_VCENTER | DT_ERASERECT);
          827|            WinEndPaint (hps);
          828|            return 0;
          829|      
          830|         case HM_QUERY_KEYS_HELP:
          831|            return ((MRESULT) IDH_KEYSHELP);
          832|            break;
          833|   
          834|         case HM_ERROR:
          835|            if ( (hwndHelpInstance && (ULONG) mp1) == HMERR_NO_MEMORY) {
          836|               WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
          837|                  "Help Terminated Due to Error", "Help Error", 0,
          838|                  MB_OK | MB_ICONEXCLAMATION | MB_MOVEABLE);
          839|               WinDestroyHelpInstance(hwndHelpInstance);
          840|            }
          841|            else {
          842|               WinMessageBox (HWND_DESKTOP, HWND_DESKTOP,
          843|                  "Help Error Occurred", "Help Error", 0,
          844|                  MB_OK | MB_ICONEXCLAMATION | MB_MOVEABLE);
          845|            }
          846|            break;
          847|            
          848|         case WM_DESTROY:
          849|            WinDestroyWindow (hwndMLE);
          850|            return 0;
          851|      }
          852|      return WinDefWindowProc (hwnd, msg, mp1, mp2);
          853|   }
          854|   
          855|   
          856|   MRESULT EXPENTRY TabWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
          857|   {
          858|      USHORT flags, vk;   /* WM_CHAR message parameters */
          859|      long curpos;   /* current postion -- relative to top of document */
          860|      int ln, col;   /* current position -- line/column */
          861|      char str[TAB + 1];   /* insert string of spaces for tab */
          862|      int i;   /* loop counter: will insert spaces for tab */
          863|      int ts;   /* number of spaces to insert for tab */
          864|   
          865|      switch (msg) {
          866|         case WM_CHAR:
          867|            flags = SHORT1FROMMP (mp1);
          868|            vk = SHORT2FROMMP (mp2);
          869|            if (flags & KC_VIRTUALKEY && !(flags & KC_KEYUP) && vk == VK_TAB) {
          870|               /* determine position of text cursor */
          871|               curpos = LONGFROMMR (WinSendMsg (hwnd, MLM_QUERYSEL, 
edit.c                                                                  Page 19

          872|                                       (MPARAM) MLFQS_MINSEL, (MPARAM) 0L));
          873|               ln = (int) LONGFROMMR (WinSendMsg (hwnd, MLM_LINEFROMCHAR,
          874|                                         MPFROMLONG (curpos), (MPARAM) 0L));
          875|               col = (int) (curpos - LONGFROMMR (WinSendMsg (hwnd, 
          876|                                                    MLM_CHARFROMLINE,
          877|                                                    MPFROMLONG ((long) ln), 
          878|                                                    (MPARAM) 0L)));
          879|               ts = TAB - (col % TAB); 
          880|               for (i = 0; i < ts; i++) 
          881|                  str[i] = ' ';
          882|               str[i] = '\0';
          883|               
          884|               WinSendMsg (hwnd, MLM_INSERT, MPFROMP (str), (MPARAM) 0L);
          885|               return 0;
          886|            }
          887|            else
          888|               break;
          889|   
          890|         default:
          891|            break;
          892|      }
          893|      pfMLE (hwnd, msg, mp1, mp2);
          894|   }
          895|   
          896|   
          897|   VOID SetPtrArrow (VOID)
          898|   {
          899|      if (!WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT))
          900|         WinShowPointer (HWND_DESKTOP, FALSE);
          901|   
          902|      WinSetPointer (HWND_DESKTOP,
          903|         WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, 0));
          904|   }
          905|   
          906|   
          907|   VOID SetPtrWait (VOID)
          908|   {
          909|      WinSetPointer (HWND_DESKTOP,
          910|         WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, 0));
          911|   
          912|      if (!WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT))
          913|         WinShowPointer (HWND_DESKTOP, TRUE);
          914|   }
          915|   
