/* FILES.C - File I/O routines & dialog boxes window procedures

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

This new version of the code uses the WinFileDlg() API to handle the filename retrieval for the
Load and Save features.
*/

#define INCL_DOSFILEMGR
#define INCL_WINSTDFILE
#include <os2.h>
#include <string.h>

#include "header.h"
#include "hexes.h"

static FILEDLG fdg;
static HFILE hfile;
static ULONG ulAction;
static ULONG ulBytesRead;

#define LOAD_ACTION (OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW )
#define SAVE_ACTION (OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS)

#define READ_ATTRS OPEN_FLAGS_NO_CACHE|              /* No need to take up precious cache space */ \
                   OPEN_FLAGS_SEQUENTIAL|            /* One-time read, remember?                */ \
                   OPEN_SHARE_DENYWRITE|             /* We don't want anyone changing it        */ \
                   OPEN_ACCESS_READONLY              // To prevent accidentally writing to it

#define WRITE_ATTRS OPEN_FLAGS_NO_CACHE|              /* No need to take up precious cache space */ \
                    OPEN_FLAGS_SEQUENTIAL|            /* One-time write, remember?               */ \
                    OPEN_SHARE_DENYREADWRITE|         /* We don't want anyone touching it        */ \
                    OPEN_ACCESS_WRITEONLY             // That's how we're gonna do it

void LoadMap(HWND hwnd) {
  fdg.cbSize=sizeof(FILEDLG);
  fdg.fl=FDS_CENTER | FDS_PRELOAD_VOLINFO | FDS_OPEN_DIALOG;
  fdg.pszTitle="Load Map";
  fdg.pszOKButton="Load";
  fdg.pfnDlgProc=NULL;
  fdg.pszIType=NULL;
  fdg.papszITypeList=NULL;
  fdg.pszIDrive=NULL;
  fdg.papszIDriveList=NULL;
  strcpy(fdg.szFullFile,"*");
  fdg.papszIDriveList=NULL;

  if (!WinFileDlg(HWND_DESKTOP,hwnd,&fdg)) return;
  if (fdg.lReturn != DID_OK) return;

  if (DosOpen(fdg.szFullFile,&hfile,&ulAction,0,FILE_NORMAL,LOAD_ACTION,READ_ATTRS,NULL)) return;
  DosRead(hfile,amap,sizeof(amap),&ulBytesRead);
  DosClose(hfile);
}

void SaveMap(HWND hwnd) {
  fdg.cbSize=sizeof(FILEDLG);
  fdg.fl=FDS_CENTER | FDS_PRELOAD_VOLINFO | FDS_SAVEAS_DIALOG;
  fdg.pszTitle="Save Map";
  fdg.pszOKButton="Save";
  fdg.pfnDlgProc=NULL;
  fdg.pszIType=NULL;
  fdg.papszITypeList=NULL;
  fdg.pszIDrive=NULL;
  fdg.papszIDriveList=NULL;
  strcpy(fdg.szFullFile,"*");
  fdg.papszIDriveList=NULL;

  if (!WinFileDlg(HWND_DESKTOP,hwnd,&fdg)) return;
  if (fdg.lReturn != DID_OK) return;

  if (DosOpen(fdg.szFullFile,&hfile,&ulAction,sizeof(amap),FILE_NORMAL,SAVE_ACTION,WRITE_ATTRS,NULL)) return;
  DosWrite(hfile,amap,sizeof(amap),&ulBytesRead);
  DosClose(hfile);
}
