#===================================================================
#
#   Sample application makefile,common definitions for the IBM C
#   compiler environment
#===================================================================
.SUFFIXES:
.SUFFIXES: .rc .res .obj .lst .c .asm .hlp .itl .ipf
#===================================================================
# Default compilation macros for sample programs
#
# Compile switchs  that are enabled
# /c      compile don't link
# /Gd-    statically link to runtime library
# /Gd+    dynamically link to runtime library
# /Ge-    Create a Dll
# /Gm+    use the multi-threaded libraries
# /Kb+    Produce basic diagnostic messages
# /Ms     use the system calling convention and not optilink
# /Re     Generate code to run in a C Set/2 runtime environment
# /Se     allow C Set/2  extensions
# /sm     allow migration extensions
# /ss     enable c++ extensions like "//" for comment lines
# /Ti     generate PM debugger information  with /Yc
# /W3     warning level three
#===================================================================

# /sm /W3 /D__MIG_LIB__

CC   = icc /c /Gd+ /Ge- /Gm+ /Kb+ /Ms /Re /Se /ss /Ti+

#/B"/NOI /CODEVIEW /ALIGN:16 /EXEPACK /M"
IMPL = implib

AFLAGS  = /Mx -t -z
ASM     = ml /c /Zm
#LFLAGS   = /NOI /CODEVIEW /ALIGN:16 /EXEPACK /M
LFLAGS   = /NOE /NOD /CODEVIEW /ALIGN:16 /EXEPACK /M
#LFLAGS   = /NOE /NOD  /ALIGN:16 /EXEPACK /M
LINK    = LINK386  $(LFLAGS)
DLLLIBS = DDE4NBS + os2386

.c.lst:
    $(CC) -Fc$*.lst -Fo$*.obj $*.c

.c.obj:
    $(CC) -Fo$*.obj $*.c

.asm.obj:
    $(ASM)   $*.asm

.ipf.hlp:
        ipfc $*.ipf /W3

.itl.hlp:
        cc  -P $*.itl
        ipfc $*.i
        del $*.i

.rc.res:
        rc -r -p -x $*.rc


