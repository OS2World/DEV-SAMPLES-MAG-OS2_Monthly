#===================================================================
#
#   Sample application makefile,common definitions for the IBM C
#   compiler environment
#===================================================================
.SUFFIXES: .rc .res .obj .c

#===================================================================
# Compilation macros.
#===================================================================

CC         = icc /c /Ms /Sm /Ti+ /O- /W3

LFLAGS   = /NOE /NOD /ALIGN:4 /CO /M /BASE:0x10000
LINK     = LINK386  $(LFLAGS)
STLIBS   = DDE4SBS + OS2386

.c.obj:
    $(CC) -Fo$*.obj $*.c

.rc.res:
        rc -r $*.rc

HEADERS = cascade.h

ALL_OBJS = cascade.obj

all: cascade.exe

cascade.lnk: cascade.mak
    echo $(ALL_OBJS)         >  cascade.lnk
    echo cascade.exe         >> cascade.lnk
    echo cascade.map         >> cascade.lnk
    echo $(STLIBS)           >> cascade.lnk
    echo cascade.def         >> cascade.lnk

cascade.res: cascade.rc $(HEADERS)

cascade.obj: cascade.c $(HEADERS)


cascade.exe: $(ALL_OBJS) cascade.def cascade.res cascade.lnk
    $(LINK) $(EFLAGS) @cascade.lnk
    rc cascade.res cascade.exe
