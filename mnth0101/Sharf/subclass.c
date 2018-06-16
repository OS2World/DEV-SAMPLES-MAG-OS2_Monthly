/* subclass.c -- Sample program to demonstrate a private window class */

#define INCL_PM
#define INCL_BASE
#include <OS2.H>
#include <process.h>

#include "entryfld.H"

static MRESULT EXPENTRY TestDlgProc (HWND, USHORT, MPARAM, MPARAM);

typedef struct                          /* Data area for dialog */
{
    CHAR    szTypedData[8];             /* Typed data area */
} WWTEST, *PWWTEST;

void main (void)
{
    HAB	    hab;                        /* Handle to anchor block */
    HMQ	    hmqMsgQueue;                /* Handle to message queue */
    WWTEST  wwtest;                     /* Dialog communication area */

    hab = WinInitialize (0);            /* Initialize PM */

    if (!RegisterEntryFieldClasses (hab))  /* Register our classes */
        exit (1);

    hmqMsgQueue = WinCreateMsgQueue (hab, 0);  /* Create message queue */

    WinDlgBox (HWND_DESKTOP, HWND_DESKTOP, TestDlgProc, 0,
               IDLG_TEST, &wwtest);

    WinDestroyMsgQueue (hmqMsgQueue);   /* Shutdown */
    WinTerminate       (hab);
}

/***************************************************************************
 *                                                                         *
 *  TestDlgProc() -- A procedure to test our new control                   *
 *                                                                         *
 ***************************************************************************/

static MRESULT EXPENTRY TestDlgProc (
HWND    hwndDlg,
USHORT  msg,
MPARAM  mp1,
MPARAM  mp2)
{
    PWWTEST     pwwtest;                /* --> dialog data area */

    switch(msg)
    {
        case WM_INITDLG:                /* Save address of passed buffer */
                                        /* So we can use it later */
                                        /* (Dialog windows reserve 4 bytes */
                                        /* at QWL_USER that we can use as */
                                        /* we wish.  We usually use this as */
                                        /* a pointer to data for the dialog */
            WinSetWindowPtr (hwndDlg, QWL_USER, PVOIDFROMMP(mp2));
                                        /* Don't allow more input than */
                                        /* we can store */
            WinSendDlgItemMsg (hwndDlg, IDEF_SUBCLASS_ENTRYFIELD,
                               EM_SETTEXTLIMIT,
                               MPFROMSHORT (sizeof (pwwtest->szTypedData)-1),
                               0);
            return 0;

        case WM_COMMAND:
            switch(SHORT1FROMMP(mp1))
            {
                                        /* Cancel (ESC) key pressed */
                                        /* Tell caller dialog was cancelled */
                case DID_CANCEL:
                    WinDismissDlg(hwndDlg, FALSE);
                    return 0;

                                        /* OK button (ENTER key) was pressed */
                case DID_OK:            /* Get address of caller's data area */
                    pwwtest = WinQueryWindowPtr (hwndDlg, QWL_USER);
                                        /* Read current contents of entry */
                                        /* field into data area */
                    WinQueryDlgItemText (hwndDlg, IDEF_SUBCLASS_ENTRYFIELD,
                                         sizeof (pwwtest->szTypedData),
                                         pwwtest->szTypedData);
                                        /* Tell user all data is input */
                    WinDismissDlg(hwndDlg, TRUE);
                    return 0;
            }
            return 0;
                               
        default:
            return(WinDefDlgProc(hwndDlg, msg, mp1, mp2));
    }
    return FALSE;
}
