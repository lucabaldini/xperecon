 
ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)

# Linux with egcs
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC -Wno-deprecated
LD            = g++
LDFLAGS       = -g
SOFLAGS       = -shared


CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTLIBS)
GLIBS         = $(ROOTGLIBS)

#-----------------------------------------------------------------------------------------------

OBJS = TPixy.o TMainGUI.o TInputFile.o TEventDisplay.o THexagonCol.o TDetector.o TCluster_hex.o TEditor.o PixiDict.o
CINT_HDRS = TInputFile.h TMainGUI.h TEventDisplay.h THexagonCol.h TDetector.h TCluster_hex.h TEditor.h PixiLinDef.h

 PROGRAMS = Pixy

Pixy:		$(OBJS)
		$(LD) $(LDFLAGS) $(OBJS) $(GLIBS) -o Pixy
		@echo "$@ done"

clean:
	@rm -f $(OBJS) *Dict.* core

.SUFFIXES: .cxx

.cxx.o:
	$(CXX) $(CXXFLAGS) -c $<


TMainGUI.o:TMainGUI.h
PixiDict.cxx: TMainGUI.h PixiLinkDef.h
		@echo "Generating dictionary Dict..."
		rootcint -f $@ -c $^


