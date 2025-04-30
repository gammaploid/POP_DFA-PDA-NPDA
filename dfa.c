#include <stdio.h>
#include <string.h>

// ANSI color codes
#define PINK_BG "\033[48;5;218m"
#define RESET "\033[0m"

typedef enum {
    S0, // initial state
    S1,
    S2, // final state
    S3 // escape reject state
} State;

State transation(State currentState, char input) {
    switch (currentState) {

// State S0 - Initial state for 0's and 1's       
        case S0:
            if (input == '0')
                return S1;
            else if (input == '1')
                return S2;
            else
                return S0;

// State S1 - Trap state for 0's transitionary for next state on input 1
        case S1:
            if (input == '0')
                return S1; // Stay in S1
            else if (input == '1')
                return S2; // Transition to S2
            else
                return S1; // Stay in S1

// State S2 - Accept state for one [1]
        case S2:
            if (input == '0')
                return S2; // Stay in S2
            else 
                return S3; // Transition to S3 (escape reject state)
    
// State S3 - Escape reject state for additional 1's input
        case S3:
            if (input == '1')
                return S3;  // Stay in S3
            else 
                 return S3;
                     
                    }
                    
    return currentState;
}


int main() {
    char input_string[100];
    char again = 'y';
    
    // Set pink background
    printf(PINK_BG);
    
    while (again == 'y' || again == 'Y') {
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
        printf("•Enter a binary string that has only one occurence of '1' |  Example: 1, 01, 00010, etc: \n \n \n ");
        
        scanf("%99s", input_string);
        
        State currentState = S0;
        
        
        printf("\nPROCESSING BINARY STRING...\n");
        printf("-------------------------------------------\n\n");

        // cuurrent state check
        for (int i = 0; i < strlen(input_string); i++) {
            char currentChar = input_string[i];

            if (currentState == S0){
                printf("-Read: %c | current state : → 'S0' \n", currentChar);
            }
            else if (currentState == S1){
                printf("-Read: %c | current state:  → 'S1' \n", currentChar);
            }
            currentState = transation(currentState, currentChar);

            if (currentState == S2){
                printf( "-Read: %c | current state: → 'S2' \n", currentChar);
            }
            if (currentState == S3){
                printf( "-Read: %c | current state: → 'S3' \n", currentChar);
            }
        }

        //Final state check
        if (currentState == S2) {
            printf("\n");
            printf("============================================\n");
            printf("Final state for string: %s → Accepted! One occurence of [1] only → Accepted!\n", input_string);
            printf("============================================\n");
            printf("\n\n	(｡◕‿‿◕｡) \n\n");
        } 
        
        else {
            printf("\n");
            printf("********************************************\n");
            printf("Final for string: %s → Rejected! More than one [1] or no [1's] yet → Rejected!\n", input_string);
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

        }

        printf("\nRun again? (y/n): ");
        scanf(" %c", &again);
        printf("\n");
    }
    
    printf("Bye ~~\n");
    // Reset color
    printf(RESET);
    return 0;
}
