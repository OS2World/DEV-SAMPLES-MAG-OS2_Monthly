/* Program demonstrates use of window words, sending menu messages to 
    submenus, use of stringtables
    Version 1.0 for OS/2 2.1
*/

#define INCL_WIN    /* This includes the window manager function prototypes, */
                    /* constants, etc. */
#include <os2.h>    /* This is the main OS/2 definitions include file */
#include "malloc.h"
#include <string.h>

#include "quotes.h"

/* Prototype for the applications window procedure */
MRESULT EXPENTRY QuotesWndProc(HWND hwnd, ULONG usMessage, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY QuotesChildWndProc(HWND hwnd, ULONG usMessage, MPARAM mp1, MPARAM mp2);

/* We need handles for both the frame window and the client window */
HWND hwndParentFrame, hwndParentClient;
HWND hwndC1Frame, hwndC2Frame, hwndC1Client, hwndC2Client;

/* Multiple windows can be maintained by one wndproc, so it's a class, rather
than an object. The class is registered by name with the system. This 
"application" has two types of windows (main and two children) so it requires
two window classes */
char szClassName[] = "Quotes";
char szChildClassName[] = "QuotesChild";

void main(int argc, char **argv)
{
    HAB hab;        /* Handle to an 'anchor block' */
    HMQ hmq;        /* Handle to the input message queue */
    QMSG qmsg;        /* Queue message structure to hold the incoming message */
    ULONG ulFrameFlags;   /* Frame creation flags */

    /* First, register with PM to get services. This returns a handle to an anchor block */
    hab = WinInitialize(0);

    /* Next, create a message queue */
    hmq = WinCreateMsgQueue(hab, 0);

    /* Now register a window class for the application parent window */
    if(!WinRegisterClass(hab,
        (PCH)szClassName,
        (PFNWP)QuotesWndProc,    /* Pointer to winproc function */
        CS_SIZEREDRAW,   /* Flags to force redraw when resized, etc. */
        sizeof(char *)))                 /* Number of bytes in 'window words' */
    DosExit(EXIT_PROCESS, 1);

    /* Now register a window class for the application child windows */
    if(!WinRegisterClass(hab,
        (PCH)szChildClassName,
        (PFNWP)QuotesChildWndProc,    /* Pointer to winproc function */
        CS_SIZEREDRAW,   /* Flags to force redraw when resized, etc. */
        sizeof(PSTRCLR)))                /* Number of bytes in 'window words' */
    DosExit(EXIT_PROCESS, 1);

    /* We want a regular window, but without an accelerator table */
    ulFrameFlags = FCF_STANDARD & ~FCF_ACCELTABLE;

    /* Now go ahead and create the window */
    /* This function call returns the frame window handle and also sets the client window handle */
    hwndParentFrame = WinCreateStdWindow(HWND_DESKTOP,    /* Parent window */
            WS_VISIBLE,             /* Window style visible */
            &ulFrameFlags,              /* pointer to frame flags */
            (PCH)szClassName,            /* Registered class name */
            "Quotable Quotes",                /* Text for title bar */
            WS_VISIBLE,             /* Client window style */
            (HMODULE)NULL,            /* Pointer to resource module */
            ID_QUOTES,                    /* Resource ID within module */
            (HWND *)&hwndParentClient);        /* Pointer to client window handle */

    if(hwndParentFrame == 0) {
        WinAlarm(HWND_DESKTOP, WA_ERROR);
        DosExit(EXIT_PROCESS, 1);
    }

    /* Now loop around processing events. This is called the 'event loop' */
    /* WinGetMsg returns FALSE when it gets the WM_QUIT message */
    while(WinGetMsg(hab, &qmsg, 0, 0, 0))    /* Get a message */
       WinDispatchMsg(hab, &qmsg);        /* Dispatch it to the window */

    /* Lastly, clean up */
    WinDestroyWindow(hwndParentFrame);
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
}

/* The client window procedure. Gets called with message components as parameters */
MRESULT EXPENTRY QuotesWndProc(HWND hwnd, ULONG usMessage, MPARAM mp1, MPARAM mp2)
{
    HPS hps;        /* Handle to a presentation space. This is where a PM program 'draws' */
    RECTL rc;       /* A rectangle structure, used to store the window coordinates */
    ULONG ulFrameFlags = FCF_STANDARD & ~FCF_MENU & ~FCF_ICON & ~FCF_ACCELTABLE;    /* Frame creation flags */
    HWND hwndActive;

    switch(usMessage) {

        case WM_CREATE:     /* process this message by returning FALSE. This lets the */

            hwndC1Frame = WinCreateStdWindow(hwnd,
                        WS_VISIBLE,
                        &ulFrameFlags,
                        (PCH)szChildClassName,
                        " Child 1",
                        WS_VISIBLE,
                        (HMODULE)NULL,
                        0,
                        (HWND *)&hwndC1Client);

            if(hwndC1Frame == 0) {
                WinAlarm(HWND_DESKTOP, WA_ERROR);
                DosExit(EXIT_PROCESS, 1);
            }

            hwndC2Frame = WinCreateStdWindow(hwnd,
                        WS_VISIBLE,
                        &ulFrameFlags,
                        (PCH)szChildClassName,
                        " Child 2",
                        WS_VISIBLE,
                        (HMODULE)NULL,
                        0,
                        (HWND *)&hwndC2Client);

            if(hwndC2Frame == 0) {
                WinAlarm(HWND_DESKTOP, WA_ERROR);
                WinAlarm(HWND_DESKTOP, WA_ERROR);
                DosExit(EXIT_PROCESS, 1);
            }

            WinSetWindowPtr(hwnd,0, "This is the background window");

            /* We can't size the children yet, since this window itself has */
            /* no size. So we'll post a message to 'remind' ourselves */
            /* to do it as soon as the window is visible */
            WinPostMsg(hwnd, UM_INITSIZE, 0L, 0L);

            return (MRESULT)FALSE;  /* system continue creating the window */
            break;

        case UM_INITSIZE:

                /* Now's our chance to size the children */
                WinQueryWindowRect(hwnd, &rc);

                WinSetWindowPos(hwndC1Frame,
                        HWND_TOP,
                        rc.xRight / 16,
                        rc.yTop / 8,
                        rc.xRight * 3 / 4,
                        rc.yTop * 3 / 4,
                        SWP_SIZE | SWP_MOVE);

                WinSetWindowPos(hwndC2Frame,
                        HWND_TOP,
                        rc.xRight/8,
                        rc.yTop/16,
                        rc.xRight * 3 / 4,
                        rc.yTop * 3 / 4,
                        SWP_SIZE | SWP_MOVE);
                break;

        case WM_COMMAND:
            /* Got a menu choice. If it applies to this window, do it */
            /* Otherwise, pass it down to the active window (if there is one) */

            switch (SHORT1FROMMP(mp1)) {
                case IDM_EXIT:
                    WinPostMsg(hwnd, WM_QUIT, mp1, mp2);
                    break;
                 default: 
                    if ((hwndActive=WinQueryActiveWindow(hwnd)) != 0L)
                        WinSendMsg(hwndActive, WM_COMMAND, mp1, mp2);
                    break;
            }
            break;

        case WM_ERASEBACKGROUND:    /* Let the frame window procedure redraw the background */
            return (MRESULT)TRUE;   /* in SYSCLR_WINDOW (usually white) */
            break;

        case WM_PAINT:        /* The 'guts' of the application */
            hps = WinBeginPaint(hwnd, 0, NULL);   /* Get a presentation space */
            WinQueryWindowRect(hwnd, &rc);         /* Get the window dimensions */
            /* Draw the message, in rectangle rc, coloured CLR_NEUTRAL (black) on
            CLR_BACKGROUND (white), centered, over-writing the entire rectangle */
            WinDrawText(hps, -1, WinQueryWindowPtr(hwnd, QWL_USER), &rc, CLR_NEUTRAL, CLR_BACKGROUND,
                        DT_CENTER | DT_VCENTER | DT_ERASERECT);
            WinEndPaint(hps);        /* Release the presentation space */
            break;

        case WM_DESTROY:
/*            WinDestroyWindow(hwndC1Frame);
            WinDestroyWindow(hwndC2Frame); */
            return (MRESULT) 0;
            break;

        case WM_CLOSE:    /* User chose CLOSE on system menu or double-clicked */
            WinPostMsg(hwnd, WM_QUIT, 0, 0);    /* So send back WM_QUIT, causing */
            break;            /* WinGetMsg to return FALSE and exit the event loop */

        default:        /* Let the system handle messages we don't */
            return WinDefWindowProc(hwnd, usMessage, mp1, mp2);
            break;
    }
    return 0L;
}

/* The child window procedure. Gets called with message components as parameters */
MRESULT EXPENTRY QuotesChildWndProc(HWND hwnd, ULONG usMessage, MPARAM mp1, MPARAM mp2)
{
    HPS hps;        /* Handle to a presentation space. This is where a PM program 'draws' */
    RECTL rc;        /* A rectangle structure, used to store the window coordinates */
    PSTRCLR p;      /* A pointer to a STRINGCOLOR structure */
    HAB hab;        /* Required to load a string from a stringtable resource */

    /* Extract a pointer to the STRCLR structure from the window */
    p = PSCFROMHWND(hwnd);

    switch(usMessage) {

        case WM_CREATE:
            /* Allocate a STRCLR structure and initialize it */
            p = (PSTRCLR) malloc(sizeof(STRCLR));
            WinSetWindowPtr(hwnd,0,(PVOID)p);
            p->ulFColor = CLR_NEUTRAL;
            p->ulBColor = CLR_BACKGROUND;
            strcpy(p->quote, "A suitable quote goes here");
            return (MRESULT)FALSE;  /* Let the system continue creating the window */
            break;

        case WM_DESTROY:
            free(p);
            return (MRESULT) 0;

        case WM_COMMAND:
            hab = WinQueryAnchorBlock(hwnd);
            switch(SHORT1FROMMP(mp1)) {
                case IDM_TWAIN:
                    WinLoadString(hab, (HMODULE)0, IDS_TWAIN, MAXSTR, p->quote);
                    break;
                case IDM_IBSEN:
                    WinLoadString(hab, (HMODULE)0, IDS_IBSEN, MAXSTR, p->quote);
                    break;
                case IDM_GATES:
                    WinLoadString(hab, (HMODULE)0, IDS_GATES, MAXSTR, p->quote);
                    break;
                case IDMF_RED: p->ulFColor = CLR_RED; break;
                case IDMF_GREEN: p->ulFColor = CLR_GREEN; break;
                case IDMF_BLUE: p->ulFColor = CLR_BLUE; break;
                case IDMB_RED: p->ulBColor = CLR_RED; break;
                case IDMB_GREEN: p->ulBColor = CLR_GREEN; break;
                case IDMB_BLUE: p->ulBColor = CLR_BLUE; break;
            }

            /* After changing the internal representation of the window, get */
            /* the system to repaint the window by invalidating it */
            WinInvalidateRect(hwnd, NULL, 0);
            return (MRESULT)0;
            break;

        case WM_ERASEBACKGROUND:    /*Don't let the frame window procedure redraw the background */
            WinInvalidateRect(hwnd, NULL, FALSE);
            return (MRESULT)FALSE;   /* in SYSCLR_WINDOW (usually white) */
            break;

        case WM_PAINT:          /* The 'guts' of the application */
                                /* Repaints the window based on the internal structure */
            hps = WinBeginPaint(hwnd, 0, &rc);   /* Get a presentation space */
            WinQueryWindowRect(hwnd, &rc);         /* Get the window dimensions */

            /* Draw the message, in rectangle rc, coloured CLR_NEUTRAL (black) on */
            /* CLR_BACKGROUND (white), centered, over-writing the entire rectangle */
            WinDrawText(hps, -1, p->quote, &rc, p->ulFColor, p->ulBColor,
                        DT_CENTER | DT_VCENTER | DT_ERASERECT);
            WinEndPaint(hps);        /* Release the presentation space */
            break;

        case WM_CLOSE:    /* User chose CLOSE on system menu or double-clicked */
            WinDestroyWindow(WinQueryWindow(hwnd, QW_PARENT));
            break;            /* WinGetMsg to return FALSE and exit the event loop */

        default:        /* Let the system handle messages we don't */
            return WinDefWindowProc(hwnd, usMessage, mp1, mp2);
            break;
    }
    return 0L;
}
