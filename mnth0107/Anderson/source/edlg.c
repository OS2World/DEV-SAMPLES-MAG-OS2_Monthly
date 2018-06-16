#define INCL_WINHELP
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "edit.h"
#include "edlg.h"

/* window procedure for about box */
MRESULT EXPENTRY AboutDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg) {
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1)) {
            case DID_OK:
            case DID_CANCEL:
               WinDismissDlg (hwnd, TRUE);
               return 0;
            default:
               break;
         }
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}


/* fill directory box with subdirectory names */
/* by Charles Petzold, from Programming the OS/2 Presentation Manager */
VOID FillDirListBox (HWND hwnd, CHAR *pcCurrentPath)
{
   static CHAR szDrive[] = "  :";
   FILEFINDBUF findbuf;
   HDIR hDir = 1;
   SHORT sDrive;
   USHORT usDriveNum, usCurPathLen, usSearchCount = 1;
   ULONG ulDriveMap;

   DosQCurDisk (&usDriveNum, &ulDriveMap);
   pcCurrentPath[0] = (CHAR)((CHAR) usDriveNum + '@');
   pcCurrentPath[1] = ':';
   pcCurrentPath[2] = '\\';
   usCurPathLen = 64;
   DosQCurDir (0, pcCurrentPath + 3, &usCurPathLen);

   WinSetDlgItemText (hwnd, DID_PATH, pcCurrentPath);
   WinSendDlgItemMsg (hwnd, DID_DIRLIST, LM_DELETEALL, NULL, NULL);

   for (sDrive = 0 ; sDrive < 26 ; sDrive++) {
      if (ulDriveMap & 1L << sDrive) {
         szDrive[1] = (CHAR)((CHAR) sDrive + 'A');

         WinSendDlgItemMsg (hwnd, DID_DIRLIST, LM_INSERTITEM,
            MPFROM2SHORT (LIT_END, 0), MPFROMP (szDrive));
      }
   }

   DosFindFirst ("*.*", &hDir, 0x0010, &findbuf, sizeof findbuf,
      &usSearchCount, 0L);
   while (usSearchCount) {
      if (findbuf.attrFile & 0x0010 &&
       (findbuf.achName[0] != '.' || findbuf.achName[1])) {
         WinSendDlgItemMsg (hwnd, DID_DIRLIST, LM_INSERTITEM,
            MPFROM2SHORT (LIT_SORTASCENDING, 0), MPFROMP (findbuf.achName));
      }                            
      DosFindNext (hDir, &findbuf, sizeof findbuf, &usSearchCount);
   }
}


/* fill file box with filenames */
/* by Charles Petzold, from Programming the OS/2 Presentation Manager */
VOID FillFileListBox (HWND hwnd)
{
   FILEFINDBUF findbuf;
   HDIR hDir = 1;
   USHORT usSearchCount = 1;

   WinSendDlgItemMsg (hwnd, DID_FILELIST, LM_DELETEALL, NULL, NULL);

   DosFindFirst ("*.*", &hDir, 0x0000, &findbuf, sizeof findbuf,
      &usSearchCount, 0L);
   while (usSearchCount) {
      WinSendDlgItemMsg (hwnd, DID_FILELIST, LM_INSERTITEM,
         MPFROM2SHORT (LIT_SORTASCENDING, 0),
         MPFROMP (findbuf.achName));

      DosFindNext (hDir, &findbuf, sizeof findbuf, &usSearchCount);
   }
}


/* window procedure for open file dialog box */
/* by Charles Petzold, from Programming the OS/2 Presentation Manager */
MRESULT EXPENTRY OpenDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   static CHAR szCurrentPath[80], szBuffer[80];
   SHORT sSelect;

   switch (msg) {
      case WM_INITDLG:
         FillDirListBox (hwnd, szCurrentPath);
         FillFileListBox (hwnd);

         WinSendDlgItemMsg (hwnd, DID_FILEEDIT, EM_SETTEXTLIMIT,
            MPFROM2SHORT (70, 0), NULL);
         return 0;

      case WM_CONTROL:
         if (SHORT1FROMMP (mp1) == DID_DIRLIST ||
          SHORT1FROMMP (mp1) == DID_FILELIST) {
            sSelect = SHORT1FROMMR (WinSendDlgItemMsg (hwnd,
                                       SHORT1FROMMP (mp1),
                                       LM_QUERYSELECTION, 
                                       (MPARAM) 0L, (MPARAM) 0L));

            WinSendDlgItemMsg (hwnd, SHORT1FROMMP (mp1), LM_QUERYITEMTEXT,
               MPFROM2SHORT (sSelect, sizeof szBuffer), MPFROMP (szBuffer));
         }

         switch (SHORT1FROMMP (mp1)) {             /* Control ID */
            case DID_DIRLIST:
               switch (SHORT2FROMMP (mp1)) {   /* notification code */
                  case LN_ENTER:
                     if (szBuffer[0] == ' ')
                        DosSelectDisk (szBuffer[1] - '@');
                     else
          				   DosChDir (szBuffer, 0L);

                        FillDirListBox (hwnd, szCurrentPath);
                        FillFileListBox (hwnd);

                        WinSetDlgItemText (hwnd, DID_FILEEDIT, "");
                        return 0;
               }
               break;

            case DID_FILELIST:
               switch (SHORT2FROMMP (mp1)) {   /* notification code */
                  case LN_SELECT:
                     WinSetDlgItemText (hwnd, DID_FILEEDIT, szBuffer);
                     return 0;

                  case LN_ENTER:
                     ParseFileName (szFileName, szBuffer);
                     WinDismissDlg (hwnd, TRUE);
                     return 0;
               }
               break;
         }
         break;

      case WM_COMMAND:
         switch (COMMANDMSG(&msg)->cmd) {
            case DID_OK:
               WinQueryDlgItemText (hwnd, DID_FILEEDIT,
                  sizeof szBuffer, szBuffer);

               switch (ParseFileName (szCurrentPath, szBuffer)) {
                  case 0:
                     WinAlarm (HWND_DESKTOP, WA_ERROR);
                     FillDirListBox (hwnd, szCurrentPath);
                     FillFileListBox (hwnd);
                     return 0;

                  case 1:
                     WinAlarm (HWND_DESKTOP, WA_NOTE);
                     FillDirListBox (hwnd, szCurrentPath);
                     FillFileListBox (hwnd);
                     WinSetDlgItemText (hwnd, DID_FILEEDIT, "");
                     return 0;

                  case 2:
                     strcpy (szFileName, szCurrentPath);
                     WinDismissDlg (hwnd, TRUE);
                     return 0;
               }
               break;

               case DID_CANCEL:
                  WinDismissDlg (hwnd, FALSE);
                  return 0;
         }
         break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}


/* window procedure for save as dialog box */
/* by Charles Petzold, from Programming the OS/2 Presentation Manager */
/* Modified (from OpenDlgProc) by Brian R. Anderson */
MRESULT EXPENTRY SaveasDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   static CHAR szCurrentPath[80], szBuffer[80];
   SHORT sSelect;

   switch (msg) {
      case WM_INITDLG:
         FillDirListBox (hwnd, szCurrentPath);

         WinSendDlgItemMsg (hwnd, DID_FILEEDIT, EM_SETTEXTLIMIT,
            MPFROM2SHORT (70, 0), NULL);
         return 0;

      case WM_CONTROL:
         if (SHORT1FROMMP (mp1) == DID_DIRLIST) {
            sSelect = SHORT1FROMMR (WinSendDlgItemMsg (hwnd,
                                       SHORT1FROMMP (mp1),
                                       LM_QUERYSELECTION, 
                                       (MPARAM) 0L, (MPARAM) 0L));

            WinSendDlgItemMsg (hwnd, SHORT1FROMMP (mp1), LM_QUERYITEMTEXT,
               MPFROM2SHORT (sSelect, sizeof szBuffer), MPFROMP (szBuffer));
         }

         switch (SHORT1FROMMP (mp1)) {             /* Control ID */
            case DID_DIRLIST:
               switch (SHORT2FROMMP (mp1)) {   /* notification code */
                  case LN_ENTER:
                     if (szBuffer[0] == ' ')
                        DosSelectDisk (szBuffer[1] - '@');
                     else
          				   DosChDir (szBuffer, 0L);

                        FillDirListBox (hwnd, szCurrentPath);
                        FillFileListBox (hwnd);

                        WinSetDlgItemText (hwnd, DID_FILEEDIT, "");
                        return 0;
               }
               break;
         }
         break;

      case WM_COMMAND:
         switch (COMMANDMSG(&msg)->cmd) {
            case DID_OK:
               WinQueryDlgItemText (hwnd, DID_FILEEDIT,
                  sizeof szBuffer, szBuffer);

               switch (ParseFileName (szCurrentPath, szBuffer)) {
                  case 0:
                     WinAlarm (HWND_DESKTOP, WA_ERROR);
                     FillDirListBox (hwnd, szCurrentPath);
                     FillFileListBox (hwnd);
                     return 0;

                  case 1:
                     WinAlarm (HWND_DESKTOP, WA_NOTE);
                     FillDirListBox (hwnd, szCurrentPath);
                     WinSetDlgItemText (hwnd, DID_FILEEDIT, "");
                     return 0;

                  case 2:
                     strcpy (szFileName, szCurrentPath);
                     WinDismissDlg (hwnd, TRUE);
                     return 0;
               }
               break;

            case DID_CANCEL:
               WinDismissDlg (hwnd, FALSE);
               return 0;
         }
         break;
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}


/* determine if pathname is a valid file or directory */
/* by Charles Petzold, from Programming the OS/2 Presentation Manager */
SHORT ParseFileName (CHAR *pcOut, CHAR *pcIn)
{
/*
 *   Input:    pcOut -- Pointer to parsed file specification.
 *             pcIn  -- Pointer to raw file specification.
 *                      
 *   Returns:      0 -- pcIn had invalid drive or directory.
 *                 1 -- pcIn was empty or had no filename.
 *                 2 -- pcOut points to drive, full dir, and file name.
 *
 *   Changes current drive and directory per pcIn string.
 */

   CHAR *pcLastSlash, *pcFileOnly;
   ULONG ulDriveMap;
   USHORT usDriveNum, usDirLen = 64;

   strupr (pcIn);

   /* If input string is empty, return 1 */
   if (pcIn[0] == '\0')
      return 1;

   /* Get drive from input string or current drive */
   if (pcIn[1] == ':') {
      if (DosSelectDisk (pcIn[0] - '@'))
         return 0;

      pcIn += 2;
   }
   DosQCurDisk (&usDriveNum, &ulDriveMap);

   *pcOut++ = (CHAR)((CHAR) usDriveNum + '@');
   *pcOut++ = ':';
   *pcOut++ = '\\';

   /* If rest of string is empty, return 1 */
   if (pcIn[0] == '\0')
      return 1;

   /* Search for last backslash.  If none, could be directory. */
   if (NULL == (pcLastSlash = strrchr (pcIn, '\\'))) {
	   if (!DosChDir (pcIn, 0L))
         return 1;

      /* Otherwise, get current dir & attach input filename */
      DosQCurDir (0, pcOut, &usDirLen);

      if (strlen (pcIn) > 12)
         return 0;

      if (*(pcOut + strlen (pcOut) - 1) != '\\')
         strcat (pcOut++, "\\");

      strcat (pcOut, pcIn);
      return 2;
   }
   
   /* If the only backslash is at beginning, change to root */
   if (pcIn == pcLastSlash) {
	   DosChDir ("\\", 0L);

      if (pcIn[1] == '\0')
         return 1;

      strcpy (pcOut, pcIn + 1);
      return 2;
   }
   
   /* Attempt to change directory -- Get current dir if OK */
   *pcLastSlash = '\0';

   if (DosChDir (pcIn, 0L))
      return 0;

   DosQCurDir (0, pcOut, &usDirLen);

   /* Append input filename, if any */
   pcFileOnly = pcLastSlash + 1;

   if (*pcFileOnly == '\0')
      return 1;

   if (strlen (pcFileOnly) > 12)
      return 0;

   if (*(pcOut + strlen (pcOut) - 1) != '\\')
      strcat (pcOut++, "\\");

   strcat (pcOut, pcFileOnly);
   return 2;
}


/* window procedure for find (text search) dialog box */
MRESULT EXPENTRY FindDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg) {
      case WM_INITDLG:
        WinSendDlgItemMsg (hwnd, DID_FINDTEXT, EM_SETTEXTLIMIT,
           MPFROM2SHORT (50, 0), NULL);
         WinSetDlgItemText (hwnd, DID_FINDTEXT, szFind);
         WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_FINDTEXT));
         return (MRESULT) TRUE;
         
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1)) {
            case DID_OK:
               if (WinQueryDlgItemText (hwnd, DID_FINDTEXT, 60, szFind))
                  WinDismissDlg (hwnd, DID_OK);
               else
                  WinDismissDlg (hwnd, DID_CANCEL);
               return 0;
               
            case DID_CANCEL:
               WinDismissDlg (hwnd, DID_CANCEL);
               return 0;
               
            default:
               break;
         }
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}


/* window procedure for replace (text search and replace) dialog box */
MRESULT EXPENTRY ReplaceDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   BOOL FAR *pb;
   
   switch (msg) {
      case WM_INITDLG:
         WinSendDlgItemMsg (hwnd, DID_NEWTEXT, EM_SETTEXTLIMIT,
            MPFROM2SHORT (50, 0), NULL);
         WinSendDlgItemMsg (hwnd, DID_OLDTEXT, EM_SETTEXTLIMIT,
            MPFROM2SHORT (50, 0), NULL);
         WinSetDlgItemText (hwnd, DID_OLDTEXT, szFind);
         WinSetDlgItemText (hwnd, DID_NEWTEXT, szReplace);
         pb = (PVOID) mp2;   /* TRUE if first entry */
         if (*pb) {
            WinEnableWindow (WinWindowFromID (hwnd, DID_DOREPLACE), FALSE);
            WinEnableWindow (WinWindowFromID (hwnd, DID_REPLACEALL), FALSE);
            WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_OLDTEXT));
         }
         else
            WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_NEWTEXT));
         return (MRESULT) TRUE;
         
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1)) {
            case DID_OK:
               if (WinQueryDlgItemText (hwnd, DID_OLDTEXT, 60, szFind)) {
                  WinQueryDlgItemText (hwnd, DID_NEWTEXT, 60, szReplace);
                  WinDismissDlg (hwnd, DID_OK);
               }
               else
                  WinDismissDlg (hwnd, DID_CANCEL);
               return 0;
               
            case DID_DOREPLACE:
               if (WinQueryDlgItemText (hwnd, DID_NEWTEXT, 60, szReplace))
                  WinDismissDlg (hwnd, DID_DOREPLACE);
               else
                  WinDismissDlg (hwnd, DID_CANCEL);
               break;
               
            case DID_REPLACEALL:
               if (WinQueryDlgItemText (hwnd, DID_NEWTEXT, 60, szReplace))
                  WinDismissDlg (hwnd, DID_REPLACEALL);
               else
                  WinDismissDlg (hwnd, DID_CANCEL);
               break;
               
            case DID_CANCEL:
               WinDismissDlg (hwnd, DID_CANCEL);
               return 0;
               
            default:
               break;
         }
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}


/* window procedure for find (text search) dialog box */
MRESULT EXPENTRY GoLnDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   switch (msg) {
      case WM_INITDLG:
         WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_LINENBR));
         return (MRESULT) TRUE;
         
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1)) {
            case DID_OK:
               if (WinQueryDlgItemText (hwnd, DID_LINENBR, 20, szLine))
                  WinDismissDlg (hwnd, DID_OK);
               else
                  WinDismissDlg (hwnd, DID_CANCEL);
               return 0;
               
            case DID_CANCEL:
               WinDismissDlg (hwnd, DID_CANCEL);
               return 0;
               
            default:
               break;
         }
   }
   return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
