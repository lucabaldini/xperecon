include ./Makefile.inc

TARGET		:= Pixy
TARGLIB		:= XpeEvent.so 

MAINDIR		:= $(shell pwd)
INCDIR		:= $(MAINDIR)/include
SRCDIR		:= $(MAINDIR)/src
BINDIR 		:= $(MAINDIR)/bin
LIBDIR 		:= $(MAINDIR)/lib
OBJDIR		:= $(MAINDIR)/obj

LINKDEFS	:= TMainGUI TEventDisplay

INCLUDES  	:= $(wildcard $(INCDIR)/*.h)                          
SOURCES     	:= $(wildcard $(SRCDIR)/*.cxx)
FSOURCES  	:= $(wildcard $(SRCDIR)/*.f)
OBJECTS		:= $(SOURCES:$(SRCDIR)/%.cxx=$(OBJDIR)/%.o)
OBJECTS      	+= $(OBJDIR)/PixiDict.o 
MAINOBJ        	:= $(OBJDIR)/TPixy.o $(OBJDIR)/anyoption.o

#-------------------------------------------------
# primary targets
#-------------------------------------------------

.PHONY : lib clean

exec: $(TARGET)

$(TARGET): $(MAINOBJ) $(TARGLIB)
	@echo "Creating executable file $@"
	@$(LD) $(LDFLAGS) $^ -o $@  $(ROOTLIBS) $(ROOTGLIBS) $(CFITSIOLIB)

lib: $(TARGLIB)

$(TARGLIB): $(filter-out $(MAINOBJ),$(OBJECTS))
	@echo "Creating shared library $@"
	@$(LD) $(SHARED) $(LDFLAGS) $^ -o $@ $(ROOTLIBS) $(ROOTGLIBS) $(CFITSIOLIB) 

clean:
	@echo "Cleaning up ..."
	@$(DEL) $(MAINDIR)/$(TARGET) $(MAINDIR)/$(TARGLIB) \
	$(OBJECTS) $(BINDIR)/*.d \
	$(INCDIR)/PixiDict.h \$(SRCDIR)/PixiDict.cxx 

#-------------------------------------------------
# specific ROOT Classes rules
#-------------------------------------------------

$(OBJDIR)/PixiDict.o: $(SRCDIR)/PixiDict.cxx
	echo "Compiling $< ..."	
	@$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

#$(SRCDIR)/PixiDict.cxx : $(INCDIR)/LinkDefs.h  
#	@echo "Creating dictionary $@"
#	@$(ROOTSYS)/bin/rootcint -f $@ -c $(CXXFLAGS) -I$(INCDIR) $(INCDIR)/XpeEvent.h $^
#	@mv -f $(SRCDIR)/PixiDict.h $(INCDIR)/

$(SRCDIR)/PixiDict.cxx : $(INCDIR)/LinkDefs.h  
	@echo "Creating dictionary $@"
	@$(ROOTSYS)/bin/rootcint -f $@ -c $(INCDIR)/TMainGUI.h $^
	@mv -f $(SRCDIR)/PixiDict.h $(INCDIR)/

$(INCDIR)/LinkDefs.h : 
	@echo "Generating $@"
	@$(INCDIR)/LinkDefMaker.pl $(LINKDEFS)
	@mv -f LinkDefs.h $(INCDIR)/

#-------------------------------------------------
# generic rules
#-------------------------------------------------

$(OBJDIR)/%.o : $(SRCDIR)/%.cxx; 
	@echo "Compiling $< ..." 
	@$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) -I$(INCDIR) -c $< -o $@

$(OBJDIR)/%.o : $(SRCDIR)/%.f; 
	@echo "Compiling $< ..." 
	@$(FC) $(FCFLAGS) -c $< -o $@

$(BINDIR)/%.d : $(SRCDIR)/%.cxx; 
	@echo "Making dependencies for file $< ..."
	@set -e;\
	$(CXX) -MM $(CXXFLAGS) $(ROOTCFLAGS) -I$(INCDIR) -w $< | \
	sed 's!$*\.o!$(BINDIR)& $@!' >$@;\
	[ -s $@ ] || rm -f $@		

ifneq ($(MAKECMDGOALS),clean)
-include $(SOURCES:$(SRCDIR)/%.cxx=$(BINDIR)/%.d)
endif
