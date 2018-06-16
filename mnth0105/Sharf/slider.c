// slider.c -- Sample program to demonstrate slider

//--------------------------------------------------------------
//  slider.c
//
//      Sample program to demonstrate sliders.  
//
//  By: Guy Scharf                      (415) 948-9186
//      Software Architects, Inc.       FAX: (415) 948-1620
//      2163 Jardin Drive               
//      Mountain View, CA   94040       
//      CompuServe: 76702,557
//      Internet: 76702.557@compuserve.com
//  (c) Copyright 1992 Software Architects, Inc.  
//
//      All Rights Reserved.
//
//  Software Architects, Inc. develops software products for 
//  independent software vendors using OS/2 and Presentation 
//  Manager.
//--------------------------------------------------------------

#define INCL_PM                         // Basic OS/2 PM
#define INCL_BASE                       // components
#include <OS2.H>

#include <stdlib.h>                     // C runtime library
#include <string.h>

#include "slider.h"                     // Slider demo defs

//  Prototypes of dialog procedures

static MRESULT EXPENTRY GetAmountDlgProc (HWND, MSGID, MPARAM,
                                          MPARAM);
static MRESULT EXPENTRY ProgressDlgProc (HWND, MSGID, MPARAM,
                                         MPARAM);

//--------------------------------------------------------------
//                                                                          
//  Main program to drive slider example
//                                                                          
//--------------------------------------------------------------

int main (void)
{
    HAB	    hab;                        // Handle to anchor block
    HMQ	    hmqMsgQueue;                // Handle to msg queue
#ifndef OS220
    HMODULE hmodSlider;                 // Handle to slider mod
#endif

    hab = WinInitialize (0);            // Initialize PM

    hmqMsgQueue = WinCreateMsgQueue (hab, 0); // Create msg queue

#ifndef OS220
    if (DosLoadModule (NULL, 0, CCL_SLIDER_DLL, &hmodSlider))
        return FALSE;
#endif

    WinDlgBox (HWND_DESKTOP, HWND_DESKTOP, GetAmountDlgProc, 0,
               IDLG_SETTIME, NULL);

#ifndef OS220
    DosFreeModule (hmodSlider);
#endif

    WinDestroyMsgQueue (hmqMsgQueue);   // Shutdown
    WinTerminate       (hab);
    return 0;
}

//--------------------------------------------------------------
//                                                                          
//  GetAmountDlgProc() -- Determine time delay desired
//                                                                          
//--------------------------------------------------------------

static MRESULT EXPENTRY GetAmountDlgProc (
HWND    hwndDlg,
MSGID   msg,
MPARAM  mp1,
MPARAM  mp2)
{
    USHORT  usTime;                     // Time for progress

    switch(msg)
    {
        //------------------------------------------------------
        //  Initialize dialog by setting labels, ticks, detents,
        //  etc.  This must all be done programmatically.
        //------------------------------------------------------
        case WM_INITDLG:

            //--------------------------------------------------
            //  Set the marks, detents, and labels
            //  Set small tick marks every 1 second
            //  Set large tick marks every 10 seconds
            //  Set detent every 15 seconds
            //--------------------------------------------------
            InitSlider (hwndDlg, IDSL_SETTIME_TIME, 0,
                        1,  4,
                        10, 8,
                        30, 30,
                        "8.Courier");

            return 0;

        //------------------------------------------------------
        //  Process pushbuttons.  Enter starts a progress bar.
        //  Cancel (ESC) just quits quietly.
        //------------------------------------------------------
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {
                                        // Cancel pressed
                                        // Dismiss dialog
                case DID_CANCEL:
                    WinDismissDlg (hwndDlg, FALSE);
                    return 0;

                                        // OK button pressed
                case DID_OK:            // Read slider position
                                        // from slider
                    usTime = SHORT1FROMMR(WinSendDlgItemMsg(
                               hwndDlg,
                               IDSL_SETTIME_TIME,
                               SLM_QUERYSLIDERINFO,
                               MPFROM2SHORT(SMA_SLIDERARMPOSITION,
                                            SMA_INCREMENTVALUE),
                               0));
                                        // Start progress bar
                                        // dialog
                    if (usTime > 0)
                        WinDlgBox (HWND_DESKTOP, HWND_DESKTOP,
                                   ProgressDlgProc, 0,
                                   IDLG_PROGRESS, &usTime);
                    return 0;
            }
            return 0;
                               
        //------------------------------------------------------
        //  All other messages go to default window procedure
        //------------------------------------------------------
        default:
            return (WinDefDlgProc (hwndDlg, msg, mp1, mp2));
    }
    return FALSE;
}


//--------------------------------------------------------------
//                                                                          
//  ProgressDlgProc() -- Show progress bar.  This is a minimal
//  function dialog.  It just moves the progress bar along
//  without reflecting any particular activity.
//                                                                          
//--------------------------------------------------------------

typedef struct
{
    USHORT  usTotalUnits;               // # units to count
    USHORT  usUnitsSoFar;               // count so far
    USHORT  usPctDone;                  // Last reported % done
} PROG, *PPROG;

#define TIMERINTERVAL    4              // Timer checks per sec

static MRESULT EXPENTRY ProgressDlgProc (
HWND    hwndDlg,
MSGID   msg,
MPARAM  mp1,
MPARAM  mp2)
{
    PPROG   pprog;                      // Progress data

    switch(msg)
    {
        //------------------------------------------------------
        //  Initialize dialog by constructing a work area and
        //  establishing a pointer to it in the dialog window
        //  word.  Set initial values to 0 and compute the
        //  number of ticks desired from the passed value
        //  (which is in seconds)
        //------------------------------------------------------
        case WM_INITDLG:                // Initialize data
            pprog = malloc (sizeof (PROG));
            WinSetWindowPtr (hwndDlg, QWL_USER, pprog);

            pprog->usUnitsSoFar = 0;    // Ticks so far
            pprog->usPctDone = 0;       // Start at 0
            pprog->usTotalUnits = (USHORT)(*(PUSHORT)mp2
                                          * TIMERINTERVAL);

            //--------------------------------------------------
            //  Set 2x wide slider, 
            //  No small tick
            //  No large ticks
            //  No detents
            //  0, 50, 100% complete
            //--------------------------------------------------
            InitSlider (hwndDlg, IDSL_PROGRESS_BAR, 2,
                        0, 4,
                        0, 8,
                        0, 50,
                        NULL);

            //--------------------------------------------------
            //  Start the timer for periodic display
            //--------------------------------------------------
            WinStartTimer (WinQueryAnchorBlock (hwndDlg),
                           hwndDlg, ID_TIMER,
                           1000/TIMERINTERVAL);

            return 0;

        //------------------------------------------------------
        //  When dialog is destroyed, we release any data
        //  we have allocated.  This prevents memory leakage.
        //------------------------------------------------------
        case WM_DESTROY:                
            pprog = WinQueryWindowPtr (hwndDlg, QWL_USER);
                                        // Stop the timer
            WinStopTimer (WinQueryAnchorBlock (hwndDlg),
                          hwndDlg, ID_TIMER);
            if (pprog != NULL)          // If data exists,
                free (pprog);           // free it
            WinSetWindowPtr (hwndDlg, QWL_USER, NULL);

            return 0;

        //------------------------------------------------------
        //  We support only a Cancel pushbutton
        //------------------------------------------------------
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1))
            {                           // Cancel key pressed
                case DID_CANCEL:        // Just quit quietly
                    WinDismissDlg (hwndDlg, FALSE);
                    return 0;
            }
            return 0;
                     
          
        //------------------------------------------------------
        //  On every timer tick, we computer the percent
        //  complete.  If different than on the last timer
        //  click, we update the progress bar.
        //------------------------------------------------------
        case WM_TIMER:
        {
            USHORT  usNewPctDone = 0;   // Percent complete

                                        // Get ptr to dialog data
            pprog = WinQueryWindowPtr (hwndDlg, QWL_USER);

                                        // Compute % complete
                                        // and update the slider
            pprog->usUnitsSoFar++;
                                        // Don't allow over 100
            usNewPctDone = (USHORT)min((100*pprog->usUnitsSoFar)
                                / pprog->usTotalUnits, 100);

                                        // If % done is changed,
                                        // update slider
            if (usNewPctDone != pprog->usPctDone)
            {
                pprog->usPctDone = usNewPctDone;
                
                WinSendDlgItemMsg (hwndDlg, IDSL_PROGRESS_BAR,
                            SLM_SETSLIDERINFO,
                            MPFROM2SHORT (SMA_SLIDERARMPOSITION,
                                            SMA_INCREMENTVALUE),
                            MPFROMSHORT (usNewPctDone));
            }

                                        // If all done, quit
            if (pprog->usUnitsSoFar > pprog->usTotalUnits)
                WinSendMsg (hwndDlg, WM_CLOSE, 0, 0);

            return 0;
        }

        //------------------------------------------------------
        //  All other messages go to default window procedure
        //------------------------------------------------------
        default:
            return (WinDefDlgProc (hwndDlg, msg, mp1, mp2));
    }

    return FALSE;
}

//--------------------------------------------------------------
// Function: InitSlider                                         
// Outputs:  none                                               
//                                                              
// This function receives all parameters for configuring a
// slider.  It sets tick marks, tick text, and detents 
// according to the input parameters.  The scale text font is 
// changed if requested.  This function works only on scale 1.
//--------------------------------------------------------------

VOID InitSlider (                       // Initialize slider
HWND    hwndDlg,                        // Handle of dialog
ULONG   idSlider,                       // ID of slider control
USHORT  usSizeMultiplier,               // Size multiplier
USHORT  usMinorTickSpacing,             // Minor tick spacing
USHORT  usMinorTickSize,                // Size of minor ticks
USHORT  usMajorTickSpacing,             // Major tick spacing
USHORT  usMajorTickSize,                // Size of major ticks
USHORT  usDetentSpacing,                // Detent spacing
USHORT  usTextSpacing,                  // Text label spacing
PSZ     pszFont)                        // Font for text or NULL
{
    USHORT  i;                          // Loop index
    CHAR    buffer[20];                 // String buffer
    USHORT  usIncrements = 0;           // Number of increments
    USHORT  usSpacing = 0;              // Spacing

    WNDPARAMS   wprm;                   // Window parameters ct;
    SLDCDATA    sldcd;                  // Slider control data

    HWND    hwndSlider = WinWindowFromID (hwndDlg, idSlider);

    //----------------------------------------------------------
    // Get original slider dimensions in increments for future
    // calls.  (My thanks to Wayne Kovsky for this sample code.)
    //----------------------------------------------------------
    wprm.fsStatus   = WPM_CTLDATA;      // Request control data
    wprm.cbCtlData  = sizeof (SLDCDATA);
    wprm.pCtlData   = &sldcd;

    if (WinSendMsg (hwndSlider, WM_QUERYWINDOWPARAMS,
                     MPFROMP(&wprm), 0))
    {                                   // Copy fields we need
        usIncrements = sldcd.usScale1Increments;
        usSpacing    = sldcd.usScale1Spacing;
    }
    
    //----------------------------------------------------------
    //  If requested, change dimensions of the slider
    //----------------------------------------------------------
    if (usSizeMultiplier > 1)
    {
        MRESULT mr;
        mr = WinSendMsg (hwndSlider,
                         SLM_QUERYSLIDERINFO,
                         MPFROMSHORT(SMA_SHAFTDIMENSIONS), 0);

        WinSendMsg (hwndSlider,
                    SLM_SETSLIDERINFO,
                    MPFROMSHORT(SMA_SHAFTDIMENSIONS),
                    MPFROMSHORT (SHORT2FROMMR(mr) *
                                             usSizeMultiplier));

    }

    //----------------------------------------------------------
    //  If requested, set minor ticks along axis
    //----------------------------------------------------------
    if (usMinorTickSpacing != 0)
        for (i = 0; i <= usIncrements; i += usMinorTickSpacing)
        {
            WinSendMsg (hwndSlider,     // Set minor tick
                        SLM_SETTICKSIZE,
                        MPFROM2SHORT(i, usMinorTickSize),
                        NULL);
        }

    //----------------------------------------------------------
    //  If requested, set major ticks along axis
    //----------------------------------------------------------
    if (usMajorTickSpacing != 0)
        for (i = 0; i <= usIncrements; i += usMajorTickSpacing)
        {
            WinSendMsg (hwndSlider,     // Set major tick
                        SLM_SETTICKSIZE,
                        MPFROM2SHORT(i, usMajorTickSize),
                        NULL);
        }

    //----------------------------------------------------------
    //  If requested, set detents
    //----------------------------------------------------------
    if (usDetentSpacing != 0)
        for (i = 0; i <= usIncrements; i += usDetentSpacing)
        {
            WinSendMsg (hwndSlider,     // Set the detent
                   SLM_ADDDETENT,
                   MPFROM2SHORT((i*usSpacing), usDetentSpacing),
                   NULL);
        }

    //----------------------------------------------------------
    //  If requested, set text labels.  Also change font if
    //  requested
    //----------------------------------------------------------
    if (usTextSpacing != 0)
    {
        if (pszFont != NULL)
            WinSetPresParam (hwndSlider,
                             PP_FONTNAMESIZE,
                             (strlen(pszFont)+1),
                             pszFont);
        for (i = 0; i <= usIncrements; i += usTextSpacing)
        {
            itoa (i, buffer, 10);       // Convert to string
            WinSendMsg (hwndSlider,     // Place in slider scale
                        SLM_SETSCALETEXT,
                        MPFROMSHORT(i),
                        MPFROMP(buffer));
        }
    }

    return;
}
