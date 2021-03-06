# Microsoft Developer Studio Project File - Name="muscle" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=muscle - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "muscle.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "muscle.mak" CFG="muscle - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "muscle - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Junk"
# PROP Intermediate_Dir "Junk"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "..\\" /I "..\..\\" /I ".\\" /I "..\\regex\regex" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "MUSCLE_ENABLE_ZLIB_ENCODING" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Build\muscle.lib"
# Begin Target

# Name "muscle - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\iogateway\AbstractMessageIOGateway.cpp
# End Source File
# Begin Source File

SOURCE=..\reflector\AbstractReflectSession.cpp
# End Source File
# Begin Source File

SOURCE=..\system\AcceptSocketsThread.cpp
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=..\util\ByteBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\compress.c
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=..\util\DebugTimer.cpp
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=..\reflector\DumbReflectSession.cpp
# End Source File
# Begin Source File

SOURCE=..\reflector\FilterSessionFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\system\GlobalMemoryAllocator.cpp
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\compress.c
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\infutil.c
# End Source File
# Begin Source File

SOURCE=..\util\MemoryAllocator.cpp
# End Source File
# Begin Source File

SOURCE=..\message\Message.cpp
# End Source File
# Begin Source File

SOURCE=..\iogateway\MessageIOGateway.cpp
# End Source File
# Begin Source File

SOURCE=..\system\MessageTransceiverThread.cpp
# End Source File
# Begin Source File

SOURCE=..\util\MiscUtilityFunctions.cpp
# End Source File
# Begin Source File

SOURCE=..\util\NetworkUtilityFunctions.cpp
# End Source File
# Begin Source File

SOURCE=..\regex\PathMatcher.cpp
# End Source File
# Begin Source File

SOURCE=..\iogateway\PlainTextMessageIOGateway.cpp
# End Source File
# Begin Source File

SOURCE=..\util\PulseNode.cpp
# End Source File
# Begin Source File

SOURCE=..\regex\QueryFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\reflector\RateLimitSessionIOPolicy.cpp
# End Source File
# Begin Source File

SOURCE=..\iogateway\RawDataMessageIOGateway.cpp
# End Source File
# Begin Source File

SOURCE=..\reflector\ReflectServer.cpp
# End Source File
# Begin Source File

SOURCE=..\regex\regex\regcomp.c
# End Source File
# Begin Source File

SOURCE=..\regex\regex\regerror.c
# End Source File
# Begin Source File

SOURCE=..\regex\regex\regexec.c
# End Source File
# Begin Source File

SOURCE=..\regex\regex\regfree.c
# End Source File
# Begin Source File

SOURCE=..\dataio\RS232DataIO.cpp
# End Source File
# Begin Source File

SOURCE=..\reflector\ServerComponent.cpp
# End Source File
# Begin Source File

SOURCE=..\system\SetupSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\system\SharedMemory.cpp
# End Source File
# Begin Source File

SOURCE=..\util\SharedUsageLimitProxyMemoryAllocator.cpp
# End Source File
# Begin Source File

SOURCE=..\reflector\StorageReflectSession.cpp
# End Source File
# Begin Source File

SOURCE=..\util\String.cpp
# End Source File
# Begin Source File

SOURCE=..\regex\StringMatcher.cpp
# End Source File
# Begin Source File

SOURCE=..\syslog\SysLog.cpp
# End Source File
# Begin Source File

SOURCE=..\system\Thread.cpp
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\trees.c
# End Source File
# Begin Source File

SOURCE=..\zlib\ZLibCodec.cpp
# End Source File
# Begin Source File

SOURCE=..\zlib\ZLibDataIO.cpp
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\zutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\iogateway\AbstractMessageIOGateway.h
# End Source File
# Begin Source File

SOURCE=..\reflector\AbstractReflectSession.h
# End Source File
# Begin Source File

SOURCE=..\reflector\AbstractSessionIOPolicy.h
# End Source File
# Begin Source File

SOURCE=..\system\AcceptSocketsThread.h
# End Source File
# Begin Source File

SOURCE=..\system\AtomicCounter.h
# End Source File
# Begin Source File

SOURCE=..\util\ByteBuffer.h
# End Source File
# Begin Source File

SOURCE=..\dataio\DataIO.h
# End Source File
# Begin Source File

SOURCE=..\util\DebugTimer.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\deflate.h
# End Source File
# Begin Source File

SOURCE=..\reflector\DumbReflectSession.h
# End Source File
# Begin Source File

SOURCE=..\dataio\FileDataIO.h
# End Source File
# Begin Source File

SOURCE=..\reflector\FilterSessionFactory.h
# End Source File
# Begin Source File

SOURCE=..\util\FlatCountable.h
# End Source File
# Begin Source File

SOURCE=..\support\Flattenable.h
# End Source File
# Begin Source File

SOURCE=..\system\GlobalMemoryAllocator.h
# End Source File
# Begin Source File

SOURCE=..\util\Hashtable.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\crc32.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\compress.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\inffast.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\inffixed.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\inftrees.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\infutil.h
# End Source File
# Begin Source File

SOURCE=..\syslog\LogCallback.h
# End Source File
# Begin Source File

SOURCE=..\util\MemoryAllocator.h
# End Source File
# Begin Source File

SOURCE=..\dataio\MemoryBufferDataIO.h
# End Source File
# Begin Source File

SOURCE=..\message\Message.h
# End Source File
# Begin Source File

SOURCE=..\iogateway\MessageIOGateway.h
# End Source File
# Begin Source File

SOURCE=..\system\MessageTransceiverThread.h
# End Source File
# Begin Source File

SOURCE=..\util\MiscUtilityFunctions.h
# End Source File
# Begin Source File

SOURCE=..\support\MuscleSupport.h
# End Source File
# Begin Source File

SOURCE=..\system\Mutex.h
# End Source File
# Begin Source File

SOURCE=..\util\NetworkUtilityFunctions.h
# End Source File
# Begin Source File

SOURCE=..\dataio\NullDataIO.h
# End Source File
# Begin Source File

SOURCE=..\util\ObjectPool.h
# End Source File
# Begin Source File

SOURCE=..\regex\PathMatcher.h
# End Source File
# Begin Source File

SOURCE=..\iogateway\PlainTextMessageIOGateway.h
# End Source File
# Begin Source File

SOURCE=..\support\Point.h
# End Source File
# Begin Source File

SOURCE=..\util\PulseNode.h
# End Source File
# Begin Source File

SOURCE=..\regex\QueryFilter.h
# End Source File
# Begin Source File

SOURCE=..\util\Queue.h
# End Source File
# Begin Source File

SOURCE=..\reflector\RateLimitSessionIOPolicy.h
# End Source File
# Begin Source File

SOURCE=..\iogateway\RawDataMessageIOGateway.h
# End Source File
# Begin Source File

SOURCE=..\support\Rect.h
# End Source File
# Begin Source File

SOURCE=..\util\RefCount.h
# End Source File
# Begin Source File

SOURCE=..\reflector\ReflectServer.h
# End Source File
# Begin Source File

SOURCE=..\regex\regex.h
# End Source File
# Begin Source File

SOURCE=..\dataio\RS232DataIO.h
# End Source File
# Begin Source File

SOURCE=..\reflector\ServerComponent.h
# End Source File
# Begin Source File

SOURCE=..\system\SetupSystem.h
# End Source File
# Begin Source File

SOURCE=..\system\SharedMemory.h
# End Source File
# Begin Source File

SOURCE=..\util\SharedUsageLimitProxyMemoryAllocator.h
# End Source File
# Begin Source File

SOURCE=..\iogateway\SignalMessageIOGateway.h
# End Source File
# Begin Source File

SOURCE=..\util\SocketHolder.h
# End Source File
# Begin Source File

SOURCE=..\reflector\StorageReflectConstants.h
# End Source File
# Begin Source File

SOURCE=..\reflector\StorageReflectSession.h
# End Source File
# Begin Source File

SOURCE=..\util\String.h
# End Source File
# Begin Source File

SOURCE=..\regex\StringMatcher.h
# End Source File
# Begin Source File

SOURCE=..\util\StringTokenizer.h
# End Source File
# Begin Source File

SOURCE=..\syslog\SysLog.h
# End Source File
# Begin Source File

SOURCE=..\dataio\TCPSocketDataIO.h
# End Source File
# Begin Source File

SOURCE=..\system\Thread.h
# End Source File
# Begin Source File

SOURCE=..\util\TimeUtilityFunctions.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\trees.h
# End Source File
# Begin Source File

SOURCE=..\support\Tuple.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=..\zlib\ZLibCodec.h
# End Source File
# Begin Source File

SOURCE=..\zlib\ZLibDataIO.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib\zutil.h
# End Source File
# End Group
# End Target
# End Project
