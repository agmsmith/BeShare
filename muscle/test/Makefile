CFLAGS = -I.. -g
CXXFLAGS = $(CFLAGS) -O3 -DMUSCLE_ENABLE_ZLIB_ENCODING -DMUSCLE_AVOID_NEWNOTHROW -DMUSCLE_SINGLE_THREAD_ONLY
LFLAGS =  
LIBS =
EXECUTABLES = testhashtable testmini minireflectclient testmessage testzip testrefcount testqueue testtuple testgateway calctypecode portablereflectclient testudp teststring testparsefile testtime deadlockfinder testendian testsysteminfo portableplaintextclient uploadstress bandwidthtester readmessage testregex testnagle testresponse testqueryfilter testtypedefs hexterm testchildprocess cvscopy testserial
REGEXOBJS = 
ZLIBOBJS = adler32.o deflate.o trees.o zutil.o inflate.o inftrees.o inffast.o crc32.o compress.o
ZIPOBJS = zip.o unzip.o ioapi.o

VPATH = ../hashtable ../message ../iogateway ../reflector ../regex ../util ../syslog ../system ../dataio ../zlib ../zlib/zlib ../zlib/zlib/contrib/minizip ../minimessage

# if the OS type variable is unset, try to set it using the uname shell command 
ifeq ($(OSTYPE),) 
  OSTYPE = $(strip $(shell uname))
endif 

# IRIX may report itself as IRIX or as IRIX64.  They are both the same to us. 
ifeq ($(OSTYPE),IRIX64) 
  OSTYPE = IRIX 
endif 

ifeq ($(OSTYPE),beos) 
   VPATH += ../besupport
   EXECUTABLES += testbesupport 
   ifeq ($(BE_HOST_CPU),ppc) 
      CXX = mwcc 
      OBJFILES += regcomp.o regerror.o regexec.o regfree.o 
      VPATH += ../regex/regex 
      CFLAGS += -I../regex/regex -I../zlib/zlib 
      MEMORY_TRACKING_SUPPORTED = no  # mwcc can't handle it correctly :^( 
   else # not ppc 
      CXXFLAGS += $(GCCFLAGS) $(CCOPTFLAGS) 
      LIBS = -lbe -lnet -lroot 
      ifeq ($(shell ls 2>/dev/null -1 /boot/develop/headers/be/bone/bone_api.h), /boot/develop/headers/be/bone/bone_api.h) 
         CFLAGS += -I/boot/develop/headers/be/bone -DBONE 
         LIBS = -nodefaultlibs -lbind -lsocket -lbe -lroot -L/boot/beos/system/lib 
      endif 
   endif 
endif

ifeq ($(SYSTEM),AtheOS)
   VPATH += ../atheossupport
   EXECUTABLES += testatheossupport
   LIBS = -latheos
endif                           

ifeq ($(OSTYPE),freebsd4.0) 
   CXXFLAGS += -I/usr/include/machine 
endif 

ifeq ($(OSTYPE),DragonFly) 
   # void 
endif 

ifeq ($(OSTYPE),Darwin) 
   LIBS += -framework IOKit -framework Carbon
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

ifeq ($(MEMORY_TRACKING_SUPPORTED),yes) 
   CXXFLAGS += -DMUSCLE_ENABLE_MEMORY_TRACKING 
endif 


# Makes all the programs that can be made using just cross-platform code
all : $(EXECUTABLES)

testatheossupport : ConvertMessages.o Message.o String.o testatheossupport.o SysLog.o SetupSystem.o ByteBuffer.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testbesupport : ConvertMessages.o Message.o String.o testbesupport.o SysLog.o SetupSystem.o ByteBuffer.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

calctypecode : calctypecode.o SysLog.o SetupSystem.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testhashtable : String.o testhashtable.o SysLog.o NetworkUtilityFunctions.o SetupSystem.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testmessage : Message.o String.o testmessage.o SysLog.o ByteBuffer.o SetupSystem.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testzip : Message.o String.o testzip.o SysLog.o ByteBuffer.o SetupSystem.o ZipFileUtilityFunctions.o MiscUtilityFunctions.o $(ZLIBOBJS) $(ZIPOBJS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testmini : Message.o String.o testmini.o SysLog.o ByteBuffer.o SetupSystem.o MiniMessage.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

minireflectclient : minireflectclient.o MiniMessage.o MiniMessageGateway.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

readmessage : Message.o String.o readmessage.o SysLog.o ByteBuffer.o SetupSystem.o ZLibUtilityFunctions.o ZLibCodec.o $(ZLIBOBJS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testresponse : testresponse.o MessageIOGateway.o AbstractMessageIOGateway.o NetworkUtilityFunctions.o SysLog.o SetupSystem.o PulseNode.o Message.o String.o ByteBuffer.o ZLibCodec.o $(ZLIBOBJS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testnagle : testnagle.o NetworkUtilityFunctions.o SysLog.o SetupSystem.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testrefcount : testrefcount.o SysLog.o String.o SetupSystem.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

teststring : teststring.o SysLog.o String.o NetworkUtilityFunctions.o SetupSystem.o MiscUtilityFunctions.o Message.o ByteBuffer.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testparsefile : testparsefile.o SysLog.o String.o MiscUtilityFunctions.o SetupSystem.o Message.o ByteBuffer.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testtime : testtime.o SysLog.o String.o NetworkUtilityFunctions.o SetupSystem.o MiscUtilityFunctions.o ByteBuffer.o Message.o

	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testendian : testendian.o MiscUtilityFunctions.o ByteBuffer.o String.o SysLog.o NetworkUtilityFunctions.o SetupSystem.o Message.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testsysteminfo : testsysteminfo.o String.o SetupSystem.o SystemInfo.o SysLog.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testqueue : testqueue.o SysLog.o String.o SetupSystem.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testtuple : testtuple.o String.o SysLog.o SetupSystem.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testserial : testserial.o SysLog.o RS232DataIO.o String.o SetupSystem.o NetworkUtilityFunctions.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testgateway : Message.o AbstractMessageIOGateway.o MessageIOGateway.o String.o testgateway.o SysLog.o PulseNode.o SetupSystem.o ZLibDataIO.o ZLibCodec.o ByteBuffer.o $(ZLIBOBJS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testregex : testregex.o StringMatcher.o String.o SysLog.o SetupSystem.o $(REGEXOBJS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

portablereflectclient : Message.o AbstractMessageIOGateway.o MessageIOGateway.o String.o portablereflectclient.o NetworkUtilityFunctions.o SysLog.o PulseNode.o SetupSystem.o ByteBuffer.o ZLibCodec.o SetupSystem.o $(ZLIBOBJS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testudp : Message.o AbstractMessageIOGateway.o MessageIOGateway.o String.o testudp.o NetworkUtilityFunctions.o SysLog.o PulseNode.o SetupSystem.o ByteBuffer.o ZLibCodec.o SetupSystem.o $(ZLIBOBJS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

bandwidthtester : Message.o AbstractMessageIOGateway.o MessageIOGateway.o String.o bandwidthtester.o NetworkUtilityFunctions.o SysLog.o PulseNode.o SetupSystem.o ByteBuffer.o ZLibCodec.o $(ZLIBOBJS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

uploadstress : Message.o AbstractMessageIOGateway.o MessageIOGateway.o String.o uploadstress.o NetworkUtilityFunctions.o SysLog.o PulseNode.o SetupSystem.o ByteBuffer.o ZLibCodec.o $(ZLIBOBJS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

portableplaintextclient : Message.o AbstractMessageIOGateway.o PlainTextMessageIOGateway.o String.o portableplaintextclient.o NetworkUtilityFunctions.o SysLog.o PulseNode.o SetupSystem.o ByteBuffer.o ZLibCodec.o $(ZLIBOBJS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testchildprocess : Message.o AbstractMessageIOGateway.o PlainTextMessageIOGateway.o String.o ChildProcessDataIO.o testchildprocess.o NetworkUtilityFunctions.o SysLog.o PulseNode.o SetupSystem.o ByteBuffer.o ZLibCodec.o $(ZLIBOBJS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

hexterm : hexterm.o NetworkUtilityFunctions.o SysLog.o SetupSystem.o String.o RS232DataIO.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testsharedmem: SysLog.o SharedMemory.o NetworkUtilityFunctions.o testsharedmem.o String.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testqueryfilter: SysLog.o ByteBuffer.o Message.o QueryFilter.o String.o testqueryfilter.o StringMatcher.o SetupSystem.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

testtypedefs: testtypedefs.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

deadlockfinder : deadlockfinder.o SysLog.o String.o SetupSystem.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

cvscopy : cvscopy.o String.o SetupSystem.o SysLog.o
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

clean :
	rm -f *.o *.xSYM $(EXECUTABLES)
