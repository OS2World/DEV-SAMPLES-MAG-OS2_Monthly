#define  INCL_PM
#define  INCL_DOS

#include <os2.h>
#include <stdio.h>
#include "main.h"
#include "..\APIDLL\stdappu.h"




MRESULT UserWndProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);


static	PPUBSTDAPP  pPubStdApp;


INT
main (VOID)
{
   // Allow StdApp to use RC menu resource.
   //ULONG   flExclude	       = (ULONG)0;

   // Allow StdApp to dynamically build ActionBar for us.
   ULONG   flExclude	       =  FCF_MENU;


   // Special services we want StdApp to do for us.
   ULONG   flServices	       = ( STDAPP_SERVICE_HPS	       |
			                          STDAPP_SERVICE_RGB	       |
				                       STDAPP_SERVICE_DEVRES     |
				                       STDAPP_SERVICE_HELP	    |
                                   STDAPP_SERVICE_CONFIRM_EXIT );



   pPubStdApp = NewStdApp( (PVOID)UserWndProc, flExclude, flServices);
   if ( !pPubStdApp )
      return(USER_RETURN_ERROR);


   pPubStdApp->pfnMsgLoop(pPubStdApp);


   //	   Any other system resources used
   //	   (e.g. memory or files) should be freed here

   pPubStdApp = pPubStdApp->pfnDelete(pPubStdApp);

   return USER_RETURN_SUCCESS;

} // End of main porcedure




MRESULT EXPENTRY
UserWndProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
  MRESULT mr;

  switch(msg)
  {

    case WM_COMMAND:
    {
       switch( SHORT1FROMMP(mp1) )
       {
          case IDMU_MSG:
          {
             break;
          }

       }  //  end switch(SHORT1FROMMP(mp1))

       break;
    }

    case WM_PAINT:
    {
       RECTL	rcl;

       // We get the hps because we requested one when we called
       // NewStdApp and we wanted it to handle RGB colors.
       HPS	hps = pPubStdApp->pfnQueryHps(pPubStdApp);

       hps = WinBeginPaint(hwnd, hps, (PRECTL)&rcl);

       WinFillRect( hps, &rcl, RGB_BLUE );

       WinEndPaint(hps);

       break;
    }

    default:
       // Default must call WinDefWindowProc()
       mr = WinDefWindowProc(hwnd, msg, mp1, mp2);
       return mr;
  }

  return (MRESULT)0;

}   //	  End of UserWndProc()








