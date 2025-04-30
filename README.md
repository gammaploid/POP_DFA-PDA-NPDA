# DFA-PDA
Deterministic Finite Automaton (DFA), Pushdown Automaton (PDA), and Non-determenstic Pushdown Automaton (NPDA) simulation in procedural style programming
 
---

## Usage Guide (Mac/Linux/Windows)

Clone:

```sh
git clone https://github.com/gammaploid/DFA-PDA-NPDA.git
```

After cloning, navigate into the project directory:

```sh
cd /DFA-PDA-NPDA
```

### Compile

To compile any of the automaton programs, open your terminal in the project directory and run:

```sh
gcc dfa.c -o dfa
gcc pda.c -o pda
gcc npda.c -o npda
```

### Run

After compiling, run the desired automaton:

```sh
./dfa    # For the DFA 
./pda    # For the PDA 
./npda   # For the NPDA
```

Each program will prompt you for input and display results in the CLI with example of accepeted strings by the the state machines. You can run multiple tests in a row by entering `y` when prompted.

---

## License

```text
This project is liscenced under the MIT License

Copyright (c) 2024 gammaploid
```

