#include <stdio.h>
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
#define CYAN "\033[36m"
#define WHITE "\033[37m"

typedef enum {
    S0, // initial state
    S1,
    S2, // final state
} State;

/*
 * Transition function for the DFA
 * 
 * @param currentState - The current state of the DFA
 * @param input - The input character
 * @return The next state of the DFA
 */
State transation(State currentState, char input) {

    // Error checking for invalid input
    if (input != '0' && input != '1' ) {
        return -1; // Invalid input
    }

    switch (currentState) {

// State S0 - Initial state for 0's and 1's       
        case S0:
            if (input == '0')
                return S0;
            else if (input == '1')
                return S1;
            break;

// State S1 - Trap state for 0's transitionary for next state on input 1
        case S1:
            if (input == '0')
                return S1; // Stay in S1
            else if (input == '1')
                return S2; // Transition to S2
            break;

// State S2 - Accept state for one [1]
        case S2:
            if (input == '0')
                return S2; // Stay in S2
            else if (input == '1')
                return S2; // Transition to S3
            break;
    }
    
    return currentState;
}



/* Validate input string contains only 0s and 1s */
int validateInput(const char* str) {
    for (int i = 0; str[i] !=  '\0'; i++) {
        if ( str[i] != '0' && str[i] != '1') {
            return 0;
        }
    }
    return 1;
}



/*
 * Reset the state of the DFA
 * 
 * @param currentState - The current state of the DFA
 * @return The initial state of the DFA
 */
State reset_state(State currentState) {
    (void)currentState; // Mark as intentionally unused
    return S0;
}

/*
 * Print the welcome message and DFA details
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
    printf("Welcome to the machine. You have entered a Deterministic Finite Automaton (DFA) \n");
    printf("============================================\n");
    printf(RESET);
        printf("\n" YELLOW "DFA Transition Table:" RESET "\n");
    printf("┌─────────┬────────────┬────────────┐\n");
    printf("│  State  │  Input: 0  │  Input: 1  │\n");
    printf("├─────────┼────────────┼────────────┤\n");
    printf("│   S0    │     S0     │     S1     │\n");
    printf("│  *S1    │     S1     │     S2     │\n");
    printf("│   S2    │     S2     │     S2     │\n");
    printf("└─────────┴────────────┴────────────┘\n");
    printf("* denotes accept state\n\n");
}

/*
 * Run test cases mode and display results
 */
void runTestCases() {    


    typedef struct {
        const int test_key;
        const char* input;
        //int result; // Will be set to 1 for PASS (Accept), 0 for FAIL (Reject)
        bool is_accept;
    } TestCase;

    TestCase testCases[] = {
        // Accepts
        {1, "1", false},
        {2 , "01", false },
        {3, "10", false},
        {4, "01000", false},
        {5, "001000", false},    
        // Rejects
        {6, "", false},
        {7, "0", false},
        {8, "0000000000", false},
        {9, "11", false},
        {10, "0110", false},
        {11, "110", false},  
    };

    size_t numTests = sizeof(testCases) / sizeof(testCases[0]);
    int passed_count = 0;


    
        printf(YELLOW "\n--- Running DFA Test Cases ---\n" RESET);
        
        for (size_t i = 0; i < numTests; i++) {

            State currentState = S0;
            const char* input_string_ = testCases[i].input;
            
            // This block is now a direct copy of the logic from the main() loop
            // to show the full processing for each test case.
            printf(CYAN "\n---------------------------------------------------------------------\n" RESET);
            printf("Demonstrating with string: \"%s\"\n", input_string_[0] == '\0' ? "(empty)" : testCases[i].input);
            printf(WHITE "Processing string..." RESET "\n");
            printf("Start State: S0\n");
    
            for (size_t i = 0; i < strlen(input_string_); i++) {
                char currentChar = input_string_[i];
                currentState = transation(currentState, currentChar);            }
    
            if (currentState == S1) {
                printf(GREEN "\n==================================================\n" RESET);
                printf("Final State is S1. String '%s' is " GREEN "Accepted!\n" RESET, testCases[i].input);
                printf(GREEN "==================================================\n" RESET);
                testCases[i].is_accept = true;

                        } else {
                printf(RED "\n**************************************************\n" RESET);
                printf("Final State is S%d. String '%s' is " RED "Rejected!\n" RESET, currentState, testCases[i].input);
                printf(RED "**************************************************\n" RESET);
                testCases[i].is_accept = false;
                currentState = reset_state(currentState);

            }
        }

 //tabulate results
 printf("┌──────────────────┬────────────────┬──────────┐\n");
 printf("│ Test Name        │ Input String   │ Status   │\n");
 printf("├──────────────────┼────────────────┼──────────┤\n");

 for (size_t i = 0; i < numTests; i++) {
     if (testCases[i].is_accept == true) {
         passed_count++;
     }
           printf("│ %-16d │ %-14s │ %s%-8s%s │\n",
         testCases[i].test_key,
        testCases[i].input[0] == '\0' ? "(empty)" : testCases[i].input,
        testCases[i].is_accept ? GREEN : RED,
        testCases[i].is_accept ? "ACCEPT" : "REJECT",
            RESET);
 }

 printf("└──────────────────┴────────────────┴──────────┘\n");
        
        printf(CYAN "\n---------------------------------------------------------------------\n" RESET);
        printf(YELLOW "--- Demonstration Complete ---\n\n" RESET);
    }

    /*
     * Run the manual mode
     * 
     * @param currentState - The current state of the DFA
     * @param input_string - The input string
     * @param again - The again character
     */
void runManual(){
     State currentState = S0;
     char input_string[100];
     char again = 'y';

    while (again == 'y' || again == 'Y') {
        printf("•Enter a binary string that has only one occurence of '1' |  Example: 1, 01, 00010, etc: \n  ");

        scanf("%99s", input_string);
        
        if (!validateInput(input_string)) {
            printf(RED "ERROR: Invalid input! Use only 0s and 1s.\n" RESET);
            continue;
        }
        else {
        printf("\nPROCESSING BINARY STRING...\n");
        printf("-------------------------------------------\n\n");
        // cuurrent state check
        for (size_t i = 0; i < strlen(input_string); i++) {
            char currentChar = input_string[i];
            currentState = transation(currentState, currentChar);

            if (currentState == S0){
                printf("-Read: %c | " WHITE " cuurent state : → 'S%d' \n" RESET, currentChar, currentState);            }
            else if (currentState == S1){
                printf("-Read: %c | " WHITE "current state : → 'S%d' \n" RESET, currentChar, currentState);            }

           else if (currentState == S2){
                printf("-Read: %c | " WHITE "current state : → 'S%d' \n" RESET, currentChar, currentState);            
        }
        }

        //Final state check
        if (currentState == S1) {
            printf("\n");
            printf("============================================\n");
            printf("Final state for string: %s → " GREEN "Accepted! One occurence of [1] only. \n" RESET, input_string);
            printf("============================================\n");
            printf("\n\n	(｡◕‿‿◕｡) \n\n");
            //printf("%d", currentState); //debug
        } else {
            printf("\n");
            printf("********************************************\n");
            printf("Final for string: %s → " RED "Rejected! More than one [1] or no [1's] yet → Rejected!\n" RESET  , input_string);
            printf("********************************************\n");
            printf("\n\n\n    \n\n");
            printf(" _________        .---'''''      '''''---.              \n");
  printf(":______.-':      :  .--------------.  :             \n");
  printf("| ______  |      | :                : |             \n");
  printf("|:______B:|      | |  Little Error: | |             \n");
  printf("|:______B:|      | |  ୧༼ಠ益ಠ༽︻╦╤─  | |             \n");
  printf("|:______B:|      | |                | |             \n");
  printf("|         |      | |  DFA doess not | |             \n");
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
//printf("%d", currentState); //debug  
        }
    }
        currentState = reset_state(currentState);
        printf("\nRun again? (y/n): ");
        scanf(" %c", &again);
        printf("\n");
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

    