 
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

OBJS = TPixy.o TMainGUI_new_v2.o TInputFile.o TEventDisplay_new_v3.o THexagonCol.o TDetector_new_v2.o TCluster_hex.o TEditor.o PixiDict.o
CINT_HDRS = TInputFile.h TMainGUI_new_v2.h TEventDisplay_new_v3.h THexagonCol.h TDetector_new_v2.h TCluster_hex.h TEditor.h PixiLinDef.h

 PROGRAMS = Pixy

Pixy:		$(OBJS)
		$(LD) $(LDFLAGS) $(OBJS) $(GLIBS) -o Pixy
		@echo "$@ done"

clean:
	@rm -f $(OBJS) *Dict.* core

.SUFFIXES: .cxx

.cxx.o:
	$(CXX) $(CXXFLAGS) -c $<


TMainGUI_new_v2.o:TMainGUI_new_v2.h
PixiDict.cxx: TMainGUI_new_v2.h PixiLinkDef.h
		@echo "Generating dictionary Dict..."
		rootcint -f $@ -c $^


