# Microsoft Developer Studio Project File - Name="HackIt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=HackIt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "HackIt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HackIt.mak" CFG="HackIt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HackIt - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "HackIt - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "HackIt - Win32 Pro" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "HackIt - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Gi /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib htmlhelp.lib trayman/Release/trayman.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /verbose

!ELSEIF  "$(CFG)" == "HackIt - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "FULLVERSION" /D "DEBUG" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib htmlhelp.lib oleacc.lib trayman/Debug/trayman.lib /nologo /subsystem:windows /profile /debug /machine:I386

!ELSEIF  "$(CFG)" == "HackIt - Win32 Pro"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Pro"
# PROP BASE Intermediate_Dir "Pro"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Pro"
# PROP Intermediate_Dir "Pro"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /Gi /GX /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "FULLVERSION" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib htmlhelp.lib trayman/Pro/trayman.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /verbose

!ENDIF 

# Begin Target

# Name "HackIt - Win32 Release"
# Name "HackIt - Win32 Debug"
# Name "HackIt - Win32 Pro"
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\AutoEntry.h
# End Source File
# Begin Source File

SOURCE=.\AutoFunction.h
# End Source File
# Begin Source File

SOURCE=.\defines.h
# End Source File
# Begin Source File

SOURCE=.\HackItDlg.h
# End Source File
# Begin Source File

SOURCE=.\HackItSettings.h
# End Source File
# Begin Source File

SOURCE=.\helpids.h
# End Source File
# Begin Source File

SOURCE=.\IllegalWindowEntry.h
# End Source File
# Begin Source File

SOURCE=.\IllegalWindowSpec.h
# End Source File
# Begin Source File

SOURCE=.\LoggingSystem.h
# End Source File
# Begin Source File

SOURCE=.\ProcessFunctions.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\SystemHooks.h
# End Source File
# Begin Source File

SOURCE=.\TextEntry.h
# End Source File
# Begin Source File

SOURCE=.\Wildcard.h
# End Source File
# Begin Source File

SOURCE=.\WindowFunctions.h
# End Source File
# Begin Source File

SOURCE=.\WindowInfo.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\AutoEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoFunction.cpp
# End Source File
# Begin Source File

SOURCE=.\HackItDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HackItMain.cpp
# End Source File
# Begin Source File

SOURCE=.\HackItSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\IllegalWindowEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\IllegalWindowSpec.cpp
# End Source File
# Begin Source File

SOURCE=.\LoggingSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\SystemHooks.cpp
# End Source File
# Begin Source File

SOURCE=.\TextEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\Wildcard.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\WindowInfo.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "bmp ico rc rc2 res"
# Begin Source File

SOURCE=.\res\bitmap4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap9.bmp
# End Source File
# Begin Source File

SOURCE=.\res\HackIt.ico
# End Source File
# Begin Source File

SOURCE=.\HackIt.rc
# End Source File
# Begin Source File

SOURCE=.\res\HackIt.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00007.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00008.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00009.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00010.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00011.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00012.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00013.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\newsplash.bmp
# End Source File
# Begin Source File

SOURCE=.\Help\splash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\splash.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\FILE_ID.DIZ
# End Source File
# Begin Source File

SOURCE=.\res\ico00014.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00015.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00016.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00017.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00018.ico
# End Source File
# Begin Source File

SOURCE=.\license.txt
# End Source File
# Begin Source File

SOURCE=.\Install\Pro.iss
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# Begin Source File

SOURCE=.\Install\Release.iss
# End Source File
# End Target
# End Project
