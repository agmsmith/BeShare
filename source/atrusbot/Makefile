# Makefile for building the Atrus chatbot.  Do plain "make" for a normal build,
# "make debug" for a build with symbols included for debugging, "make clean" to
# force a complete rebuild next time.  Works for BeOS R5 Intel 32 bit CPU, BeOS
# R5 PowerPC 32 bit, Haiku 32 and 64 bit on Intel x86 CPUs, and Fedora 27 Linux.
# Last updated by AGMS20181104

# Use default compilers for C and C++, leave CC and CXX unset (gets you gcc &
# g++), unless needed.
#CC  = gcc
#CXX = gcc

# compilation flags used under any OS or compiler (may be appended to, below)
CXXFLAGS += -I.. -I../muscle -I../muscle/zlib/zlib \
    -DMUSCLE_ENABLE_ZLIB_ENCODING -DMUSCLE_SINGLE_THREAD_ONLY

# compilation flags that are specific to the gcc compiler (hard-coded)
GCCFLAGS    = -fno-exceptions -DMUSCLE_NO_EXCEPTIONS -Wall -W -Wno-multichar

# flags to include when compiling the optimized version (with 'make')
CCOPTFLAGS  = -O2

# flags to include when linking (set per operating system, below)
LFLAGS      = 

# libraries to include when linking (set per operating system, below)
LIBS        = 

# names of the executables to compile
EXECUTABLES = Atrus

# Make uses this to find .cpp files when it's checking dependency datestamps.
VPATH += ../muscle/message ../muscle/besupport ../muscle/iogateway \
         ../muscle/reflector ../muscle/regex ../muscle/system ../muscle/util \
         ../muscle/syslog ../muscle/zlib ../muscle/zlib/zlib

# Various object file and other inputs to the build process.
REGEXOBJS = 
ZLIBOBJS = adler32.o deflate.o trees.o zutil.o inflate.o inftrees.o inffast.o \
           crc32.o compress.o

# If muscled is crashing, try setting this to no; some older compilers can't
# handle memory tracking!  Like GCC2, makes "delete new BMenuItem" fail
# mysteriously, even before anything else has started up right at the beginning
# of main().
MEMORY_TRACKING_SUPPORTED = no

# OSTYPE is "Haiku" or "beos" with that capitalization; if the OS type
# variable is unset, try to set it using the uname shell command.
ifeq ($(OSTYPE),)
  OSTYPE = $(strip $(shell uname))
endif

ifeq ($(OSTYPE),beos)
   ifeq ($(BE_HOST_CPU),ppc)
      CC = mwcc
      CXX = mwcc
      REGEXOBJS = regcomp.o regexec.o regerror.o regfree.o
      VPATH += ../muscle/regex/regex
      CXXFLAGS += -I../muscle/regex/regex -I../muscle/zlib/zlib
      CXXFLAGS += -DMUSCLE_MAX_OUTPUT_CHUNK=1000 -Dstd=""
      CXXFLAGS += $(CCOPTFLAGS)
      LIBS = -ltranslation
      MEMORY_TRACKING_SUPPORTED = no  # mwcc can't handle it correctly :^(
   else # BeOS with Intel x86 CPU.
      CXXFLAGS += $(GCCFLAGS) $(CCOPTFLAGS)
      LIBS = -lbe -lnet -lroot -ltranslation
      ifeq ($(shell ls 2>/dev/null -1 /boot/develop/headers/be/bone/bone_api.h),/boot/develop/headers/be/bone/bone_api.h)
         CXXFLAGS += -I/boot/develop/headers/be/bone -DBONE
         LIBS = -nodefaultlibs -lbind -lsocket -lbe -lroot -L/boot/beos/system/lib
      else # Plain old BeOS R5, not Bone.
        CXXFLAGS += -DMUSCLE_MAX_OUTPUT_CHUNK=1000
      endif
   endif
else # not beos
    ifeq ($(OSTYPE),Haiku)
      CXXFLAGS += $(GCCFLAGS) $(CCOPTFLAGS)
      LIBS = -lbe -lnetwork -lroot -ltranslation
    else
        ifneq (,$(findstring g++,$(CXX)))
          # not using g++ don't need $GCCFLAGS
          CXXFLAGS += $(CCOPTFLAGS)
        else
          CXXFLAGS += $(GCCFLAGS) $(CCOPTFLAGS)
        endif
    endif
endif

ifeq ($(strip $(MEMORY_TRACKING_SUPPORTED)),yes)
   CXXFLAGS += -DMUSCLE_ENABLE_MEMORY_TRACKING
endif

# Makes all the programs that can be made using just cross-platform code
all : $(EXECUTABLES)

# Sneaky double rule (with a blank line between them - needed) that makes a
# debug build.
debug : CCOPTFLAGS = -g

debug : all

Atrus : main.o VoidBot.o Bot.o BotData.o UserList.o User.o Message.o \
    AbstractMessageIOGateway.o MessageIOGateway.o String.o \
    NetworkUtilityFunctions.o SysLog.o PathMatcher.o StringMatcher.o \
    PulseNode.o SetupSystem.o ByteBuffer.o ZLibCodec.o QueryFilter.o \
    adler32.o deflate.o trees.o zutil.o inflate.o inftrees.o inffast.o \
    crc32.o compress.o $(REGEXOBJS) $(ZLIBOBJS)
	$(CXX) $(LIBS) $(LFLAGS) -o $@ $^

clean :
	rm -fv *.o *.xSYM $(EXECUTABLES)

