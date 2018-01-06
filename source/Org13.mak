# Microsoft Developer Studio Generated NMAKE File, Based on Org13.dsp
!IF "$(CFG)" == ""
CFG=Org13 - Win32 Debug
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの Org13 - Win32 Debug を設定します。
!ENDIF 

!IF "$(CFG)" != "Org13 - Win32 Release" && "$(CFG)" != "Org13 - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "Org13.mak" CFG="Org13 - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "Org13 - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "Org13 - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Org13 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Org211.exe" "$(OUTDIR)\Org13.bsc"


CLEAN :
	-@erase "$(INTDIR)\Click.obj"
	-@erase "$(INTDIR)\Click.sbr"
	-@erase "$(INTDIR)\DlgEdit.obj"
	-@erase "$(INTDIR)\DlgEdit.sbr"
	-@erase "$(INTDIR)\DlgEZCopy.obj"
	-@erase "$(INTDIR)\DlgEZCopy.sbr"
	-@erase "$(INTDIR)\DlgPlayer.obj"
	-@erase "$(INTDIR)\DlgPlayer.sbr"
	-@erase "$(INTDIR)\DlgSet.obj"
	-@erase "$(INTDIR)\DlgSet.sbr"
	-@erase "$(INTDIR)\DlgTrack.obj"
	-@erase "$(INTDIR)\DlgTrack.sbr"
	-@erase "$(INTDIR)\Filer.obj"
	-@erase "$(INTDIR)\Filer.sbr"
	-@erase "$(INTDIR)\Gdi.obj"
	-@erase "$(INTDIR)\Gdi.sbr"
	-@erase "$(INTDIR)\MainPut.obj"
	-@erase "$(INTDIR)\MainPut.sbr"
	-@erase "$(INTDIR)\Mouse.obj"
	-@erase "$(INTDIR)\Mouse.sbr"
	-@erase "$(INTDIR)\OrgData.obj"
	-@erase "$(INTDIR)\OrgData.sbr"
	-@erase "$(INTDIR)\OrgEdit.obj"
	-@erase "$(INTDIR)\OrgEdit.sbr"
	-@erase "$(INTDIR)\OrgFile.obj"
	-@erase "$(INTDIR)\OrgFile.sbr"
	-@erase "$(INTDIR)\OrgFileMIDI.obj"
	-@erase "$(INTDIR)\OrgFileMIDI.sbr"
	-@erase "$(INTDIR)\OrgPlay.obj"
	-@erase "$(INTDIR)\OrgPlay.sbr"
	-@erase "$(INTDIR)\rxoFunction.obj"
	-@erase "$(INTDIR)\rxoFunction.sbr"
	-@erase "$(INTDIR)\Script1.res"
	-@erase "$(INTDIR)\Scroll.obj"
	-@erase "$(INTDIR)\Scroll.sbr"
	-@erase "$(INTDIR)\Sound.obj"
	-@erase "$(INTDIR)\Sound.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WinMain.obj"
	-@erase "$(INTDIR)\WinMain.sbr"
	-@erase "$(INTDIR)\WinTimer.obj"
	-@erase "$(INTDIR)\WinTimer.sbr"
	-@erase "$(OUTDIR)\Org13.bsc"
	-@erase "$(OUTDIR)\Org211.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Org13.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\Script1.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Org13.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Click.sbr" \
	"$(INTDIR)\DlgEdit.sbr" \
	"$(INTDIR)\DlgEZCopy.sbr" \
	"$(INTDIR)\DlgPlayer.sbr" \
	"$(INTDIR)\DlgSet.sbr" \
	"$(INTDIR)\DlgTrack.sbr" \
	"$(INTDIR)\Filer.sbr" \
	"$(INTDIR)\Gdi.sbr" \
	"$(INTDIR)\MainPut.sbr" \
	"$(INTDIR)\Mouse.sbr" \
	"$(INTDIR)\OrgData.sbr" \
	"$(INTDIR)\OrgEdit.sbr" \
	"$(INTDIR)\OrgFile.sbr" \
	"$(INTDIR)\OrgFileMIDI.sbr" \
	"$(INTDIR)\OrgPlay.sbr" \
	"$(INTDIR)\rxoFunction.sbr" \
	"$(INTDIR)\Scroll.sbr" \
	"$(INTDIR)\Sound.sbr" \
	"$(INTDIR)\WinMain.sbr" \
	"$(INTDIR)\WinTimer.sbr"

"$(OUTDIR)\Org13.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=dsound.lib ddraw.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Org211.pdb" /machine:I386 /out:"$(OUTDIR)\Org211.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Click.obj" \
	"$(INTDIR)\DlgEdit.obj" \
	"$(INTDIR)\DlgEZCopy.obj" \
	"$(INTDIR)\DlgPlayer.obj" \
	"$(INTDIR)\DlgSet.obj" \
	"$(INTDIR)\DlgTrack.obj" \
	"$(INTDIR)\Filer.obj" \
	"$(INTDIR)\Gdi.obj" \
	"$(INTDIR)\MainPut.obj" \
	"$(INTDIR)\Mouse.obj" \
	"$(INTDIR)\OrgData.obj" \
	"$(INTDIR)\OrgEdit.obj" \
	"$(INTDIR)\OrgFile.obj" \
	"$(INTDIR)\OrgFileMIDI.obj" \
	"$(INTDIR)\OrgPlay.obj" \
	"$(INTDIR)\rxoFunction.obj" \
	"$(INTDIR)\Scroll.obj" \
	"$(INTDIR)\Sound.obj" \
	"$(INTDIR)\WinMain.obj" \
	"$(INTDIR)\WinTimer.obj" \
	"$(INTDIR)\Script1.res"

"$(OUTDIR)\Org211.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Org13 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Org140.exe" "$(OUTDIR)\Org13.bsc"


CLEAN :
	-@erase "$(INTDIR)\Click.obj"
	-@erase "$(INTDIR)\Click.sbr"
	-@erase "$(INTDIR)\DlgEdit.obj"
	-@erase "$(INTDIR)\DlgEdit.sbr"
	-@erase "$(INTDIR)\DlgEZCopy.obj"
	-@erase "$(INTDIR)\DlgEZCopy.sbr"
	-@erase "$(INTDIR)\DlgPlayer.obj"
	-@erase "$(INTDIR)\DlgPlayer.sbr"
	-@erase "$(INTDIR)\DlgSet.obj"
	-@erase "$(INTDIR)\DlgSet.sbr"
	-@erase "$(INTDIR)\DlgTrack.obj"
	-@erase "$(INTDIR)\DlgTrack.sbr"
	-@erase "$(INTDIR)\Filer.obj"
	-@erase "$(INTDIR)\Filer.sbr"
	-@erase "$(INTDIR)\Gdi.obj"
	-@erase "$(INTDIR)\Gdi.sbr"
	-@erase "$(INTDIR)\MainPut.obj"
	-@erase "$(INTDIR)\MainPut.sbr"
	-@erase "$(INTDIR)\Mouse.obj"
	-@erase "$(INTDIR)\Mouse.sbr"
	-@erase "$(INTDIR)\OrgData.obj"
	-@erase "$(INTDIR)\OrgData.sbr"
	-@erase "$(INTDIR)\OrgEdit.obj"
	-@erase "$(INTDIR)\OrgEdit.sbr"
	-@erase "$(INTDIR)\OrgFile.obj"
	-@erase "$(INTDIR)\OrgFile.sbr"
	-@erase "$(INTDIR)\OrgFileMIDI.obj"
	-@erase "$(INTDIR)\OrgFileMIDI.sbr"
	-@erase "$(INTDIR)\OrgPlay.obj"
	-@erase "$(INTDIR)\OrgPlay.sbr"
	-@erase "$(INTDIR)\rxoFunction.obj"
	-@erase "$(INTDIR)\rxoFunction.sbr"
	-@erase "$(INTDIR)\Script1.res"
	-@erase "$(INTDIR)\Scroll.obj"
	-@erase "$(INTDIR)\Scroll.sbr"
	-@erase "$(INTDIR)\Sound.obj"
	-@erase "$(INTDIR)\Sound.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WinMain.obj"
	-@erase "$(INTDIR)\WinMain.sbr"
	-@erase "$(INTDIR)\WinTimer.obj"
	-@erase "$(INTDIR)\WinTimer.sbr"
	-@erase "$(OUTDIR)\Org13.bsc"
	-@erase "$(OUTDIR)\Org140.exe"
	-@erase "$(OUTDIR)\Org140.ilk"
	-@erase "$(OUTDIR)\Org140.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Org13.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x411 /fo"$(INTDIR)\Script1.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Org13.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Click.sbr" \
	"$(INTDIR)\DlgEdit.sbr" \
	"$(INTDIR)\DlgEZCopy.sbr" \
	"$(INTDIR)\DlgPlayer.sbr" \
	"$(INTDIR)\DlgSet.sbr" \
	"$(INTDIR)\DlgTrack.sbr" \
	"$(INTDIR)\Filer.sbr" \
	"$(INTDIR)\Gdi.sbr" \
	"$(INTDIR)\MainPut.sbr" \
	"$(INTDIR)\Mouse.sbr" \
	"$(INTDIR)\OrgData.sbr" \
	"$(INTDIR)\OrgEdit.sbr" \
	"$(INTDIR)\OrgFile.sbr" \
	"$(INTDIR)\OrgFileMIDI.sbr" \
	"$(INTDIR)\OrgPlay.sbr" \
	"$(INTDIR)\rxoFunction.sbr" \
	"$(INTDIR)\Scroll.sbr" \
	"$(INTDIR)\Sound.sbr" \
	"$(INTDIR)\WinMain.sbr" \
	"$(INTDIR)\WinTimer.sbr"

"$(OUTDIR)\Org13.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=dsound.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Org140.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Org140.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Click.obj" \
	"$(INTDIR)\DlgEdit.obj" \
	"$(INTDIR)\DlgEZCopy.obj" \
	"$(INTDIR)\DlgPlayer.obj" \
	"$(INTDIR)\DlgSet.obj" \
	"$(INTDIR)\DlgTrack.obj" \
	"$(INTDIR)\Filer.obj" \
	"$(INTDIR)\Gdi.obj" \
	"$(INTDIR)\MainPut.obj" \
	"$(INTDIR)\Mouse.obj" \
	"$(INTDIR)\OrgData.obj" \
	"$(INTDIR)\OrgEdit.obj" \
	"$(INTDIR)\OrgFile.obj" \
	"$(INTDIR)\OrgFileMIDI.obj" \
	"$(INTDIR)\OrgPlay.obj" \
	"$(INTDIR)\rxoFunction.obj" \
	"$(INTDIR)\Scroll.obj" \
	"$(INTDIR)\Sound.obj" \
	"$(INTDIR)\WinMain.obj" \
	"$(INTDIR)\WinTimer.obj" \
	"$(INTDIR)\Script1.res"

"$(OUTDIR)\Org140.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Org13.dep")
!INCLUDE "Org13.dep"
!ELSE 
!MESSAGE Warning: cannot find "Org13.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Org13 - Win32 Release" || "$(CFG)" == "Org13 - Win32 Debug"
SOURCE=.\Click.cpp

"$(INTDIR)\Click.obj"	"$(INTDIR)\Click.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DlgEdit.cpp

"$(INTDIR)\DlgEdit.obj"	"$(INTDIR)\DlgEdit.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DlgEZCopy.cpp

"$(INTDIR)\DlgEZCopy.obj"	"$(INTDIR)\DlgEZCopy.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DlgPlayer.cpp

"$(INTDIR)\DlgPlayer.obj"	"$(INTDIR)\DlgPlayer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DlgSet.cpp

"$(INTDIR)\DlgSet.obj"	"$(INTDIR)\DlgSet.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DlgTrack.cpp

"$(INTDIR)\DlgTrack.obj"	"$(INTDIR)\DlgTrack.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Filer.cpp

"$(INTDIR)\Filer.obj"	"$(INTDIR)\Filer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Gdi.cpp

"$(INTDIR)\Gdi.obj"	"$(INTDIR)\Gdi.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MainPut.cpp

"$(INTDIR)\MainPut.obj"	"$(INTDIR)\MainPut.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Mouse.cpp

"$(INTDIR)\Mouse.obj"	"$(INTDIR)\Mouse.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OrgData.cpp

"$(INTDIR)\OrgData.obj"	"$(INTDIR)\OrgData.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OrgEdit.cpp

"$(INTDIR)\OrgEdit.obj"	"$(INTDIR)\OrgEdit.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OrgFile.cpp

"$(INTDIR)\OrgFile.obj"	"$(INTDIR)\OrgFile.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OrgFileMIDI.cpp

"$(INTDIR)\OrgFileMIDI.obj"	"$(INTDIR)\OrgFileMIDI.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OrgPlay.cpp

"$(INTDIR)\OrgPlay.obj"	"$(INTDIR)\OrgPlay.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rxoFunction.cpp

"$(INTDIR)\rxoFunction.obj"	"$(INTDIR)\rxoFunction.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Script1.rc

"$(INTDIR)\Script1.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Scroll.cpp

"$(INTDIR)\Scroll.obj"	"$(INTDIR)\Scroll.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Sound.cpp

"$(INTDIR)\Sound.obj"	"$(INTDIR)\Sound.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\WinMain.cpp

"$(INTDIR)\WinMain.obj"	"$(INTDIR)\WinMain.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\WinTimer.cpp

"$(INTDIR)\WinTimer.obj"	"$(INTDIR)\WinTimer.sbr" : $(SOURCE) "$(INTDIR)"



!ENDIF 

