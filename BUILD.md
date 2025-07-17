# Build Instructions

Simple automata simulator suite with DFA, PDA, and NPDA implementations.

## Components

- **Command-line tools** (dfa, pda, npda) - No dependencies required
- **GUI application** - Requires GLFW3, GLEW, OpenGL 3.3+

## Dependencies (GUI only)

- **GLFW3** - Window management
- **GLEW** - OpenGL extensions
- **OpenGL 3.3+** - Graphics
- **C99 compiler** (GCC, Clang, MSVC)

---

## Quick Start

### Command-line tools (no dependencies)
```bash
make dfa    # Build DFA simulator
make pda    # Build PDA simulator  
make npda   # Build NPDA simulator
make all    # Build everything
```

### GUI Application

#### macOS
```bash
brew install glfw glew pkg-config
make gui
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt install libglfw3-dev libglew-dev libgl1-mesa-dev pkg-config
make gui
```

#### Windows (MSYS2)
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-glfw mingw-w64-x86_64-glew
make gui-windows
```

## Project Structure

```
POP_DFA-PDA-NPDA/
├── dfa.c               # DFA command-line simulator
├── pda.c               # PDA command-line simulator  
├── npda.c              # NPDA command-line simulator
├── GUI.c               # GUI application (all three simulators)
├── nuklear.h           # GUI library
├── nuklear_glfw_gl3.h  # GLFW integration
├── Makefile            # Build system
├── CMakeLists.txt      # CMake configuration
└── BUILD.md            # This file
```

## Features

### Command-line Applications
- **DFA**: Language with exactly one '1' in {0,1}*
- **PDA**: Language {0^n 1^(n+1)} 
- **NPDA**: Palindromes in {0,1}*

### GUI Application
- All three simulators in one interface
- Interactive input processing
- Transition table displays
- Test case demonstrations
- Cross-platform (Windows, macOS, Linux)

## Usage

Run any executable for interactive mode:
```bash
./dfa    # DFA command-line interface
./pda    # PDA command-line interface  
./npda   # NPDA command-line interface
./gui    # GUI application (all simulators)
``` 