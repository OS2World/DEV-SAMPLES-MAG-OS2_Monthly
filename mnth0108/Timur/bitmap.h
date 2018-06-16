/* BITMAP.H

Copyright (c) 1993 Timur Tabi
Copyright (c) 1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

void BitmapShutdown(void);
HBITMAP BitmapLoad(ULONG ulBitmapID);
void BitmapDraw(HBITMAP hbm, HBITMAP hbmMask, POINTL ptl);
