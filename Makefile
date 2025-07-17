
######################################################################################
##                           AUTOMATA SIMULATOR MAKEFILE                          ##
######################################################################################

# Project: DFA, PDA, NPDA Simulator Suite
# Core Components: dfa.c, pda.c, npda.c, GUI.c

.PHONY: usage all gui clean gui-linux gui-macos gui-windows

usage:
	@echo "=================================="
	@echo "  Automata Simulator Build System"
	@echo "=================================="
	@echo "make gui           Build GUI application (auto-detect platform)"
	@echo "make dfa           Build DFA command-line tool"
	@echo "make pda           Build PDA command-line tool" 
	@echo "make npda          Build NPDA command-line tool"
	@echo "make all           Build all components"
	@echo "make clean         Clean all build artifacts"
	@echo ""
	@echo "Platform-specific GUI builds:"
	@echo "make gui-linux     Build GUI for Linux"
	@echo "make gui-macos     Build GUI for macOS"
	@echo "make gui-windows   Build GUI for Windows"
	@echo ""
	@echo "Dependencies: GLFW3, GLEW, OpenGL 3.3+"
	@echo "See BUILD.md for installation instructions"

all: gui dfa pda npda

########################################################################################
##   GUI Application - Cross Platform

# Detect operating system
UNAME_S := $(shell uname -s)

# Compiler settings
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2

# Platform-specific settings
ifeq ($(UNAME_S),Linux)
    PLATFORM = LINUX
    LIBS = -lglfw -lGLEW -lGL -lm -ldl
    CFLAGS += -D_DEFAULT_SOURCE
endif

ifeq ($(UNAME_S),Darwin)
    PLATFORM = MACOS
    LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    LIBS += $(shell pkg-config --libs glfw3 glew)
    CFLAGS += $(shell pkg-config --cflags glfw3 glew)
endif

# Windows (MinGW)
ifeq ($(OS),Windows_NT)
    PLATFORM = WINDOWS
    LIBS = -lglfw3 -lglew32 -lopengl32 -lgdi32 -luser32 -lkernel32
    CFLAGS += -DGLEW_STATIC
    EXE_EXT = .exe
endif

# Default target
gui: GUI.c nuklear.h nuklear_glfw_gl3.h
	@echo "Building for $(PLATFORM)..."
	$(CC) $(CFLAGS) GUI.c -o gui$(EXE_EXT) $(LIBS)
	@echo "Build complete! Run with: ./gui$(EXE_EXT)"

# Platform-specific targets for explicit building
gui-linux: 
	@echo "Force building for Linux..."
	$(CC) -std=c99 -Wall -O2 -D_DEFAULT_SOURCE GUI.c -o gui -lglfw -lGLEW -lGL -lm -ldl

gui-macos:
	@echo "Force building for macOS..."
	$(CC) -std=c99 -Wall -O2 GUI.c -o gui $$(pkg-config --cflags --libs glfw3 glew) -framework OpenGL

gui-windows:
	@echo "Force building for Windows..."
	$(CC) -std=c99 -Wall -O2 -DGLEW_STATIC GUI.c -o gui.exe -lglfw3 -lglew32 -lopengl32 -lgdi32

########################################################################################
##   Command-line Applications

dfa: dfa.c
	@echo "Building DFA command-line application..."
	$(CC) $(CFLAGS) dfa.c -o dfa

pda: pda.c  
	@echo "Building PDA command-line application..."
	$(CC) $(CFLAGS) pda.c -o pda

npda: npda.c
	@echo "Building NPDA command-line application..."
	$(CC) $(CFLAGS) npda.c -o npda

########################################################################################
##   Clean

clean:
	@echo "Cleaning build artifacts..."
	rm -f gui gui.exe dfa pda npda
	rm -f AutomataSimulator AutomataSimulator.exe
	rm -rf build/
