#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ANSI color codes
#define PINK_BG "\033[48;5;218m"
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define WHITE "\033[37m"

#define MAX_SIZE 1000
/*
 * Define the states of the PDA
 * 
 * @param S0 - Initial state for 0's and 1's
 * @param S1 - Pop phase, matching 1's with 0's
 * @param S2 - Accept state, no further input allowed
 * @param S3 - Reject state, stay here
 */
typedef enum {
    S0,  
    S1,  
    S2,  
    S3  
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
    free(stack->items);
    stack->items = NULL;
    stack->top = -1;
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

/*
 * Transition function for the PDA
 * 
 * @param currentState - The current state of the PDA
 * @param stack - The stack of the PDA
 * @param input - The input character (must be '0' or '1')
 * @return The next state of the PDA
 */
State transition(State currentState, Stack *stack, char input) {

            switch (currentState) {
                /// ---- State S0 - Initial state, push phase for 0's ----
                case S0:
                    if (input == '0' ) {
                        push(stack, '0');
                        return S0;
                    }                
                    else if (input == '1' && peek(stack) == '$') {
                        return S2; 
                    }
                    else if (input == '1' && peek(stack) == '0') {
                        pop(stack);
                        return S1;
                    }
                   
        
                /// ---- State S1 - Pop phase, matching 1's with 0's ----
                case S1:
                    if (input == '0') {
                        return S3; 
                    }
                    else if (input == '1' && peek(stack) == '0') {
                        pop(stack);
                        return S1;
                    }
                    else if (input == '1' && peek(stack) == '$') {
                        return S2; 
                    }
                    else {
                        return S3; // Reject
                    }
        
                case S2:
                if (input == '0' || input == '1') {
                    return S3; 
                }
        
                case S3:
                    if (input == '0' || input == '1') {
                        return S3; 
                    }
        
                default:
                    return S3; 
            }
        }

/*
 * Print the welcome message and PDA details
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
            printf("         ||    |______      `````\\____/ \\\n");
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
            printf("============================================\n");
            printf("Welcome to the machine. You have entered a Pushdown Automaton (PDA) \n");
            printf("============================================\n");
            printf(RESET);
        }

/*
 * Print the transition table for the PDA
 */
  void printTransitionTable() {
           printf("\n" YELLOW "PDA Transition Table:" RESET "\n");
            printf("┌───────┬─────────┬───────────┬────────────────┬────────────┐\n");
            printf("│ State │  Input  │ Stack Top │     Action     │ Next State │\n");
            printf("├───────┼─────────┼───────────┼────────────────┼────────────┤\n");
            printf("│  S0   │    0    │     -     │   Push 0       │     S0     │\n");
            printf("│  S0   │    1    │     $     │   No action    │     S2     │\n");
            printf("│  S0   │    1    │     0     │   Pop 0        │     S1     │\n");
            printf("│  S1   │    0    │     -     │   Error        │     S3     │\n");
            printf("│  S1   │    1    │     0     │   Pop 0        │     S1     │\n");
            printf("│  S1   │    1    │     $     │   No action    │     S2     │\n");
            printf("│ *S2   │   0/1   │     -     │   Error        │     S3     │\n");
            printf("│  S3   │   0/1   │     -     │   Stay         │     S3     │\n");
            printf("└───────┴─────────┴───────────┴────────────────┴────────────┘\n");
            printf("* denotes accept state\n\n");
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
        {2, "011", false},         
        {3, "00111", false},       
        {4, "0001111", false},     
        {5, "000011111", false},   
        // Rejecting
        {6, "", false},            
        {7, "0", false},           
        {8, "00", false},          
        {9, "01", false},          
        {10, "11", false},         
        {11, "10", false},         
        {12, "000111", false},     
        {13, "00011111", false},   
        {14, "0101", false},       
    };

    size_t numTests = sizeof(testCases) / sizeof(testCases[0]);

    printf(YELLOW "\n--- Running PDA Test Cases ---\n" RESET);
    
    for (size_t i = 0; i < numTests; i++) {
        State currentState = S0;
        Stack stack;
        initStack(&stack);
        push(&stack, '$'); 
        
        const char* input_string_ = testCases[i].input;
        
        printf(MAGENTA "\n---------------------------------------------------------------------\n" RESET);
        printf("Demonstrating with string: \"%s\"\n", input_string_[0] == '\0' ? "(empty)" : testCases[i].input);
        printf(WHITE "Processing string..." RESET "\n");
        printf("Start State: S0 | Stack: $ \n");

        // Process each character and show transitions
        for (size_t j = 0; j < strlen(input_string_); j++) {
            char currentChar = input_string_[j];
            
            currentState = transition(currentState, &stack, currentChar);
            
            if (currentState == S0){
                printf("-Read: %c | transitioned to → 'S0'| Stack Top: %c \n", currentChar, peek(&stack));
            }
            else if (currentState == S1){
                printf("-Read: %c | transitioned to → 'S1'| Stack Top: %c \n", currentChar, peek(&stack));
            }
            else if (currentState == S2){
                printf("-Read: %c | transitioned to → 'S2'| Stack Top: %c \n", currentChar, peek(&stack));
            }
            else if (currentState == S3){
                printf("-Read: %c | transitioned to → 'S3'| Stack Top: %c \n", currentChar, peek(&stack));
            }
        }

        if (currentState == S2 && peek(&stack) == '$') {
            printf(GREEN "\n==================================================\n" RESET);
            printf("Final State is S2, Stack is empty. String '%s' is " GREEN "Accepted!\n" RESET, testCases[i].input);
            printf("It's in the language {0^n 1^(n+1)}\n");
            printf(GREEN "==================================================\n" RESET);
            testCases[i].is_accept = true;
        } else {
            printf(RED "\n**************************************************\n" RESET);
            printf("Final State is S%d, Stack Top: %c. String '%s' is " RED "Rejected!\n" RESET, currentState, peek(&stack), testCases[i].input);
            printf("It's NOT in the language {0^n 1^(n+1)}\n");
            printf(RED "**************************************************\n" RESET);
            testCases[i].is_accept = false;
        }
        
        freeStack(&stack);
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


void runManual(){ 
    char again = 'y';
    char input_string[MAX_SIZE];

    while (again == 'y' || again == 'Y') {
    printf("============================================\n");
    printf("•Enter a binary string of the language {0^n 1^n+1}  |  Example: 1, 011, 00111, etc. :  \n ");
    
    scanf("%99s", input_string);

    printf("\nPROCESSING BINARY STRING...\n");
    printf("-------------------------------------------\n\n");
    
    State currentState = S0;
    Stack stack;
           
    initStack(&stack);
    push(&stack, '$');
 if (!validateInput(input_string)) {
            printf(RED "ERROR: Invalid input! Use only 0s and 1s.\n" RESET);
            continue;}
        else {
    // Current state check
    int len = strlen(input_string);
    for (int i = 0; i < len; ++i) {
        char currentChar = input_string[i];
        
        currentState = transition(currentState, &stack, currentChar);

        if (currentState == S0){
            printf("-Read: %c | transitioned to → 'S0'| Stack Top: %c \n", currentChar, peek(&stack));
        }
        else if (currentState == S1){
            printf("-Read: %c | transitioned to → 'S1'| Stack Top: %c \n", currentChar, peek(&stack));
        }
        else if (currentState == S2){
            printf("-Read: %c | transitioned to → 'S2'| Stack Top: %c \n", currentChar, peek(&stack));
        }
        else if (currentState == S3){
            printf("-Read: %c | transitioned to → 'S3'| Stack Top: %c \n", currentChar, peek(&stack));
        }
    }
}

    //Final state check
    if (currentState == S2 && peek(&stack) == '$') {
        printf("\n");
        printf("============================================\n");
        printf("Final state for string:  %s → Accepted!\n It's in the language {0^n 1^n+1}\n", input_string);
        printf("============================================\n");
        printf("\n\n	(｡◕‿‿◕｡) \n\n");
    } else {
        printf("\n");
        printf("============================================\n");
        printf("Final state for string: %s → Rejected! Our hard working PDA couldn't compute\n", input_string);
        printf("It's NOT in the language {0^n 1^n+1}\n");
        printf("============================================\n");

        printf("\n\n\n");
        printf(" _________        .---'''''      '''''---.              \n");
        printf(":______.-':      :  .--------------.  :             \n");
        printf("| ______  |      | :                : |             \n");
        printf("|:______B:|      | |  Little Error: | |             \n");
        printf("|:______B:|      | |  ୧༼ಠ益ಠ༽︻╦╤─  | |             \n");
        printf("|:______B:|      | |                | |             \n");
        printf("|         |      | |  PDA doess not | |             \n");
        printf("|:_____:  |      | |   not like     | |             \n");
        printf("|    ==   |      : :    your string : :             \n");
        printf("|       O |      :  '--------------'  :             \n");
        printf("|       o |      :'---...______...---'              \n");
        printf("|       o |-._.-i___/'             \\._              \n");
        printf("'-.____o_|   '-.   '-...______...-'  `-._          \n");
        printf(":_________:      `.____________________   `-.___.-. \n");
        printf("                 .'.eeeeeeeeeeeeeeeeee.'.      :___:\n");
        printf("    fsc        .'.eeeeeeeeeeeeeeeeeeeeee.'.         \n");
        printf("              :____________________________:\n");

    }

    printf("\nRun again? (y/n): ");
    scanf(" %c", &again);
    printf("\n");
    freeStack(&stack);
    }
}


int main() {
    printWelcomeMessage();
   
    int menuChoice = 0;

    do {
        // Main menu
        printf(YELLOW "--- Main Menu ---\n" RESET);
        printf("1. Enter a string manually\n");
        printf("2. Run automated test \n");
        printf("3. Exit\n");
        printf("> ");

        scanf(" %d", &menuChoice);

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
    
    
    printf("Bye ~~\n");
    // Reset color
    printf(RESET);
    return 0;
}
