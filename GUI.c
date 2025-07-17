#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION

/* Platform-specific includes */
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#ifdef __linux__
    #define GL_GLEXT_PROTOTYPES
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <ctype.h>
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#define PINK_BG "\033[48;5;218m"
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define MAX_SIZE 1000
#define MAX_STACK_SIZE 100
#define MAX_CONFIGURATIONS 100 
#define MAX_INPUT_SIZE 100

// ========================= DFA IMPLEMENTATION =========================
typedef enum {
    S0, // initial state
    S1,
    S2, // final state
} State;

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
                return S2; // Stay in S2
            break;
    }
                    
    return currentState;
}

int validateInput(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != '0' && str[i] != '1') {
            return 0;
        }
    }
    return 1;
}

State reset_state(State currentState) {
    (void)currentState; // Mark as intentionally unused
    return S0;
}

/* Process DFA string and return result */
int processDFA(const char* input, char* result_text, size_t result_size) {
    if (!validateInput(input)) {
        snprintf(result_text, result_size, "ERROR: Invalid input! Use only 0s and 1s.");
        return 0;
    }
    
    if (strlen(input) == 0) {
        snprintf(result_text, result_size, "REJECTED: Empty string (no 1s)");
        return 0;
    }
    
    State currentState = S0;
    
    /* Process each character */
    for (int i = 0; i < (int)strlen(input); i++) {
        currentState = transation(currentState, input[i]);
    }
    
    /* Check final state */
    if (currentState == S1) {
        snprintf(result_text, result_size, "ACCEPTED: Contains exactly one '1'");
        return 1;
    } else if (currentState == S0) {
        snprintf(result_text, result_size, "REJECTED: Contains no '1's");
        return 0;
    } else {
        snprintf(result_text, result_size, "REJECTED: Contains more than one '1'");
        return 0;
    }
}

// ========================= PDA IMPLEMENTATION =========================
typedef enum {
    PDA_S0,  // pushing 0's
    PDA_S1,  // popping 0's with 1's
    PDA_S2,  // accept after extra 1
    PDA_S3   // reject  
} PDA_State;

typedef struct {
    char *items;
    int top;
} PDA_Stack;

void PDA_initStack(PDA_Stack *stack) {
    stack->items = malloc(MAX_SIZE * sizeof(char));
    if (!stack->items) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    stack->top = -1;
}

void PDA_push(PDA_Stack *stack, char symbol) {
    stack->items[++(stack->top)] = symbol;
}

char PDA_pop(PDA_Stack *stack) {
    if (stack->top < 0) {
        return '$'; // Return bottom marker instead of crashing
    }
    return stack->items[(stack->top)--];
}

char PDA_peek(PDA_Stack *stack) {
    if (stack->top < 0)
        return '$';  // treat empty as bottom marker
    return stack->items[stack->top];
}

int PDA_isEmpty(PDA_Stack *stack) {
    return stack->top == -1;
}

void PDA_freeStack(PDA_Stack *stack) {
    if (stack->items) {
        free(stack->items);
        stack->items = NULL;
    }
    stack->top = -1;
}

PDA_State PDA_transition(PDA_State currentState, PDA_Stack *stack, char input) {
    // Input validation first
    if (input != '0' && input != '1') {
        return PDA_S3; // Invalid input - reject
    }
    
    switch (currentState) {
        case PDA_S0:
            // input: 0, stack_top: $ (empty) -> push 0, stay in S0
            if (input == '0' && PDA_peek(stack) == '$') {
                PDA_push(stack, '0');
                return PDA_S0;
            }
            // input: 0, stack_top: 0 -> push 0, stay in S0
            else if (input == '0' && PDA_peek(stack) == '0') {
                PDA_push(stack, '0');
                return PDA_S0;
            }
            // input: 1, stack_top: $ (empty) -> n=0, need exactly 1 one -> accept
            else if (input == '1' && PDA_peek(stack) == '$') {
                return PDA_S2; // Accept state (0⁰1¹)
            }
            // input: 1, stack_top: 0 -> start pop phase, pop 0, go to S1
            else if (input == '1' && PDA_peek(stack) == '0') {
                PDA_pop(stack);
                return PDA_S1;
            }
            else {
                return PDA_S3; // Reject
            }

        case PDA_S1:
            // input: 0, stack_top: any -> ERROR! Can't have 0 after starting 1's
            if (input == '0') {
                return PDA_S3; // Reject - violates 0ⁿ1ⁿ⁺¹ pattern
            }
            // input: 1, stack_top: 0 -> pop 0, continue in S1
            else if (input == '1' && PDA_peek(stack) == '0') {
                PDA_pop(stack);
                return PDA_S1;
            }
            // input: 1, stack_top: $ (empty) -> this is the extra 1, accept!
            else if (input == '1' && PDA_peek(stack) == '$') {
                return PDA_S2; // Accept state
            }
            else {
                return PDA_S3; // Reject
            }

        case PDA_S2:
            // Any input after reaching accept state should reject
            return PDA_S3; // Reject - string should end at accept state

        case PDA_S3:
            return PDA_S3; // Stay in reject state

        default:
            return PDA_S3; // Unknown state -> reject
    }
}

/* Process PDA string and return result */
int processPDA(const char* input, char* result_text, size_t result_size) {
    if (!validateInput(input)) {
        snprintf(result_text, result_size, "ERROR: Invalid input! Use only 0s and 1s.");
        return 0;
    }
    
    PDA_State currentState = PDA_S0;
    PDA_Stack stack;
    PDA_initStack(&stack);
    PDA_push(&stack, '$'); // Initialize with bottom marker
    
    /* Process each character */
    for (int i = 0; i < (int)strlen(input); i++) {
        currentState = PDA_transition(currentState, &stack, input[i]);
    }
    
    /* Check final state and stack condition */
    int result = 0;
    if (currentState == PDA_S2 && PDA_peek(&stack) == '$') {
        snprintf(result_text, result_size, "ACCEPTED: String is in language {0^n 1^(n+1)}");
        result = 1;
    } else {
        snprintf(result_text, result_size, "REJECTED: String is NOT in language {0^n 1^(n+1)}");
        result = 0;
    }
    
    PDA_freeStack(&stack);
    return result;
}

// ========================= NPDA IMPLEMENTATION =========================
typedef enum {
    NPDA_S0, // pushing symbols
    NPDA_S1, // popping symbols
    NPDA_S2, // accept state
} NPDA_State;

typedef struct {
    char *items;
    int top;
} NPDA_Stack;

typedef struct {
    NPDA_State state;
    NPDA_Stack stack;
    int input_pos;
} NPDA_Configuration;

void NPDA_initStack(NPDA_Stack *stack) {
    stack->items = malloc(MAX_SIZE * sizeof(char));
    if (!stack->items) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    stack->top = -1;
}

void NPDA_push(NPDA_Stack *stack, char symbol) {
    stack->items[++(stack->top)] = symbol;
}

char NPDA_pop(NPDA_Stack *stack) {
    if (stack->top < 0) {
        return '$';
    }
    return stack->items[(stack->top)--];
}

char NPDA_peek(NPDA_Stack *stack) {
    if (stack->top < 0)
        return '$';
    return stack->items[stack->top];
}

int NPDA_isEmpty(NPDA_Stack *stack) {
    return stack->top == -1;
}

void NPDA_freeStack(NPDA_Stack *stack) {
    if (stack->items) {
        free(stack->items);
        stack->items = NULL;
    }
}

void NPDA_copyStack(const NPDA_Stack* src, NPDA_Stack* dest) {
    NPDA_initStack(dest);
    for (int i = 0; i <= src->top; i++) {
        dest->items[i] = src->items[i];
    }
    dest->top = src->top;
}

int NPDA_transition(const char* input_string, int len) {
    NPDA_Configuration current_configs[MAX_CONFIGURATIONS];
    NPDA_Configuration next_configs[MAX_CONFIGURATIONS];

    int num_current_configs = 1;
    int num_next_configs = 0;

    NPDA_initStack(&current_configs[0].stack);
    current_configs[0].state = NPDA_S0;
    current_configs[0].input_pos = 0;
    
    // Add epsilon transition to S1 from start
    if (num_current_configs < MAX_CONFIGURATIONS) {
        current_configs[num_current_configs].state = NPDA_S1;
        NPDA_copyStack(&current_configs[0].stack, &current_configs[num_current_configs].stack);
        current_configs[num_current_configs].input_pos = 0;
        num_current_configs++;
    }

    while (num_current_configs > 0) {
        num_next_configs = 0;
        int accepted = 0;

        for (int i = 0; i < num_current_configs; i++) {
            NPDA_Configuration conf = current_configs[i];
            
            if (conf.input_pos == len) {
                if (conf.state == NPDA_S1 && NPDA_isEmpty(&conf.stack)) {
                    accepted = 1;
                } 
                continue;
            }

            char c = input_string[conf.input_pos];

            if (conf.state == NPDA_S0) {
                // Choice 1: Push
                if (num_next_configs < MAX_CONFIGURATIONS) {
                    next_configs[num_next_configs].state = NPDA_S0;
                    NPDA_copyStack(&conf.stack, &next_configs[num_next_configs].stack);
                    NPDA_push(&next_configs[num_next_configs].stack, c);
                    next_configs[num_next_configs].input_pos = conf.input_pos + 1;
                    num_next_configs++;
                }

                // Choice 2: Skip (Odd Middle)
                if (num_next_configs < MAX_CONFIGURATIONS) {
                    next_configs[num_next_configs].state = NPDA_S1;
                    NPDA_copyStack(&conf.stack, &next_configs[num_next_configs].stack);
                    next_configs[num_next_configs].input_pos = conf.input_pos + 1;
                    num_next_configs++;
                }
                
                // Choice 3: Epsilon-Jump (Even Middle)
                if (num_next_configs < MAX_CONFIGURATIONS) {
                    next_configs[num_next_configs].state = NPDA_S1;
                    NPDA_copyStack(&conf.stack, &next_configs[num_next_configs].stack);
                    next_configs[num_next_configs].input_pos = conf.input_pos;
                    num_next_configs++;
                }

            } else if (conf.state == NPDA_S1) {
                if (!NPDA_isEmpty(&conf.stack) && NPDA_peek(&conf.stack) == c) {
                    if (num_next_configs < MAX_CONFIGURATIONS) {
                        next_configs[num_next_configs].state = NPDA_S1;
                        NPDA_copyStack(&conf.stack, &next_configs[num_next_configs].stack);
                        NPDA_pop(&next_configs[num_next_configs].stack);
                        next_configs[num_next_configs].input_pos = conf.input_pos + 1;
                        num_next_configs++;
                    }
                }
            }
        }

        for (int i = 0; i < num_current_configs; i++) {
            NPDA_freeStack(&current_configs[i].stack);
        }
        
        if (accepted) {
            for (int i = 0; i < num_next_configs; i++) {
                NPDA_freeStack(&next_configs[i].stack);
            }
            return 1;
        }

        num_current_configs = num_next_configs;
        for (int i = 0; i < num_next_configs; i++) {
            current_configs[i] = next_configs[i];
        }
    }

    return 0;
}

/* Process NPDA string and return result */
int processNPDA(const char* input, char* result_text, size_t result_size) {
    if (!validateInput(input)) {
        snprintf(result_text, result_size, "ERROR: Invalid input! Use only 0s and 1s.");
        return 0;
    }
    
    int len = strlen(input);
    int result = NPDA_transition(input, len);
    
    if (result) {
        snprintf(result_text, result_size, "ACCEPTED: String is a palindrome");
        return 1;
    } else {
        snprintf(result_text, result_size, "REJECTED: String is NOT a palindrome");
        return 0;
    }
}

// ========================= GUI IMPLEMENTATION =========================
static void error_callback(int e, const char *d) {
    printf("Error %d: %s\n", e, d);
}

// Test case structures
typedef struct {
    char input[50];
    char result[200];
    int accepted;
} TestCase;

// Test case arrays
static const char* dfa_test_inputs[] = {"1", "01", "10", "01000", "001000", "", "0", "0000000000", "11", "0110", "110"};
static const char* pda_test_inputs[] = {"1", "011", "00111", "0001111", "000011111", "", "0", "01", "11", "10", "0101"};
static const char* npda_test_inputs[] = {"1", "0", "11", "00", "101", "010", "1001", "10", "01", "100", ""};

static TestCase dfa_test_results[11];
static TestCase pda_test_results[11];
static TestCase npda_test_results[11];

static int dfa_tests_run = 0;
static int pda_tests_run = 0;
static int npda_tests_run = 0;

void run_dfa_tests() {
    for (int i = 0; i < 11; i++) {
        strcpy(dfa_test_results[i].input, dfa_test_inputs[i]);
        dfa_test_results[i].accepted = processDFA(dfa_test_inputs[i], dfa_test_results[i].result, sizeof(dfa_test_results[i].result));
    }
    dfa_tests_run = 1;
}

void run_pda_tests() {
    for (int i = 0; i < 11; i++) {
        strcpy(pda_test_results[i].input, pda_test_inputs[i]);
        pda_test_results[i].accepted = processPDA(pda_test_inputs[i], pda_test_results[i].result, sizeof(pda_test_results[i].result));
    }
    pda_tests_run = 1;
}

void run_npda_tests() {
    for (int i = 0; i < 11; i++) {
        strcpy(npda_test_results[i].input, npda_test_inputs[i]);
        npda_test_results[i].accepted = processNPDA(npda_test_inputs[i], npda_test_results[i].result, sizeof(npda_test_results[i].result));
    }
    npda_tests_run = 1;
}

int main() {
    /* GUI State Variables */
    // DFA variables
    static char dfa_input_buffer[500] = "";
    static char dfa_result_text[1000] = "Enter a binary string and click Process";
    static struct nk_color dfa_result_color = {255, 255, 255, 255};
    static int dfa_result_accepted = 0;
    static int dfa_show_output = 0;
    static int dfa_show_transition_table = 0;
    
    // PDA variables
    static char pda_input_buffer[500] = "";
    static char pda_result_text[1000] = "Enter a binary string and click Process";
    static struct nk_color pda_result_color = {255, 255, 255, 255};
    static int pda_result_accepted = 0;
    static int pda_show_output = 0;
    static int pda_show_transition_table = 0;
    
    // NPDA variables
    static char npda_input_buffer[500] = "";
    static char npda_result_text[1000] = "Enter a binary string and click Process";
    static struct nk_color npda_result_color = {255, 255, 255, 255};
    static int npda_result_accepted = 0;
    static int npda_show_output = 0;
    static int npda_show_transition_table = 0;
    
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        printf("ERROR: Failed to initialize GLFW\n");
        return 1;
    }
    
    /* Setup OpenGL 3.3 Core Profile - Cross Platform */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

#ifdef _WIN32
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

    /* Platform-agnostic window hints */
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    
    GLFWwindow *window = glfwCreateWindow(1800, 1400, "Automata Simulator Suite", NULL, NULL);
    if (!window) {
        printf("ERROR: Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    /* Initialize GLEW - Cross Platform */
#ifndef __APPLE__
    glewExperimental = GL_TRUE;  /* Needed for core profile */
#endif
    
    GLenum glew_status = glewInit();
    if (glew_status != GLEW_OK) {
        printf("ERROR: Failed to initialize GLEW: %s\n", glewGetErrorString(glew_status));
        glfwTerminate();
        return 1;
    }
    
    /* Print OpenGL info for debugging */
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    
    glViewport(0, 0, 1200, 1200);
    
    struct nk_glfw glfw = {0};
    struct nk_context *ctx;
    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);
    if (!ctx) {
        printf("ERROR: Failed to initialize Nuklear\n");
        return 1;
    }

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);
    nk_glfw3_font_stash_end(&glfw);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        nk_glfw3_new_frame(&glfw);

        // ========================= DFA WINDOW =========================
        /* Set purple header color */
        ctx->style.window.header.normal = nk_style_item_color(nk_rgb(147, 0, 211));
        ctx->style.window.header.hover = nk_style_item_color(nk_rgb(167, 20, 231));
        ctx->style.window.header.active = nk_style_item_color(nk_rgb(127, 0, 191));
        
        if (nk_begin(ctx, "DFA Simulator", nk_rect(20, 20, 560, 760), 
                     NK_WINDOW_BORDER|NK_WINDOW_TITLE|NK_WINDOW_MOVABLE)) {
            
            /* Title*/
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label_colored(ctx, "Binary String DFA", NK_TEXT_CENTERED, nk_rgb(204, 255, 229));
            
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label_colored(ctx, "Language: exactly one '1' in {0,1}*", NK_TEXT_CENTERED, nk_rgb(0, 128, 0));
            
            /* Spacing */
            nk_layout_row_dynamic(ctx, 10, 1);
            nk_spacing(ctx, 1);
            
            /* Input Section */
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Input Binary String:", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, dfa_input_buffer, 
                                         sizeof(dfa_input_buffer), nk_filter_ascii);

            /* Process Button */
            nk_layout_row_dynamic(ctx, 35, 1);
            if (nk_button_label(ctx, "Process String")) {
                dfa_result_accepted = processDFA(dfa_input_buffer, dfa_result_text, sizeof(dfa_result_text));
                if (dfa_result_accepted) {
                    dfa_result_color = nk_rgb(0, 255, 0);
                } else {
                    dfa_result_color = nk_rgb(255, 100, 100); 
                }
            }
            
            nk_layout_row_dynamic(ctx, 35, 3);
            
            if (nk_button_label(ctx, "Clear")) {
                dfa_input_buffer[0] = '\0';
                strcpy(dfa_result_text, "Enter a binary string and click Process");
                dfa_result_color = nk_rgb(255, 255, 255);
                dfa_show_output = 0;
                dfa_show_transition_table = 0;
                dfa_tests_run = 0;
            }
            
            if (nk_button_label(ctx, "Transition Table")) {
                dfa_show_output = 1;
                dfa_show_transition_table = 1;
            }
            
            if (nk_button_label(ctx, "Run Test Cases")) {
                run_dfa_tests();
                dfa_show_output = 1;
                dfa_show_transition_table = 0;
            }
            
            /* Spacing */
            nk_layout_row_dynamic(ctx, 15, 1);
            nk_spacing(ctx, 1);
            
            /* Result Display */
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Result:", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 60, 1);
            nk_label_colored_wrap(ctx, dfa_result_text, dfa_result_color);
            
            /* Output Display Area */
            if (dfa_show_output) {
                nk_layout_row_dynamic(ctx, 15, 1);
                nk_spacing(ctx, 1);
                
                if (dfa_show_transition_table) {
                    /* Show Transition Table */
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "TRANSITION TABLE:", NK_TEXT_LEFT);
                    
                    nk_layout_row_dynamic(ctx, 20, 1);
                    nk_label(ctx, "State S0 (Initial):", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '0' -> Stay in S0", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '1' -> Go to S1", NK_TEXT_LEFT);
                    nk_spacing(ctx, 1);
                    
                    nk_label(ctx, "State S1 (Accept) *:", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '0' -> Stay in S1", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '1' -> Go to S2", NK_TEXT_LEFT);
                    nk_spacing(ctx, 1);
                    
                    nk_label(ctx, "State S2 (Reject):", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '0' -> Stay in S2", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '1' -> Stay in S2", NK_TEXT_LEFT);
                    nk_spacing(ctx, 1);
                    
                    nk_label(ctx, "LANGUAGE: exactly one '1' in {0,1}*", NK_TEXT_LEFT);
                    
                } else {
                    /* Show Test Results */
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "ACTUAL TEST RESULTS:", NK_TEXT_LEFT);
                    
                    if (dfa_tests_run) {
                        nk_layout_row_dynamic(ctx, 18, 1);
                        for (int i = 0; i < 11; i++) {
                            char test_display[300];
                            const char* input_display = strlen(dfa_test_results[i].input) == 0 ? "(empty)" : dfa_test_results[i].input;
                            const char* result_display = dfa_test_results[i].accepted ? "ACCEPTED" : "REJECTED";
                            snprintf(test_display, sizeof(test_display), "Test %d: '%s' -> %s", i+1, input_display, result_display);
                            
                            if (dfa_test_results[i].accepted) {
                                nk_label_colored(ctx, test_display, NK_TEXT_LEFT, nk_rgb(0, 255, 0));
                            } else {
                                nk_label_colored(ctx, test_display, NK_TEXT_LEFT, nk_rgb(255, 100, 100));
                            }
                        }
                    } else {
                        nk_layout_row_dynamic(ctx, 20, 1);
                        nk_label(ctx, "Click 'Run Test Cases' to see actual results", NK_TEXT_LEFT);
                    }
                }
            }
            
            /* Examples Section */
            if (!dfa_show_output) {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_label(ctx, "Valid examples: 1, 01, 10, 0100", NK_TEXT_CENTERED);
                nk_label(ctx, "Invalid: 11, 101, 0011, 00, (empty)", NK_TEXT_CENTERED);            }
        }
        nk_end(ctx);

        // ========================= PDA WINDOW =========================
        /* Set purple header color */
        ctx->style.window.header.normal = nk_style_item_color(nk_rgb(147, 0, 211));
        ctx->style.window.header.hover = nk_style_item_color(nk_rgb(167, 20, 231));
        ctx->style.window.header.active = nk_style_item_color(nk_rgb(127, 0, 191));
        
        if (nk_begin(ctx, "PDA Simulator", nk_rect(600, 20, 560, 760), 
                     NK_WINDOW_BORDER|NK_WINDOW_TITLE|NK_WINDOW_MOVABLE)) {
            
            /* Title*/
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label_colored(ctx, "Pushdown Automaton (PDA)", NK_TEXT_CENTERED, nk_rgb(204, 255, 229));
            
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label_colored(ctx, "Language: {0^n 1^(n+1)} in {0,1}*", NK_TEXT_CENTERED, nk_rgb(0, 128, 0));
            
            /* Spacing */
            nk_layout_row_dynamic(ctx, 10, 1);
            nk_spacing(ctx, 1);
            
            /* Input Section */
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Input Binary String:", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, pda_input_buffer, 
                                         sizeof(pda_input_buffer), nk_filter_ascii);

            /* Process Button */
            nk_layout_row_dynamic(ctx, 35, 1);
            if (nk_button_label(ctx, "Process String")) {
                pda_result_accepted = processPDA(pda_input_buffer, pda_result_text, sizeof(pda_result_text));
                if (pda_result_accepted) {
                    pda_result_color = nk_rgb(0, 255, 0);
                } else {
                    pda_result_color = nk_rgb(255, 100, 100); 
                }
            }
            
            nk_layout_row_dynamic(ctx, 35, 3);
            
            if (nk_button_label(ctx, "Clear")) {
                pda_input_buffer[0] = '\0';
                strcpy(pda_result_text, "Enter a binary string and click Process");
                pda_result_color = nk_rgb(255, 255, 255);
                pda_show_output = 0;
                pda_show_transition_table = 0;
                pda_tests_run = 0;
            }
            
            if (nk_button_label(ctx, "Transition Table")) {
                pda_show_output = 1;
                pda_show_transition_table = 1;
            }
            
            if (nk_button_label(ctx, "Run Test Cases")) {
                run_pda_tests();
                pda_show_output = 1;
                pda_show_transition_table = 0;
            }
            
            /* Spacing */
            nk_layout_row_dynamic(ctx, 15, 1);
            nk_spacing(ctx, 1);
            
            /* Result Display */
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Result:", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 60, 1);
            nk_label_colored_wrap(ctx, pda_result_text, pda_result_color);
            
            /* Output Display Area */
            if (pda_show_output) {
                nk_layout_row_dynamic(ctx, 15, 1);
                nk_spacing(ctx, 1);
                
                if (pda_show_transition_table) {
                    /* Show Transition Table */
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "TRANSITION TABLE:", NK_TEXT_LEFT);
                    
                    nk_layout_row_dynamic(ctx, 18, 1);
                    nk_label(ctx, "State S0:", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '0', Stack '$' -> Push 0, Stay S0", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '0', Stack '0' -> Push 0, Stay S0", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '1', Stack '$' -> No action, Go S2", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '1', Stack '0' -> Pop 0, Go S1", NK_TEXT_LEFT);
                    nk_spacing(ctx, 1);
                    
                    nk_label(ctx, "State S1:", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '0' -> Error, Go S3", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '1', Stack '0' -> Pop 0, Stay S1", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Input '1', Stack '$' -> No action, Go S2", NK_TEXT_LEFT);
                    nk_spacing(ctx, 1);
                    
                    nk_label(ctx, "State S2 (Accept) *:", NK_TEXT_LEFT);
                    nk_label(ctx, "  - Any input -> Error, Go S3", NK_TEXT_LEFT);
                    nk_label(ctx, "State S3 (Reject): Stay S3", NK_TEXT_LEFT);
                    
                } else {
                    /* Show Test Results */
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "ACTUAL TEST RESULTS:", NK_TEXT_LEFT);
                    
                    if (pda_tests_run) {
                        nk_layout_row_dynamic(ctx, 18, 1);
                        for (int i = 0; i < 11; i++) {
                            char test_display[300];
                            const char* input_display = strlen(pda_test_results[i].input) == 0 ? "(empty)" : pda_test_results[i].input;
                            const char* result_display = pda_test_results[i].accepted ? "ACCEPTED" : "REJECTED";
                            snprintf(test_display, sizeof(test_display), "Test %d: '%s' -> %s", i+1, input_display, result_display);
                            
                            if (pda_test_results[i].accepted) {
                                nk_label_colored(ctx, test_display, NK_TEXT_LEFT, nk_rgb(0, 255, 0));
                            } else {
                                nk_label_colored(ctx, test_display, NK_TEXT_LEFT, nk_rgb(255, 100, 100));
                            }
                        }
                    } else {
                        nk_layout_row_dynamic(ctx, 20, 1);
                        nk_label(ctx, "Click 'Run Test Cases' to see actual results", NK_TEXT_LEFT);
                    }
                }
            }
            
            /* Examples Section */
            if (!pda_show_output) {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_label(ctx, "Valid examples: 1, 011, 00111, 0001111", NK_TEXT_CENTERED);
                nk_label(ctx, "Invalid: 01, 11, 10, 0101, (empty)", NK_TEXT_CENTERED);            }
        }
        nk_end(ctx);

        // ========================= NPDA WINDOW =========================
        /* Set purple header color */
        ctx->style.window.header.normal = nk_style_item_color(nk_rgb(147, 0, 211));
        ctx->style.window.header.hover = nk_style_item_color(nk_rgb(167, 20, 231));
        ctx->style.window.header.active = nk_style_item_color(nk_rgb(127, 0, 191));
        
        if (nk_begin(ctx, "NPDA Simulator", nk_rect(1180, 20, 560, 760), 
                     NK_WINDOW_BORDER|NK_WINDOW_TITLE|NK_WINDOW_MOVABLE)) {
            
            /* Title*/
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label_colored(ctx, "Non-Deterministic PDA (NPDA)", NK_TEXT_CENTERED, nk_rgb(204, 255, 229));
            
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label_colored(ctx, "Language: Palindromes in {0,1}*", NK_TEXT_CENTERED, nk_rgb(0, 128, 0));
            
            /* Spacing */
            nk_layout_row_dynamic(ctx, 10, 1);
            nk_spacing(ctx, 1);
            
            /* Input Section */
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Input Binary String:", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, npda_input_buffer, 
                                         sizeof(npda_input_buffer), nk_filter_ascii);

            /* Process Button */
            nk_layout_row_dynamic(ctx, 35, 1);
            if (nk_button_label(ctx, "Process String")) {
                npda_result_accepted = processNPDA(npda_input_buffer, npda_result_text, sizeof(npda_result_text));
                if (npda_result_accepted) {
                    npda_result_color = nk_rgb(0, 255, 0);
                } else {
                    npda_result_color = nk_rgb(255, 100, 100); 
                }
            }
            
            nk_layout_row_dynamic(ctx, 35, 3);
            
            if (nk_button_label(ctx, "Clear")) {
                npda_input_buffer[0] = '\0';
                strcpy(npda_result_text, "Enter a binary string and click Process");
                npda_result_color = nk_rgb(255, 255, 255);
                npda_show_output = 0;
                npda_show_transition_table = 0;
                npda_tests_run = 0;
            }
            
            if (nk_button_label(ctx, "Transition Table")) {
                npda_show_output = 1;
                npda_show_transition_table = 1;
            }
            
            if (nk_button_label(ctx, "Run Test Cases")) {
                run_npda_tests();
                npda_show_output = 1;
                npda_show_transition_table = 0;
            }
            
            /* Spacing */
            nk_layout_row_dynamic(ctx, 15, 1);
            nk_spacing(ctx, 1);
            
            /* Result Display */
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_label(ctx, "Result:", NK_TEXT_LEFT);
            
            nk_layout_row_dynamic(ctx, 60, 1);
            nk_label_colored_wrap(ctx, npda_result_text, npda_result_color);
            
            /* Output Display Area */
            if (npda_show_output) {
                nk_layout_row_dynamic(ctx, 15, 1);
                nk_spacing(ctx, 1);
                
                if (npda_show_transition_table) {
                    /* Show Transition Table */
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "TRANSITION TABLE:", NK_TEXT_LEFT);
                    
                    nk_layout_row_dynamic(ctx, 18, 1);
                    nk_label(ctx, "State S0:", NK_TEXT_LEFT);
                    nk_label(ctx, "    --- Transition: Stay S0 ---", NK_TEXT_LEFT);
                    nk_label(ctx, " -Input 1: Read 1, stack empty|| Read 1, pop 0, push 10 ||Read 1, pop 1, push 11", NK_TEXT_LEFT);
                    nk_label(ctx, " -Input 0: Read 0, stack empty || Read 0, pop 1, push 01 || Read 0, pop 0, push 00", NK_TEXT_LEFT);
                    nk_label(ctx, "    --- Transition: Go -> S1 ---", NK_TEXT_LEFT);
                    nk_label(ctx, "-Input 1: Read 1, pop 1 || Read epsilon, pop 1", NK_TEXT_LEFT);
                    nk_label(ctx, "-Input 0: Read 0, pop 0 || Read epsilon, pop 0", NK_TEXT_LEFT);
                    nk_spacing(ctx, 1);
                    
                    nk_label(ctx, "State S1:", NK_TEXT_LEFT);
                    nk_label(ctx, "    --- Transition: Stay S0 ---", NK_TEXT_LEFT);
                    nk_label(ctx, "-Input 1: Read 1, pop 1 ", NK_TEXT_LEFT);
                    nk_label(ctx, "-Input 0: Read 0, pop 0 ", NK_TEXT_LEFT);
                    nk_spacing(ctx, 1);
                    nk_label(ctx, "    --- Transition: Go -> S2 ---", NK_TEXT_LEFT);
                    nk_label(ctx, "- Read epsilon, stack (empty) $, go S2  ", NK_TEXT_LEFT);
                    
                    nk_label(ctx, "State S2:", NK_TEXT_LEFT);
                    nk_label(ctx, "- Accept state", NK_TEXT_LEFT);

                    
                } else {
                    /* Show Test Results */
                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "ACTUAL TEST RESULTS:", NK_TEXT_LEFT);
                    
                    if (npda_tests_run) {
                        nk_layout_row_dynamic(ctx, 18, 1);
                        for (int i = 0; i < 11; i++) {
                            char test_display[300];
                            const char* input_display = strlen(npda_test_results[i].input) == 0 ? "(empty)" : npda_test_results[i].input;
                            const char* result_display = npda_test_results[i].accepted ? "ACCEPTED" : "REJECTED";
                            snprintf(test_display, sizeof(test_display), "Test %d: '%s' -> %s", i+1, input_display, result_display);
                            
                            if (npda_test_results[i].accepted) {
                                nk_label_colored(ctx, test_display, NK_TEXT_LEFT, nk_rgb(0, 255, 0));
                            } else {
                                nk_label_colored(ctx, test_display, NK_TEXT_LEFT, nk_rgb(255, 100, 100));
                            }
                        }
                    } else {
                        nk_layout_row_dynamic(ctx, 20, 1);
                        nk_label(ctx, "Click 'Run Test Cases' to see actual results", NK_TEXT_LEFT);
                    }
                }
            }
            
            /* Examples Section */
            if (!npda_show_output) {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_label(ctx, "Valid examples: 1, 0, 11, 00, 101, 010, 1001", NK_TEXT_CENTERED);
                nk_label(ctx, "Invalid: 10, 01, 100, 110, 0011", NK_TEXT_CENTERED);            }
        }
        nk_end(ctx);

        /* Render */
        glClear(GL_COLOR_BUFFER_BIT);
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(window);
    }

    nk_glfw3_shutdown(&glfw);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}