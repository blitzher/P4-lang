# Requires the following project directory structure:
#  /bin
#  /obj
#  /src

# Use 'make remove' to clean up the hole project

# Name of target file


CXX        = clang++
CXXFLAGS   = -std=c++17 \
             -Wall -Wextra -Wpointer-arith -Wcast-qual \
             -Wno-missing-braces -Wempty-body -Wno-error=uninitialized \
             -Wno-error=deprecated-declarations \
             -pedantic \
             -Os

LD         = clang++ -o
LDFLAGS    = -Wall -pedantic

SRCDIR     = src
OBJDIR     = obj
BINDIR     = bin
TESTDIR    := $(SRCDIR)/testing

SOURCES   := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES  := $(wildcard $(SRCDIR)/*.h)
OBJECTS   := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

T_TARGET_SOURCES := $(wildcard $(TESTDIR)/*.test.cpp)
T_SOURCES   := $(wildcard $(TESTDIR)/*.cpp)
T_INCLUDES  := $(wildcard $(TESTDIR)/*.h)
T_OBJECTS   := $(T_SOURCES:$(TESTDIR)/%.cpp=$(OBJDIR)/%.o)
T_LIBOBJS	:= $(filter-out %.test.o, $(T_OBJECTS))

RM         = rm -f

T_TARGETS 	 := $(T_TARGET_SOURCES:$(TESTDIR)/%.test.cpp=$(BINDIR)/%.test)
TARGET       = main

$(T_TARGETS):: $(T_OBJECTS)
	@$(LD) $@ $(LDFLAGS) $(T_LIBOBJS) $(subst $(BINDIR),$(OBJDIR),$@).o

$(T_OBJECTS): $(OBJDIR)/%.o : $(TESTDIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LD) $@ $(LDFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(RM) $(OBJECTS) $(T_OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(RM) $(BINDIR)/$(TARGET)
	@$(RM) $(T_TARGETS)
	@echo "Executable removed!"

$(T_TARGETS)::
	@echo "Testing $@"
	@./$@

test: $(T_TARGETS)
	@mkdir -p bin obj
	@echo "Done test rule"

run: $(BINDIR)/$(TARGET) 
	@mkdir -p bin obj
	./$< examples/Carbonara.rcp
