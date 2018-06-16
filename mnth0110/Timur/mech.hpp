/* MECH.H

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

needs: hexes.hpp, bitmap.hpp
*/

class MECH : public HEX {
  int iDirection;
  BITMAP *pbmp[6];         // One bitmap for each direction
  void ShowPosition(void);
public:
  MECH(void);
  ~MECH(void);
  void move(HEX);
  void draw(void);
};

#ifdef MECH_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN MECH *currentMech;

#undef EXTERN
