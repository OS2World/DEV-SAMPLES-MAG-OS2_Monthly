all: subclass.exe

SUBCLASS.obj : SUBCLASS.C
    cl /c /W4 /AL /Zpi /G2s /Od subclass.c

ENTRYFLD.obj : ENTRYFLD.C
    cl /c /W4 /AL /Zpi /G2s /Od ENTRYFLD.c

SUBCLASS.res : SUBCLASS.RC
    rc -r SUBCLASS.rc SUBCLASS.res

subclass.exe : subclass.res entryfld.obj subclass.obj
    link /co subclass.obj+entryfld.obj,subclass.exe,NUL,LLIBCEP+OS2,subclass.def
    rc SUBCLASS.res subclass.exe
