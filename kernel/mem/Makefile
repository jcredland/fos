include ../Makefile.inc

# Dependency options
DEPENDENCY_OPTIONS = -MM
PROJECT = project
#
#-- Do not edit below this line --
#
# Subdirs to search for additional source files
SUBDIRS := $(shell ls -F | grep "\/" )
DIRS := ./ $(SUBDIRS)
SOURCE_FILES := $(foreach d, $(DIRS), $(wildcard $(d)*.cpp) )
#
# Create an object file of every cpp and asm file
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCE_FILES)) $(patsubst %.asm, %.o, $(SOURCE_FILES))
#
# Dependencies
DEPENDENCIES = $(patsubst %.cpp, %.d, $(SOURCE_FILES))
#
# Create .d files
%.d: %.cpp
	$(CXX) $(CXXFLAGS) $(DEPENDENCY_OPTIONS) $< -MT "$*.o $*.d" -MF $*.d
#
#     # Make $(PROJECT) the default target
all: $(OBJECTS) $(DEPENDENCIES) $(wildcard *.asm)

#$(PROJECT): $(OBJECTS)
#	$(CXX) $(CXXFLAGS) -o $(PROJECT) $(OBJECTS) $(LIBS)

# Include dependencies (if there are any)
# ifneq "$(strip $(DEPENDENCIES))" ""
#   include $(DEPENDENCIES)
#   endif
#
#   # Compile every cpp file to an object
%.o: %.cpp
	    $(CXX) $(CXXFLAGS) -c $(COMPILE_OPTIONS) -o $@ $< $(HEADERS)

# Assemble any .asm files
%.o: %.asm
	nasm -f elf -o $@ $<	

# Build & Run Project
# run: $(PROJECT)
#     ./$(PROJECT) $(COMMANDLINE_OPTIONS)
#
#     # Clean & Debug
.PHONY: makefile-debug
makefile-debug:

.PHONY: clean
clean:
	rm -f $(PROJECT) $(OBJECTS)

.PHONY: depclean
depclean:
	rm -f $(DEPENDENCIES)

clean-all: clean depclean

