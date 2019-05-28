# Microsoft Developer Studio Project File - Name="PD218" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PD218 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PD218.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PD218.mak" CFG="PD218 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PD218 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PD218 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PD218 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Od /I "C:\Program Files\IVI Foundation\VISA\WinNT\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D _WIN32_WINNT=0X0500 /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 visa32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/RTL8711_ATE.exe" /libpath:"C:\Program Files\IVI Foundation\VISA\WinNT\lib\msc"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "PD218 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "C:\Program Files\IVI Foundation\VISA\WinNT\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D _WIN32_WINNT=0X0500 /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 visa32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/ESP8266_ATE.exe" /pdbtype:sept /libpath:"C:\Program Files\IVI Foundation\VISA\WinNT\lib\msc"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "PD218 - Win32 Release"
# Name "PD218 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\agilent_n4010_wlantest.cpp
# End Source File
# Begin Source File

SOURCE=.\audioRouting.cpp
# End Source File
# Begin Source File

SOURCE=.\audioSacle.cpp
# End Source File
# Begin Source File

SOURCE=.\conectDev.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\config_deal.cpp
# End Source File
# Begin Source File

SOURCE=.\config_set.cpp
# End Source File
# Begin Source File

SOURCE=.\Dongle.cpp
# End Source File
# Begin Source File

SOURCE=.\err.cpp
# End Source File
# Begin Source File

SOURCE=.\GPIB.cpp
# End Source File
# Begin Source File

SOURCE=.\input_address.cpp
# End Source File
# Begin Source File

SOURCE=.\ledAdcCal.cpp
# End Source File
# Begin Source File

SOURCE=.\new_date.cpp
# End Source File
# Begin Source File

SOURCE=.\new_password.cpp
# End Source File
# Begin Source File

SOURCE=.\Newdevice.cpp
# End Source File
# Begin Source File

SOURCE=.\password.cpp
# End Source File
# Begin Source File

SOURCE=.\PD218.cpp
# End Source File
# Begin Source File

SOURCE=.\PD218.rc
# End Source File
# Begin Source File

SOURCE=.\PD218Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\rfpwr_loss_cal.cpp
# End Source File
# Begin Source File

SOURCE=.\selDir.cpp
# End Source File
# Begin Source File

SOURCE=.\selDutCom.cpp
# End Source File
# Begin Source File

SOURCE=.\setpar.cpp
# End Source File
# Begin Source File

SOURCE=.\SortHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SortListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TestItem.cpp
# End Source File
# Begin Source File

SOURCE=.\tset.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\agilent_n4010_wlantest.h
# End Source File
# Begin Source File

SOURCE=.\AgN4010BT4_PublicDefine.h
# End Source File
# Begin Source File

SOURCE=.\AgN4010BT4API.h
# End Source File
# Begin Source File

SOURCE=.\audioRouting.h
# End Source File
# Begin Source File

SOURCE=.\audioSacle.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\config_deal.h
# End Source File
# Begin Source File

SOURCE=.\config_set.h
# End Source File
# Begin Source File

SOURCE=.\GPIB.h
# End Source File
# Begin Source File

SOURCE=.\input_address.h
# End Source File
# Begin Source File

SOURCE=.\ledAdcCal.h
# End Source File
# Begin Source File

SOURCE=.\new_date.h
# End Source File
# Begin Source File

SOURCE=.\new_password.h
# End Source File
# Begin Source File

SOURCE=.\Newdevice.h
# End Source File
# Begin Source File

SOURCE=.\password.h
# End Source File
# Begin Source File

SOURCE=.\PD218.h
# End Source File
# Begin Source File

SOURCE=.\PD218Dlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\rfpwr_loss_cal.h
# End Source File
# Begin Source File

SOURCE=.\selDir.h
# End Source File
# Begin Source File

SOURCE=.\selDutCom.h
# End Source File
# Begin Source File

SOURCE=.\setpar.h
# End Source File
# Begin Source File

SOURCE=.\SortHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SortListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\TestEngine.h
# End Source File
# Begin Source File

SOURCE=.\TestFlash.h
# End Source File
# Begin Source File

SOURCE=.\TestItem.h
# End Source File
# Begin Source File

SOURCE=.\tset.h
# End Source File
# Begin Source File

SOURCE=.\uEnergyTest.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\PD218.ico
# End Source File
# Begin Source File

SOURCE=.\res\PD218.rc2
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
