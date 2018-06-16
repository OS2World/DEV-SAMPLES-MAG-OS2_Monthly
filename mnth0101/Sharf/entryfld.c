/* entryfld.c -- Upper case conversion subclass procedure */

#define INCL_PM
#define INCL_BASE
#include <OS2.H>

#include <ctype.h>

#include "entryfld.h"

/***************************************************************************
 *                                                                         *
 *  Pointer to window procedure for WC_ENTRYFIELD window class.  We        *
 *  use this to subclass the entry field.  This field is initialized       *
 *  in RegisterEntryFieldClasses().                                        *
 *                                                                         *
 ***************************************************************************/

static PFNWP pfnEditWndProc;

/***************************************************************************
 *                                                                         *
 *  UDUpperWndProc                                                         *
 *                                                                         *
 *  Subclass edit fields for all uppercase, printable input                *
 *                                                                         *
 ***************************************************************************/

static MRESULT EXPENTRY UDUpperWndProc (
HWND    hwnd,
USHORT  msg,
MPARAM  mp1,
MPARAM  mp2)
{
    CHRMSG    *p;                      /* --> entire character msg */

    if (msg == WM_CHAR)
    {                  /* Upper case conversion code taken from */
                       /* Cheatham, Reich, Robinson -- */
                       /* "OS/2 Presentation Manager Programming" */
        p = CHARMSG (&msg);     /* Get char msg */
        if (((p->fs & (KC_KEYUP|KC_CHAR)) == KC_CHAR)  /* Only want */
            && (! (p->fs & KC_VIRTUALKEY) ) ) /* key downstroke message */
        {
            if (isalpha (p->chr) && islower (p->chr))  /* Lower case? */
                p->chr = (USHORT) _toupper ((UCHAR)p->chr);  /* Up it */
                       /* end of extract from "OS/2 PM Programming" */

            if (!isprint (p->chr))   /* Eliminate non-printables */
            {
                WinAlarm (HWND_DESKTOP, WA_ERROR);  /* Warn the user */
                return ((MRESULT) TRUE);  /* Say we processed the msg */
            }

        }
    } 

    return (pfnEditWndProc(hwnd, msg, mp1, mp2));
}



/***************************************************************************
 *                                                                         *
 *  Register the entry field classes.  Return TRUE if successfull, FALSE   *
 *  if not successful.  We register the class and save the entry point     *
 *  to the original entry field procedure for later use.                   *
 *                                                                         *
 ***************************************************************************/

USHORT  RegisterEntryFieldClasses (     /* Register entry field classes */
HAB     hab)                            /* I - Handle to anchor block */
{
    CLASSINFO   ci;                     /* Window class information */

                                        /* Get window class data */
    WinQueryClassInfo (hab, WC_ENTRYFIELD, &ci);

                                        /* Save WC_ENTRYFIELD procedure */
    pfnEditWndProc = ci.pfnWindowProc;

                                        /* Register our new class */
    return (WinRegisterClass(hab, WC_UDUPPER, UDUpperWndProc, CS_SIZEREDRAW,
                            ci.cbWindowData));

}

/* entryfld.c */
