/* BITMAP.H

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

class BITMAP {
  HBITMAP hbm;
  HBITMAP hbmMask;
  void init(int ID, int maskID, POINTL ptl1);
public:
  POINTL ptl;
  BITMAP(int, int=0);
  BITMAP(POINTL, int, int=0);
  ~BITMAP(void);
  void draw(void);
  void quickdraw(void);
};
