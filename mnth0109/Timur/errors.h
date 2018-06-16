/* ERRORS.H

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

The error codes are decimal numbers that are unique to:

1. Module
2. Function in the module
3. Error condition in the function

So each error code should, theoretically, tell you exactly where
the error code occurred.  It does not, however, tell you how it
got there.  If anyone knows how to do this without much hassle,
please let me know.

The error codes usually apply to OS/2 functions that have failed.
The functions that have nothing to do with OS/2 API's
usually don't need such a sophisticated error scheme.

If function x calls function y, and function y returns an error
code, then function x will return the same error code (after
cleaning up any allocated resources that no longer apply), but
only if function x calls function y IN ONE PLACE.  That is,
if there are two calls to function y, then function x cannot simply
pass the error code back.  In fact, it's a bad idea if function
x calls function y in more than one place anyway.  The sequence
of function which caused the error becomes difficult to determine.

The values are constructed as follows.  The 32-bit unsigned value
is divided into 8 hexadecimal digits.

__000000 Indicates the module.  This allows room for 256 modules
00__0000 Indicates the function within that module.  This allows 256 functions per module
0000__00 Indicates the location or API call within the function.  Allows 256 per function
000000__ is extra information available
*/

typedef unsigned ERROR;           // 32-bit error code

/*
If function x calls function y, and function y returns a non-zero error code, then this
define will cause function x to exit immediately while returning the same error code.
It uses the global variable gec (Global Error Code), which acts more like temporary
storage than a global variable.
*/
#define RETURN(e) if ((gec=(e)) != 0) return gec

#ifdef ERRORS_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN ERROR gec;

#undef EXTERN


#define ERR_NOERROR  0

// bitmap.c
#define ERR_BITMAP 0x01000000
#define ERR_BITMAP_SHUT (ERR_BITMAP + 0x010000)
#define ERR_BITMAP_SHUT_PS            (ERR_BITMAP_SHUT + 0x0100)   // Could not destroy PS
#define ERR_BITMAP_SHUT_DC            (ERR_BITMAP_SHUT + 0x0200)   // Could not close DC

#define ERR_BITMAP_LOAD (ERR_BITMAP + 0x020000)
#define ERR_BITMAP_LOAD_DC            (ERR_BITMAP_LOAD + 0x0100)   // Could not open DC
#define ERR_BITMAP_LOAD_PS            (ERR_BITMAP_LOAD + 0x0200)   // Could not create PS
#define ERR_BITMAP_LOAD_HBM           (ERR_BITMAP_LOAD + 0x0300)   // Could not load bitmap

// mech.c
#define ERR_MECH 0x03000000

// menu.c
#define ERR_MENU 0x04000000

// target.c
#define ERR_TARGET 0x05000000

// files.c
#define ERR_FILES 0x06000000
#define ERR_FILES_CANCEL              (ERR_FILES + 0x000100)       // User cancelled a load/save
#define ERR_FILES_DLG                 (ERR_FILES + 0x000200)       // WinFileDlg() failed somehow

// terrain.c
#define ERR_TERR 0x07000000
#define ERR_TERR_INIT (ERR_TERR + 0x010000)

ERROR ErrorBox(ERROR);
APIRET ErrorDosError(APIRET);
void ErrorWinError(void);
