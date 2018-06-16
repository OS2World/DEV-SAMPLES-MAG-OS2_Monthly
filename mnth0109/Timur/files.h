/* FILES.H - Prototypes for file I/O functions

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.
*/

void FileInit(void);
// This function initializes the FILES module

ERROR FileOpen(char *szTitle, char *szFileName);
//

ERROR FileSave(char *szTitle, char *szFileName);
//
