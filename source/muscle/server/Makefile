# compilation flags used under any OS or compiler (may be appended to, below)
CXXFLAGS   += -I.. -DMUSCLE_SINGLE_THREAD_ONLY -DMUSCLE_ENABLE_ZLIB_ENCODING

# compilation flags that are specific to the gcc compiler (hard-coded)
GCCFLAGS    = -fno-exceptions -DMUSCLE_NO_EXCEPTIONS -Wall -W -Wno-multichar

# flags to include when compiling the optimized version (with 'make')
CCOPTFLAGS  = -O2

# flags to include when linking (set per operating system, below)
LFLAGS      = 

# libraries to include when linking (set per operating system, below)
LIBS        = 

# names of the executables to compile
EXECUTABLES = muscled admin

# object files to include in all executables
OBJFILES = Message.o AbstractMessageIOGateway.o MessageIOGateway.o String.o AbstractReflectSession.o DumbReflectSession.o StorageReflectSession.o DataNode.o ReflectServer.o StringMatcher.o muscled.o MiscUtilityFunctions.o NetworkUtilityFunctions.o SysLog.o PulseNode.o PathMatcher.o FilterSessionFactory.o RateLimitSessionIOPolicy.o MemoryAllocator.o GlobalMemoryAllocator.o SetupSystem.o ServerComponent.o ZLibCodec.o ByteBuffer.o QueryFilter.o
ZLIBOBJS = adler32.o deflate.o trees.o zutil.o inflate.o inftrees.o inffast.o crc32.o compress.o

# Where to find .cpp files
VPATH = ../message ../besupport ../iogateway ../reflector ../regex ../util ../syslog ../system ../zlib ../zlib/zlib

# if muscled is crashing, try setting this to no; some older compilers can't handle memory tracking!
MEMORY_TRACKING_SUPPORTED = yes

# if the OS type variable is unset, try to set it using the uname shell command
ifeq ($(OSTYPE),)
  OSTYPE = $(strip $(shell uname))
endif

# IRIX may report itself as IRIX or as IRIX64.  They are both the same to us.
ifeq ($(OSTYPE),IRIX64)
  OSTYPE = IRIX
endif

ifeq ($(OSTYPE),beos)
   ifeq ($(BE_HOST_CPU),ppc)
      CC = mwcc
      CXX = mwcc
      OBJFILES += regcomp.o regerror.o regexec.o regfree.o
      VPATH += ../regex/regex
      CXXFLAGS += -I../regex/regex -I../zlib/zlib
      MEMORY_TRACKING_SUPPORTED = no  # mwcc can't handle it correctly :^(
   else # not ppc
      CXXFLAGS += $(GCCFLAGS) $(CCOPTFLAGS)
      LIBS = -lbe -lnetwork -lroot
      ifeq ($(shell ls 2>/dev/null -1 /boot/develop/headers/be/bone/bone_api.h), /boot/develop/headers/be/bone/bone_api.h)
         CXXFLAGS += -I/boot/develop/headers/be/bone -DBONE
         LIBS = -nodefaultlibs -lbind -lsocket -lbe -lroot -L/boot/beos/system/lib
      endif
   endif
else # not beos
    ifeq ($(OSTYPE),Haiku)
      CXXFLAGS += $(GCCFLAGS) $(CCOPTFLAGS)
      LIBS = -lbe -lnetwork
    else
        ifneq (,$(findstring g++,$(CXX)))
          # not using g++ don't need $GCCFLAGS
          CXXFLAGS += $(CCOPTFLAGS)
        else
          CXXFLAGS += $(GCCFLAGS) $(CCOPTFLAGS)
        endif
    endif
endif

ifeq ($(OSTYPE),freebsd4.0)
   CXXFLAGS += -I/usr/include/machine
endif

ifeq ($(OSTYPE),DragonFly)
   # void
endif

ifeq ($(OSTYPE),IRIX)
   CXXFLAGS += -DSGI -DMIPS
   ifneq (,$(findstring g++,$(CXX))) # if we are using SGI's CC compiler, we gotta change a few things
      CXX = CC
      CCFLAGS = -g2 -n32 -LANG:std -woff 1110,1174,1552,1460,3303
      LFLAGS  = -g2 -n32
      CXXFLAGS += -DNEW_H_NOT_AVAILABLE
      MEMORY_TRACKING_SUPPORTED = no # SGI CC can't handle it correctly :^(
   endif
endif

ifeq ($(OSTYPE), SunOS)
    CXXFLAGS += -DSUN
    ifneq (,$(findstring g++,$(CXX))) # if we are using the SunPro compilers, we have to change some things
	CXX = CC
    endif
    LIBS = -lsocket -lnsl
endif

ifeq ($(strip $(MEMORY_TRACKING_SUPPORTED)),yes)
   CXXFLAGS += -DMUSCLE_ENABLE_MEMORY_TRACKING
endif

all : $(EXECUTABLES)

debug : CCOPTFLAGS = -g
debug : all

muscled : $(OBJFILES) $(ZLIBOBJS)
	$(CXX) $(LIBS) $(LFLAGS) -o $@ $^

admin : Message.o AbstractMessageIOGateway.o MessageIOGateway.o String.o NetworkUtilityFunctions.o SysLog.o admin.o PulseNode.o SetupSystem.o ZLibCodec.o ByteBuffer.o GlobalMemoryAllocator.o $(ZLIBOBJS)
	$(CXX) $(LIBS) $(LFLAGS) -o $@ $^

clean :
	rm -fv *.o *.xSYM $(EXECUTABLES)

