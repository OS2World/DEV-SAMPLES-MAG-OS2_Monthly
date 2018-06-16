/* MENU.H - Prototypes for menu functions
Copyright (c) 1992 Timur Tabi
Copyright (c) 1992 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.
*/

// Global variables
#ifdef MENU_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN USHORT usCurTer;                         // The Menu ID of the currently selected terrain type
EXTERN enum {MODE_TARGET, MODE_EDIT} eMode;     // What the user is currently doing


#undef EXTERN
void InitMenu(void);
void MainCommand(USHORT usCmd);
