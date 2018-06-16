/* ERRORMSG.H - Error messages

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

This header file should only be included into ERRORS.C
*/

typedef struct {
  ERROR ec;
  char *szMsg;
} MESSAGE;

MESSAGE amsg[]= {
{ERR_BITMAP                             ,"BITMAP"},
{ERR_BITMAP_SHUT                        ,"BitmapShutdown"},
{ERR_BITMAP_SHUT_PS                     ,"Could not destroy PS"},             // Could not destroy PS
{ERR_BITMAP_SHUT_DC                     ,"Could not close DC"},               // Could not close DC

{ERR_BITMAP_LOAD                        ,"BitmapLoad"},
{ERR_BITMAP_LOAD_DC                     ,"Could not open DC"},                // Could not open DC
{ERR_BITMAP_LOAD_PS                     ,"Could not create PS"},              // Could not create PS
{ERR_BITMAP_LOAD_HBM                    ,"Could not load bitmap"},            // Could not load bitmap

// mech.c
{ERR_MECH                               ,"MECH"},

// menu.c
{ERR_MENU                               ,"MENU"},

// target.c
{ERR_TARGET                             ,"TARGET"},

// files.c
{ERR_FILES                              ,"FILES"},
{ERR_FILES_DLG                          ,"WinFileDlg() failed"},

// terrain.c
{ERR_TERR                               ,"TERRAIN"},
{ERR_TERR_INIT                          ,"TerrainInit"},

// SENTINEL
{0,"Unknown Error: %u"}};
