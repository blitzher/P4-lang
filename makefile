# Requires the following project directory structure:
#  /bin
#  /obj
#  /src

# Use 'make remove' to clean up the whole project

CXX        = clang++
CXXFLAGS   = -std=c++17 \
             -Wall -Wextra -Wpointer-arith -Wcast-qual \
             -Wno-missing-braces -Wempty-body -Wno-error=uninitialized \
             -Wno-error=deprecated-declarations \
             -pedantic -fansi-escape-codes \
             -Os -g

LD         = clang++ -o
LDFLAGS    = -Wall -pedantic

SRCDIR     = src
OBJDIR     = obj
BINDIR     = bin
DISDIR 	   = dist
TESTDIR    := $(SRCDIR)/testing

SOURCES   := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES  := $(wildcard $(SRCDIR)/*.h)
OBJECTS   := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

T_TARGET_SOURCES := $(wildcard $(TESTDIR)/*.test.cpp)
T_SOURCES   := $(wildcard $(TESTDIR)/*.cpp)
T_INCLUDES  := $(wildcard $(TESTDIR)/*.h)
T_OBJECTS   := $(T_SOURCES:$(TESTDIR)/%.cpp=$(OBJDIR)/%.o)
T_LIBOBJS	:= $(filter-out %.test.o, $(T_OBJECTS))

RM         = rm -rf
MKDIRS 	   = mkdir -p $(BINDIR) $(OBJDIR)

T_TARGETS 	 := $(T_TARGET_SOURCES:$(TESTDIR)/%.test.cpp=$(BINDIR)/%.test)
TARGET       = main


$(T_TARGETS):: $(T_OBJECTS) $(OBJECTS)
	@$(LD) $@ $(LDFLAGS) $(T_LIBOBJS) $(filter-out obj/main.o, $(OBJECTS)) $(subst $(BINDIR),$(OBJDIR),$@).o

$(T_OBJECTS): $(OBJDIR)/%.o: $(TESTDIR)/%.cpp
	@$(MKDIRS)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LD) $@$(BIN_EXT) $(LDFLAGS) $(OBJECTS)
	@cp $@ $@.exe
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(MKDIRS)
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

# Named rules, ignore files if such exist
.PHONY: clean remove dirs test run gui
clean:
	@$(RM) $(OBJDIR) $(DISDIR)
	@echo "Cleanup complete!"

remove: clean
	@$(RM) $(BINDIR)
	@echo "Executables removed!"

$(T_TARGETS)::
	@echo "\033[kRunning \033[4m$(subst $(BINDIR)/,,$@)\033[0m"
	@./$@

# runnable targets 
run: $(BINDIR)/$(TARGET)
	./$< examples/Carbonara.rcp

run-infinite-recursion: $(BINDIR)/$(TARGET)
	./$< src/test-recipes/recursion0.rcp -o dist

test::
	@rm -rf $(BINDIR)/.tests
test:: $(T_TARGETS)
	@echo "Finished tests"

watch:
	while true; do make run -q || make run; sleep 0.5; done

gui: $(BINDIR)/$(TARGET)
	python3 src/GUI.py
	

ducks: doxyfile
	doxygen doxyfile
