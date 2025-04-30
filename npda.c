#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ANSI color codes
#define PINK_BG "\033[48;5;218m"
#define RESET "\033[0m"

#define MAX_SIZE 1000

typedef enum {
    S0,  // initial state
    S1,  // accept state
    S2,  // Non-deterministic transition state
    S3   // reject state
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

State transition(State currentState, Stack *stack, char input, int *pos, int len) {
    switch (currentState) {

        // State S0 - push 1st half of input into stack 
        // for positions before the midpoint -> push char to the stack
        // if string length is odd and we reach the middle pos skip -> go to S2)
        // if even midpoint or the second half peek anc compare
        case S0:
        //
            if (*pos < len / 2) {
                // if tsill in first half -> push char to stack
                push(stack, input);
                return S0;
            }
            // non deterministic transition, aka ε-transition
            else if 
                ((len % 2 != 0) && (*pos == len / 2))
                // if len == odd skip middle char -> go to S2
                {
                return S2;// skip mid char, ε-transition 
            }
            else {
                // if even midpoint or second half -> pop and compare by going to S1
                // else go to S3 (reject)
                if (!isEmpty(stack) && pop(stack) == input) {
                    return S1;
                } else {
                    return S3;
                }
            }
       // State S2
       // first comparison after skipping the middle char when len is odd  
        case S2:
         
            // compare input with popped symbol
            if (!isEmpty(stack) && pop(stack) == input) {
                return S1;
            } else {
                return S3;  // reject state
            }
            
        // State S1
        /// matching 2nd half of input with stack 
        //  pop and compare symbols from the stack
        // all match -> go to S1 accept state, otherwise go to S3 reject state
        case S1:
            // Pop from stack and compare with input
            if (!isEmpty(stack) && pop(stack) == input) {
                return S1;
            } else {
                return S3; // reject state
            }

        default:
            return S3; // reject state
    }
}


int main() {
    char input_string[MAX_SIZE];
    char again = 'y';
    State currentState;
    Stack stack;
    int pos, len;
    
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
printf("Welcome to the machine. You have entered a Non-Deterministic Pushdown Automaton (NPDA) \n");
printf("============================================\n");
        printf("Enter a string of palindrome of any two charchters | Example: 1, 010, 110011110011, abba,  etc. :  \n \n \n ");
        
        scanf("%999s", input_string);
        printf("\nPROCESSING STRING...\n");
        printf("-------------------------------------------\n\n");
        
        // Reset per-input
        initStack(&stack);
        currentState = S0;
        len = strlen(input_string);

        // Process characters
        for (pos = 0; pos < len; pos++) {
            char currentChar = input_string[pos];

            // Debug print via switch
            switch (currentState) {
                case S0:
                    printf("-Read: %c | current state: → 'S0' | Stack Top: %c\n",
                           currentChar,
                           peek(&stack) ? peek(&stack) : '$');
                    break;
                case S2:
                    printf("-Read: %c | current state: → 'S2' | Stack Top: %c\n",
                           currentChar,
                           peek(&stack) ? peek(&stack) : '$');
                    break;
                case S1:
                    printf("-Read: %c | current state: → 'S1' | Stack Top: %c\n",
                           currentChar,
                           peek(&stack) ? peek(&stack) : '$');
                    break;
                default:
                    break;
            }

            currentState = transition(currentState, &stack, currentChar, &pos, len);
            if (currentState == S3) {
                break;  // Early exit on mismatch
            }
        }



        // Final accept if compare-state or just-skipped-middle, with empty stack
        if ((currentState == S1 || currentState == S2) && isEmpty(&stack)) {
            printf("============================================\n");
            printf("Final state for string:  %s → Accepted!\n It's a palindrome\n", input_string);
            printf("============================================\n");
            printf("\n\n	(｡◕‿‿◕｡) \n\n");
        } else {
            printf("\n");
            printf("============================================\n");
            printf("Final state for string: %s → Rejected! Our hard working NPDA couldn't compute\n", input_string);
            printf("It's NOT a palindrome\n");
            printf("============================================\n");

            printf("\n\n\n");
            printf(" _________        .---'''''      '''''---.              \n");
            printf(":______.-':      :  .--------------.  :             \n");
            printf("| ______  |      | :                : |             \n");
            printf("|:______B:|      | |  Little Error: | |             \n");
            printf("|:______B:|      | |  ୧༼ಠ益ಠ༽︻╦╤─  | |             \n");
            printf("|:______B:|      | |                | |             \n");
            printf("|         |      | | NPDA doess not | |             \n");
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