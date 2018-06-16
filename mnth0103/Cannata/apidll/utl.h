#ifndef INCL_UTL
#define INCL_UTL

#ifndef API_SUCCESS
#define API_SUCCESS   1
#endif

#ifndef API_FAILED
#define API_FAILED    0
#endif


#ifndef UTLMAG_TEXT_LENGTH
#define UTLMAG_TEXT_LENGTH    128
#endif


BOOL  APIENTRY UtlDisplayError( HAB hab
                              , PSZ pszString);

// Message box ids
#define IDD_MSGBOX                    9500

//  Messagetable ids
#define IDMSG_INITFAILED                 1
#define IDMSG_FRAMEWINCREATEFAILED       2
#define IDMSG_CLIENTWINCREATEFAILED      3
#define IDMSG_CANNOTGETHPS               4
#define IDMSG_HELPLOADERROR              5
#define IDMSG_CANNOTLOADSTRING           6
#define IDMSG_CANNOTOPENPRINTER          7
#define IDMSG_HELPDISPLAYERROR           8
#define IDMSG_CANNOTALLOCATEMEMORY       9
#define IDMSG_GPISETCOLOR               10
#define IDMSG_GPISETCURRENTPOSITION     11
#define IDMSG_GPIFULLARC                12
#define IDMSG_GPIBEGINPATH              13
#define IDMSG_GPICLOSEFIGURE            14
#define IDMSG_GPIENDPATH                15
#define IDMSG_GPIFILLPATH               16
#define IDMSG_GPISETPAGEVIEWPORT        17
#define IDMSG_GPISETARCPARAMS           18
#define IDMSG_CANNOTLOADMESSAGE         19
#define IDMSG_CANNOTLOADMODULE          20
#define IDMSG_CONFIRMEXITOF             21
#define IDMSG_STDAPPSERVICE             22


#define IDMSG_ACTBARCREATEERR           23
#define IDMSG_CREATEHLPSUBMENUWIN       24
#define IDMSG_ADDHLPSUBMENUWIN          25
#define IDMSG_CREATEEXITSUBMENUWIN      26
#define IDMSG_ADDEXITSUBMENUWIN         27
#define IDMSG_CONFIRMEXITDLG            28


USHORT APIENTRY UtlUserMsgBox( PSZ     pszMsg
                             , PSZ     pszMsgBoxTitle
                             , USHORT  fsStyle
                             , BOOL    fBeep
                             , USHORT  usMsgID );

PSZ APIENTRY  UtlSubsitiuteStrings( HAB     hab
                                  , HMODULE hmod
                                  , USHORT  usMsgID
                                  , PSZ     szTarget
                                  , PSZ     szSub1);

BOOL   APIENTRY UtlLoadMessage( HAB     hab
                              , HMODULE hmod
                              , USHORT  usMsgID
                              , USHORT  usSize
                              , PSZ     pszBuffer);

BOOL   APIENTRY UtlLoadString( HAB     hab
                             , HMODULE hmod
                             , USHORT  usMsgID
                             , USHORT  usSize
                             , PSZ     pszBuffer);

USHORT APIENTRY UtlMsgBox( HAB      hab
                         , HMODULE  hmod
                         , USHORT   MsgID
                         , USHORT   fsStyle
                         , BOOL     fBeep);
#endif

