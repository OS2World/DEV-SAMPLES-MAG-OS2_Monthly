/* FILES.C - File I/O routines & dialog boxes window procedures

Copyright (c) 1992 Timur Tabi
Copyright (c) 1992 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

NOTE: This code is adapted from examples in Petzold's "Programming the OS/2 Presentation Manager".
      However, this version is quite buggy.  It will be replaced by the template code provided with
      the OS/2 2.0 Toolkit.  Sorry for the inconvenience.  Next time I'll know better :-)
*/

#define INCL_WIN
#include <os2.h>
#include <string.h>
#include "dialog.h"
#include "hexes.h"

#define PATH_LEN 128

FILEFINDBUF3 ffb;
HDIR hdir=HDIR_SYSTEM;
ULONG ulSearchCount=1;
char szFileName[PATH_LEN];
char szCurrentPath[PATH_LEN];
ULONG ulCurPathLen=PATH_LEN;
extern HWND hwndClient;                       // For redrawing the screen after a Load Map

#define READ_ATTRS OPEN_FLAGS_NO_CACHE|              /* No need to take up precious cache space */ \
                   OPEN_FLAGS_SEQUENTIAL|            /* One-time read, remember?                */ \
                   OPEN_SHARE_DENYWRITE|             /* We don't want anyone changing it        */ \
                   OPEN_ACCESS_READONLY              // To prevent accidentally writing to it

#define WRITE_ATTRS OPEN_FLAGS_NO_CACHE|              /* No need to take up precious cache space */ \
                    OPEN_FLAGS_SEQUENTIAL|            /* One-time write, remember?               */ \
                    OPEN_SHARE_DENYREADWRITE|         /* We don't want anyone touching it        */ \
                    OPEN_ACCESS_WRITEONLY             // That's how we're gonna do it

void LoadMap(void) {
  HFILE hfile;
  ULONG ulAction;
  ULONG ulBytesRead;

  if (DosOpen(szFileName,&hfile,&ulAction,0,0,OPEN_ACTION_OPEN_IF_EXISTS|OPEN_ACTION_FAIL_IF_NEW,READ_ATTRS,0)) return;
  if (DosRead(hfile,amap,sizeof(amap),&ulBytesRead)) return;
  if (ulBytesRead != sizeof(amap)) return;
  DosClose(hfile);
}

void SaveMap(void) {
  HFILE hfile;
  ULONG ulAction;
  ULONG ulBytesRead;

  if (DosOpen(szFileName,&hfile,&ulAction,sizeof(amap),0,OPEN_ACTION_CREATE_IF_NEW|OPEN_ACTION_REPLACE_IF_EXISTS,WRITE_ATTRS,0)) return;
  if (DosWrite(hfile,amap,sizeof(amap),&ulBytesRead)) return;
  if (ulBytesRead != sizeof(amap)) return;
  DosClose(hfile);
}


#define DIR_BOX_ATTRS (MUST_HAVE_DIRECTORY|FILE_ARCHIVED|FILE_HIDDEN|FILE_READONLY)
#define FILE_BOX_ATTRS (FILE_ARCHIVED|FILE_HIDDEN|FILE_READONLY)

void FillDirListBox(HWND hwnd) {
  static char *szDrive=" :";                            // for adding drives to the list box
  static ULONG ulDriveNum, ulDriveMap;
  BYTE bDrive;

//
  DosQueryCurrentDisk(&ulDriveNum,&ulDriveMap);
  szCurrentPath[0]=(char) (ulDriveNum + '@');
  szCurrentPath[1]=':';
  szCurrentPath[2]='\\';
  ulCurPathLen=PATH_LEN;
  DosQueryCurrentDir(0,szCurrentPath+3,&ulCurPathLen);
  WinSetDlgItemText(hwnd,IDD_PATH,szCurrentPath);               // Show current path on dialog box

  WinSendDlgItemMsg(hwnd,IDD_DIRLIST,LM_DELETEALL,NULL,NULL);   // Erase all entries

// Add the list of directories in the current dir to the list box
  ulSearchCount=1;
  if (!DosFindFirst("*",&hdir,DIR_BOX_ATTRS,&ffb,sizeof(ffb),&ulSearchCount,FIL_STANDARD))
    do
      WinSendDlgItemMsg(hwnd,IDD_DIRLIST,LM_INSERTITEM,MPFROM2SHORT(LIT_SORTASCENDING,0),MPFROMP(ffb.achName));
    while (!DosFindNext(hdir,&ffb,sizeof(ffb),&ulSearchCount));

// Add the list of available drives to the list box
  for (bDrive=0; bDrive<26; bDrive++)
    if (ulDriveMap & 1L << bDrive) {
      szDrive[0]=(char) (bDrive + 'A');
      WinSendDlgItemMsg(hwnd,IDD_DIRLIST,LM_INSERTITEM,MPFROM2SHORT(LIT_END,0),MPFROMP(szDrive));
    }
}

void FillFileListBox(HWND hwnd) {
  WinSendDlgItemMsg(hwnd,IDD_FILELIST,LM_DELETEALL,NULL,NULL);

  ulSearchCount=1;
  if (!DosFindFirst("*",&hdir,FILE_BOX_ATTRS,&ffb,sizeof(ffb),&ulSearchCount,FIL_STANDARD))
    do
      WinSendDlgItemMsg(hwnd,IDD_FILELIST,LM_INSERTITEM,MPFROM2SHORT(LIT_SORTASCENDING,0),MPFROMP(ffb.achName));
    while (!DosFindNext(hdir,&ffb,sizeof(ffb),&ulSearchCount));
}

SHORT ParseFileName(char *szOut, char *szIn) {
/*
This routine parses szIn and returns:
        0 - invalid string
        1 - new drive/directory only
        2 - valid filename
szOut contains the new directory and/or filename.
*/
  char *pcLastSlash,*pcFileOnly;
  ULONG ulDriveMap,ulDriveNum,ulDirLen = PATH_LEN;

  if (*szIn == 0) return 1;

// If szIn contains a drive specifier, change the current disk first
  if (szIn[1] == ':') {
    if (DosSelectDisk((ULONG) (*szIn - '@'))) return 0;
    szIn += 2;                                // skip over drive specifier
  }

// Get the current disk and drive map data.  The drive map data is ignored
  DosQueryCurrentDisk(&ulDriveNum,&ulDriveMap);
  *szOut++ = (char) (ulDriveNum + '@');
  *szOut++ = ':';
  *szOut++ = '\\';

  if (*szIn == 0) return 1;

  if (NULL == (pcLastSlash = strrchr(szIn,'\\'))) {
    if (!DosSetCurrentDir(szIn)) return 1;
    DosQueryCurrentDir(0,szOut,&ulDirLen);
    if (*(szOut + strlen(szOut) - 1) != '\\') strcat(szOut++,"\\");
    strcat(szOut,szIn);
    return 2;
  }

  if (szIn == pcLastSlash) {
    DosSetCurrentDir("\\");
    if (szIn[1] == 0) return 1;                   // if szIn is only a backslash, change to root
    strcpy(szOut,szIn+1);
    return 2;
  }

  *pcLastSlash=0;
  if (DosSetCurrentDir(szIn)) return 0;
  DosQueryCurrentDir(0,szOut,&ulDirLen);
  pcFileOnly=pcLastSlash+1;
  if (*pcFileOnly == 0) return 1;
  if (*(szOut + strlen(szOut) - 1) != '\\') strcat(szOut++,"\\");
  strcat(szOut,pcFileOnly);
  return 2;
}

MRESULT EXPENTRY LoadDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
  static char szBuffer[80];
  USHORT usSelect;

  switch (msg) {
    case WM_INITDLG:                            // Fill the list boxes
      FillDirListBox(hwnd);
      FillFileListBox(hwnd);
//    WinSendDlgItemMsg(hwnd,IDD_FILEEDIT,EM_SETTEXTLIMIT,MPFROM2SHORT(80,0),NULL);
      return 0;

    case WM_CONTROL:                            // User has selected a file/dir
      if (SHORT1FROMMP(mp1) == IDD_DIRLIST || SHORT1FROMMP(mp1) == IDD_FILELIST) {
// The following line returns -1, indicating an error code.  The result is that the user cannot
// select the file from the list box.
        usSelect = (USHORT) WinSendDlgItemMsg(hwnd,SHORT1FROMMP(mp1),LM_QUERYSELECTION,0L,0L);
        WinSendDlgItemMsg(hwnd,SHORT1FROMMP(mp1),LM_QUERYITEMTEXT,MPFROM2SHORT(usSelect,sizeof(szBuffer)),MPFROMP(szBuffer));
      }
      switch (SHORT1FROMMP(mp1)) {              // User has selected a directory
        case IDD_DIRLIST:
          if (SHORT2FROMMP(mp1) == LN_ENTER) {
            if (*szBuffer == ' ')
              DosSetDefaultDisk((ULONG) (szBuffer[1]-'@'));
            else
              DosSetCurrentDir(szBuffer);
            FillDirListBox(hwnd);
            FillFileListBox(hwnd);
            WinSetDlgItemText(hwnd,IDD_FILEEDIT,"");    // Erase the filename entry-field
            return 0;
          }
          break; // case IDD_DIRLIST
        case IDD_FILELIST:                      // User has selected a file
          switch (SHORT2FROMMP(mp1)) {
            case LN_SELECT:
              WinSetDlgItemText(hwnd,IDD_FILELIST,szBuffer);
              return 0;
            case LN_ENTER:
              ParseFileName(szFileName,szBuffer);
              WinDismissDlg(hwnd,TRUE);
              return 0;
          }
          break;        // case IDD_FILELIST
      }                 // switch (SHORT1FROMMP(mp1))
      break;            // case WM_CONTROL
    case WM_COMMAND:
      switch (SHORT1FROMMP(mp1)) {
        case DID_OK:
          WinQueryDlgItemText(hwnd,IDD_FILEEDIT,sizeof(szBuffer),szBuffer);
          switch (ParseFileName(szCurrentPath,szBuffer)) {
            case 0:
              WinAlarm(HWND_DESKTOP,WA_ERROR);
              FillDirListBox(hwnd);
              FillFileListBox(hwnd);
              return 0;
            case 1:
              FillDirListBox(hwnd);
              FillFileListBox(hwnd);
              WinSetDlgItemText(hwnd,IDD_FILEEDIT,"");
              return 0;
            case 2:
              strcpy(szFileName,szCurrentPath);
              WinDismissDlg(hwnd,TRUE);
              LoadMap();
              WinPostMsg(hwndClient,WM_PAINT,0,0);              // Show the new map
              return 0;
          }
          break;
        case DID_CANCEL:
          WinDismissDlg(hwnd,FALSE);
          return 0;
      }                 // switch (SHORT1FROMMP(mp1))
      break;            // case WM_COMMAND
  }                     // switch (msg)
  return WinDefDlgProc(hwnd,msg,mp1,mp2);
}

MRESULT EXPENTRY SaveDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
  static char szBuffer[80];
  USHORT usSelect;

  switch (msg) {
    case WM_INITDLG:                            // Fill the list boxes
      FillDirListBox(hwnd);
      FillFileListBox(hwnd);
//    WinSendDlgItemMsg(hwnd,IDD_FILEEDIT,EM_SETTEXTLIMIT,MPFROM2SHORT(80,0),NULL);
      return 0;

    case WM_CONTROL:                            // User has selected a file/dir
      if (SHORT1FROMMP(mp1) == IDD_DIRLIST || SHORT1FROMMP(mp1) == IDD_FILELIST) {
        usSelect = (USHORT) WinSendDlgItemMsg(hwnd,SHORT1FROMMP(mp1),LM_QUERYSELECTION,0L,0L);
// The following line returns -1, indicating an error code.  The result is that the user cannot
// select the file from the list box.
        WinSendDlgItemMsg(hwnd,SHORT1FROMMP(mp1),LM_QUERYITEMTEXT,MPFROM2SHORT(usSelect,sizeof(szBuffer)),MPFROMP(szBuffer));
      }
      switch (SHORT1FROMMP(mp1)) {              // User has selected a directory
        case IDD_DIRLIST:
          if (SHORT2FROMMP(mp1) == LN_ENTER) {
            if (*szBuffer == ' ')
              DosSetDefaultDisk((ULONG) (szBuffer[1]-'@'));
            else
              DosSetCurrentDir(szBuffer);
            FillDirListBox(hwnd);
            FillFileListBox(hwnd);
            WinSetDlgItemText(hwnd,IDD_FILEEDIT,"");
            return 0;
          }
          break; // case IDD_DIRLIST
        case IDD_FILELIST:                      // User has selected a file
          switch (SHORT2FROMMP(mp1)) {
            case LN_SELECT:
              WinSetDlgItemText(hwnd,IDD_FILELIST,szBuffer);
              return 0;
            case LN_ENTER:
              ParseFileName(szFileName,szBuffer);
              WinDismissDlg(hwnd,TRUE);
              return 0;
          }
          break;        // case IDD_FILELIST
      }                 // switch (SHORT1FROMMP(mp1))
      break;            // case WM_CONTROL
    case WM_COMMAND:
      switch (SHORT1FROMMP(mp1)) {
        case DID_OK:
          WinQueryDlgItemText(hwnd,IDD_FILEEDIT,sizeof(szBuffer),szBuffer);
          switch (ParseFileName(szCurrentPath,szBuffer)) {
            case 0:
              WinAlarm(HWND_DESKTOP,WA_ERROR);
              FillDirListBox(hwnd);
              FillFileListBox(hwnd);
              return 0;
            case 1:
              FillDirListBox(hwnd);
              FillFileListBox(hwnd);
              WinSetDlgItemText(hwnd,IDD_FILEEDIT,"");
              return 0;
            case 2:
              strcpy(szFileName,szCurrentPath);
              WinDismissDlg(hwnd,TRUE);
              SaveMap();
              return 0;
          }
          break;
        case DID_CANCEL:
          WinDismissDlg(hwnd,FALSE);
          return 0;
      }                 // switch (SHORT1FROMMP(mp1))
      break;            // case WM_COMMAND
  }                     // switch (msg)
  return WinDefDlgProc(hwnd,msg,mp1,mp2);
}
