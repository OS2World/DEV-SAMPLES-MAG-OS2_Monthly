/* TARGET.HPP

Copyright (c) 1992-1993 Timur Tabi
Copyright (c) 1992-1993 Fasa Corporation

The following trademarks are the property of Fasa Corporation:
BattleTech, CityTech, AeroTech, MechWarrior, BattleMech, and 'Mech.
The use of these trademarks should not be construed as a challenge to these marks.

*/

class TARGET {
  HPS hpsLine;                     // The HPS for target-line drawing
  HPS hpsPath;
  HEX hexStart;
  HEX hexEnd;
  POINTL ptlStart;                 // The X,Y coordinate of the line's origin
  POINTL ptlEnd;                   // The X,Y coordinate of the line's end
  float m;                         // The slope of the targetting line
  float b;                         // The intercept of the targetting line
  int range;                       // The range, in hexagons, from start to end
  int angle;                       // The angle, in degrees, where 0 is to the right
  int iVis;                        // Visibility
  int dx;                          // = ptlEnd.x - ptlStart.x
  void InitPath(void);
  void ShowPath(void);
  int NearestSlope(int iSide1, int iSide2);
  int ExitSide(HEX hex);
  int FirstSide(void);
  int isClose(POINTL ptl);
  int GetRange(void);
  int Intersect(POINTL ptl1, POINTL ptl2);
public:
  TARGET(HEX);
  ~TARGET(void);
  void move(HEX);
};

#ifdef TARGET_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN TARGET *currentTarget;
EXTERN long lNumColors;                        // The number of colors, for Highlight()
EXTERN HWND hwndInfoBox;

#undef EXTERN
