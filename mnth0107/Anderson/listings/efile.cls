            Listing for efile.c                                         Page 31

         1430|   #define INCL_WINHELP
         1431|   #define INCL_WIN
         1432|   #define INCL_GPI
         1433|   #define INCL_DOS
         1434|   #include <os2.h>
         1435|   #include <stdio.h>
         1436|   #include <string.h>
         1437|   #include <stdlib.h>
         1438|   #include "efile.h"
         1439|   #include "edit.h"
         1440|   
         1441|   /* open a file, read file into dynamically allocated buffer */
         1442|   /* b is a pointer to the beginning of the buffer            */
         1443|   /* On success, return file size (in bytes)                  */
         1444|   /* On failure, return:                                      */
         1445|   /*               CANTREAD if unable to open file            */ 
         1446|   /*               TOOLONG if file too big (> 64K)            */
         1447|   /*               NOMEMORY if unable to allocate memory      */
         1448|   LONG ReadFile (char *fn, PCHAR *b)
         1449|   {
         1450|      HFILE file;   /* handle */
         1451|      USHORT ac;   /* action code (from DosOpen) */
         1452|      USHORT rc;   /* return code (from API functions) */
         1453|      USHORT rd;   /* bytes actually read */
         1454|      ULONG cbSize;   /* file size in bytes */
         1455|      ULONG dummy;   /* used when seeking back to beginning of file */ 
         1456|      SEL sel;   /* segment selector -- for text buffer */
         1457|   
         1458|      /* try to open existing file --  access */
         1459|      rc = DosOpen (fn, &file, &ac, 0L, 0, 0x0001, 0x20C0, 0L);
         1460|      if (rc != 0 || ac != 0x0001)    /* failed */
         1461|         return CANTREAD;
         1462|      
         1463|      DosChgFilePtr (file, 0L, 2, &cbSize);   /* determine file size */
         1464|      if (cbSize > 65535L)
         1465|         return TOOLONG;
         1466|   
         1467|      DosChgFilePtr (file, 0L, 0, &dummy);   /* reset pointer to beginning */
         1468|      
         1469|      rc = DosAllocSeg ((int)cbSize, &sel, 0);
         1470|      if (rc != 0)
         1471|         return NOMEMORY;
         1472|         
         1473|      *b = MAKEP (sel, 0);   /* buffer pointer */
         1474|   
         1475|      /* read entire file into buffer */   
         1476|      DosRead (file, *b, (int)cbSize, &rd);
         1477|      DosClose (file);
         1478|      
         1479|      return (LONG) cbSize;
         1480|   }
         1481|   
         1482|   
         1483|   /* create a dynamically allocated buffer to use for writing to file */
         1484|   /* on success, return 0;  on failure return NOMEMORY                */
efile.c                                                                 Page 32

         1485|   LONG MakeWriteBuffer (LONG cb, PCHAR *b)
         1486|   {
         1487|      USHORT rc;   /* return code (from API functions) */
         1488|      SEL sel;   /* segment selector -- for text buffer */
         1489|      
         1490|      rc = DosAllocSeg ((int)cb, &sel, 0);
         1491|      if (rc != 0)
         1492|         return NOMEMORY;
         1493|         
         1494|      *b = MAKEP (sel, 0);   /* buffer pointer */
         1495|      return 0;
         1496|   }
         1497|   
         1498|   
         1499|   /* write buffer to file (cb is number of bytes in buffer) */
         1500|   /* on success return 0;  on failure return CANTWRITE      */
         1501|   LONG WriteFile (char *fn, LONG cb, PCHAR b)
         1502|   {
         1503|      HFILE file;   /* handle */
         1504|      USHORT ac;   /* action code (from DosOpen) */
         1505|      USHORT rc;   /* return code (from API functions) */
         1506|      USHORT wr;   /* bytes actually written */
         1507|   
         1508|      /* create file, overwriting any existing file */
         1509|      rc = DosOpen (fn, &file, &ac, 0L, 0, 0x0012, 0x20C1, 0L);
         1510|      if (rc == 0 && (ac == 0x0002 || ac == 0x0003)) {    /* ok */
         1511|         /* write entire buffer to file */   
         1512|         rc = DosWrite (file, b, (USHORT) cb, &wr);
         1513|         if (rc != 0 || wr != (USHORT) cb)
         1514|            return CANTWRITE;
         1515|            
         1516|         /* close file */
         1517|         rc = DosClose (file);
         1518|         if (rc != 0)
         1519|            return CANTWRITE;
         1520|            
         1521|         return 0;
         1522|      }
         1523|      else
         1524|         return CANTWRITE;
         1525|   }
         1526|   
         1527|   
         1528|   /* release storage */
         1529|   VOID Release (PCHAR b)
         1530|   {
         1531|      SEL sel;
         1532|      
         1533|      sel = SELECTOROF (b);
         1534|      DosFreeSeg (sel);
         1535|   }
         1536|   
