#define  INCL_PM

#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "utl.h"


BOOL APIENTRY
UtlDisplayError(HAB hab, PSZ pszString)
{

  CHAR      szBuffer[80];
  LONG      errid;
  SHORT     sOS2Rc    = 0;
  PSHORT    psOS2Rc;
  PSZ       pszErrMsg = "";
  PSHORT    psOffset;
  PERRINFO  perriInfo = (PERRINFO)0;
  PSZ       pszTitle;

  // Based on code found in on Winn - OS/2 PM GPI

  // get last error
  errid = WinGetLastError(hab);


  // if last error anything but PMERR_OK display the message box
  if (ERRORIDERROR(errid) != PMERR_OK)
  {

     // get error info

     perriInfo = WinGetErrorInfo(hab);

     // get PM error message pointer

     psOffset = MAKEP( SELECTOROF(perriInfo)
                     , (perriInfo->offaoffszMsg +
                        OFFSETOF(perriInfo)));

     pszErrMsg = MAKEP( SELECTOROF(perriInfo)
                      , (*psOffset + OFFSETOF(perriInfo)));

     // if error code is PMERR_BASE_ERROR get base OS/2 error code

     if (ERRORIDERROR(errid) == PMERR_BASE_ERROR)
     {
        psOS2Rc = MAKEP( SELECTOROF(perriInfo)
                       , (perriInfo->offBinaryData +
                          OFFSETOF(perriInfo)));

        sOS2Rc  = *psOS2Rc;

        // format the message string
        sprintf( szBuffer,"%x/%x/%d/%s %s\n"
               , ERRORIDERROR(errid) // PM error code
               , ERRORIDSEV(errid)   // severity
               , sOS2Rc              // base OS/2 error number
               , pszErrMsg           // PM error message
               , pszString);         // callers string

        // message box title
        pszTitle = "Error/Severity/OS2RC";
     }
     else // format the message string
     {
        sprintf( szBuffer,"%x/%x/ %s\n%s"
               , ERRORIDERROR(errid)    // PM error code
               , ERRORIDSEV(errid)      // severity
               , pszErrMsg              // PM error message
               , pszString);            // callers string

         // message box title
         pszTitle = "Error/Severity";
     }
  }



  // display the error

  WinMessageBox( HWND_DESKTOP   // parent window
               , (HWND)0        // owner window
               , szBuffer       // message text
               , pszTitle       // message box title
               , 1              // message box id
               , MB_CUACRITICAL // window style
               | MB_SYSTEMMODAL
               | MB_ENTER
               );

  // free the error info

  if (perriInfo)
    WinFreeErrorInfo(perriInfo);

  return API_FAILED;
}  // End  UtlDisplayError()




BOOL APIENTRY
UtlLoadString( HAB hab, HMODULE hmod, USHORT usMsgID,
               USHORT usSize, PSZ pszBuffer)
{

   if (!WinLoadString(hab, hmod, usMsgID, usSize, pszBuffer))
   {
      UtlUserMsgBox( "Failed to load string."
                   , "Ult.c"
                   , MB_ENTER|MB_ERROR
                   , TRUE
                   , 0 );

      return API_FAILED;
   }

   return API_SUCCESS;

}  // End of UtlLoadString()


BOOL APIENTRY
UtlLoadMessage(HAB hab, HMODULE hmod, USHORT usMsgID,
               USHORT usSize, PSZ pszBuffer)
{

   if (!WinLoadMessage(hab, hmod, usMsgID, usSize, pszBuffer))
   {

      UtlUserMsgBox( "Failed to load message."
                   , "Ult.c"
                   , MB_ENTER|MB_ERROR
                   , TRUE
                   , 0 );

      return API_FAILED;
   }

   return API_SUCCESS;

}   // End of UtlLoadMessage()


PSZ APIENTRY
UtlSubsitiuteStrings( HAB hab, HMODULE hmod, USHORT usMsgID, PSZ szTarget,
                      PSZ szSub1)
{
  PSZ     pszReturnString = (PSZ)0;
  CHAR    szFormat[UTLMAG_TEXT_LENGTH];
  BOOL    fRc;

  fRc = UtlLoadMessage( hab
                      , hmod
                      , usMsgID
                      , UTLMAG_TEXT_LENGTH
                      , szFormat);

  if ( fRc == API_SUCCESS )
  {
     //sprintf(szTarget, szFormat, szSub1);
     strcpy(szTarget, szFormat);
     strcat(szTarget, szSub1);
     pszReturnString = szTarget;
  }

  return pszReturnString;
}


USHORT APIENTRY
UtlMsgBox( HAB hab, HMODULE hmod, USHORT usMsgID,
           USHORT fsStyle, BOOL fBeep)
{
USHORT  usRc;
CHAR    szText[128];
BOOL    fRc;

  fRc = UtlLoadMessage( hab
                      , hmod
                      , usMsgID
                      , 128
                      , (PSZ)szText);

  if ( fRc == API_FAILED )
  {
     WinAlarm( HWND_DESKTOP
             , WA_ERROR);

     return MBID_ERROR;
  }

  if (fBeep)
     WinAlarm( HWND_DESKTOP
             , WA_ERROR);

  usRc = WinMessageBox( HWND_DESKTOP
                      , (HWND)0
                      , (PSZ)szText
                      , (PSZ)"Message Box"
                      , IDD_MSGBOX
                      , fsStyle);
  return usRc;

}  // End of UtlMsgBox()



USHORT APIENTRY
UtlUserMsgBox( PSZ     pszMsg
             , PSZ     pszMsgBoxTitle
             , USHORT  fsStyle
             , BOOL    fBeep
             , USHORT  usMsgID )
{
   USHORT usRc;

   if (fBeep)
      WinAlarm( HWND_DESKTOP
              , WA_ERROR);

   usRc = WinMessageBox( HWND_DESKTOP
                       , (HWND)0
                       , pszMsg
                       , pszMsgBoxTitle
                       , usMsgID
                       , fsStyle);
   return usRc;
}



USHORT APIENTRY
UtlDisplayDebug(PSZ pszText, PSZ pszTitle)
{
  USHORT usRc;

  usRc = WinMessageBox( HWND_DESKTOP
                      , (HWND)0
                      , pszText
                      , pszTitle
                      , 0
                      , MB_ENTER);
  return usRc;
}  // End of UtlDisplayDebug()




