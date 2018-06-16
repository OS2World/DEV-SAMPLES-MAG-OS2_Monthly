.*==============================================================*\
.*
.* Help.ipf - Information Tag Language file the screens for
.*              help on the Help menu
.*
.*==============================================================*/

.*--------------------------------------------------------------*\
.*  Main Help menu
.*      res = STDAPP_PANEL_HELP
.*--------------------------------------------------------------*/
:h1 res=9992 name=STDAPP_PANEL_HELP.Help.Menu
:i1 id=Help.Help Menu
:p.Insert standard Help menu text here.

.*--------------------------------------------------------------*\
.*  Help menu New command help panel
.*      res = STDAPP_PANEL_HELPHELPFORHELP
.*--------------------------------------------------------------*/
:h1 res=9991 name=STDAPP_PANEL_HELPHELPFORHELP.Help For Help
:i2 refid=Help.Help for Help
:p. Place information for the Help Help for Help menu here.


.*--------------------------------------------------------------*\
.*  Help menu Open command help panel
.*      res = STDAPP_PANEL_HELPEXTENDED
.*--------------------------------------------------------------*/
:h1 res=9990 name=STDAPP_PANEL_HELPEXTENDED.Extended
:i2 refid=Help.Extended
:p.Place information for the Help Extended menu item here.
:note.This screen will usually be handled by the default.


.*--------------------------------------------------------------*\
.*  Help menu Save command help panel
.*      res = STDAPP_PANEL_HELPKEYS
.*--------------------------------------------------------------*/
:h1 res=9989 name=STDAPP_PANEL_HELPKEYS.Keys
:i2 refid=Help.Keys
:p.Place information for the Help Keys menu item here.


.*--------------------------------------------------------------*\
.*  Help menu Save As command help panel
.*      res = STDAPP_PANEL_HELPINDEX
.*--------------------------------------------------------------*/
:h1 res=9988 name=STDAPP_PANEL_HELPINDEX.Index
:i2 refid=Help.Index
:p.Place information for the Help Index menu item here.

