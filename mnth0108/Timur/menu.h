/* MENU.H - Prototypes for menu functions

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.
*/

// Global variables
#ifdef MENU_C

int iMode=IDM_MODE_TARGET;
int iCurTer=0;

#else

extern int iMode,iCurTer;

#endif

void MainCommand(USHORT usCmd);
