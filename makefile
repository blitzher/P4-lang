# Requires the following project directory structure:
#  /bin
#  /obj
#  /src

# Use 'make remove' to clean up the hole project

# Name of target file


CXX        = clang++
CXXFLAGS   = -std=c++20 \
             -Wall -Wextra -Werror -Wpointer-arith -Wcast-qual \
             -Wno-missing-braces -Wempty-body -Wno-error=uninitialized \
             -Wno-error=deprecated-declarations \
             -pedantic \
             -Os

LD         = clang++ -o
LDFLAGS    = -Wall -pedantic

SRCDIR     = src
OBJDIR     = obj
BINDIR     = bin
TESTDIR    = testing

SOURCES   := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES  := $(wildcard $(SRCDIR)/*.h)
OBJECTS   := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

T_SOURCES   := $(wildcard $(SRCDIR)/$(TESTDIR)/*.cpp)
T_INCLUDES  := $(wildcard $(SRCDIR)/$(TESTDIR)/*.h)
T_OBJECTS   := $(SOURCES:$(T_SRCDIR)/$(TESTDIR)/%.cpp=$(OBJDIR)/%.o)

RM         = rm -f

T_TARGETS = $(wildcard $(TESTDIR)/*.test.cpp)
TARGET       = main

$(BINDIR)/$(T_TARGETS): $(OBJECTS)
	@$(LD) $@ $(LDFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(T_OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LD) $@ $(LDFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(RM) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(RM) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

test: $(BINDIR)/$(T_TARGETS)
	@echo $<

run: $(BINDIR)/$(TARGET)
	@./$< examples/Carbonara.rcp
