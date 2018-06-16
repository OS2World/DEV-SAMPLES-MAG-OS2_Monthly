#define  INCL_PM
#define  INCL_DOS

#include <os2.h>
#include "stdappi.h"




VOID HlpDisplayHelpPanel(PPRVSTDAPP pPrvStdApp, SHORT idPanel);



BOOL       ValidateMenuHelpStdApp( PPRVSTDAPP pPrvStdApp );
BOOL       ValidateMenuExitStdApp( PPRVSTDAPP pPrvStdApp );
BOOL       ValidateMenuStdApp(PPRVSTDAPP pPrvStdApp);
BOOL       ValidateMenuServiceStdApp( PPRVSTDAPP pPrvStdApp );
MRESULT    CommandStdApp(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);
MRESULT    InitMenuStdApp(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2);


VOID       APIENTRY  EnableMenuItemStdApp( HWND hwndMenu
                                         , SHORT sIditem
                                         , BOOL bEnable);
HAB        APIENTRY  QueryHabStdApp( PPUBSTDAPP pPubStdApp );
HPS        APIENTRY  QueryHpsStdApp( PPUBSTDAPP pPubStdApp );
HWND       APIENTRY  QueryFrameStdApp( PPUBSTDAPP pPubStdApp );
HWND       APIENTRY  QueryClientStdApp( PPUBSTDAPP pPubStdApp );
BOOL       APIENTRY  MsgLoopStdApp( PPUBSTDAPP pPubStdApp );
BOOL       APIENTRY  QueryDevResStdApp( PPUBSTDAPP pPubStdApp
                                      , PULONG pulXRes
                                      , PULONG pulYRes );
BOOL       APIENTRY  QueryDevColorStdApp( PPUBSTDAPP pPubStdApp
                                        , PULONG pulColorPlanes
                                        , PULONG pulColorBitcount );
PPUBSTDAPP APIENTRY  DeleteStdApp( PPUBSTDAPP pPubStdApp );
MRESULT    EXPENTRY  ProcStdApp(HWND hwnd, USHORT msg, MPARAM mp1
                               , MPARAM mp2);



PPUBSTDAPP APIENTRY
NewStdApp( PVOID pvUserProc, ULONG flExclude, ULONG flServices)
{
   SWP         swpFrame;
   FRAMECDATA  fcData;
   PPRVSTDAPP  pPrvStdApp;
   PPUBSTDAPP  pPubStdApp;
   BOOL        fRc;


   pPubStdApp = (PPUBSTDAPP)calloc( 1, sizeof(PUBSTDAPP) );
   if ( pPubStdApp )
   {
      pPrvStdApp = (PPRVSTDAPP)calloc( 1, sizeof(PRVSTDAPP) );
      if ( pPrvStdApp )
      {
         APIRET apiret;
         // Our private data.
         pPubStdApp->pv = pPrvStdApp;



         pPrvStdApp->hab = WinInitialize(0);
         if ( !pPrvStdApp->hab )
            goto ErrorDrain;

         pPrvStdApp->hmq = WinCreateMsgQueue(pPrvStdApp->hab, 0);
         if ( !pPrvStdApp->hmq )
            goto ErrorDrain;

         fRc = UtlLoadString( pPrvStdApp->hab
                            , pPrvStdApp->hmodExe
                            , STDAPP_IDS_WIN_CLASSNAME
                            , MAXNAMEL
                            , pPrvStdApp->szClassName);
         if ( fRc == API_FAILED )
            goto ErrorDrain;



         fRc = UtlLoadString( pPrvStdApp->hab
                            , pPrvStdApp->hmodExe
                            , STDAPP_IDS_WIN_TITLE
                            , MAXNAMEL
                            , pPrvStdApp->szTitle);

         if ( fRc == API_FAILED )
            strcpy(pPrvStdApp->szTitle, pPrvStdApp->szClassName);


         fRc = WinRegisterClass( pPrvStdApp->hab
                               , (PSZ)pPrvStdApp->szClassName
                               , (PFNWP)ProcStdApp
                               , CS_SIZEREDRAW| CS_CLIPCHILDREN
         // An Extra two pointer slots are allocated.
         // The first is for the
         // user to have access to the slot indexed by
         // QWL_USER and the second
         // to hold pPubStdApp for our wndproc.
                               , (sizeof(PVOID) * 2) );
         if ( !fRc )
            goto ErrorDrain;

         // Is module loaded ?
         apiret = DosQueryModuleHandle( SERVICE_DLL_FULLNAME
                                      , &pPrvStdApp->hmodDll );
         if (apiret)
         {
            // Module not loaded let's try to load.
            apiret = DosLoadModule ( pPrvStdApp->szText1
                                   , STDAPP_TEXT_LENGTH
                                   , SERVICE_DLL_NAME
                                   , &pPrvStdApp->hmodDll );
            if (apiret)
            {
               UtlUserMsgBox( "Failed to load StdApp module."
                            , "StdApp Service"
                            , MB_ENTER | MB_ERROR
                            , TRUE
                            , 0 );

               goto ErrorDrain ;
            }
         }

         // set frame control data
         fcData.cb            = sizeof( fcData );
         fcData.hmodResources = (HMODULE)0;
         fcData.idResources   = STDAPP_IDR_WIN;

         // set the default frame window flags
         //ulCtlData = FCF_STANDARD;
         fcData.flCreateFlags = FCF_TITLEBAR      | FCF_SYSMENU  |
                                FCF_SIZEBORDER    | FCF_MINMAX   |
                                FCF_SHELLPOSITION | FCF_TASKLIST |
                                FCF_MENU          | FCF_ICON     |
                                FCF_ACCELTABLE    ;

         // give a caller chance to remove some standard frame controls.
         fcData.flCreateFlags &= ~(flExclude);

         pPrvStdApp->flServices = flServices;

         WinQueryTaskSizePos( pPrvStdApp->hab, 0, &swpFrame );

         pPrvStdApp->hFrame = WinCreateWindow( HWND_DESKTOP
                                             , WC_FRAME
                                             , pPrvStdApp->szTitle
                                             , 0UL
                                             , swpFrame.x
                                             , swpFrame.y
                                             , swpFrame.cx
                                             , swpFrame.cy
                                             , (HWND)0
                                             , HWND_TOP
                                             , STDAPP_IDR_WIN
                                             , &fcData
                                             , (PVOID)0);

         if (!pPrvStdApp->hFrame)
         {
            fRc = UtlLoadMessage( pPrvStdApp->hab
                                , pPrvStdApp->hmodDll
                                , IDMSG_FRAMEWINCREATEFAILED
                                , STDAPP_TEXT_LENGTH
                                , pPrvStdApp->szText1);

            if ( fRc ==API_SUCCESS )
              UtlDisplayError( pPrvStdApp->hab, pPrvStdApp->szText1);

            goto ErrorDrain;
         }

         pPrvStdApp->hClient = WinCreateWindow( pPrvStdApp->hFrame
                                              , (PSZ)pPrvStdApp->szClassName
                                              , (PSZ)0
                                              , 0UL
                                              , 0
                                              , 0
                                              , 0
                                              , 0
                                              , pPrvStdApp->hFrame
                                              , HWND_BOTTOM
                                              , FID_CLIENT
                                              , (PVOID)pPubStdApp
                                              , (PVOID)0 );


         if (!pPrvStdApp->hClient)
         {
            fRc = UtlLoadMessage( pPrvStdApp->hab
                                , pPrvStdApp->hmodDll
                                , IDMSG_CLIENTWINCREATEFAILED
                                , STDAPP_TEXT_LENGTH
                                , pPrvStdApp->szText1);

            if ( fRc ==API_SUCCESS )
              UtlDisplayError( pPrvStdApp->hab, pPrvStdApp->szText1);

            goto ErrorDrain;
         }

         pPrvStdApp->pfnUserProc = (PFNWP)pvUserProc;
         WinSetWindowPtr( pPrvStdApp->hClient, STDAPP_PTR_INDEX, pPubStdApp );
         UtlLoadMessage( pPrvStdApp->hab
                       , pPrvStdApp->hmodDll
                       , IDMSG_STDAPPSERVICE
                       , STDAPP_TEXT_LENGTH
                       , pPrvStdApp->szStdAppService );


         if (flServices & STDAPP_SERVICE_HELP || STDAPP_SERVICE_CONFIRM_EXIT)
         {

            ValidateMenuServiceStdApp( pPrvStdApp );
         }

         if (flServices & STDAPP_SERVICE_HPS)
         {
            SIZEL  sizl;

            pPrvStdApp->hdcClient = WinOpenWindowDC(pPrvStdApp->hClient);
            if (pPrvStdApp->hdcClient )
            {
                pPrvStdApp->hpsClient = GpiCreatePS( pPrvStdApp->hab
                                                   , pPrvStdApp->hdcClient
                                                   , &sizl
                                                   , PU_ARBITRARY
                                                   | GPIT_MICRO
                                                   | GPIA_ASSOC);

                if ( pPrvStdApp->hpsClient )
                {
                   if ( flServices & STDAPP_SERVICE_RGB )
                      GpiCreateLogColorTable( pPrvStdApp->hpsClient
                                            , 0L
                                            , LCOLF_RGB
                                            , 0L
                                            , 0L
                                            , (PLONG)NULL);

                   if (flServices & STDAPP_SERVICE_DEVRES)
                   {
                      // get the device resolutions
                      DevQueryCaps( pPrvStdApp->hdcClient
                                  , (LONG)CAPS_VERTICAL_RESOLUTION
                                  , 1L
                                  , &pPrvStdApp->cyRes);

                      DevQueryCaps( pPrvStdApp->hdcClient
                                  , CAPS_HORIZONTAL_RESOLUTION
                                  , 1L
                                  , &pPrvStdApp->cxRes);

                      DevQueryCaps( pPrvStdApp->hdcClient
                                  , CAPS_COLOR_PLANES
                                  , 1L
                                  , &pPrvStdApp->cColorPlanes);

                      DevQueryCaps( pPrvStdApp->hdcClient
                                  , CAPS_COLOR_BITCOUNT
                                  , 1L
                                  , &pPrvStdApp->cColorBitcount);
                   }
                }
            }
         }

//         WinInvalidateRect(pPrvStdApp->hFrame, (PRECTL)0, TRUE);

         WinShowWindow( pPrvStdApp->hFrame, TRUE );

//         WinShowWindow( pPrvStdApp->hClient, TRUE );

         // Callers methods
         pPubStdApp->pfnMsgLoop       = MsgLoopStdApp;
         pPubStdApp->pfnDelete        = DeleteStdApp;
         pPubStdApp->pfnQueryHps      = QueryHpsStdApp;
         pPubStdApp->pfnQueryHab      = QueryHabStdApp;
         pPubStdApp->pfnQueryFrame    = QueryFrameStdApp;
         pPubStdApp->pfnQueryClient   = QueryClientStdApp;
         pPubStdApp->pfnQueryDevColor = QueryDevColorStdApp;
         pPubStdApp->pfnQueryDevRes   = QueryDevResStdApp;
      }
      else
      {
         free(pPubStdApp);

         return (PPUBSTDAPP)0;
      }
   }

   return pPubStdApp;

ErrorDrain:

   pPubStdApp = DeleteStdApp(pPubStdApp);

   return pPubStdApp;

}  // end NewStdApp()






MRESULT EXPENTRY
ProcStdApp(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   MRESULT    mr;

   PPUBSTDAPP pPubStdApp = WinQueryWindowPtr( hwnd, STDAPP_PTR_INDEX);
   PPRVSTDAPP pPrvStdApp = (PPRVSTDAPP)0;
   BOOL       fReturnMr = FALSE;

   if ( pPubStdApp && pPubStdApp->pv )
      pPrvStdApp = pPubStdApp->pv;

   switch(msg)
   {

      case HM_QUERY_KEYS_HELP:
      {
         // return id of key help panel
         mr = (MRESULT)STDAPP_PANEL_HELPKEYS;
         fReturnMr = TRUE;
         break;
      }


      case WM_COMMAND:
      {
         mr = CommandStdApp( hwnd, msg, mp1, mp2);
         fReturnMr = TRUE;
         break;
      }

      case WM_INITMENU:
      {
         mr = InitMenuStdApp(hwnd, msg, mp1, mp2);
         fReturnMr = TRUE;
         break;
      }

     // The party line !
     // Any messages not processed are passed on
     // to the user's window proc.  It is
     // responsible for passing any messages it
     // doesn't handle onto WinDefWindowProc()

      default:
        // Fall through to exit code
        break;
   }

   if ( !fReturnMr )
   {
      if ( pPrvStdApp )
         mr = pPrvStdApp->pfnUserProc(hwnd, msg, mp1, mp2);
      else
         mr = WinDefWindowProc( hwnd, msg, mp1, mp2);
   }

   return mr;

}   // End of ProcStdApp()



// returns TRUE if msgloop executed atleast once
BOOL APIENTRY
MsgLoopStdApp(PPUBSTDAPP pPubStdApp)
{
   BOOL fRc = FALSE;

   if ( pPubStdApp && pPubStdApp->pv )
   {
      PPRVSTDAPP pPrvStdApp;
      QMSG       qmsg;        // message structure

      pPrvStdApp = pPubStdApp->pv;

      while(WinGetMsg(pPrvStdApp->hmq, (PQMSG)&qmsg, (HWND)0, 0, 0))
          WinDispatchMsg(pPrvStdApp->hmq, (PQMSG)&qmsg);


      fRc = TRUE;
   }

   return fRc;

}  // end MsgLoopStdApp()


PPUBSTDAPP APIENTRY
DeleteStdApp( PPUBSTDAPP pPubStdApp )
{
   if ( pPubStdApp && pPubStdApp->pv )
   {
      PPRVSTDAPP pPrvStdApp;

      pPrvStdApp = pPubStdApp->pv;
      if ( pPrvStdApp )
      {
         if ( pPrvStdApp->flServices & STDAPP_SERVICE_HELP )
            HlpDestroyHelpInstance(pPrvStdApp);

         // destroy the  window if it exists
         if (WinIsWindow(pPrvStdApp->hab, pPrvStdApp->hFrame))
            WinDestroyWindow(pPrvStdApp->hFrame);

         if ( pPrvStdApp->hmq )
            WinDestroyMsgQueue(pPrvStdApp->hmq);

         if ( pPrvStdApp->hab )
            WinTerminate(pPrvStdApp->hab);

         free(pPrvStdApp);
      }

      free(pPubStdApp);
   }

   return (PPUBSTDAPP)0;

}  // DeleteStdApp()


HAB APIENTRY
QueryHabStdApp( PPUBSTDAPP pPubStdApp )
{
   HAB hab = (HAB)0;

   if ( pPubStdApp && pPubStdApp->pv )
   {
      PPRVSTDAPP pPrvStdApp;

      pPrvStdApp = pPubStdApp->pv;
      if ( pPrvStdApp->hab )
         hab = pPrvStdApp->hab;
   }

   return hab;
}

HPS APIENTRY
QueryHpsStdApp( PPUBSTDAPP pPubStdApp )
{
   HPS hps = (HPS)0;

   if ( pPubStdApp && pPubStdApp->pv )
   {
      PPRVSTDAPP pPrvStdApp;

      pPrvStdApp = pPubStdApp->pv;

      if ( pPrvStdApp->hpsClient )
         hps = pPrvStdApp->hpsClient;
   }

   return hps;
}


HWND APIENTRY
QueryClientStdApp( PPUBSTDAPP pPubStdApp )
{
   HWND hClient = (HWND)0;

   if ( pPubStdApp && pPubStdApp->pv )
   {
      PPRVSTDAPP pPrvStdApp;

      pPrvStdApp = pPubStdApp->pv;

      if ( pPrvStdApp->hClient )
         hClient = pPrvStdApp->hClient;
   }

   return hClient;
}


BOOL APIENTRY
QueryDevResStdApp( PPUBSTDAPP pPubStdApp, PULONG pulXRes
                 , PULONG pulYRes )
{
   BOOL  fRc = API_FAILED;

   if ( pPubStdApp && pPubStdApp->pv )
   {
      PPRVSTDAPP pPrvStdApp;

      pPrvStdApp = pPubStdApp->pv;
      if ( pPrvStdApp )
      {
         *pulXRes  = pPrvStdApp->cxRes;
         *pulYRes  = pPrvStdApp->cyRes;
         fRc       = API_SUCCESS;
      }
   }

   return fRc;
}

BOOL APIENTRY
QueryDevColorStdApp( PPUBSTDAPP pPubStdApp, PULONG pulColorPlanes
                   , PULONG pulColorBitcount )
{
   BOOL  fRc = API_FAILED;

   if ( pPubStdApp && pPubStdApp->pv )
   {
      PPRVSTDAPP pPrvStdApp;

      pPrvStdApp         = pPubStdApp->pv;
      *pulColorPlanes    = pPrvStdApp->cColorPlanes;
      *pulColorBitcount  = pPrvStdApp->cColorBitcount;
      fRc     = API_SUCCESS;
   }

   return fRc;
}

HWND APIENTRY
QueryFrameStdApp( PPUBSTDAPP pPubStdApp )
{
   HWND hFrame = (HWND)0;

   if ( pPubStdApp && pPubStdApp->pv )
   {
      PPRVSTDAPP pPrvStdApp;

      pPrvStdApp = pPubStdApp->pv;

      if ( pPrvStdApp->hFrame )
         hFrame = pPrvStdApp->hFrame;
   }

   return hFrame;
}

MRESULT
InitMenuStdApp(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   PPUBSTDAPP pPubStdApp = WinQueryWindowPtr(hwnd, STDAPP_PTR_INDEX);
   MRESULT    mr         = 0;

   switch( SHORT1FROMMP(mp1) )
   {
      case STDAPP_IDM_HELP:
      {
         if ( pPubStdApp && pPubStdApp->pv )
         {
            PPRVSTDAPP pPrvStdApp = pPubStdApp->pv;
            BOOL       fEnable;

            // Enable or disable the Help menu depending upon whether the
            // help manager has been enabled
            fEnable = (pPrvStdApp->flServices & STDAPP_SERVICE_HELP);
            if ( fEnable )
            {
               EnableMenuItemStdApp( HWNDFROMMP(mp2)
                                   , STDAPP_IDM_HELPUSINGHELP
                                   , fEnable );

               EnableMenuItemStdApp( HWNDFROMMP(mp2)
                                   , STDAPP_IDM_HELPGENERAL
                                   , fEnable );

               EnableMenuItemStdApp( HWNDFROMMP(mp2)
                                   , STDAPP_IDM_HELPKEYS
                                   , fEnable );

               EnableMenuItemStdApp( HWNDFROMMP(mp2)
                                   , STDAPP_IDM_HELPINDEX
                                   , fEnable );
            }
         }
         break;
      }

      default:
         break;
   }

   return mr;

}   //       End of WndStdInitMenu()





MRESULT
CommandStdApp(HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{
   MRESULT    mr = 0;
   PPUBSTDAPP pPubStdApp = WinQueryWindowPtr(hwnd, STDAPP_PTR_INDEX);

   if ( pPubStdApp && pPubStdApp->pv )
   {
      PPRVSTDAPP pPrvStdApp = pPubStdApp->pv;

      switch(SHORT1FROMMP(mp1))
      {
         case STDAPP_IDM_EXIT:
         {
            BOOL fQuit = TRUE;

            if ( pPrvStdApp->flServices & STDAPP_SERVICE_CONFIRM_EXIT )
            {
               UtlSubsitiuteStrings( pPrvStdApp->hab
                                   , pPrvStdApp->hmodDll
                                   , IDMSG_CONFIRMEXITOF
                                   , pPrvStdApp->szText1
                                   , pPrvStdApp->szTitle);

               if ( (UtlUserMsgBox( pPrvStdApp->szText1
                                  , pPrvStdApp->szStdAppService
                                  , MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION   //MB_QUERY
                                  , FALSE
                                  , 0 )) == MBID_NO )
                  fQuit = FALSE;

            }

            if ( fQuit )
               WinPostMsg( hwnd, WM_QUIT, (MPARAM)0, (MPARAM)0 );

            break;
         }

         case STDAPP_IDM_HELPUSINGHELP:
         {
            HlpUsingHelp(pPrvStdApp);
            break;
         }

         case STDAPP_IDM_HELPGENERAL:
         {
            HlpGeneral(pPrvStdApp);
            break;
         }

         case STDAPP_IDM_HELPKEYS:
         {
            HlpKeys(pPrvStdApp);
            break;
         }

         case STDAPP_IDM_HELPINDEX:
         {
            HlpIndex(pPrvStdApp);
            break;
         }

         default:
         {
            // User command processing routine is called
            // here so any ids not procecessed here can be
            // processed

            pPrvStdApp = pPubStdApp->pv;
            mr         = pPrvStdApp->pfnUserProc(hwnd, msg, mp1, mp2) ;
         }
      }
   }

   return mr;
}  // End of CommandStdApp()





VOID  APIENTRY
EnableMenuItemStdApp( HWND hwndMenu, SHORT sIditem, BOOL bEnable)
{
  SHORT sFlag;

  if( bEnable )
    sFlag = 0;
  else
    sFlag = MIA_DISABLED;


  WinSendMsg( hwndMenu
            , MM_SETITEMATTR
            , MPFROM2SHORT(sIditem, TRUE)
            , MPFROM2SHORT(MIA_DISABLED, sFlag));

} // End of EnableMenuItemStdApp()







BOOL
ValidateMenuServiceStdApp( PPRVSTDAPP pPrvStdApp )
{
   BOOL   fRc=API_SUCCESS;

   if ( (fRc=ValidateMenuStdApp(pPrvStdApp)) == API_SUCCESS )
   {
      BOOL   fExitRc;
      BOOL   fHelpRc;

      if ( (fExitRc=ValidateMenuExitStdApp(pPrvStdApp)) == API_FAILED )
         pPrvStdApp->flServices &= ~( STDAPP_SERVICE_CONFIRM_EXIT );

      if ( (fHelpRc=ValidateMenuHelpStdApp(pPrvStdApp)) == API_FAILED )
         pPrvStdApp->flServices &= ~( STDAPP_SERVICE_HELP );

      if ( fExitRc == API_FAILED || fHelpRc == API_FAILED )
         fRc = API_FAILED;
   }
   else
   {
      pPrvStdApp->flServices &= ~( STDAPP_SERVICE_HELP
                                 | STDAPP_SERVICE_CONFIRM_EXIT );
      fRc = API_FAILED;
   }
   return fRc;
}





BOOL
ValidateMenuStdApp( PPRVSTDAPP pPrvStdApp )
{
static MT mt;

   USHORT   usMsgStyle = MB_CUAWARNING | MB_SYSTEMMODAL | MB_ENTER;
   BOOL     fRc = API_SUCCESS;

   pPrvStdApp->hMenu = WinWindowFromID( pPrvStdApp->hFrame, FID_MENU);

   if ( !pPrvStdApp->hMenu )
   {
      if ( !mt.cb )
         mt.cb = sizeof(mt);

      pPrvStdApp->hMenu = WinCreateMenu( pPrvStdApp->hFrame, &mt );

      if ( !pPrvStdApp->hMenu )
      {
         UtlLoadMessage( pPrvStdApp->hab
                       , pPrvStdApp->hmodDll
                       , IDMSG_ACTBARCREATEERR
                       , STDAPP_TEXT_LENGTH
                       , pPrvStdApp->szText1 );

         UtlUserMsgBox( pPrvStdApp->szText1
                      , pPrvStdApp->szStdAppService
                      , usMsgStyle
                      , FALSE
                      , 0 );

      fRc = API_FAILED;

      }
      else
         WinSendMsg( pPrvStdApp->hFrame
                   , WM_UPDATEFRAME
                   , MPFROMLONG(FCF_MENU)
                   , MPFROMLONG(0) );
   }

   return fRc;
}


BOOL
ValidateMenuExitStdApp( PPRVSTDAPP pPrvStdApp )
{

   MENUITEM mi;
   USHORT   usMsgStyle = MB_CUAWARNING | MB_SYSTEMMODAL | MB_ENTER;
   SHORT    sRc;
   BOOL     fRc = API_FAILED;

   if ( pPrvStdApp->flServices & STDAPP_SERVICE_CONFIRM_EXIT )
   {
      // To confirm exit there should be an Exit Menu Item
      fRc = (BOOL)WinSendMsg( pPrvStdApp->hMenu
                            , MM_QUERYITEM
                            , MPFROM2SHORT(STDAPP_IDM_EXIT,TRUE)
                            , &mi );

      if ( !fRc )
      {
         static PSZ apszExit[] = {
                                 EXIT_MENU_TEXT
                                 };

         static MENUITEM amiExit[] = {
                {MIT_END, MIS_SUBMENU, 0, STDAPP_IDM_EXIT, (HWND)0, (ULONG)0}
                                     };

         amiExit[0].hwndSubMenu = WinCreateWindow( pPrvStdApp->hClient
                                                 , WC_MENU
                                                 , apszExit[0]
                                                 , WS_VISIBLE
                                                 , 0
                                                 , 0
                                                 , 0
                                                 , 0
                                                 , pPrvStdApp->hMenu
                                                 , HWND_TOP
                                                 , amiExit[0].id
                                                 , (PVOID)0
                                                 , (PVOID)0 );

         if ( amiExit[0].hwndSubMenu )
         {

         WinSendMsg( pPrvStdApp->hFrame
                   , WM_UPDATEFRAME
                   , MPFROMLONG(FCF_MENU)
                   , MPFROMLONG(0) );

            sRc = (SHORT)WinSendMsg( pPrvStdApp->hMenu
                                  , MM_INSERTITEM
                                  , MPFROMP(&amiExit[0])
                                  , MPFROMP(apszExit[0]) );

            fRc = API_SUCCESS;
            if ( sRc == MIT_MEMERROR || sRc == MIT_ERROR )
            {

               UtlLoadMessage( pPrvStdApp->hab
                             , pPrvStdApp->hmodDll
                             , IDMSG_ADDHLPSUBMENUWIN
                             , STDAPP_TEXT_LENGTH
                             , pPrvStdApp->szText1 );

               UtlUserMsgBox( pPrvStdApp->szText1
                            , pPrvStdApp->szStdAppService
                            , usMsgStyle
                            , FALSE
                            , 0 );

               fRc = API_FAILED;

            } // end if ( sRc == MIT_MEMERROR || sRc == MIT_ERROR )
            else
               fRc = API_SUCCESS;

         }    // end  if ( amiExit[0].hwndSubMenu )
         else
         {
            UtlLoadMessage( pPrvStdApp->hab
                          , pPrvStdApp->hmodDll
                          , IDMSG_CREATEEXITSUBMENUWIN
                          , STDAPP_TEXT_LENGTH
                          , pPrvStdApp->szText1 );

            UtlUserMsgBox( pPrvStdApp->szText1
                         , pPrvStdApp->szStdAppService
                         , usMsgStyle
                         , FALSE
                         , 0 );
         }

      }       // end exit item does not exist
      else
         fRc = API_SUCCESS;

   }          // end if ( pPrvStdApp->flServices & STDAPP_SERVICE_CONFIRM_EXIT)

   if ( fRc == API_SUCCESS )
   {
      HWND hSysMenu;

      hSysMenu =  WinWindowFromID( pPrvStdApp->hFrame, FID_SYSMENU );

      if ( hSysMenu ) // with confirm on exit ACTIONBAR Exit is only way out.
         EnableMenuItemStdApp( hSysMenu, SC_CLOSE, FALSE);

   } // end if ( fRc == API_SUCCESS )

   return fRc;

}  // end ValidateMenuExitStdApp( PPRVSTDAPP pPrvStdApp )





BOOL
ValidateMenuHelpStdApp( PPRVSTDAPP pPrvStdApp )
{
   MENUITEM mi;
   USHORT   usMsgStyle = MB_CUAWARNING | MB_SYSTEMMODAL | MB_ENTER;
   SHORT    sRc;
   BOOL     fRc = API_FAILED;

   static PSZ apszHelp[] = {
                           HELP_MENU_TEXT,
                           HELP_INDEX_TEXT,
                           HELP_GENERAL_TEXT,
                           HELP_KEYS_TEXT,
                           HELP_USING_TEXT
                           };

   static MENUITEM amiHelp[] = {
   {MIT_END, MIS_SUBMENU, 0, STDAPP_IDM_HELP,          (HWND)0, (ULONG)0},
   {MIT_END, MIS_TEXT,    0, STDAPP_IDM_HELPINDEX,     (HWND)0, (ULONG)0},
   {MIT_END, MIS_TEXT,    0, STDAPP_IDM_HELPGENERAL,   (HWND)0, (ULONG)0},
   {MIT_END, MIS_TEXT,    0, STDAPP_IDM_HELPKEYS,      (HWND)0, (ULONG)0},
   {MIT_END, MIS_TEXT,    0, STDAPP_IDM_HELPUSINGHELP, (HWND)0, (ULONG)0}};

   // Check if user supplied Help menu item
   fRc = (BOOL)WinSendMsg( pPrvStdApp->hMenu
                         , MM_QUERYITEM
                         , MPFROM2SHORT(STDAPP_IDM_HELP,TRUE)
                         , &mi );

   if ( fRc )
   {
      HlpInitHelp(pPrvStdApp);
      fRc = API_SUCCESS;
   }
   else  // No User Provided Help
   {
      amiHelp[0].hwndSubMenu = WinCreateWindow( pPrvStdApp->hClient
                                              , WC_MENU
                                              , apszHelp[0]
                                              , WS_VISIBLE
                                              , 0
                                              , 0
                                              , 0
                                              , 0
                                              , pPrvStdApp->hMenu
                                              , HWND_TOP
                                              , amiHelp[0].id
                                              , (PVOID)0
                                              , (PVOID)0 );

      if ( amiHelp[0].hwndSubMenu )
      {
         sRc = (SHORT)WinSendMsg( pPrvStdApp->hMenu
                                  , MM_INSERTITEM
                                  , MPFROMP(&amiHelp[0])
                                  , MPFROMP(apszHelp[0]) );
         if ( sRc != MIT_MEMERROR && sRc != MIT_ERROR )
         {
            SHORT sMax = (STDAPP_IDM_HELP - STDAPP_IDM_HELPUSINGHELP) + 1;
            SHORT i;

            fRc = API_SUCCESS;

            // add menuitem(s) to submenu window
            for (i=1; i <sMax; i++)
            {
                sRc = (SHORT)WinSendMsg( amiHelp[0].hwndSubMenu
                                         , MM_INSERTITEM
                                         , MPFROMP(&amiHelp[i])
                                         , MPFROMP(apszHelp[i]) );

                if ( sRc == MIT_MEMERROR || sRc == MIT_ERROR )
                {
                   UtlLoadMessage( pPrvStdApp->hab
                                 , pPrvStdApp->hmodDll
                                 , IDMSG_ADDHLPSUBMENUWIN
                                 , STDAPP_TEXT_LENGTH
                                 , pPrvStdApp->szText1 );

                   UtlUserMsgBox( pPrvStdApp->szText1
                                , pPrvStdApp->szStdAppService
                                , usMsgStyle
                                , FALSE
                                , 0 );
                   fRc = API_FAILED;
                   break;
                } // end if ( sRc == MIT_MEMERROR || sRc == MIT_ERROR )

            WinSendMsg( pPrvStdApp->hFrame
                      , WM_UPDATEFRAME
                      , MPFROMLONG(FCF_MENU)
                      , MPFROMLONG(0) );

            } // end for (i=1; i <sMax; i++)

            if ( fRc != API_FAILED )
               HlpInitHelp(pPrvStdApp);

         }  // end if ( sRc != MIT_MEMERROR && sRc != MIT_ERROR )
         else
         {
            UtlLoadMessage( pPrvStdApp->hab
                          , pPrvStdApp->hmodDll
                          , IDMSG_CREATEHLPSUBMENUWIN
                          , STDAPP_TEXT_LENGTH
                          , pPrvStdApp->szText1 );

            UtlUserMsgBox( pPrvStdApp->szText1
                         , pPrvStdApp->szStdAppService
                         , usMsgStyle
                         , FALSE
                         , 0 );

            fRc = API_FAILED;
         }

      }  // end if ( amiHelp[0].hwndSubMenu )

      else
      {
         UtlUserMsgBox( pPrvStdApp->szText1
                      , pPrvStdApp->szStdAppService
                      , usMsgStyle
                      , FALSE
                      , 0 );

         fRc = API_FAILED;
      }

   }     // end else No User Provided Help

   return fRc;

}  // end ValidateMenuHelpStdApp( PPRVSTDAPP pPrvStdApp )









VOID APIENTRY
HlpInitHelp( PPRVSTDAPP pPrvStdApp )
{
  HELPINIT helpInit;
  BOOL     fRc;

  // if we return because of an error, Help will be disabled

  if ( !pPrvStdApp )
     return;



  // inititalize help init structure
  helpInit.cb                       = sizeof(HELPINIT);
  helpInit.ulReturnCode             = 0L;

  helpInit.pszTutorialName          = (PSZ)NULL;
  helpInit.phtHelpTable             = (PHELPTABLE)MAKELONG( STDAPP_HELP_TABLE,
                                                            0xFFFF);
  if ( pPrvStdApp->hmodDll )
     helpInit.hmodHelpTableModule   = pPrvStdApp->hmodDll;
  else
     helpInit.hmodHelpTableModule   = (HMODULE)0;

  helpInit.hmodAccelActionBarModule = (HMODULE)0;
  helpInit.idAccelTable             = 0;
  helpInit.idActionBar              = 0;


  fRc = UtlLoadString( pPrvStdApp->hab
                     , pPrvStdApp->hmodExe
                     , STDAPP_IDS_HELPWINDOWTITLE
                     , MAXNAMEL
                     , (PSZ)pPrvStdApp->szWindowTitle);

  if ( fRc == API_FAILED)
     goto ErrorDrain;


  helpInit.pszHelpWindowTitle = (PSZ)pPrvStdApp->szWindowTitle;

  helpInit.fShowPanelId = CMIC_HIDE_PANEL_ID;


  fRc = UtlLoadString( pPrvStdApp->hab
                     , pPrvStdApp->hmodExe
                     , STDAPP_IDS_HELPLIBRARYNAME
                     , CCHMAXPATH
                     , (PSZ)pPrvStdApp->szLibName);

  if ( fRc == API_FAILED)
     goto ErrorDrain;


  helpInit.pszHelpLibraryName = (PSZ)pPrvStdApp->szLibName;

  // creating help instance
  pPrvStdApp->hHelpInstance = WinCreateHelpInstance(pPrvStdApp->hab, &helpInit);

  if (!pPrvStdApp->hHelpInstance || helpInit.ulReturnCode)
  {
    fRc = UtlLoadMessage( pPrvStdApp->hab
                        , pPrvStdApp->hmodDll
                        , IDMSG_HELPLOADERROR
                        , STDAPP_TEXT_LENGTH
                        , (PSZ)pPrvStdApp->szText1);

    if ( fRc == API_SUCCESS)
       UtlDisplayError(pPrvStdApp->hab, (PSZ)pPrvStdApp->szText1);

    goto ErrorDrain;
  }


  // associate help instance with  frame
  fRc = (BOOL)WinAssociateHelpInstance( pPrvStdApp->hHelpInstance
                                      , pPrvStdApp->hFrame);
  if (!fRc )
  {
    fRc = UtlLoadMessage( pPrvStdApp->hab
                        , pPrvStdApp->hmodDll
                        , IDMSG_HELPLOADERROR
                        , STDAPP_TEXT_LENGTH
                        , (PSZ)pPrvStdApp->szText1);

    if ( fRc == API_SUCCESS)
      UtlDisplayError(pPrvStdApp->hab, (PSZ)pPrvStdApp->szText1);

    goto ErrorDrain;
  }

  return;

ErrorDrain:

  pPrvStdApp->flServices &= ~(STDAPP_SERVICE_HELP);

}   // End of HlpInitHelp()




VOID APIENTRY
HlpUsingHelp(PPRVSTDAPP pPrvStdApp)
{
   BOOL fRc;

//    this just displays the system help for help panel
    fRc =(BOOL)WinSendMsg( pPrvStdApp->hHelpInstance
                         , HM_DISPLAY_HELP
                         , 0
                         , 0);
    if ( fRc )
      UtlMsgBox( pPrvStdApp->hab
               , pPrvStdApp->hmodDll
               , IDMSG_HELPDISPLAYERROR
               , MB_ENTER | MB_ERROR, TRUE);
}   // End of HlpUsingHelp()




VOID APIENTRY
HlpGeneral(PPRVSTDAPP pPrvStdApp)
{
   BOOL fRc;

//   this just displays the system General help panel
     fRc = (BOOL)WinSendMsg( pPrvStdApp->hHelpInstance
                           , HM_EXT_HELP
                           , 0
                           , 0);
     if ( fRc )
         UtlMsgBox( pPrvStdApp->hab
                  , pPrvStdApp->hmodDll
                  , IDMSG_HELPDISPLAYERROR
                  , MB_ENTER | MB_ERROR, TRUE);
}       // End of HlpGeneral()




VOID APIENTRY
HlpKeys(PPRVSTDAPP pPrvStdApp)
{
   BOOL fRc;

// this just displays the system keys help panel
    fRc = (BOOL)WinSendMsg( pPrvStdApp->hHelpInstance
                          , HM_KEYS_HELP
                          , 0
                          , 0);
    if ( fRc )
      UtlMsgBox( pPrvStdApp->hab
               , pPrvStdApp->hmodDll
               , IDMSG_HELPDISPLAYERROR
               , MB_ENTER | MB_ERROR, TRUE);
}   //   End of HlpKeys()




VOID APIENTRY
HlpIndex(PPRVSTDAPP pPrvStdApp)
{
   BOOL fRc;

// this just displays the system help index panel
    fRc = (BOOL)WinSendMsg( pPrvStdApp->hHelpInstance
                          , HM_HELP_INDEX
                          , 0
                          , 0);
    if ( fRc )
      UtlMsgBox( pPrvStdApp->hab
               , pPrvStdApp->hmodDll
               , IDMSG_HELPDISPLAYERROR
               , MB_ENTER | MB_ERROR, TRUE);
}   // End of HlpIndex()




VOID
HlpDisplayHelpPanel(PPRVSTDAPP pPrvStdApp, SHORT idPanel)
{
   BOOL fRc;

   fRc = (BOOL)WinSendMsg( pPrvStdApp->hHelpInstance
                         , HM_DISPLAY_HELP
                         , MPFROM2SHORT(idPanel, 0)
                         , MPFROMSHORT(HM_RESOURCEID) );
   if ( fRc )
     UtlMsgBox( pPrvStdApp->hab
              , pPrvStdApp->hmodDll
              , IDMSG_HELPDISPLAYERROR
              , MB_ENTER | MB_ERROR, TRUE);
}  //    End of HlpDisplayHelpPanel()



VOID APIENTRY
HlpDestroyHelpInstance(PPRVSTDAPP pPrvStdApp)
{
  if (pPrvStdApp->flServices &    STDAPP_SERVICE_HELP)
  {
     pPrvStdApp->flServices  &= ~(STDAPP_SERVICE_HELP);
     WinDestroyHelpInstance(pPrvStdApp->hHelpInstance);
  }
}   //        End of HlpDestroyHelpInstance()














