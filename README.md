# Automata Simulator Suite

Cross-platform implementation of DFA, PDA, and NPDA simulators with both command-line and GUI interfaces.

## GUI Demonstration

| Main Interface | Automated Testing | Manual Input |
| :---: | :---: | :---: |
| ![GUI Interface](demo/g1.png) | ![GUI Automated Testing](demo/g2.png) | ![GUI Manual Input](demo/g3.png) |

## Features

- **DFA**: Language with exactly one '1' in {0,1}*
- **PDA**: Language {0^n 1^(n+1)} 
- **NPDA**: Palindromes in {0,1}*
- Command-line tools with interactive testing
- Unified GUI application with all three simulators
- Cross-platform support (Windows, macOS, Linux)

## CLI Demonstrations

| DFA Demo | PDA Demo | NPDA Demo |
| :---: | :---: | :---: |
| ![DFA Demo](demo/dfa.gif) | ![PDA Demo](demo/pda.gif) | ![NPDA Demo](demo/npda.gif) |

## Quick Start

```bash
git clone https://github.com/gammaploid/POP_DFA-PDA-NPDA.git
cd POP_DFA-PDA-NPDA

# Build command-line tools (no dependencies)
make dfa pda npda

# Build GUI (requires GLFW, GLEW, OpenGL)
make gui

# Run
./dfa    # DFA command-line interface
./pda    # PDA command-line interface  
./npda   # NPDA command-line interface
./gui    # GUI application (all simulators)
```

## Dependencies

- **Command-line tools**: None (pure C)
- **GUI application**: GLFW3, GLEW, OpenGL 3.3+

See [BUILD.md](BUILD.md) for detailed installation instructions.

## License

MIT License - see [LICENSE](LICENSE) for details.
