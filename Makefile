############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################
include Makefile.arch

#Site Specific  Flags
SYSINCLUDES	=
SYSLIBS         = -L/unix/anita/software/install/lib/ -lgsl 

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
CXXFLAGS     += $(ROOTCFLAGS) $(FFTFLAG) $(SYSINCLUDES) $(INC_ANITA_UTIL)
LDFLAGS      += -g $(ROOTLDFLAGS) 

LIBS          = $(ROOTLIBS) -lMathMore -lMinuit -lGeom $(SYSLIBS) $(LD_ANITA_UTIL) $(FFTLIBS)  -lAnitaEvent -lMagicDisplay
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

#Now the bits we're actually compiling
ROOT_LIBRARY = libAnitaTelem.${DLLSUF}
LIB_OBJS =  AnitaHeaderHandler.o  #AnitaCmdEchoHandler.o   AnitaSurfHkHandler.o AnitaFileHandler.o  AnitaHkHandler.o  AnitaTurfRateHandler.o AnitaGenericHeaderHandler.o  AnitaMonitorHandler.o AnitaAuxiliaryHandler.o AnitaGpsHandler.o AnitaSlowRateHandler.o  plotUtils.o RunNumServer.o rawWebDict.o
CLASS_HEADERS =  AnitaHeaderHandler.h #AnitaCmdEchoHandler.h    AnitaSurfHkHandler.h AnitaFileHandler.h  AnitaHkHandler.h  AnitaTurfRateHandler.h AnitaGenericHeaderHandler.h  AnitaMonitorHandler.h AnitaAuxiliaryHandler.h AnitaGpsHandler.h AnitaSlowRateHandler.h  plotUtils.h RunNumServer.h



all :  processTelemFile  $(ROOT_LIBRARY) # lib/libConfig.so lib/libkvp.so 


processTelemFile : processTelemFile.o #$(ROOT_LIBRARY)
	@echo "<**Linking**> "$@ 
	$(LD) $(LDFLAGS) processTelemFile.o $(LIBS) $(ROOT_LIBRARY) -o $@
	@chmod 755 $<

lib/libConfig.so:
	@cd configLib ; make

lib/libkvp.so:
	@cd kvpLib; make

#The library
$(ROOT_LIBRARY) : $(LIB_OBJS) 
	@echo "Linking $@ ..."
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
	$(LD) $(SOFLAGS) $^ $(OutPutOpt) $@
ifeq ($(MACOSX_MINOR),4)
	ln -sf $@ $(subst .$(DLLSUF),.so,$@)
else
	$(LD) -bundle -undefined $(UNDEFOPT) $(LDFLAGS) $^ \
	 $(OutPutOpt) $(subst .$(DLLSUF),.so,$@)
endif
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $(LIB_OBJS) $(LIBS)  -o $@
endif

%.$(OBJSUF) : %.$(SRCSUF)
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) -c $< -o  $@

%.$(OBJSUF) : %.C
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) $ -c $< -o  $@


rawWebDict.C: $(CLASS_HEADERS)
	@echo "Generating dictionary ..."
	@ rm -f *Dict* 
	rootcint $@ -c $(INC_ANITA_UTIL) $(CLASS_HEADERS) LinkDef.h

install: $(ROOT_LIBRARY)
ifeq ($(PLATFORM),macosx)
	cp $(ROOT_LIBRARY) $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY)) $(ANITA_UTIL_LIB_DIR)
else
	cp $(ROOT_LIBRARY) $(ANITA_UTIL_LIB_DIR)
endif
	cp  $(CLASS_HEADERS) $(ANITA_UTIL_INC_DIR)

clean:
	@rm -f *Dict*
	@rm -f *.${OBJSUF}
	@rm -f $(LIBRARY)
	@rm -f $(ROOT_LIBRARY)
	@rm -f $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY))	
	@rm -f $(TEST)
#	@cd configLib ; make clean
#	@cd kvpLib ; make clean
#############################################################################



