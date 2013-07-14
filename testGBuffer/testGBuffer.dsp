# Microsoft Developer Studio Project File - Name="testGBuffer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=testGBuffer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "testGBuffer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "testGBuffer.mak" CFG="testGBuffer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "testGBuffer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "testGBuffer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "testGBuffer - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\temp\GBuffer\obj\testGBuffer\Release"
# PROP Intermediate_Dir "C:\temp\GBuffer\obj\testGBuffer\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /I "C:\Program Files (x86)\Microsoft Visual Studio\VC98\MFC\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "GIS_API_EXPORTS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /i "C:\Program Files (x86)\Microsoft Visual Studio\VC98\MFC\Include" /i "C:\Program Files (x86)\Microsoft Visual Studio\VC98\Include" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /incremental:yes /debug /machine:I386 /libpath:"C:\temp\gise\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy "C:\temp\GBuffer\obj\testGBuffer\Release\testGBuffer.exe" "C:\temp\GBuffer\bin\testGBuffer.exe"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "testGBuffer - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\temp\GBuffer\obj\testGBuffer\Debug"
# PROP Intermediate_Dir "C:\temp\GBuffer\obj\testGBuffer\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "GIS_API_EXPORTS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"C:\temp\GBuffer\obj\testGBuffer\Debug/testGBufferd.exe" /pdbtype:sept /libpath:"C:\temp\gise\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy "C:\temp\GBuffer\obj\testGBuffer\Debug\testGBufferd.exe" "C:\temp\GBuffer\bin\testGBufferd.exe"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "testGBuffer - Win32 Release"
# Name "testGBuffer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\bmpfile.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\RoadFindSelDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\testGBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\testGBuffer.rc
# End Source File
# Begin Source File

SOURCE=.\testGBufferDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\testGBufferView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\bmpfile.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RoadFindSelDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\testGBuffer.h
# End Source File
# Begin Source File

SOURCE=.\testGBufferDoc.h
# End Source File
# Begin Source File

SOURCE=.\testGBufferView.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\testGBuffer.ico
# End Source File
# Begin Source File

SOURCE=.\res\testGBuffer.rc2
# End Source File
# Begin Source File

SOURCE=.\res\testGBufferDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "GBuffer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GP_BaseTools.cpp
# End Source File
# Begin Source File

SOURCE=.\GP_BaseTools.h
# End Source File
# Begin Source File

SOURCE=.\GP_BitMap.cpp
# End Source File
# Begin Source File

SOURCE=.\GP_BitMap.h
# End Source File
# Begin Source File

SOURCE=.\GP_CommonType.h
# End Source File
# Begin Source File

SOURCE=.\GP_Dir.cpp
# End Source File
# Begin Source File

SOURCE=.\GP_Dir.h
# End Source File
# Begin Source File

SOURCE=.\GP_Document.cpp
# End Source File
# Begin Source File

SOURCE=.\GP_Document.h
# End Source File
# Begin Source File

SOURCE=.\GP_DrawingArea.cpp
# End Source File
# Begin Source File

SOURCE=.\GP_DrawingArea.h
# End Source File
# Begin Source File

SOURCE=.\GP_File.cpp
# End Source File
# Begin Source File

SOURCE=.\GP_File.h
# End Source File
# Begin Source File

SOURCE=.\GP_Node.cpp
# End Source File
# Begin Source File

SOURCE=.\GP_Node.h
# End Source File
# Begin Source File

SOURCE=.\GP_PixelBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\GP_PixelBuffer.h
# End Source File
# Begin Source File

SOURCE=.\GP_PixelDrawer.cpp
# End Source File
# Begin Source File

SOURCE=.\GP_PixelDrawer.h
# End Source File
# Begin Source File

SOURCE=.\GP_Utility.h
# End Source File
# End Group
# Begin Group "testtool"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GP_MemPool.h
# End Source File
# Begin Source File

SOURCE=.\GP_Performance.cpp
# End Source File
# Begin Source File

SOURCE=.\GP_Performance.h
# End Source File
# Begin Source File

SOURCE=.\GP_Time.cpp
# End Source File
# Begin Source File

SOURCE=.\GP_Time.h
# End Source File
# End Group
# Begin Group "DataIO"

# PROP Default_Filter ""
# End Group
# Begin Group "pico"

# PROP Default_Filter ""
# Begin Group "zlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pico\zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\deflate.h
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\inffast.h
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\inflate.h
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\trees.c
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\trees.h
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\zutil.c
# End Source File
# Begin Source File

SOURCE=.\pico\zlib\zutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\pico\blockset.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\cache.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\cacheentry.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\compress.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\dir.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\diriterator.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\file.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\freespace.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\gen.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\hashtable.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\log.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\lru.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\offsetfile.hpp
# End Source File
# Begin Source File

SOURCE=.\pico\os.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\osdir.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\osfile.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\picostorage.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\slices.cpp
# End Source File
# Begin Source File

SOURCE=.\pico\storageparams.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
