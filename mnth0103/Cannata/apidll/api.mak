include ..\dll.inc
DLLS    = api.dll
IMPLIBS = api.lib
HEADERS  = StdAppi.h StdAppu.h   utl.h
DLL_OBJ1 = StdApp.obj            utl.obj

all: api.res $(DLLS) $(IMPLIBS)

StdWnd.obj:  StdApp.c $(HEADERS)

Utl.obj:     Utl.c    $(HEADERS)


api.res:   $(HEADERS) api.rc
            rc -r api.rc

api.lnk: api.mak
    echo $(DLL_OBJ1)           > api.lnk
    echo $(DLLS)               >> api.lnk
    echo api.map               >> api.lnk
    echo $(DLLLIBS)            >> api.lnk
    echo api.def               >> api.lnk




$(DLLS)   : $(DLL_OBJ1) api.def api.lnk api.mak
            $(LINK) @api.lnk
            rc api.res api.dll
            copy api.dll ..\

$(IMPLIBS): $(DLLS)
            $(IMPL) $(IMPLIBS) api.def
            copy api.lib ..\



