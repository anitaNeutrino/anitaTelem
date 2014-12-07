############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################

RC     := root-config
ifeq ($(shell which $(RC) 2>&1 | sed -ne "s@.*/$(RC)@$(RC)@p"),$(RC))
MKARCH := $(wildcard $(shell $(RC) --etcdir)/Makefile.arch)
RCONFIG := $(wildcard $(shell $(RC) --incdir)/RConfigure.h)
endif
ifneq ($(MKARCH),)
include $(MKARCH)
else
ifeq ($(ROOTSYS),)
ROOTSYS = ..
endif
include $(ROOTSYS)/etc/Makefile.arch
endif


#Site Specific  Flags
SYSINCLUDES	=
SYSLIBS         = 

ifdef ANITA_UTIL_INSTALL_DIR
ANITA_UTIL_LIB_DIR=${ANITA_UTIL_INSTALL_DIR}/lib
ANITA_UTIL_INC_DIR=${ANITA_UTIL_INSTALL_DIR}/include
LD_ANITA_UTIL=-L$(ANITA_UTIL_LIB_DIR)
INC_ANITA_UTIL=-I$(ANITA_UTIL_INC_DIR) 
else
ANITA_UTIL_LIB_DIR=/usr/local/lib
ANITA_UTIL_INC_DIR=/usr/local/include
ifdef EVENT_READER_DIR
LD_ANITA_UTIL=-L$(EVENT_READER_DIR)
INC_ANITA_UTIL=-I$(EVENT_READER_DIR)
endif
endif

#Toggles the FFT functions on and off
USE_FFT_TOOLS=1

ifdef USE_FFT_TOOLS
FFTLIBS = -lRootFftwWrapper -lfftw3
FFTFLAG = -DUSE_FFT_TOOLS
else
FFTLIBS =
FFTFLAG =
endif

ROOTLIBS += -lGui -lTreePlayer

#Generic and Site Specific Flags
CXXFLAGS     += -I. $(ROOTCFLAGS) $(FFTFLAG) $(SYSINCLUDES) $(INC_ANITA_UTIL)
LDFLAGS      += -L. -g $(ROOTLDFLAGS) 

LIBS          = $(ROOTLIBS) -lMathMore -lMinuit -lGeom $(SYSLIBS) $(LD_ANITA_UTIL) $(FFTLIBS)  -lAnitaEvent -lMagicDisplay -lAwareWeb -lprofiler
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

#Now the bits we're actually compiling
ROOT_LIBRARY = libAnitaTelem.${DllSuf}
LIB_OBJS =  AnitaHeaderHandler.o AnitaHkHandler.o AnitaGpsHandler.o AnitaMonitorHandler.o  AnitaSurfHkHandler.o  AnitaTurfRateHandler.o  AnitaAuxiliaryHandler.o rawWebDict.o AnitaCmdEchoHandler.o AnitaFileHandler.o  AnitaGenericHeaderHandler.o AnitaGpuHandler.o # AnitaSlowRateHandler.o  plotUtils.o RunNumServer.o 
CLASS_HEADERS =  AnitaHeaderHandler.h  AnitaHkHandler.h AnitaGpsHandler.h  AnitaMonitorHandler.h AnitaSurfHkHandler.h AnitaTurfRateHandler.h AnitaAuxiliaryHandler.h AnitaCmdEchoHandler.h  AnitaFileHandler.h  AnitaGenericHeaderHandler.h AnitaGpuHandler.h  #        AnitaSlowRateHandler.h  plotUtils.h RunNumServer.h



all :   $(ROOT_LIBRARY) processTelemFile  # lib/libConfig.so lib/libkvp.so 


processTelemFile : processTelemFile.o $(ROOT_LIBRARY)
	@echo "<**Linking**> "$@ 
	$(LD) $(LDFLAGS) processTelemFile.o $(LIBS) -L. -lAnitaTelem -o $@
	@chmod 755 $<

lib/libConfig.so:
	@cd configLib ; make

lib/libkvp.so:
	@cd kvpLib; make

#The library
$(ROOT_LIBRARY) : $(LIB_OBJS) 
	@echo "Linking $@ ..."	
	@echo "Platform $(PLATFORM)"
ifeq ($(ARCH),aix5)
		$(MAKESHARED) $(OutPutOpt) $@ $(LIBS) -p 0 $^
else
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$@ $(LDFLAGS) $^ $(OutPutOpt) $@ $(LIBS) $(EXPLLINKLIBS)
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
endif
endif
else
ifeq ($(PLATFORM),win32)
		bindexplib $* $^ > $*.def
		lib -nologo -MACHINE:IX86 $^ -def:$*.def \
		   $(OutPutOpt)$(EVENTLIB)
		$(LD) $(SOFLAGS) $(LDFLAGS) $^ $*.exp $(LIBS) \
		   $(OutPutOpt)$@
		$(MT_DLL)
else
		$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(LIBS) $(EXPLLINKLIBS)
endif
endif
endif
		@echo "$@ done"

%.$(ObjSuf) : %.$(SrcSuf)
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) -c $< -o  $@

%.$(ObjSuf) : %.C
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) $ -c $< -o  $@


rawWebDict.C: $(CLASS_HEADERS)
	@echo "Generating dictionary ..."
	@ rm -f *Dict* 
	rootcint $@ -c $(INC_ANITA_UTIL) $(CLASS_HEADERS) LinkDef.h

install: $(ROOT_LIBRARY)
ifeq ($(PLATFORM),macosx)
	cp $(ROOT_LIBRARY) $(subst .$(DllSuf),.so,$(ROOT_LIBRARY)) $(ANITA_UTIL_LIB_DIR)
else
	cp $(ROOT_LIBRARY) $(ANITA_UTIL_LIB_DIR)
endif
	cp  $(CLASS_HEADERS) $(ANITA_UTIL_INC_DIR)

clean:
	@rm -f *Dict*
	@rm -f *.${ObjSuf}
	@rm -f $(LIBRARY)
	@rm -f $(ROOT_LIBRARY)
	@rm -f $(subst .$(DllSuf),.so,$(ROOT_LIBRARY))	
	@rm -f $(TEST)
#	@cd configLib ; make clean
#	@cd kvpLib ; make clean
#############################################################################



