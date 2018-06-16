.*==============================================================*\
.*
.* Main.ipf - Information Tag Language file for the main
.*              sample application's help manager
.*
.*==============================================================*/
:userdoc.

.*--------------------------------------------------------------*\
.*  Main window extended help panel
.*      res = STDAPP_PANEL_MAIN
.*--------------------------------------------------------------*/
:h1 res=9987 name=STDAPP_PANEL_MAIN.About Itself
:i1 id=aboutMain.About Itself
:p.The StdApp API provides the basic and advanced access to a standard
window. Here come the SOMs.

.*-- Import the Help menu help file --
.im hlp.ipf


:euserdoc.
