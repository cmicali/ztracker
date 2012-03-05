# Microsoft Developer Studio Project File - Name="zt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=zt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zt.mak" CFG="zt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zt - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "zt - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "zt"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release\temp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Zp16 /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib sdlmain.lib sdl.lib zlib.lib libpng2.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy docs\help.txt .
# End Special Build Tool

!ELSEIF  "$(CFG)" == "zt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug\temp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /Gi /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
# SUBTRACT RSC /x
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib sdlmain.lib sdl.lib zlib.lib libpng2.lib /nologo /subsystem:windows /profile /map /debug /machine:I386
# SUBTRACT LINK32 /verbose /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy docs\help.txt .
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "zt - Win32 Release"
# Name "zt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "CUI_Pages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CUI_About.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Arpeggioeditor.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Config.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Help.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_InstEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_LoadMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Loadscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Logoscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Midimacroeditor.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_NewSong.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Ordereditor.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Page.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Patterneditor.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_PENature.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_PEParms.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_PEVol.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Playsong.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_RUSure.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_SaveMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Savescreen.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_SliderInput.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Songconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_SongDuration.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_SongMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\CUI_Sysconfig.cpp
# End Source File
# End Group
# Begin Group "fXML"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\fxml.cpp
# End Source File
# Begin Source File

SOURCE=.\fxmlelement.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\CDataBuf.cpp
# End Source File
# Begin Source File

SOURCE=.\conf.cpp
# End Source File
# Begin Source File

SOURCE=.\edit_cols.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\img.cpp
# End Source File
# Begin Source File

SOURCE=.\import_export.cpp
# End Source File
# Begin Source File

SOURCE=.\it.cpp
# End Source File
# Begin Source File

SOURCE=.\keybuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\lc_sdl_wrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\list.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# ADD CPP /Ze
# SUBTRACT CPP /Fr
# End Source File
# Begin Source File

SOURCE=".\midi-io.cpp"
# End Source File
# Begin Source File

SOURCE=.\module.cpp
# End Source File
# Begin Source File

SOURCE=.\OutputDevices.cpp
# End Source File
# Begin Source File

SOURCE=.\playback.cpp
# End Source File
# Begin Source File

SOURCE=.\skins.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\zlib_wrapper.cpp
# End Source File
# Begin Source File

SOURCE=".\ztfile-io.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "fXML headers"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=.\fxml.h
# End Source File
# Begin Source File

SOURCE=.\fxmlelement.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CDataBuf.h
# End Source File
# Begin Source File

SOURCE=.\conf.h
# End Source File
# Begin Source File

SOURCE=.\CUI_Page.h
# End Source File
# Begin Source File

SOURCE=.\edit_cols.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\img.h
# End Source File
# Begin Source File

SOURCE=.\import_export.h
# End Source File
# Begin Source File

SOURCE=.\it.h
# End Source File
# Begin Source File

SOURCE=.\keybuffer.h
# End Source File
# Begin Source File

SOURCE=.\lc_sdl_wrapper.h
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=".\midi-io.h"
# End Source File
# Begin Source File

SOURCE=.\module.h
# End Source File
# Begin Source File

SOURCE=.\OutputDevices.h
# End Source File
# Begin Source File

SOURCE=.\playback.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Skins.h
# End Source File
# Begin Source File

SOURCE=.\timer.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface.h
# End Source File
# Begin Source File

SOURCE=.\zlib_wrapper.h
# End Source File
# Begin Source File

SOURCE=.\zt.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\zt.ico
# End Source File
# Begin Source File

SOURCE=.\zt.rc
# End Source File
# End Group
# Begin Group "Documents"

# PROP Default_Filter "*.txt"
# Begin Source File

SOURCE=.\docs\CHANGELOG.txt

!IF  "$(CFG)" == "zt - Win32 Release"

!ELSEIF  "$(CFG)" == "zt - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\docs\help.txt
# End Source File
# End Group
# End Target
# End Project
