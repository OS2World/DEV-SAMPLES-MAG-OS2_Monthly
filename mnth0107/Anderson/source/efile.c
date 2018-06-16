#define INCL_WINHELP
#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "efile.h"
#include "edit.h"

/* open a file, read file into dynamically allocated buffer */
/* b is a pointer to the beginning of the buffer            */
/* On success, return file size (in bytes)                  */
/* On failure, return:                                      */
/*               CANTREAD if unable to open file            */ 
/*               TOOLONG if file too big (> 64K)            */
/*               NOMEMORY if unable to allocate memory      */
LONG ReadFile (char *fn, PCHAR *b)
{
   HFILE file;   /* handle */
   USHORT ac;   /* action code (from DosOpen) */
   USHORT rc;   /* return code (from API functions) */
   USHORT rd;   /* bytes actually read */
   ULONG cbSize;   /* file size in bytes */
   ULONG dummy;   /* used when seeking back to beginning of file */ 
   SEL sel;   /* segment selector -- for text buffer */

   /* try to open existing file --  access */
   rc = DosOpen (fn, &file, &ac, 0L, 0, 0x0001, 0x20C0, 0L);
   if (rc != 0 || ac != 0x0001)    /* failed */
      return CANTREAD;
   
   DosChgFilePtr (file, 0L, 2, &cbSize);   /* determine file size */
   if (cbSize > 65535L)
      return TOOLONG;

   DosChgFilePtr (file, 0L, 0, &dummy);   /* reset pointer to beginning */
   
   rc = DosAllocSeg ((int)cbSize, &sel, 0);
   if (rc != 0)
      return NOMEMORY;
      
   *b = MAKEP (sel, 0);   /* buffer pointer */

   /* read entire file into buffer */   
   DosRead (file, *b, (int)cbSize, &rd);
   DosClose (file);
   
   return (LONG) cbSize;
}


/* create a dynamically allocated buffer to use for writing to file */
/* on success, return 0;  on failure return NOMEMORY                */
LONG MakeWriteBuffer (LONG cb, PCHAR *b)
{
   USHORT rc;   /* return code (from API functions) */
   SEL sel;   /* segment selector -- for text buffer */
   
   rc = DosAllocSeg ((int)cb, &sel, 0);
   if (rc != 0)
      return NOMEMORY;
      
   *b = MAKEP (sel, 0);   /* buffer pointer */
   return 0;
}


/* write buffer to file (cb is number of bytes in buffer) */
/* on success return 0;  on failure return CANTWRITE      */
LONG WriteFile (char *fn, LONG cb, PCHAR b)
{
   HFILE file;   /* handle */
   USHORT ac;   /* action code (from DosOpen) */
   USHORT rc;   /* return code (from API functions) */
   USHORT wr;   /* bytes actually written */

   /* create file, overwriting any existing file */
   rc = DosOpen (fn, &file, &ac, 0L, 0, 0x0012, 0x20C1, 0L);
   if (rc == 0 && (ac == 0x0002 || ac == 0x0003)) {    /* ok */
      /* write entire buffer to file */   
      rc = DosWrite (file, b, (USHORT) cb, &wr);
      if (rc != 0 || wr != (USHORT) cb)
         return CANTWRITE;
         
      /* close file */
      rc = DosClose (file);
      if (rc != 0)
         return CANTWRITE;
         
      return 0;
   }
   else
      return CANTWRITE;
}


/* release storage */
VOID Release (PCHAR b)
{
   SEL sel;
   
   sel = SELECTOROF (b);
   DosFreeSeg (sel);
}
