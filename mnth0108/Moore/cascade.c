/*****************************************************************************\
*                                                                             *
* CASCADE is a sample C application that demonstrates the use of conditional  *
* cascade menus in OS/2 2.0.  The following code creates a standard window    *
* that has a conditional cascade menu and three menu items.  The third menu   *
* item is the default selection of the conditional cascade menu.  Each menu   *
* displays a different string when selected.                                  *
*                                                                             *
* AUTHOR:  Larry Moore                                                        *
*                                                                             *
* CASCADE.C                                                                   *
*                                                                             *
\*****************************************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>
#include "cascade.h"
#include <string.h>





INT main (VOID)
{
    HAB   hab;
    HMQ   hmq;
    QMSG  qmsg;
    ULONG flCreate;
    HWND  HwndClient = NULLHANDLE;
    HWND  HwndFrame = NULLHANDLE;

    /* Initialize and return anchor block handle. */
    if ((hab = WinInitialize (0)) != 0)
    {
         /* Create a message queue. */
         if ((hmq = WinCreateMsgQueue (hab,
                                       0)) != 0)
         {
              /* Register a class. */
              if (WinRegisterClass (hab,
                                    (PSZ) CASCADE_WINDOW_CLASS,
                                    (PFNWP) CascadeWindowProc,
                                    CS_SIZEREDRAW,
                                    0) != 0)
              {
                   /* Set the frame flags for this window.  FCF_STANDARD */
                   /* not used because some flags, including FCF_ICON    */
                   /* are not used in this example.                      */
                   flCreate = FCF_TITLEBAR | FCF_SYSMENU | FCF_MINBUTTON |
                              FCF_SIZEBORDER | FCF_MENU | FCF_SHELLPOSITION |
                              FCF_TASKLIST;

                   /* Create the window. */
                   if ((HwndFrame = WinCreateStdWindow (HWND_DESKTOP,
                                                        WS_VISIBLE,
                                                        &flCreate,
                                                        CASCADE_WINDOW_CLASS,
                                                        "Cascade Menu Sample",
                                                        WS_VISIBLE,
                                                        (HMODULE) 0,
                                                        ID_WINDOW,
                                                        &HwndClient)) != 0)
                   {
                        /* Run through this loop until a */
                        /* close message is sent.        */
                        while (WinGetMsg (hab,
                                          &qmsg,
                                          0L,
                                          0,
                                          0) != 0)
                        {
                             WinDispatchMsg (hab,
                                             &qmsg);
                        }
                        WinDestroyWindow (HwndFrame);
                   }
              }
              WinDestroyMsgQueue (hmq);
         }
         WinTerminate (hab);
    }

    return (0);
}





MRESULT EXPENTRY CascadeWindowProc (
    HWND   hwnd,
    ULONG  msg,
    MPARAM mp1,
    MPARAM mp2
)
{
    HWND            hwndMenu;          /* Handle of the menu bar            */
    ULONG           ulStyle;           /* Contains sub-menu reserved words  */
    static MENUITEM MenuItem;          /* Contains handle for sub-menu      */
    HPS             hps;               /* Presentation space handle         */
    RECTL           rc;                /* Bounding rectangle for painting   */
    POINTL          pt;                /* Point struct for string display   */
    static char     StringBuffer [8];  /* Buffer used in string display     */

    switch (msg)
    {
    case WM_CREATE:
         /* Get the handle of the menu. */
         hwndMenu = WinWindowFromID (WinQueryWindow (hwnd, QW_PARENT),
                                     FID_MENU);

         /* Get the handle of the DISPLAY sub-menu. */
         WinSendMsg (hwndMenu,
                     MM_QUERYITEM,
                     MPFROM2SHORT (ID_DISPLAY, TRUE),
                     MPFROMP (&MenuItem));
         
         /* Sub-menu exists, so continue. */
         if (MenuItem.hwndSubMenu != NULLHANDLE)
         {
              /* Retrieve the current window word. */
              ulStyle = WinQueryWindowULong (MenuItem.hwndSubMenu,
                                             QWL_STYLE);

              /* Set the window word as a conditional cascade */
              /* menu and put it back into the DISPLAY menu.  */
              ulStyle |= (ULONG) MS_CONDITIONALCASCADE;
              WinSetWindowULong (MenuItem.hwndSubMenu,
                                 QWL_STYLE,
                                 ulStyle);

              /* Now that a conditional cascade menu has been created, */
              /* let's set a default menu item.  Normally, the first   */
              /* menu item is the default, so for this example, let's  */
              /* set the third menu item (ID_STRING_3) as the default. */
              /* By doing this, a check is automatically placed next   */
              /* to ID_STRING_3.                                       */
              /* NOTE: Creating a conditional cascade menu does not    */
              /* automatically put a check mark on the default menu    */
              /* item.  This message must be sent to do that.          */
              WinSendMsg (MenuItem.hwndSubMenu,
                          MM_SETDEFAULTITEMID,
                          (MPARAM) ID_STRING_3,
                          (MPARAM) 0);
         }
         break;

    case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
         {
         /* Menu item STRING 1 has been selected.  Set the string */
         /* and invalidate the region to force repainting.        */
         case ID_STRING_1:
              strcpy (StringBuffer,
                      STRING_1);
              WinInvalidateRegion (hwnd,
                                   0L,
                                   FALSE);
              break;

         /* Menu item STRING 2 has been selected.  Set the string */
         /* and invalidate the region to force repainting.        */
         case ID_STRING_2:
              strcpy (StringBuffer,
                      STRING_2);
              WinInvalidateRegion (hwnd,
                                   0L,
                                   FALSE);
              break;

         /* Since STRING 3 was chosen as the default menu item */
         /* for DISPLAY, this message is sent if either        */
         /* DISPLAY or STRING 3 is selected.                   */
         case ID_STRING_3:
              strcpy (StringBuffer,
                      STRING_3);
              WinInvalidateRegion (hwnd,
                                   0L,
                                   FALSE);
              break;

         /* The EXIT menu is selected. */
         case ID_EXIT:
              WinPostMsg (hwnd,
                          WM_QUIT,
                          (MPARAM) 0,
                          (MPARAM) 0);
         }
         break;

    /* Repaint the window and display the test. */
    case WM_PAINT:
         hps = WinBeginPaint (hwnd,
                              0L,
                              &rc);

         /* Set the display coordinates. */
         pt.x = 200;
         pt.y = 200;

         /* Clear the client area and display the string. */
         GpiErase (hps);
         GpiCharStringAt (hps,
                          &pt,
                          (LONG) strlen (StringBuffer),
                          StringBuffer);
         WinEndPaint (hps);
         return ((MRESULT) FALSE);

     case WM_CLOSE:
         WinPostMsg (hwnd,
                     WM_QUIT,
                     (MPARAM) 0,
                     (MPARAM) 0);
         break;

    /* Cause PM to paint the background. */
    case WM_ERASEBACKGROUND:
         return ((MRESULT) TRUE);
    }

    return (WinDefWindowProc (hwnd,
                              msg,
                              mp1,
                              mp2));
}
