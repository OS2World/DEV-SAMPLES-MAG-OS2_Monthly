            Listing for edlg.c                                          Page 21

          928|   #define INCL_WINHELP
          929|   #define INCL_WIN
          930|   #define INCL_GPI
          931|   #define INCL_DOS
          932|   #include <os2.h>
          933|   #include <stdio.h>
          934|   #include <string.h>
          935|   #include <stdlib.h>
          936|   #include "edit.h"
          937|   #include "edlg.h"
          938|   
          939|   /* window procedure for about box */
          940|   MRESULT EXPENTRY AboutDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
          941|   {
          942|      switch (msg) {
          943|         case WM_COMMAND:
          944|            switch (SHORT1FROMMP (mp1)) {
          945|               case DID_OK:
          946|               case DID_CANCEL:
          947|                  WinDismissDlg (hwnd, TRUE);
          948|                  return 0;
          949|               default:
          950|                  break;
          951|            }
          952|      }
          953|      return WinDefDlgProc (hwnd, msg, mp1, mp2);
          954|   }
          955|   
          956|   
          957|   /* fill directory box with subdirectory names */
          958|   /* by Charles Petzold, from Programming the OS/2 Presentation Manager */
          959|   VOID FillDirListBox (HWND hwnd, CHAR *pcCurrentPath)
          960|   {
          961|      static CHAR szDrive[] = "  :";
          962|      FILEFINDBUF findbuf;
          963|      HDIR hDir = 1;
          964|      SHORT sDrive;
          965|      USHORT usDriveNum, usCurPathLen, usSearchCount = 1;
          966|      ULONG ulDriveMap;
          967|   
          968|      DosQCurDisk (&usDriveNum, &ulDriveMap);
          969|      pcCurrentPath[0] = (CHAR)((CHAR) usDriveNum + '@');
          970|      pcCurrentPath[1] = ':';
          971|      pcCurrentPath[2] = '\\';
          972|      usCurPathLen = 64;
          973|      DosQCurDir (0, pcCurrentPath + 3, &usCurPathLen);
          974|   
          975|      WinSetDlgItemText (hwnd, DID_PATH, pcCurrentPath);
          976|      WinSendDlgItemMsg (hwnd, DID_DIRLIST, LM_DELETEALL, NULL, NULL);
          977|   
          978|      for (sDrive = 0 ; sDrive < 26 ; sDrive++) {
          979|         if (ulDriveMap & 1L << sDrive) {
          980|            szDrive[1] = (CHAR)((CHAR) sDrive + 'A');
          981|   
          982|            WinSendDlgItemMsg (hwnd, DID_DIRLIST, LM_INSERTITEM,
edlg.c                                                                  Page 22

          983|               MPFROM2SHORT (LIT_END, 0), MPFROMP (szDrive));
          984|         }
          985|      }
          986|   
          987|      DosFindFirst ("*.*", &hDir, 0x0010, &findbuf, sizeof findbuf,
          988|         &usSearchCount, 0L);
          989|      while (usSearchCount) {
          990|         if (findbuf.attrFile & 0x0010 &&
          991|          (findbuf.achName[0] != '.' || findbuf.achName[1])) {
          992|            WinSendDlgItemMsg (hwnd, DID_DIRLIST, LM_INSERTITEM,
          993|               MPFROM2SHORT (LIT_SORTASCENDING, 0), MPFROMP (findbuf.achName));
          994|         }                            
          995|         DosFindNext (hDir, &findbuf, sizeof findbuf, &usSearchCount);
          996|      }
          997|   }
          998|   
          999|   
         1000|   /* fill file box with filenames */
         1001|   /* by Charles Petzold, from Programming the OS/2 Presentation Manager */
         1002|   VOID FillFileListBox (HWND hwnd)
         1003|   {
         1004|      FILEFINDBUF findbuf;
         1005|      HDIR hDir = 1;
         1006|      USHORT usSearchCount = 1;
         1007|   
         1008|      WinSendDlgItemMsg (hwnd, DID_FILELIST, LM_DELETEALL, NULL, NULL);
         1009|   
         1010|      DosFindFirst ("*.*", &hDir, 0x0000, &findbuf, sizeof findbuf,
         1011|         &usSearchCount, 0L);
         1012|      while (usSearchCount) {
         1013|         WinSendDlgItemMsg (hwnd, DID_FILELIST, LM_INSERTITEM,
         1014|            MPFROM2SHORT (LIT_SORTASCENDING, 0),
         1015|            MPFROMP (findbuf.achName));
         1016|   
         1017|         DosFindNext (hDir, &findbuf, sizeof findbuf, &usSearchCount);
         1018|      }
         1019|   }
         1020|   
         1021|   
         1022|   /* window procedure for open file dialog box */
         1023|   /* by Charles Petzold, from Programming the OS/2 Presentation Manager */
         1024|   MRESULT EXPENTRY OpenDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
         1025|   {
         1026|      static CHAR szCurrentPath[80], szBuffer[80];
         1027|      SHORT sSelect;
         1028|   
         1029|      switch (msg) {
         1030|         case WM_INITDLG:
         1031|            FillDirListBox (hwnd, szCurrentPath);
         1032|            FillFileListBox (hwnd);
         1033|   
         1034|            WinSendDlgItemMsg (hwnd, DID_FILEEDIT, EM_SETTEXTLIMIT,
         1035|               MPFROM2SHORT (70, 0), NULL);
         1036|            return 0;
         1037|   
edlg.c                                                                  Page 23

         1038|         case WM_CONTROL:
         1039|            if (SHORT1FROMMP (mp1) == DID_DIRLIST ||
         1040|             SHORT1FROMMP (mp1) == DID_FILELIST) {
         1041|               sSelect = SHORT1FROMMR (WinSendDlgItemMsg (hwnd,
         1042|                                          SHORT1FROMMP (mp1),
         1043|                                          LM_QUERYSELECTION, 
         1044|                                          (MPARAM) 0L, (MPARAM) 0L));
         1045|   
         1046|               WinSendDlgItemMsg (hwnd, SHORT1FROMMP (mp1), LM_QUERYITEMTEXT,
         1047|                  MPFROM2SHORT (sSelect, sizeof szBuffer), MPFROMP (szBuffer));
         1048|            }
         1049|   
         1050|            switch (SHORT1FROMMP (mp1)) {             /* Control ID */
         1051|               case DID_DIRLIST:
         1052|                  switch (SHORT2FROMMP (mp1)) {   /* notification code */
         1053|                     case LN_ENTER:
         1054|                        if (szBuffer[0] == ' ')
         1055|                           DosSelectDisk (szBuffer[1] - '@');
         1056|                        else
         1057|             				   DosChDir (szBuffer, 0L);
         1058|   
         1059|                           FillDirListBox (hwnd, szCurrentPath);
         1060|                           FillFileListBox (hwnd);
         1061|   
         1062|                           WinSetDlgItemText (hwnd, DID_FILEEDIT, "");
         1063|                           return 0;
         1064|                  }
         1065|                  break;
         1066|   
         1067|               case DID_FILELIST:
         1068|                  switch (SHORT2FROMMP (mp1)) {   /* notification code */
         1069|                     case LN_SELECT:
         1070|                        WinSetDlgItemText (hwnd, DID_FILEEDIT, szBuffer);
         1071|                        return 0;
         1072|   
         1073|                     case LN_ENTER:
         1074|                        ParseFileName (szFileName, szBuffer);
         1075|                        WinDismissDlg (hwnd, TRUE);
         1076|                        return 0;
         1077|                  }
         1078|                  break;
         1079|            }
         1080|            break;
         1081|   
         1082|         case WM_COMMAND:
         1083|            switch (COMMANDMSG(&msg)->cmd) {
         1084|               case DID_OK:
         1085|                  WinQueryDlgItemText (hwnd, DID_FILEEDIT,
         1086|                     sizeof szBuffer, szBuffer);
         1087|   
         1088|                  switch (ParseFileName (szCurrentPath, szBuffer)) {
         1089|                     case 0:
         1090|                        WinAlarm (HWND_DESKTOP, WA_ERROR);
         1091|                        FillDirListBox (hwnd, szCurrentPath);
         1092|                        FillFileListBox (hwnd);
edlg.c                                                                  Page 24

         1093|                        return 0;
         1094|   
         1095|                     case 1:
         1096|                        WinAlarm (HWND_DESKTOP, WA_NOTE);
         1097|                        FillDirListBox (hwnd, szCurrentPath);
         1098|                        FillFileListBox (hwnd);
         1099|                        WinSetDlgItemText (hwnd, DID_FILEEDIT, "");
         1100|                        return 0;
         1101|   
         1102|                     case 2:
         1103|                        strcpy (szFileName, szCurrentPath);
         1104|                        WinDismissDlg (hwnd, TRUE);
         1105|                        return 0;
         1106|                  }
         1107|                  break;
         1108|   
         1109|                  case DID_CANCEL:
         1110|                     WinDismissDlg (hwnd, FALSE);
         1111|                     return 0;
         1112|            }
         1113|            break;
         1114|      }
         1115|      return WinDefDlgProc (hwnd, msg, mp1, mp2);
         1116|   }
         1117|   
         1118|   
         1119|   /* window procedure for save as dialog box */
         1120|   /* by Charles Petzold, from Programming the OS/2 Presentation Manager */
         1121|   /* Modified (from OpenDlgProc) by Brian R. Anderson */
         1122|   MRESULT EXPENTRY SaveasDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
         1123|   {
         1124|      static CHAR szCurrentPath[80], szBuffer[80];
         1125|      SHORT sSelect;
         1126|   
         1127|      switch (msg) {
         1128|         case WM_INITDLG:
         1129|            FillDirListBox (hwnd, szCurrentPath);
         1130|   
         1131|            WinSendDlgItemMsg (hwnd, DID_FILEEDIT, EM_SETTEXTLIMIT,
         1132|               MPFROM2SHORT (70, 0), NULL);
         1133|            return 0;
         1134|   
         1135|         case WM_CONTROL:
         1136|            if (SHORT1FROMMP (mp1) == DID_DIRLIST) {
         1137|               sSelect = SHORT1FROMMR (WinSendDlgItemMsg (hwnd,
         1138|                                          SHORT1FROMMP (mp1),
         1139|                                          LM_QUERYSELECTION, 
         1140|                                          (MPARAM) 0L, (MPARAM) 0L));
         1141|   
         1142|               WinSendDlgItemMsg (hwnd, SHORT1FROMMP (mp1), LM_QUERYITEMTEXT,
         1143|                  MPFROM2SHORT (sSelect, sizeof szBuffer), MPFROMP (szBuffer));
         1144|            }
         1145|   
         1146|            switch (SHORT1FROMMP (mp1)) {             /* Control ID */
         1147|               case DID_DIRLIST:
edlg.c                                                                  Page 25

         1148|                  switch (SHORT2FROMMP (mp1)) {   /* notification code */
         1149|                     case LN_ENTER:
         1150|                        if (szBuffer[0] == ' ')
         1151|                           DosSelectDisk (szBuffer[1] - '@');
         1152|                        else
         1153|             				   DosChDir (szBuffer, 0L);
         1154|   
         1155|                           FillDirListBox (hwnd, szCurrentPath);
         1156|                           FillFileListBox (hwnd);
         1157|   
         1158|                           WinSetDlgItemText (hwnd, DID_FILEEDIT, "");
         1159|                           return 0;
         1160|                  }
         1161|                  break;
         1162|            }
         1163|            break;
         1164|   
         1165|         case WM_COMMAND:
         1166|            switch (COMMANDMSG(&msg)->cmd) {
         1167|               case DID_OK:
         1168|                  WinQueryDlgItemText (hwnd, DID_FILEEDIT,
         1169|                     sizeof szBuffer, szBuffer);
         1170|   
         1171|                  switch (ParseFileName (szCurrentPath, szBuffer)) {
         1172|                     case 0:
         1173|                        WinAlarm (HWND_DESKTOP, WA_ERROR);
         1174|                        FillDirListBox (hwnd, szCurrentPath);
         1175|                        FillFileListBox (hwnd);
         1176|                        return 0;
         1177|   
         1178|                     case 1:
         1179|                        WinAlarm (HWND_DESKTOP, WA_NOTE);
         1180|                        FillDirListBox (hwnd, szCurrentPath);
         1181|                        WinSetDlgItemText (hwnd, DID_FILEEDIT, "");
         1182|                        return 0;
         1183|   
         1184|                     case 2:
         1185|                        strcpy (szFileName, szCurrentPath);
         1186|                        WinDismissDlg (hwnd, TRUE);
         1187|                        return 0;
         1188|                  }
         1189|                  break;
         1190|   
         1191|               case DID_CANCEL:
         1192|                  WinDismissDlg (hwnd, FALSE);
         1193|                  return 0;
         1194|            }
         1195|            break;
         1196|      }
         1197|      return WinDefDlgProc (hwnd, msg, mp1, mp2);
         1198|   }
         1199|   
         1200|   
         1201|   /* determine if pathname is a valid file or directory */
         1202|   /* by Charles Petzold, from Programming the OS/2 Presentation Manager */
edlg.c                                                                  Page 26

         1203|   SHORT ParseFileName (CHAR *pcOut, CHAR *pcIn)
         1204|   {
         1205|   /*
         1206|    *   Input:    pcOut -- Pointer to parsed file specification.
         1207|    *             pcIn  -- Pointer to raw file specification.
         1208|    *                      
         1209|    *   Returns:      0 -- pcIn had invalid drive or directory.
         1210|    *                 1 -- pcIn was empty or had no filename.
         1211|    *                 2 -- pcOut points to drive, full dir, and file name.
         1212|    *
         1213|    *   Changes current drive and directory per pcIn string.
         1214|    */
         1215|   
         1216|      CHAR *pcLastSlash, *pcFileOnly;
         1217|      ULONG ulDriveMap;
         1218|      USHORT usDriveNum, usDirLen = 64;
         1219|   
         1220|      strupr (pcIn);
         1221|   
         1222|      /* If input string is empty, return 1 */
         1223|      if (pcIn[0] == '\0')
         1224|         return 1;
         1225|   
         1226|      /* Get drive from input string or current drive */
         1227|      if (pcIn[1] == ':') {
         1228|         if (DosSelectDisk (pcIn[0] - '@'))
         1229|            return 0;
         1230|   
         1231|         pcIn += 2;
         1232|      }
         1233|      DosQCurDisk (&usDriveNum, &ulDriveMap);
         1234|   
         1235|      *pcOut++ = (CHAR)((CHAR) usDriveNum + '@');
         1236|      *pcOut++ = ':';
         1237|      *pcOut++ = '\\';
         1238|   
         1239|      /* If rest of string is empty, return 1 */
         1240|      if (pcIn[0] == '\0')
         1241|         return 1;
         1242|   
         1243|      /* Search for last backslash.  If none, could be directory. */
         1244|      if (NULL == (pcLastSlash = strrchr (pcIn, '\\'))) {
         1245|   	   if (!DosChDir (pcIn, 0L))
         1246|            return 1;
         1247|   
         1248|         /* Otherwise, get current dir & attach input filename */
         1249|         DosQCurDir (0, pcOut, &usDirLen);
         1250|   
         1251|         if (strlen (pcIn) > 12)
         1252|            return 0;
         1253|   
         1254|         if (*(pcOut + strlen (pcOut) - 1) != '\\')
         1255|            strcat (pcOut++, "\\");
         1256|   
         1257|         strcat (pcOut, pcIn);
edlg.c                                                                  Page 27

         1258|         return 2;
         1259|      }
         1260|      
         1261|      /* If the only backslash is at beginning, change to root */
         1262|      if (pcIn == pcLastSlash) {
         1263|   	   DosChDir ("\\", 0L);
         1264|   
         1265|         if (pcIn[1] == '\0')
         1266|            return 1;
         1267|   
         1268|         strcpy (pcOut, pcIn + 1);
         1269|         return 2;
         1270|      }
         1271|      
         1272|      /* Attempt to change directory -- Get current dir if OK */
         1273|      *pcLastSlash = '\0';
         1274|   
         1275|      if (DosChDir (pcIn, 0L))
         1276|         return 0;
         1277|   
         1278|      DosQCurDir (0, pcOut, &usDirLen);
         1279|   
         1280|      /* Append input filename, if any */
         1281|      pcFileOnly = pcLastSlash + 1;
         1282|   
         1283|      if (*pcFileOnly == '\0')
         1284|         return 1;
         1285|   
         1286|      if (strlen (pcFileOnly) > 12)
         1287|         return 0;
         1288|   
         1289|      if (*(pcOut + strlen (pcOut) - 1) != '\\')
         1290|         strcat (pcOut++, "\\");
         1291|   
         1292|      strcat (pcOut, pcFileOnly);
         1293|      return 2;
         1294|   }
         1295|   
         1296|   
         1297|   /* window procedure for find (text search) dialog box */
         1298|   MRESULT EXPENTRY FindDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
         1299|   {
         1300|      switch (msg) {
         1301|         case WM_INITDLG:
         1302|           WinSendDlgItemMsg (hwnd, DID_FINDTEXT, EM_SETTEXTLIMIT,
         1303|              MPFROM2SHORT (50, 0), NULL);
         1304|            WinSetDlgItemText (hwnd, DID_FINDTEXT, szFind);
         1305|            WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_FINDTEXT));
         1306|            return (MRESULT) TRUE;
         1307|            
         1308|         case WM_COMMAND:
         1309|            switch (SHORT1FROMMP (mp1)) {
         1310|               case DID_OK:
         1311|                  if (WinQueryDlgItemText (hwnd, DID_FINDTEXT, 60, szFind))
         1312|                     WinDismissDlg (hwnd, DID_OK);
edlg.c                                                                  Page 28

         1313|                  else
         1314|                     WinDismissDlg (hwnd, DID_CANCEL);
         1315|                  return 0;
         1316|                  
         1317|               case DID_CANCEL:
         1318|                  WinDismissDlg (hwnd, DID_CANCEL);
         1319|                  return 0;
         1320|                  
         1321|               default:
         1322|                  break;
         1323|            }
         1324|      }
         1325|      return WinDefDlgProc (hwnd, msg, mp1, mp2);
         1326|   }
         1327|   
         1328|   
         1329|   /* window procedure for replace (text search and replace) dialog box */
         1330|   MRESULT EXPENTRY ReplaceDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
         1331|   {
         1332|      BOOL FAR *pb;
         1333|      
         1334|      switch (msg) {
         1335|         case WM_INITDLG:
         1336|            WinSendDlgItemMsg (hwnd, DID_NEWTEXT, EM_SETTEXTLIMIT,
         1337|               MPFROM2SHORT (50, 0), NULL);
         1338|            WinSendDlgItemMsg (hwnd, DID_OLDTEXT, EM_SETTEXTLIMIT,
         1339|               MPFROM2SHORT (50, 0), NULL);
         1340|            WinSetDlgItemText (hwnd, DID_OLDTEXT, szFind);
         1341|            WinSetDlgItemText (hwnd, DID_NEWTEXT, szReplace);
         1342|            pb = (PVOID) mp2;   /* TRUE if first entry */
         1343|            if (*pb) {
         1344|               WinEnableWindow (WinWindowFromID (hwnd, DID_DOREPLACE), FALSE);
         1345|               WinEnableWindow (WinWindowFromID (hwnd, DID_REPLACEALL), FALSE);
         1346|               WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_OLDTEXT));
         1347|            }
         1348|            else
         1349|               WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_NEWTEXT));
         1350|            return (MRESULT) TRUE;
         1351|            
         1352|         case WM_COMMAND:
         1353|            switch (SHORT1FROMMP (mp1)) {
         1354|               case DID_OK:
         1355|                  if (WinQueryDlgItemText (hwnd, DID_OLDTEXT, 60, szFind)) {
         1356|                     WinQueryDlgItemText (hwnd, DID_NEWTEXT, 60, szReplace);
         1357|                     WinDismissDlg (hwnd, DID_OK);
         1358|                  }
         1359|                  else
         1360|                     WinDismissDlg (hwnd, DID_CANCEL);
         1361|                  return 0;
         1362|                  
         1363|               case DID_DOREPLACE:
         1364|                  if (WinQueryDlgItemText (hwnd, DID_NEWTEXT, 60, szReplace))
         1365|                     WinDismissDlg (hwnd, DID_DOREPLACE);
         1366|                  else
         1367|                     WinDismissDlg (hwnd, DID_CANCEL);
edlg.c                                                                  Page 29

         1368|                  break;
         1369|                  
         1370|               case DID_REPLACEALL:
         1371|                  if (WinQueryDlgItemText (hwnd, DID_NEWTEXT, 60, szReplace))
         1372|                     WinDismissDlg (hwnd, DID_REPLACEALL);
         1373|                  else
         1374|                     WinDismissDlg (hwnd, DID_CANCEL);
         1375|                  break;
         1376|                  
         1377|               case DID_CANCEL:
         1378|                  WinDismissDlg (hwnd, DID_CANCEL);
         1379|                  return 0;
         1380|                  
         1381|               default:
         1382|                  break;
         1383|            }
         1384|      }
         1385|      return WinDefDlgProc (hwnd, msg, mp1, mp2);
         1386|   }
         1387|   
         1388|   
         1389|   /* window procedure for find (text search) dialog box */
         1390|   MRESULT EXPENTRY GoLnDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
         1391|   {
         1392|      switch (msg) {
         1393|         case WM_INITDLG:
         1394|            WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_LINENBR));
         1395|            return (MRESULT) TRUE;
         1396|            
         1397|         case WM_COMMAND:
         1398|            switch (SHORT1FROMMP (mp1)) {
         1399|               case DID_OK:
         1400|                  if (WinQueryDlgItemText (hwnd, DID_LINENBR, 20, szLine))
         1401|                     WinDismissDlg (hwnd, DID_OK);
         1402|                  else
         1403|                     WinDismissDlg (hwnd, DID_CANCEL);
         1404|                  return 0;
         1405|                  
         1406|               case DID_CANCEL:
         1407|                  WinDismissDlg (hwnd, DID_CANCEL);
         1408|                  return 0;
         1409|                  
         1410|               default:
         1411|                  break;
         1412|            }
         1413|      }
         1414|      return WinDefDlgProc (hwnd, msg, mp1, mp2);
         1415|   }
         1416|   
