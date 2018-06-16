/* TARGET0.H

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

void APIENTRY Highlight(ULONG ul);
// Highlights the source hexagon.  Intended to be started as a separate thread

int ExitSide(HEXINDEX hi);
// Returns the exit side of hexagon hi.

int FirstSide(HEXINDEX hiFrom, HEXINDEX hiTo);
// Returns the side of hiFrom through which a trajectory, from hiFrom to HiTo, would exit.
