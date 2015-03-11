include Makefile.inc

# Dependency options

# Subdirs to search for additional source files
#
SUBDIRS := $(shell ls -F | grep "\/" | egrep -v "^_" )
DIRS := ./ $(SUBDIRS)
SOURCE_FILES := $(foreach d, $(DIRS), $(wildcard $(d)*.cpp) )
HEADER_FILES := $(foreach d, $(DIRS), $(wildcard $(d)*.h) )
ASM_SOURCE_FILES := $(foreach d, $(DIRS), $(wildcard $(d)*.asm) )
#
# Create an object file of every cpp and asm file
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCE_FILES)) $(patsubst %.asm, %.o, $(ASM_SOURCE_FILES))

DEP_OUTPUT = kernel.d
DEPENDENCY_OPTIONS = -MM

#
#     # Make $(PROJECT) the default target
all: $(OBJECTS) 
#
print-%:
	@echo $* = $($*)
#
# Create .d dependency files
#$(DEP_OUTPUT): $(SOURCE_FILES) $(HEADER_FILES)
#	$(CXX) $(CXXFLAGS) $(DEPENDENCY_OPTIONS) $(SOURCE_FILES) > $(DEP_OUTPUT)

#-include $(DEP_OUTPUT)
-include $(OBJECTS:.o=.d)

# Compile every cpp file to an object
%.o: %.cpp
	    $(CXX) $(CXXFLAGS) -MMD -c $(COMPILE_OPTIONS) -o $@ $< 

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
	rm -rf *.o */*.o
	rm -rf *.d */*.d

.PHONY: depclean
depclean:
	rm -f $(DEPENDENCIES)

clean-all: clean depclean

