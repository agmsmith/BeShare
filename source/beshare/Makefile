# Makefile for building BeShare.
# Works for BeOS Intel 32 bit CPU, maybe BeOS PowerPC 32 bit, Haiku 32 and 64 bit Intel x86 CPUs.
# Remember = assignments evaluated later, := evaluates while reading makefile.
# Last updated by AGMS20181028

# Use default compilers for C and C++, leave CC and CXX unset (gets you gcc & g++), unless needed.
#CC  = gcc
#CXX = gcc

# compilation flags used under any OS or compiler (may be appended to, below)
CXXFLAGS += -I../muscle -Isanta -Isanta/BetterScrollView \
            -Isanta/ColumnListView -Isanta/Cursors -Isanta/NewStrings \
            -Isanta/PrefilledBitmap -Isanta/ScrollViewCorner \
            -I../muscle/zlib/zlib -DMUSCLE_ENABLE_ZLIB_ENCODING

# compilation flags that are specific to the gcc compiler (hard-coded)
GCCFLAGS    = -fno-exceptions -DMUSCLE_NO_EXCEPTIONS -Wall -W -Wno-multichar

# flags to include when compiling the optimized version (with 'make')
CCOPTFLAGS  = -O2

# flags to include when linking (set per operating system, below)
LFLAGS      = 

# libraries to include when linking (set per operating system, below)
LIBS        = 

# names of the executables to compile
EXECUTABLES = BeShare md5test filechopper

# Make uses this to find .cpp files when it's checking dependency datestamps.
VPATH += ../muscle/message ../muscle/besupport ../muscle/iogateway \
         ../muscle/reflector ../muscle/regex ../muscle/system ../muscle/util \
         ../muscle/syslog ../muscle/zlib ../muscle/zlib/zlib santa \
         santa/BetterScrollView santa/ColumnListView santa/Cursors \
         santa/NewStrings santa/PrefilledBitmap santa/ScrollViewCorner

# Various object file and other inputs to the build process.
REGEXOBJS = 
SANTAOBJS = BetterScrollView.o CLVColumn.o CLVColumnLabelView.o CLVEasyItem.o \
            CLVListItem.o ColumnListView.o Cursors.o NewStrings.o \
            PrefilledBitmap.o ScrollViewCorner.o
ZLIBOBJS = adler32.o deflate.o trees.o zutil.o inflate.o inftrees.o inffast.o \
           crc32.o compress.o
RESOURCE = BeShare.rsrc

# if muscled is crashing, try setting this to no; some older compilers can't handle memory tracking!
MEMORY_TRACKING_SUPPORTED = yes

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

debug : CCOPTFLAGS = -g
debug : all

# Left over stuff from BeShare prior to 2.27 or something.  Can't find that .rsrc file.
savebeos : CFLAGS += -DSAVE_BEOS
savebeos : RESOURCE = BeShare-sbe.rsrc
savebeos : all

BeShare : ReflowingTextView.o ShareNetClient.o ShareFileTransfer.o \
          ChatWindow.o ShareApplication.o ShareWindow.o PrivateChatWindow.o \
          MessageTransceiverThread.o Thread.o AcceptSocketsThread.o Message.o \
          SetupSystem.o String.o main.o SysLog.o AbstractMessageIOGateway.o \
          PulseNode.o MessageIOGateway.o PlainTextMessageIOGateway.o \
          PathMatcher.o QueryFilter.o ConvertMessages.o RemoteFileItem.o \
          RemoteUserItem.o RemoteInfo.o ShareColumn.o ShareMIMEInfo.o \
          NetworkUtilityFunctions.o ShareUtils.o SplitPane.o md5.o \
          StringMatcher.o ShareStrings.o ReflectServer.o \
          AbstractReflectSession.o DumbReflectSession.o \
          StorageReflectSession.o GlobalMemoryAllocator.o DataNode.o \
          ServerComponent.o PathMatcher.o StringMatcher.o \
          RateLimitSessionIOPolicy.o ColorPicker.o \
          ByteBuffer.o ZLibCodec.o ZLibUtilityFunctions.o \
          $(SANTAOBJS) $(REGEXOBJS) $(ZLIBOBJS)
	$(CXX) $(LIBS) $(LFLAGS) -o $@ $^
	xres -o $@ $(RESOURCE)
	mimeset -F $@	

md5test : md5.o md5test.o
	$(CXX) $(LIBS) $(LFLAGS) -o $@ $^

filechopper : filechopper.o
	$(CXX) $(LIBS) $(LFLAGS) -o $@ $^

clean :
	rm -fv *.o *.xSYM $(EXECUTABLES)
