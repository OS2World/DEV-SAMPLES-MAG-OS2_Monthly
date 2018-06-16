#--------------------
# edit make file
#--------------------

edit.exe : edit.obj efile.obj edlg.obj edit.def edit.res edit.hlp
    link edit+efile+edlg, /align:16, NUL, os2, edit
    rc edit.res

edit.hlp : edit.ipf
   ipfc edit.ipf
                
edit.obj : edit.c edit.h
   cl -c -G2sw -W3 edit.c
     
efile.obj : efile.c edit.h
   cl -c -G2sw -W3 efile.c
    
edlg.obj : edlg.c edit.h
   cl -c -G2sw -W3 edlg.c

edit.res : edit.rc edit.h edit.ico
    rc -r edit.rc
