# Microsoft Developer Studio Project File - Name="quakeforge" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=quakeforge - Win32 GLDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "quakeforge.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "quakeforge.mak" CFG="quakeforge - Win32 GLDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "quakeforge - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "quakeforge - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "quakeforge - Win32 GLDebug" (based on "Win32 (x86) Application")
!MESSAGE "quakeforge - Win32 GLRelease" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "quakeforge___Win32_Release"
# PROP BASE Intermediate_Dir "quakeforge___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "quakeforge___Win32_Release"
# PROP Intermediate_Dir "quakeforge___Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\scitech\include" /I "..\common" /I "..\uquake" /I ".\common" /I ".\uquake" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dxguid.lib .\scitech\lib\win32\vc\mgllt.lib winmm.lib wsock32.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\scitech\include" /I "..\common" /I "..\uquake" /I ".\common" /I ".\uquake" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dxguid.lib .\scitech\lib\win32\vc\mgllt.lib winmm.lib wsock32.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "quakeforge___Win32_GLDebug"
# PROP BASE Intermediate_Dir "quakeforge___Win32_GLDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "quakeforge___Win32_GLDebug"
# PROP Intermediate_Dir "quakeforge___Win32_GLDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\scitech\include" /I "..\common" /I "..\uquake" /I ".\common" /I ".\uquake" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\scitech\include" /I "..\common" /I "..\uquake" /I ".\common" /I ".\uquake" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "GLQUAKE" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 dxguid.lib .\scitech\lib\win32\vc\mgllt.lib winmm.lib wsock32.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dxguid.lib .\scitech\lib\win32\vc\mgllt.lib comctl32.lib winmm.lib wsock32.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"quakeforge___Win32_GLDebug/glquakeforge.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "quakeforge___Win32_GLRelease"
# PROP BASE Intermediate_Dir "quakeforge___Win32_GLRelease"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "quakeforge___Win32_GLRelease"
# PROP Intermediate_Dir "quakeforge___Win32_GLRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I ".\scitech\include" /I "..\common" /I "..\uquake" /I ".\common" /I ".\uquake" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\scitech\include" /I "..\common" /I "..\uquake" /I ".\common" /I ".\uquake" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "GLQUAKE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 dxguid.lib .\scitech\lib\win32\vc\mgllt.lib winmm.lib wsock32.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dxguid.lib .\scitech\lib\win32\vc\mgllt.lib comctl32.lib winmm.lib wsock32.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"quakeforge___Win32_GLRelease/glquakeforge.exe"

!ENDIF 

# Begin Target

# Name "quakeforge - Win32 Release"
# Name "quakeforge - Win32 Debug"
# Name "quakeforge - Win32 GLDebug"
# Name "quakeforge - Win32 GLRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\common\cd_wrapper.c
# End Source File
# Begin Source File

SOURCE=.\uquake\chasecam.c
# End Source File
# Begin Source File

SOURCE=.\uquake\cl_demo.c
# End Source File
# Begin Source File

SOURCE=.\uquake\cl_input.c
# End Source File
# Begin Source File

SOURCE=.\uquake\cl_main.c
# End Source File
# Begin Source File

SOURCE=.\uquake\cl_parse.c
# End Source File
# Begin Source File

SOURCE=.\uquake\cl_tent.c
# End Source File
# Begin Source File

SOURCE=.\uquake\cmd.c
# End Source File
# Begin Source File

SOURCE=.\uquake\common.c
# End Source File
# Begin Source File

SOURCE=.\uquake\conproc.c
# End Source File
# Begin Source File

SOURCE=.\uquake\console.c
# End Source File
# Begin Source File

SOURCE=.\common\crc.c
# End Source File
# Begin Source File

SOURCE=.\uquake\cvar.c
# End Source File
# Begin Source File

SOURCE=.\uquake\d_edge.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_fill.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_init.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_modech.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_part.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_polyse.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_scan.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_sky.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_sprite.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\d_surf.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_vars.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_zpoint.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\draw.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\gl_draw.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\gl_mesh.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\gl_model.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\gl_refrag.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\gl_rlight.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\gl_rmain.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\gl_rmisc.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\gl_rsurf.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\gl_screen.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\gl_vidnt.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\gl_warp.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\host.c
# End Source File
# Begin Source File

SOURCE=.\uquake\host_cmd.c
# End Source File
# Begin Source File

SOURCE=.\common\in_win.c
# End Source File
# Begin Source File

SOURCE=.\common\keys.c
# End Source File
# Begin Source File

SOURCE=.\common\mathlib.c
# End Source File
# Begin Source File

SOURCE=.\common\mdfour.c
# End Source File
# Begin Source File

SOURCE=.\uquake\menu.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\model.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\net_dgrm.c
# End Source File
# Begin Source File

SOURCE=.\uquake\net_loop.c
# End Source File
# Begin Source File

SOURCE=.\uquake\net_main.c
# End Source File
# Begin Source File

SOURCE=.\uquake\net_vcr.c
# End Source File
# Begin Source File

SOURCE=.\uquake\net_win.c
# End Source File
# Begin Source File

SOURCE=.\uquake\net_wins.c
# End Source File
# Begin Source File

SOURCE=.\uquake\net_wipx.c
# End Source File
# Begin Source File

SOURCE=.\common\nonintel.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# ADD CPP /D "_WIN32"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# ADD CPP /D "_WIN32"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1
# ADD CPP /D "_WIN32"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1
# ADD CPP /D "_WIN32"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\pr_cmds.c
# End Source File
# Begin Source File

SOURCE=.\common\pr_edict.c
# End Source File
# Begin Source File

SOURCE=.\common\pr_exec.c
# End Source File
# Begin Source File

SOURCE=.\common\r_aclip.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\r_alias.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\r_bsp.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\r_draw.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\r_edge.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\r_efrag.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\r_light.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\r_main.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\r_misc.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\r_part.c
# End Source File
# Begin Source File

SOURCE=.\common\r_sky.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\r_sprite.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\r_surf.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\r_vars.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\register_check.c
# End Source File
# Begin Source File

SOURCE=.\uquake\sbar.c
# End Source File
# Begin Source File

SOURCE=.\uquake\screen.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\snd_dma.c
# End Source File
# Begin Source File

SOURCE=.\common\snd_mem.c
# End Source File
# Begin Source File

SOURCE=.\common\snd_mix.c
# End Source File
# Begin Source File

SOURCE=.\common\snd_win.c
# End Source File
# Begin Source File

SOURCE=.\uquake\sv_main.c
# End Source File
# Begin Source File

SOURCE=.\uquake\sv_move.c
# End Source File
# Begin Source File

SOURCE=.\uquake\sv_phys.c
# End Source File
# Begin Source File

SOURCE=.\uquake\sv_user.c
# End Source File
# Begin Source File

SOURCE=.\common\sys_common.c
# End Source File
# Begin Source File

SOURCE=.\uquake\sys_win.c
# End Source File
# Begin Source File

SOURCE=.\common\vid_win.c

!IF  "$(CFG)" == "quakeforge - Win32 Release"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uquake\view.c
# End Source File
# Begin Source File

SOURCE=.\common\wad.c
# End Source File
# Begin Source File

SOURCE=.\uquake\winquake.rc
# End Source File
# Begin Source File

SOURCE=.\uquake\world.c
# End Source File
# Begin Source File

SOURCE=.\common\zone.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\common\anorms.h
# End Source File
# Begin Source File

SOURCE=.\common\asm_draw.h
# End Source File
# Begin Source File

SOURCE=.\common\asm_i386.h
# End Source File
# Begin Source File

SOURCE=.\common\block16.h
# End Source File
# Begin Source File

SOURCE=.\common\block8.h
# End Source File
# Begin Source File

SOURCE=.\uquake\bspfile.h
# End Source File
# Begin Source File

SOURCE=.\common\cdaudio.h
# End Source File
# Begin Source File

SOURCE=.\uquake\client.h
# End Source File
# Begin Source File

SOURCE=.\uquake\cmd.h
# End Source File
# Begin Source File

SOURCE=.\uquake\common.h
# End Source File
# Begin Source File

SOURCE=.\common\common_quakedef.h
# End Source File
# Begin Source File

SOURCE=.\common\config.h
# End Source File
# Begin Source File

SOURCE=.\uquake\conproc.h
# End Source File
# Begin Source File

SOURCE=.\uquake\console.h
# End Source File
# Begin Source File

SOURCE=.\common\crc.h
# End Source File
# Begin Source File

SOURCE=.\uquake\cvar.h
# End Source File
# Begin Source File

SOURCE=.\common\d_iface.h
# End Source File
# Begin Source File

SOURCE=.\common\d_ifacea.h
# End Source File
# Begin Source File

SOURCE=.\common\d_local.h
# End Source File
# Begin Source File

SOURCE=.\common\dga_check.h
# End Source File
# Begin Source File

SOURCE=.\common\draw.h
# End Source File
# Begin Source File

SOURCE=.\uquake\gl_model.h
# End Source File
# Begin Source File

SOURCE=.\common\gl_warp_sin.h
# End Source File
# Begin Source File

SOURCE=.\common\glquake.h
# End Source File
# Begin Source File

SOURCE=.\common\glquake2.h
# End Source File
# Begin Source File

SOURCE=.\common\input.h
# End Source File
# Begin Source File

SOURCE=.\common\keys.h
# End Source File
# Begin Source File

SOURCE=.\common\mathlib.h
# End Source File
# Begin Source File

SOURCE=.\common\mdfour.h
# End Source File
# Begin Source File

SOURCE=.\uquake\menu.h
# End Source File
# Begin Source File

SOURCE=.\uquake\model.h
# End Source File
# Begin Source File

SOURCE=.\common\modelgen.h
# End Source File
# Begin Source File

SOURCE=.\uquake\net.h
# End Source File
# Begin Source File

SOURCE=.\uquake\net_dgrm.h
# End Source File
# Begin Source File

SOURCE=.\uquake\net_loop.h
# End Source File
# Begin Source File

SOURCE=.\common\net_ser.h
# End Source File
# Begin Source File

SOURCE=.\uquake\net_udp.h
# End Source File
# Begin Source File

SOURCE=.\uquake\net_vcr.h
# End Source File
# Begin Source File

SOURCE=.\common\net_wins.h
# End Source File
# Begin Source File

SOURCE=.\common\net_wipx.h
# End Source File
# Begin Source File

SOURCE=.\common\pr_comp.h
# End Source File
# Begin Source File

SOURCE=.\uquake\progdefs.h
# End Source File
# Begin Source File

SOURCE=.\common\progs.h
# End Source File
# Begin Source File

SOURCE=.\uquake\protocol.h
# End Source File
# Begin Source File

SOURCE=.\common\quakeasm.h
# End Source File
# Begin Source File

SOURCE=.\uquake\quakedef.h
# End Source File
# Begin Source File

SOURCE=.\common\r_local.h
# End Source File
# Begin Source File

SOURCE=.\common\r_shared.h
# End Source File
# Begin Source File

SOURCE=.\common\register_check.h
# End Source File
# Begin Source File

SOURCE=.\uquake\render.h
# End Source File
# Begin Source File

SOURCE=.\uquake\resource.h
# End Source File
# Begin Source File

SOURCE=.\common\sbar.h
# End Source File
# Begin Source File

SOURCE=.\uquake\screen.h
# End Source File
# Begin Source File

SOURCE=.\uquake\server.h
# End Source File
# Begin Source File

SOURCE=.\common\sound.h
# End Source File
# Begin Source File

SOURCE=.\common\spritegn.h
# End Source File
# Begin Source File

SOURCE=.\common\sys.h
# End Source File
# Begin Source File

SOURCE=.\common\uint32.h
# End Source File
# Begin Source File

SOURCE=.\common\vgamodes.h
# End Source File
# Begin Source File

SOURCE=.\common\vid.h
# End Source File
# Begin Source File

SOURCE=.\common\view.h
# End Source File
# Begin Source File

SOURCE=.\common\wad.h
# End Source File
# Begin Source File

SOURCE=.\common\winquake.h
# End Source File
# Begin Source File

SOURCE=.\uquake\world.h
# End Source File
# Begin Source File

SOURCE=.\common\zone.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Asm files"

# PROP Default_Filter ".s"
# Begin Source File

SOURCE=.\common\d_draw.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\d_draw.s
InputName=d_draw

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\d_draw.s
InputName=d_draw

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_draw16.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\d_draw16.s
InputName=d_draw16

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\d_draw16.s
InputName=d_draw16

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_parta.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\d_parta.s
InputName=d_parta

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\d_parta.s
InputName=d_parta

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_polysa.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\d_polysa.s
InputName=d_polysa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\d_polysa.s
InputName=d_polysa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_scana.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\d_scana.s
InputName=d_scana

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\d_scana.s
InputName=d_scana

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_spr8.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\d_spr8.s
InputName=d_spr8

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\d_spr8.s
InputName=d_spr8

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\d_varsa.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\d_varsa.s
InputName=d_varsa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\d_varsa.s
InputName=d_varsa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\math.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\math.s
InputName=math

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\math.s
InputName=math

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\quakeforge___Win32_GLDebug
InputPath=.\common\math.s
InputName=math

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\quakeforge___Win32_GLRelease
InputPath=.\common\math.s
InputName=math

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\r_aclipa.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\r_aclipa.s
InputName=r_aclipa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\r_aclipa.s
InputName=r_aclipa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\r_aliasa.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\r_aliasa.s
InputName=r_aliasa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\r_aliasa.s
InputName=r_aliasa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\r_drawa.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\r_drawa.s
InputName=r_drawa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\r_drawa.s
InputName=r_drawa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\r_edgea.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\r_edgea.s
InputName=r_edgea

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\r_edgea.s
InputName=r_edgea

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\r_varsa.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\r_varsa.s
InputName=r_varsa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\r_varsa.s
InputName=r_varsa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\snd_mixa.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\snd_mixa.s
InputName=snd_mixa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\snd_mixa.s
InputName=snd_mixa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\quakeforge___Win32_GLDebug
InputPath=.\common\snd_mixa.s
InputName=snd_mixa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# Begin Custom Build
OutDir=.\quakeforge___Win32_GLRelease
InputPath=.\common\snd_mixa.s
InputName=snd_mixa

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\surf16.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\surf16.s
InputName=surf16

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\surf16.s
InputName=surf16

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\surf8.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\surf8.s
InputName=surf8

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\surf8.s
InputName=surf8

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP BASE Ignore_Default_Tool 1
# PROP Exclude_From_Build 1
# PROP Ignore_Default_Tool 1

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\sys_wina.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\sys_wina.s
InputName=sys_wina

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\sys_wina.s
InputName=sys_wina

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\quakeforge___Win32_GLDebug
InputPath=.\common\sys_wina.s
InputName=sys_wina

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
OutDir=.\quakeforge___Win32_GLRelease
InputPath=.\common\sys_wina.s
InputName=sys_wina

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\worlda.s

!IF  "$(CFG)" == "quakeforge - Win32 Release"

# Begin Custom Build
OutDir=.\quakeforge___Win32_Release
InputPath=.\common\worlda.s
InputName=worlda

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
InputPath=.\common\worlda.s
InputName=worlda

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLDebug"

# Begin Custom Build
OutDir=.\quakeforge___Win32_GLDebug
InputPath=.\common\worlda.s
InputName=worlda

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "quakeforge - Win32 GLRelease"

# Begin Custom Build
OutDir=.\quakeforge___Win32_GLRelease
InputPath=.\common\worlda.s
InputName=worlda

"$(OUTDIR)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cl /I "\quakeforge\quakeforge\common" /EP > $(OUTDIR)\$(InputName).spp $(InputPath) 
	gas2masm\debug\gas2masm < $(OUTDIR)\$(InputName).spp >                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	ml /c /Cp /coff /Fo$(OUTDIR)\$(InputName).obj /Zm /Zi                                                                                                                                                                                                    $(OUTDIR)\$(InputName).asm 
	del $(OUTDIR)\$(InputName).spp 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
