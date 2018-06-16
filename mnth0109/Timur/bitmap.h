/* BITMAP.H

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

ERROR BitmapShutdown(void);
// Shuts down the bitmap module

ERROR BitmapLoad(ULONG ulBitmapID, PHBITMAP phbm);
// Loads the bitmap from the exeutables resources.

void BitmapDraw(HBITMAP hbm, HBITMAP hbmMask, POINTL ptl);
// Draws bitmap hbm, with optional mask hbmMask, at location ptl
