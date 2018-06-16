# IBM Developer's Workframe/2 Make File Creation run at 00:11:08 on 07/29/92

# Make File Creation run in directory:
#   D:\P\MAGAZINE\SLIDER;

.SUFFIXES:

.SUFFIXES: .c .rc

ALL: SLIDER.EXE \
     SLIDER.RES

slider.exe:  \
  SETTIME.RES \
  SLIDER.OBJ \
  SLIDER.RES \
  SLIDER.MAK
   @REM @<<SLIDER.@0
     /A:16 /CO /F /M /ST:16384 /NOL /PM:PM +
     SLIDER.OBJ
     slider.exe
     
     
     ;
<<
   LINK386.EXE @SLIDER.@0
   RC SETTIME.RES slider.exe
   RC SLIDER.RES slider.exe

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Sm /Ss /Kbocgapexr /Ls /Le /Ti /W2 /C .\$*.c

!include SLIDER.DEP
