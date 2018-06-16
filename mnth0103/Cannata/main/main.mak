include     ..\exe.inc
LIBS      = ..\API.LIB + DDE4NBS + os2386
#LIBS      =  DDE4NBS + os2386
HEADERS   = ..\APIDLL\StdAppu.h ..\APIDLL\utl.h main.h
ALL_OBJ1  = main.obj
ALL_IPF   = main.ipf hlp.ipf
all: main.lnk main.exe main.hlp

main.res:   $(HEADERS) main.ico main.rc
            rc -r main.rc

main.lnk: main.mak
          echo $(ALL_OBJ1)               > main.lnk
          echo main.exe                 >> main.lnk
          echo main.map                 >> main.lnk
          echo $(LIBS)                  >> main.lnk
          echo main.def                 >> main.lnk

main.res:     main.rc   $(HEADERS)

main.obj:     main.c    $(HEADERS)


#=============== help file generation ====================#
main.hlp: $(ALL_IPF)

main.exe: $(ALL_OBJ1) main.def main.res
    $(LINK) @main.lnk
    rc main.res main.exe

