/* WINDOW.H

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

#ifdef WINDOW_C
#define EXTERN
#else
#define EXTERN extern
#endif

// Various window variables
EXTERN HAB hab;
EXTERN HWND hwndFrame;
EXTERN HWND hwndClient;

// For determining the title and menu bar heights
EXTERN HWND hwndMenu;

#undef EXTERN

void WindowSetTitle(char *szMapName);
// This function sets the window title
