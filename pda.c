#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI color codes
#define PINK_BG "\033[48;5;218m"
#define RESET "\033[0m"

#define MAX_SIZE 1000

typedef enum {
    S0,  // pushing a's
    S1,  // popping a's with b's
    S2   // accept after extra b
} State;

typedef struct {
    char *items;
    int top;
} Stack;
void initStack(Stack *stack) {
    stack->items = malloc(MAX_SIZE * sizeof(char));
    if (!stack->items) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    stack->top = -1;
}

void push(Stack *stack, char symbol) {
    stack->items[++(stack->top)] = symbol;
}

char pop(Stack *stack) {
    if (stack->top < 0) {
        fprintf(stderr, "Stack underflow.\n");
        exit(1);
    }
    return stack->items[(stack->top)--];
}

char peek(Stack *stack) {
    if (stack->top < 0)
        return '$';  // treat empty as bottom marker
    return stack->items[stack->top];
}

int isEmpty(Stack *stack) {
    return stack->top == -1;
}

void freeStack(Stack *stack) {
    free(stack->items);
    stack->items = NULL;
    stack->top = -1;
}

// State transation(State currentState, Stack *stack, char input) {
//     switch (currentState) {
/// ---- State S0 - Initial state for 0's and 1's ----  
        // case S0:
        // // input: 0,$ -> 0 , stay in S0
        //     if (input == '0' && peek(stack) == '$')
        //     {push(stack, '0');
        //         return S0;
        //     }
        // // input: 0,0 -> 0 , stay in S0  
        //     else if (input == '0' && peek(stack) == '0')
        //     {push(stack, '0');
        //         return S0;
        //     }
        // // input: 0,1 -> 0 , go to S1    
        //     else if (input == '1' && peek(stack) == '0')
        //     {pop(stack);
        //         return S1;}

        // break;}
        State transation(State currentState, Stack *stack, char input) {
            switch (currentState) {
                /// ---- State S0 - Initial state for 0's and 1's ----
                // input: 0,$ -> push 0, stay in S0
                // input: 0,0 -> push 0, stay in S0
                // input: 1,0 -> pop, go to S1
                // input: 1,$ -> go to S2
                case S0:
                    if (input == '0' && peek(stack) == '$') {
                        push(stack, '0');
                        return S0;
                    } else if (input == '0' && peek(stack) == '0') {
                        push(stack, '0');
                        return S0;
                    } else if (input == '1' && peek(stack) == '0') {
                        pop(stack);
                        return S1;
                    } else if (input == '1' && peek(stack) == '$') {
                        return S2;
                    }
                    break;
        
                /// ---- State S1 - Popping a's with b's ----
                // input: 1,0 -> pop, stay in S1
                // input: 1,$ -> go to S2
                // input: 0,x -> invalid, return S0
                case S1:
                    if (input == '1' && peek(stack) == '0') {
                        pop(stack);
                        return S1;
                    } else if (input == '1' && peek(stack) == '$') {
                        return S2;
                    } else if (input == '0') {
                        return S0;
                    }
                    break;
        
                /// ---- State S2 - Accept state, no further transitions ----
                case S2:
                    break;
        
                default:
                    break;
            }
            return -1;  // invalid transition
        }



int main() {
    char input_string[MAX_SIZE];
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
        printf("Welcome to the machine. You have entered a Pushdown Automaton (PDA) \n");
        printf("============================================\n");
        printf("•Enter a binary string of the language {0^n 1^n+1}  |  Example: 1, 011, 00111, etc. :  \n \n \n ");
        
        scanf("%99s", input_string);

        printf("\nPROCESSING BINARY STRING...\n");
        printf("-------------------------------------------\n\n");
        
        State currentState = S0;
        Stack stack;
               
        initStack(&stack);
        push(&stack, '$');

        // Current state check
        for (int i = 0; i < strlen(input_string); ++i) {
            char currentChar = input_string[i];

            if (currentState == S0){
                printf("-Read: %c | current state : → 'S0'| Stack Top: %c \n", currentChar, peek(&stack));
            }
            else if (currentState == S1){
                printf("-Read: %c | current state:  → 'S1'| Stack Top: %c \n", currentChar, peek(&stack));
            }
            else if (currentState == S2){
                printf("-Read: %c | current state:  → 'S2'| Stack Top: %c \n", currentChar, peek(&stack));
            }
            
            currentState = transation(currentState, &stack, currentChar);

            if (currentState == -1) {
                freeStack(&stack);
                break;
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
    
    printf("Bye ~~\n");
    // Reset color
    printf(RESET);
    return 0;
}
