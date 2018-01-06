# Microsoft Developer Studio Project File - Name="Org13" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Org13 - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "Org13.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Org13 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dsound.lib ddraw.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/Org211.exe"

!ELSEIF  "$(CFG)" == "Org13 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dsound.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/Org140.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Org13 - Win32 Release"
# Name "Org13 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Click.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEZCopy.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSet.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\Filer.cpp
# End Source File
# Begin Source File

SOURCE=.\Gdi.cpp
# End Source File
# Begin Source File

SOURCE=.\MainPut.cpp
# End Source File
# Begin Source File

SOURCE=.\Mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\OrgData.cpp
# End Source File
# Begin Source File

SOURCE=.\OrgEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\OrgFile.cpp
# End Source File
# Begin Source File

SOURCE=.\OrgFileMIDI.cpp
# End Source File
# Begin Source File

SOURCE=.\OrgPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\rxoFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\Script1.rc
# End Source File
# Begin Source File

SOURCE=.\Scroll.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\WinMain.cpp
# End Source File
# Begin Source File

SOURCE=.\WinTimer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Click.h
# End Source File
# Begin Source File

SOURCE=.\DefOrg.h
# End Source File
# Begin Source File

SOURCE=.\Filer.h
# End Source File
# Begin Source File

SOURCE=.\Gdi.h
# End Source File
# Begin Source File

SOURCE=.\Mouse.h
# End Source File
# Begin Source File

SOURCE=.\OrgData.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\rxoFunction.h
# End Source File
# Begin Source File

SOURCE=.\Scroll.h
# End Source File
# Begin Source File

SOURCE=.\Setting.h
# End Source File
# Begin Source File

SOURCE=.\Sound.h
# End Source File
# Begin Source File

SOURCE=.\Timer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Bitmap\button_track_1.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\button_track_2.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\button_track_3.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\button_track_4.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\cursor.cur
# End Source File
# Begin Source File

SOURCE=.\Bitmap\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\Bitmap\ffff.ico
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Head.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Left.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Loupe_MINUS.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Loupe_PLUS.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Music.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Note.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Number.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\org_back.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\org_home.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\org_next.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\org_play.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\org_stop.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Pan.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Play.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Right.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Solo.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\Stop.bmp
# End Source File
# Begin Source File

SOURCE=.\Bitmap\wave100.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Wave\Bass01.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Bass02.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Bass03.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Bass04.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Bass05.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Bass06.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Bass07.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Bass08.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Bell.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\cat.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Clap01.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Clash.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Crash.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Crash02.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Dram00.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Dram01.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Dram02.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Dram03.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Dram04.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Dram05.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Dram06.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Dram07.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Dram08.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Dram09.wav
# End Source File
# Begin Source File

SOURCE=.\help.html
# End Source File
# Begin Source File

SOURCE=.\help.txt
# End Source File
# Begin Source File

SOURCE=.\Wave\HiClose.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\HiClose03.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\HiClose04.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\HiClose05.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\HiClose2.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\HiOpen.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\HiOpen03.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\HiOpen04.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\HiOpen2.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\OrcDrm01.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Per01.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Per02.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Pesi01.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Quick01.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\RevSym01.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Ride01.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Snare01.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Snare02.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Snare03.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Snare04.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Snare05.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Snare06.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Snare07.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Snare08.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Tom01.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Tom02.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Tom03.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Tom04.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Tom05.wav
# End Source File
# Begin Source File

SOURCE=.\Wave\Wave.dat
# End Source File
# End Target
# End Project
