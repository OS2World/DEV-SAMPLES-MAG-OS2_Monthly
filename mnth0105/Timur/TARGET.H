/* TARGET.H

Copyright (c) 1992 Timur Tabi
Copyright (c) 1992 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

void TgtInitialize(HWND);
// Initializes the targetting structure

void TgtShutdown(void);
// Shuts the targetting mechanism down

void TgtStart(HEXINDEX);
// Activates targetting

void TgtEnd(void);
// Terminates targetting

void TgtMove(HEXINDEX);
// Moves the targetting line
