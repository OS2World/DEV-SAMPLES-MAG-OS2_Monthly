            Listing for makefile                                        Page 1

            1|   #--------------------
            2|   # edit make file
            3|   #--------------------
            4|   
            5|   edit.exe : edit.obj efile.obj edlg.obj edit.def edit.res edit.hlp
            6|       link edit+efile+edlg, /align:16, NUL, os2, edit
            7|       rc edit.res
            8|   
            9|   edit.hlp : edit.ipf
           10|      ipfc edit.ipf
           11|                   
           12|   edit.obj : edit.c edit.h
           13|      cl -c -G2sw -W3 edit.c
           14|        
           15|   efile.obj : efile.c edit.h
           16|      cl -c -G2sw -W3 efile.c
           17|       
           18|   edlg.obj : edlg.c edit.h
           19|      cl -c -G2sw -W3 edlg.c
           20|   
           21|   edit.res : edit.rc edit.h edit.ico
           22|       rc -r edit.rc
           23|   
