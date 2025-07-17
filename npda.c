#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define PINK_BG "\033[48;5;218m"
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define WHITE   "\033[37m"
#define MAX_SIZE 1000
#define MAX_STACK_SIZE 100
#define MAX_CONFIGURATIONS 100 
#define MAX_INPUT_SIZE 100

typedef enum {
    S0, // pushing a's
    S1, // popping a's with b's
    S2, // accept after extra b
    //S3 // reject  
} State;

/*
 * Define the stack of the PDA
 * 
 * @param items - The items in the stack
 * @param top - The top of the stack
 */
typedef struct {
    char *items;
    int top;
} Stack;


// --- Configuration (State + Stack + Position) ---
// This struct represents one possible path of the NPDA.
typedef struct {
    State state;                // The state (q0 or q1)
    Stack stack;            // The stack for this path
    int input_pos;          // Position in the input string
} Configuration;


/*
 * Initialize the stack of the PDA
 * 
 * @param stack - The stack of the PDA
 */
void initStack(Stack *stack) {
    stack->items = malloc(MAX_SIZE * sizeof(char));
    if (!stack->items) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    stack->top = -1;
}


/*
 * Push a symbol onto the stack
 * 
 * @param stack - The stack of the PDA
 * @param symbol - The symbol to push
 */
void push(Stack *stack, char symbol) {
    stack->items[++(stack->top)] = symbol;
}

/*
 * Pop a symbol from the stack
 * 
 * @param stack - The stack of the PDA
 * @return The symbol popped from the stack
 */
char pop(Stack *stack) {
    if (stack->top < 0) {
        fprintf(stderr, "Stack underflow.\n");
        exit(1);
    }
    return stack->items[(stack->top)--];
}

/*
 * Peek at the top of the stack
 * 
 * @param stack - The stack of the PDA
 * @return The symbol at the top of the stack
 */
char peek(Stack *stack) {
    if (stack->top < 0)
        return '$';  // treat empty as bottom marker
    return stack->items[stack->top];
}

/*
 * Check if the stack is empty
 * 
 * @param stack - The stack of the PDA
 * @return 1 if the stack is empty, 0 otherwise
 */
int isEmpty(Stack *stack) {
    return stack->top == -1;
}


/*
 * Free the stack of the PDA
 * 
 * @param stack - The stack of the PDA
 */
void freeStack(Stack *stack) {
    if (stack->items) {
        free(stack->items);
        stack->items = NULL; // Prevent double-freeing
    }
}

/*
 * Function to duplicate a stack for a new simulation path
 * @param src - Source stack to copy from
 * @param dest - Destination stack to copy to
 */
void copyStack(const Stack* src, Stack* dest) {
    initStack(dest);
    for (int i = 0; i <= src->top; i++) {
        dest->items[i] = src->items[i];
    }
    dest->top = src->top;
}


/*
 * Validate input string contains only 0s and 1s
 */
int validateInput(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != '0' && str[i] != '1') {
            return 0;
        }
    }
    return 1;
}

const char* stateToString(State s) {
    switch (s) {
        case S0: return "S0";
        case S1: return "S1";
        case S2: return "S2";
    }
    return "??";
}
void printStack(const Stack* stack) {
    printf("[");
    for (int i = 0; i <= stack->top; i++) {
        printf("%c", stack->items[i]);
    }
    printf("]");
}

/*
 * Print the welcome message and NPDA details
 */
void printWelcomeMessage() {
    printf(PINK_BG);
    printf("                              \\\\\\\\\n");
    printf("                            \\\\\\\\\\\\\\\\\n");
    printf("                          \\\\\\\\\\\\\\\\\\\\\\\n");
    printf("  -----------,-|           |C>   // )\\\\|\n");
    printf("           ,','|          /    || ,'/////|\n");
    printf("---------,','  |         (,    ||   /////\n");
    printf("         ||    |          \\\\  ||||//''''|\n");
    printf("         ||    |           |||||||     _|\n");
    printf("         ||    |______      ```\\____/ \\\n");
    printf("         ||    |     ,|         _/_____/ \\\n");
    printf("         ||  ,'    ,' |        /          |\n");
    printf("         ||,'    ,'   |       |         \\  |\n");
    printf("_________|/    ,'     |      /           | |\n");
    printf("_____________,'      ,',_____|      |    | |\n");
    printf("             |     ,','      |      |    | |\n");
    printf("             |   ,','    ____|_____/    /  |\n");
    printf("             | ,','  __/ |             /   |\n");
    printf("_____________|','   ///_/-------------/   |\n");
    printf("              |===========,'\n \n ");
    printf("==================================================================\n");
    printf("Welcome to the machine. Non-Deterministic Pushdown Automaton (NPDA)\n");
    printf("This machine checks for palindromes over the alphabet {0, 1}\n");
    printf("==================================================================\n");
    printf(RESET);
}

/*
 * Print the transition table for the NPDA
 */
void printTransitionTable() {
    printf("\n" YELLOW "NPDA Transition Table for Palindromes:" RESET "\n");
    printf("┌───────┬────────┬───────────┬──────────────────┬────────────┐\n");
    printf("│ State │ Input  │ Stack Top │      Action      │ Next State │\n");
    printf("├───────┼────────┼───────────┼──────────────────┼────────────┤\n");
    printf("│  S0   │   1    │     1     │   Push 1,1       │     S0     │\n");
    printf("│  S0   │   1    │     0     │   Push 1,0       │     S0     │\n");
    printf("│  S0   │   0    │     1     │   Push 0,1       │     S0     │\n");
    printf("│  S0   │   0    │     0     │   Push 0,0       │     S0     │\n");
    printf("│  S0   │   0    │     @     │   Push 0         │     S0     │\n");
    printf("│  S0   │   1    │     @     │   Push 1         │     S0     │\n");
    printf("│  S0   │   1    │     1     │   Pop 1          │     S1     │\n");
    printf("│  S0   │   0    │     0     │   Pop 0          │     S1     │\n");
    printf("│  S0   │   ε    │     1     │   Pop 1          │     S1     │\n");
    printf("│  S0   │   ε    │     0     │   Pop 0          │     S1     │\n");
    printf("│  S1   │   0    │     0     │   Pop 0          │     S1     │\n");
    printf("│  S1   │   1    │     1     │   Pop 1          │     S1     │\n");
    printf("│  S1   │   ε    │     Z     │   No change      │    *S2     │\n");
    printf("└───────┴────────┴───────────┴──────────────────┴────────────┘\n");
    printf("@ = empty stack, Z = stack bottom, S2 = accept state\n\n");
}

/*
 * Non-deterministic transition function for the NPDA
 * 
 * @param input_string - The input string
 * @param len - Length of input string
 * @return 1 if accepted, 0 if rejected
 */int transition(const char* input_string, int len) {
    Configuration current_configs[MAX_CONFIGURATIONS];
    Configuration next_configs[MAX_CONFIGURATIONS];

     int num_current_configs = 1;
    int num_next_configs = 0;
    int generation = 0;

    initStack(&current_configs[0].stack);
    current_configs[0].state = S0;
    current_configs[0].input_pos =  0;
    
    if (num_current_configs < MAX_CONFIGURATIONS) {
        current_configs[num_current_configs].state = S1;
        copyStack(&current_configs[0].stack, &current_configs[num_current_configs].stack);
        current_configs[num_current_configs].input_pos = 0;
        num_current_configs++;
    }

    while (num_current_configs > 0) {
        printf("\n--- Generation %d ---\n", generation++);
        printf("Processing %d current configuration(s)...\n", num_current_configs );

        num_next_configs = 0;
        int accepted = 0;

        for (int i = 0; i < num_current_configs; i++) {
            Configuration conf = current_configs[i];
            //debug
            // printf("conf.state: %d\n", conf.state);
            // printf("conf.input_pos: %d\n", conf.input_pos);
            // printf("conf.stack.top: %d\n", conf.stack.top);
            // printf("conf.stack.items: %s\n", conf.stack.items);
            // printf("conf.stack.items[0]: %c\n", conf.stack.items[0]);
            // printf("conf.stack.items[1]: %c\n", conf.stack.items[1]);
            // printf("conf.stack.items[2]: %c\n", conf.stack.items[2]);
            // --- Print the configuration we are processing ---
            printf("  [%d] Processing conf: State=%s, Pos=%d, Stack=", i, stateToString(conf.state), conf.input_pos);
            printStack(&conf.stack);
            printf("\n");

            if (conf.input_pos == len) {
                if (conf.state == S1 && isEmpty(&conf.stack)) {
                    printf("    -> Path leads to ACCEPTANCE (End of input, S1, Empty Stack)\n");
                    accepted = 1;
                    //debug
                    // printf("conf.state: %d\n", conf.state);
                    // printf("conf.input_pos: %d\n", conf.input_pos);
                    // printf("conf.stack.top: %d\n", conf.stack.top);
                    // printf("conf.stack.items: %s\n", conf.stack.items);
                } else {
                    printf("    -> Path dies (End of input, non-accepting)\n");
                    //debug
                    // printf("conf.state: %d\n", conf.state);
                    // printf("conf.input_pos: %d\n", conf.input_pos);
                    // printf("conf.stack.top: %d\n", conf.stack.top);
                    // printf("conf.stack.items: %s\n", conf.stack.items);
                }
                continue;
            }


          char c = input_string[conf.input_pos];
            printf("    (Input char: '%c')\n", c);

            if (conf.state == S0) {

                printf("    ->  Path 1 (Push): Creating new config (S0, pos+1, push '%c')\n", c);
                if (num_next_configs < MAX_CONFIGURATIONS) {
                    next_configs[num_next_configs].state = S0;
                    copyStack(&conf.stack, &next_configs[num_next_configs].stack);
                    push(&next_configs[num_next_configs].stack, c);
                    next_configs[num_next_configs].input_pos = conf.input_pos + 1;
                    num_next_configs++;
                    //debug
                    // printf("next_configs[num_next_configs].state: %d\n", next_configs[num_next_configs].state);
                    // printf("next_configs[num_next_configs].input_pos: %d\n", next_configs[num_next_configs].input_pos);
                    // printf("next_configs[num_next_configs].stack.top: %d\n", next_configs[num_next_configs].stack.top);
                    // printf("next_configs[num_next_configs].stack.items: %s\n", next_configs[num_next_configs].stack.items);
                    // printf("next_configs[num_next_configs].stack.items[0]: %c\n", next_configs[num_next_configs].stack.items[0]);
                    // printf("next_configs[num_next_configs].stack.items[1]: %c\n", next_configs[num_next_configs].stack.items[1]);
                }

                // Odd Middle
                printf("    -> Path 2 (Skip): Creating new config (S1, pos+1, stack unchanged)\n");
                if (num_next_configs < MAX_CONFIGURATIONS) {
                    next_configs[num_next_configs].state = S1;
                    copyStack(&conf.stack, &next_configs[num_next_configs].stack);
                    next_configs[num_next_configs].input_pos = conf.input_pos + 1;
                    num_next_configs++;
                    //debug
                    // printf("next_configs[num_next_configs].state: %d\n", next_configs[num_next_configs].state);
                    // printf("next_configs[num_next_configs].input_pos: %d\n", next_configs[num_next_configs].input_pos);
                    // printf("next_configs[num_next_configs].stack.top: %d\n", next_configs[num_next_configs].stack.top);
                    // printf("next_configs[num_next_configs].stack.items: %s\n", next_configs[num_next_configs].stack.items);
                    // printf("next_configs[num_next_configs].stack.items[0]: %c\n", next_configs[num_next_configs].stack.items[0]);
                    // printf("next_configs[num_next_configs].stack.items[1]: %c\n", next_configs[num_next_configs].stack.items[1]);
                }
                
                //even middle
                printf("    -> Path 3 (Epsilon-Jump): Creating new config (S1, pos unchanged, stack unchanged)\n");
                if (num_next_configs < MAX_CONFIGURATIONS) {

                    next_configs[num_next_configs].state =  S1;
                    copyStack(&conf.stack, &next_configs[num_next_configs].stack);
                    next_configs[num_next_configs].input_pos = conf.input_pos;
                    num_next_configs++;
                    //debug 
                    // printf("next_configs[num_next_configs].state: %d\n", next_configs[num_next_configs].state);
                    // printf("next_configs[num_next_configs].input_pos: %d\n", next_configs[num_next_configs].input_pos);
                    // printf("next_configs[num_next_configs].stack.top: %d\n", next_configs[num_next_configs].stack.top);
                    // printf("next_configs[num_next_configs].stack.items: %s\n", next_configs[num_next_configs].stack.items);
                    // printf("next_configs[num_next_configs].stack.items[0]: %c\n", next_configs[num_next_configs].stack.items[0]);
                    // printf("next_configs[num_next_configs].stack.items[1]: %c\n", next_configs[num_next_configs].stack.items[1]);
                }

            } else if (conf.state == S1) {
                if (!isEmpty(&conf.stack) && peek(&conf.stack) == c) {
                    printf("    -> Path (Match & Pop): Input '%c' matches stack top. Creating new config (S1, pos+1, pop)\n",  c);
                    if (num_next_configs < MAX_CONFIGURATIONS) {

                        next_configs[num_next_configs].state = S1;
                        copyStack(&conf.stack, &next_configs[num_next_configs].stack);
                        pop(&next_configs[num_next_configs].stack);
                        next_configs[num_next_configs].input_pos = conf.input_pos + 1;
                        num_next_configs++;
                        //debug
                        // printf("next_configs[num_next_configs].state: %d\n", next_configs[num_next_configs].state);
                        // printf("next_configs[num_next_configs].input_pos: %d\n", next_configs[num_next_configs].input_pos);
                        // printf("next_configs[num_next_configs].stack.top: %d\n", next_configs[num_next_configs].stack.top);
                        // printf("next_configs[num_next_configs].stack.items: %s\n", next_configs[num_next_configs].stack.items);
                        // printf("next_configs[num_next_configs].stack.items[0]: %c\n", next_configs[num_next_configs].stack.items[0]);
                        // printf("next_configs[num_next_configs].stack.items[1]: %c\n", next_configs[num_next_configs].stack.items[1]);
                }
                }
                 else {
                     printf("    -> Path dies (no match to pop)\n");
                }
            }
        }

        //free current configs
        for (int i = 0; i < num_current_configs; i++) {
            free(current_configs[i].stack.items);
        }
        //free next configs
        if (accepted) {
            for (int i = 0; i < num_next_configs; i++) {
                free(next_configs[i].stack.items);
            }
            return 1;
        }
        //copy next configs to current configs
        num_current_configs = num_next_configs;
        for (int i = 0; i < num_next_configs; i++) {
            current_configs[i] = next_configs[i];
        }
    }

  return 0;
}


/*
 * Run test cases mode and display results
 */
void runTestCases() {
    typedef struct {
        const int test_key;
        const char* input;
        bool is_accept;
    } TestCase;

    TestCase testCases[] = {
        // Accepting 
        {1, "1", false},            
        {2, "0", false},            
        {3, "11", false},           
        {4, "00", false},           
        {5, "101", false},          
        {6, "010", false},          
        {7, "1001", false},         
        {8, "0110", false},         
        {9, "10101", false},        
        {10, "01010", false},       
        // Rejecting
        {11, "", false},            
        {12, "10", false},          
        {13, "01", false},          
        {14, "100", false},         
        {15, "110", false},         
        {16, "1010", false},        
    };

    size_t numTests = sizeof(testCases) / sizeof(testCases[0]);



    printf(YELLOW "\n--- Running NPDA Test Cases ---\n" RESET);
    
    for (int i = 0; i < (int)numTests; i++) {
        const char* input_string_ = testCases[i].input;
        
        printf(MAGENTA "\n---------------------------------------------------------------------\n" RESET);
        printf("Demonstrating with string: \"%s\"\n", input_string_[0] == '\0' ? "(empty)" : testCases[i].input);
        printf(WHITE "Processing string..." RESET "\n");
        printf("Starting NPDA simulation...\n");

        // Run the NPDA transition function
        int len = strlen(input_string_);
        int result = transition(input_string_, len);
        
        if (result) {
            printf(GREEN "\n==================================================\n" RESET);
            printf("String '%s' is " GREEN "Accepted!\n" RESET, testCases[i].input);
            printf("It is a valid palindrome.\n");
            printf(GREEN "==================================================\n" RESET);
            testCases[i].is_accept = true;
        } else {
            printf(RED "\n**************************************************\n" RESET);
            printf("String '%s' is " RED "Rejected!\n" RESET, testCases[i].input);
            printf("It is NOT a valid palindrome.\n");
            printf(RED "**************************************************\n" RESET);
            testCases[i].is_accept = false;
        }
    }

    // Tabulate results
    printf("┌──────────────────┬────────────────┬──────────┐\n");
    printf("│ Test Name        │ Input String   │ Status   │\n");
    printf("├──────────────────┼────────────────┼──────────┤\n");

    for (size_t i = 0; i < numTests; i++) {
        printf("│ %-16d │ %-14s │ %s%-8s%s │\n",
            testCases[i].test_key,
            testCases[i].input[0] == '\0' ? "(empty)" : testCases[i].input,
            testCases[i].is_accept ? GREEN : RED,
            testCases[i].is_accept ? "ACCEPT" : "REJECT",
            RESET);
    }

    printf("└──────────────────┴────────────────┴──────────┘\n");

    printf(MAGENTA "\n---------------------------------------------------------------------\n" RESET);
    printf(YELLOW "--- Demonstration Complete ---\n\n" RESET);
}

/*
 * Run the manual mode
 */
void runManual() {

    char input_string[MAX_INPUT_SIZE];
    char again = 'y';

    while (again == 'y' || again == 'Y') {
        printf("============================================\n");
        printf("•Enter a binary palindrome string | Example: 1, 101, 0110, etc. :  \n ");
        
        scanf("%99s", input_string);

        if (!validateInput(input_string)) {
            printf(RED "ERROR: Invalid input! Use only 0s and 1s.\n" RESET);
            continue;
        }
        else {
            printf("\nPROCESSING BINARY STRING...\n");
            printf("-------------------------------------------\n\n");
            
            // Run the NPDA transition function
            int len = strlen(input_string);
            printf("Starting NPDA simulation for string: '%s'\n", input_string);
            printf("Simulating non-deterministic transitions...\n\n");
            
            int result = transition(input_string, len);

            //Final state check
            if (result) {
                printf("\n");
                printf("============================================\n");
                printf("Final result for string: %s → " GREEN "Accepted!\n" RESET, input_string);
                printf("It is a valid palindrome.\n");
                printf("============================================\n");
                printf("\n\n	(｡◕‿‿◕｡) \n\n");
            } else {
                printf("\n");
                printf("********************************************\n");
                printf("Final result for string: %s → " RED "Rejected!\n" RESET, input_string);
                printf("It is NOT a valid palindrome.\n");
                printf("********************************************\n");
                printf("\n\n\n");
                printf(" _________        .---'''''      '''''---.              \n");
                printf(":______.-':      :  .--------------.  :             \n");
                printf("| ______  |      | :                : |             \n");
                printf("|:______B:|      | |  Little Error: | |             \n");
                printf("|:______B:|      | |  ୧༼ಠ益ಠ༽︻╦╤─  | |             \n");
                printf("|:______B:|      | |                | |             \n");
                printf("|         |      | | NPDA does not  | |             \n");
                printf("|:_____:  |      | |   like your    | |             \n");
                printf("|    ==   |      : :    string      : :             \n");
                printf("|       O |      :  '--------------'  :             \n");
                printf("|       o |      :'---...______...---'              \n");
                printf("|       o |-._.-i___/'             \\._              \n");
                printf("'-.____o_|   '-.   '-...______...-'  `-._          \n");
                printf(":_________:      `.____________________   `-.___.-. \n");
                printf("                 .'.eeeeeeeeeeeeeeeeee.'.      :___:\n");
                printf("    fsc        .'.eeeeeeeeeeeeeeeeeeeeee.'.         \n");
                printf("              :____________________________:\n");
            }
        }

        printf("\nRun again? (y/n): ");
        scanf(" %c", &again);
        printf("\n");
    }
}

int main() {
    printWelcomeMessage();
    printTransitionTable();
   
    int menuChoice = 0;

    do {
        
        printf(YELLOW "--- Main Menu ---\n" RESET);
        printf("1. Enter a string manually\n");
        printf("2. Run automated test suite\n");
        printf("3. Exit\n");
        printf("> ");

        int scan_result = scanf(" %d", &menuChoice);
        if (scan_result != 1) { 
            while(getchar() != '\n');
            menuChoice = 0; 
        }

        switch (menuChoice) {
            case 1:
                runManual();
                break;
            case 2:
                runTestCases();
                break;
            case 3:
                break;
            default:
                printf(RED "\nInvalid choice. Please enter 1, 2, or 3.\n\n" RESET);
                break;
        }
    } while (menuChoice != 3);
    
    printf("\nBye ~~\n");
    printf(RESET);
    return 0;
}
