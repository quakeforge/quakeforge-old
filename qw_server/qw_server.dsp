# Microsoft Developer Studio Project File - Name="qw_server" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=qw_server - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qw_server.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qw_server.mak" CFG="qw_server - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qw_server - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "qw_server - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "qw_server - Win32 IPv6" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qw_server - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "qw_server___Win32_Release"
# PROP BASE Intermediate_Dir "qw_server___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /WX /GX /O2 /I "../common" /I "../qw_server" /I "../win32" /I "../win32/vc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "QUAKEWORLD" /D "SERVERONLY" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:console /machine:I386 /out:"Release/qw-server-win.exe"

!ELSEIF  "$(CFG)" == "qw_server - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "qw_server___Win32_Debug"
# PROP BASE Intermediate_Dir "qw_server___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /WX /Gm /GX /ZI /Od /I "../common" /I "../qw_server" /I "../win32" /I "../win32/vc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "QUAKEWORLD" /D "SERVERONLY" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/qw-server-win.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "qw_server - Win32 IPv6"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "qw_server___Win32_IPv6"
# PROP BASE Intermediate_Dir "qw_server___Win32_IPv6"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "qw_server___Win32_IPv6"
# PROP Intermediate_Dir "qw_server___Win32_IPv6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /WX /GX /O2 /I "../common" /I "../qw_server" /I "../win32" /I "../win32/vc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "QUAKEWORLD" /D "SERVERONLY" /YX /FD /c
# ADD CPP /nologo /WX /GX /O2 /I "../common" /I "../qw_server" /I "../win32" /I "../win32/vc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "QUAKEWORLD" /D "SERVERONLY" /D "HAVE_IPV6" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:console /machine:I386 /out:"Release/qw-server-win.exe"
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib wship6.lib /nologo /subsystem:console /machine:I386 /out:"Release/qw-server-win.exe"
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "qw_server - Win32 Release"
# Name "qw_server - Win32 Debug"
# Name "qw_server - Win32 IPv6"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\common\cmd.c
# End Source File
# Begin Source File

SOURCE=..\common\common.c
# End Source File
# Begin Source File

SOURCE=..\common\crc.c
# End Source File
# Begin Source File

SOURCE=..\common\cvar.c
# End Source File
# Begin Source File

SOURCE=..\common\dirent.c
# End Source File
# Begin Source File

SOURCE=..\common\fnmatch.c
# End Source File
# Begin Source File

SOURCE=..\common\lib_replace.c
# End Source File
# Begin Source File

SOURCE=..\common\mathlib.c
# End Source File
# Begin Source File

SOURCE=..\common\mdfour.c
# End Source File
# Begin Source File

SOURCE=..\common\model.c
# End Source File
# Begin Source File

SOURCE=..\common\net_chan.c
# End Source File
# Begin Source File

SOURCE=..\common\net_com.c
# End Source File
# Begin Source File

SOURCE=..\common\net_udp.c

!IF  "$(CFG)" == "qw_server - Win32 Release"

!ELSEIF  "$(CFG)" == "qw_server - Win32 Debug"

!ELSEIF  "$(CFG)" == "qw_server - Win32 IPv6"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\net_udp6.c

!IF  "$(CFG)" == "qw_server - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "qw_server - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "qw_server - Win32 IPv6"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\pmove.c
# End Source File
# Begin Source File

SOURCE=..\common\pmovetst.c
# End Source File
# Begin Source File

SOURCE=..\common\pr_cmds.c
# End Source File
# Begin Source File

SOURCE=..\common\pr_edict.c
# End Source File
# Begin Source File

SOURCE=..\common\pr_exec.c
# End Source File
# Begin Source File

SOURCE=..\common\qargs.c
# End Source File
# Begin Source File

SOURCE=..\common\qendian.c
# End Source File
# Begin Source File

SOURCE=..\common\quakefs.c
# End Source File
# Begin Source File

SOURCE=..\common\quakeio.c
# End Source File
# Begin Source File

SOURCE=..\common\register_check.c
# End Source File
# Begin Source File

SOURCE=.\sv_ccmds.c
# End Source File
# Begin Source File

SOURCE=.\sv_ents.c
# End Source File
# Begin Source File

SOURCE=.\sv_init.c
# End Source File
# Begin Source File

SOURCE=.\sv_main.c
# End Source File
# Begin Source File

SOURCE=.\sv_move.c
# End Source File
# Begin Source File

SOURCE=.\sv_nchan.c
# End Source File
# Begin Source File

SOURCE=.\sv_phys.c
# End Source File
# Begin Source File

SOURCE=.\sv_send.c
# End Source File
# Begin Source File

SOURCE=.\sv_user.c
# End Source File
# Begin Source File

SOURCE=..\common\sys_common.c
# End Source File
# Begin Source File

SOURCE=.\sys_win.c
# End Source File
# Begin Source File

SOURCE=..\common\world.c
# End Source File
# Begin Source File

SOURCE=..\common\zone.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
