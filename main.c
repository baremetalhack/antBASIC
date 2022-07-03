//
// m a i n . c
//
// BAREMETALHACK.COM --> public domain
//

#include <stdio.h>      // FILE{}, printf()
#include <setjmp.h>     // jmp_buf{}, setjmp()
#include <signal.h>     // sigemptyset(), sigaction(), SIG*, SA_*
#include <stdlib.h>     // atoi()
#include <string.h>     // strncpy()

#include "token.h"
#include "bcode.h"
#include "eval.h"
#include "program.h"
#include "basic.h"
#include "escape.h"
#include "debug.h"

#ifdef PI
#include "pi_gpio.h"
#endif //PI

//
// Global variables
//

FILE *Debug;
jmp_buf IntRecovery, EvalRecovery, BasicRecovery;
int Verbose = 1;

//
// Helper functions
//

static void
welcome(void)
{
    if (Verbose) {
        printf("\n=== Doctor BMH's %s%s%s v%d.%d.%d ===\n", \
                ESCAPE_LGREEN, BASIC_NAME, ESCAPE_DEFAULT, \
                BASIC_MAJOR, BASIC_MINOR, BASIC_PATCH);
        printf("  (enter %sHELP%s for summary, %sEND%s for exit)\n", \
                ESCAPE_LBLUE, ESCAPE_DEFAULT, ESCAPE_LBLUE, ESCAPE_DEFAULT);
        printf("\n");
    }
}

static int volatile sigint_edit_flag = 0;

static void
sigint_edit_handler()
{
    sigint_edit_flag = 1;
}

int
sigint_edit_abort(void) {
    return sigint_edit_flag;
}

void
sigint_edit_register(void)
{
    struct sigaction act;

    act.sa_handler = sigint_edit_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigint_edit_flag = 0;
    sigaction(SIGINT, &act, NULL);
}

static void
sigint_handler()
{
    longjmp(IntRecovery, 1);
}

void
sigint_register(void)
{
    struct sigaction act;

    act.sa_handler = sigint_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESETHAND | SA_NODEFER;
    sigaction(SIGINT, &act, NULL);
}

static void
location(void)
{
    printf("%s", ESCAPE_LGREEN);
    if (Mode == BASIC_MODELOAD)
        printf(" in \"%s\"", Text);
    else if (Mode == BASIC_MODEDIRECT) {
        if (Pnum)
            printf(" in line %d", Pnum);
    } else
        printf(" in line %d", Lnum);
    printf("%s\n", ESCAPE_DEFAULT);
}

static int
is_command(int inst) {
    switch (inst) {
        case BCODE_CLEAR:
        case BCODE_CLS:
        case BCODE_DUMP:
        case BCODE_DELETE:
        case BCODE_EDIT:
        case BCODE_FILES:
        case BCODE_FREE:
        case BCODE_HELP:
        case BCODE_LIST:
        case BCODE_LOAD:
        case BCODE_MERGE:
        case BCODE_NEW:
        case BCODE_RENUM:
        case BCODE_RUN:
        case BCODE_SAVE:
            return 1;

        default:
            return 0;
    }
}

//
// Main function
//

int
main(int argc, char *argv[])
{
    char text[ TOKEN_MAXBUF ], *msg;
    byte_t cmd[ BCODE_MAXSIZE ];
    int ret, size, num;
    bcode_t b;
    word_t val;

    // Initializer

    if (argc >= 2)
        Verbose = 0;
    Debug = stderr;
    token_init();
    eval_init(&EvalRecovery);
    cont_init();
    prog_init();
    basic_init(&BasicRecovery);
#ifdef PI
    ret = gpio_init();
    if (ret < 0) {
        printf("%sCould not identify Raspberry Pi on this system.%s\n", \
            ESCAPE_LRED, ESCAPE_DEFAULT);
        return 255;
    }
#endif // PI
    welcome();
    sigint_register();

    // === Shell mode ===

    if (argc >= 2) {                    // Program filename is specified
        if (argc >= 3)                  // --- First number argument is specified
            Var[ 0 ] = atoi(argv[ 2 ]); // --- Store the value in variable 'A'
        if (argc >= 4)                  // --- Second number argument is specified
            Var[ 1 ] = atoi(argv[ 3 ]); // --- Store the value in variable 'B'
        if (argc >= 5)                  // --- String argument is specified
            strncpy((char*) String, (const char*) argv[ 4 ], CONT_MAXSTRING); 
                                        // --- Store the value in string array '@'
        ret = basic_load(argv[ 1 ]);    // --- Load the program
        if (ret) {
            printf("%santBASIC: load error%s\n", ESCAPE_LRED, ESCAPE_DEFAULT);
            return 1;
        }
        if ((ret = setjmp(IntRecovery)) != 0) {
            printf("\n%santBASIC: user aborted%s\n", ESCAPE_LRED, ESCAPE_DEFAULT);
            return 2;
        }
        if ((ret = setjmp(EvalRecovery)) != 0) {
            printf("%santBASIC: eval() error%s\n", ESCAPE_LRED, ESCAPE_DEFAULT);
            return 3;
        }
        if ((ret = setjmp(BasicRecovery)) != 0) {
            printf("%santBASIC: syntax error%s\n", ESCAPE_LRED, ESCAPE_DEFAULT);
            return 4;
        }
        Mode = BASIC_MODESHELL;         // --- Switch to SHELL mode
        ret = basic_exec();             // --- Execute the program
        return ret;                     // --- return exit status to the shell
    }

    // Error handler

    if ((ret = setjmp(IntRecovery)) != 0) {
        if (Lnum) {
            printf("\n%sAborted ", ESCAPE_LRED);
            location();
            Lnum = 0;
        } else
            printf("\n");
        sigint_register();
    }
    if ((ret = setjmp(EvalRecovery)) != 0) {
        printf("%sError: %s", ESCAPE_LRED, eval_result(ret));
        location();
    }
    if ((ret = setjmp(BasicRecovery)) != 0) {
        if (ret & BASIC_BCODE_ERROR) {
            ret -= BASIC_BCODE_ERROR;
            msg = bcode_result(ret);
        } else if (ret & BASIC_PROG_ERROR) {
            ret -= BASIC_PROG_ERROR;
            msg = prog_result(ret);
        }
#ifdef PI
        else if (ret & BASIC_GPIO_ERROR) {
            ret -= BASIC_GPIO_ERROR;
            msg = gpio_result(ret);
        }
#endif
        else
            msg = basic_result(ret);
        printf("%sError: %s", ESCAPE_LRED, msg);
        location();
    }

    // === Interactive direct mode ===

    while (1) {
        printf("\e[0;0m");          // Reset screen attributes
        Mode = BASIC_MODEDIRECT;    // Enter direct mode
        Pnum = 0;
        ret = basic_readline("> ", text, TOKEN_MAXBUF);
        if (ret == 0)
            continue;
        token_source(text);
        size = bcode_compile(cmd);
        if (size < 0) {
            printf("%sError: %s", ESCAPE_LRED, bcode_result(-size));
            location();
            continue;
        }
#ifdef DEBUG
        bcode_dump(cmd);
        fprintf(Debug, "\n");
#endif // DEBUG

        bcode_start(cmd);
        bcode_read(&b);
        if (b.type == BCODE_TYPE_NUMBER10) {        // Edit program
            num = b.num;
            if (bcode_next() == BCODE_EOL) {        // --- delete a line
                ret = prog_delete(num, num);
                if (ret) {
                    printf("%sError: %s", ESCAPE_LRED, prog_result(ret));
                    location();
                }
            } else {                                // --- insert a line
                bcode_read(&b);
                ret = prog_insert(num, &cmd[ b.pos ], size - 3);
                if (ret) {
                    printf("%sError: %s", ESCAPE_LRED, prog_result(ret));
                    location();
                }
            }
            continue;
        } else if (b.inst == BCODE_INQ) {           // Evaluate expression
            if (bcode_next() == BCODE_AT && bcode_twoahead() != BCODE_LBRACK) {
                bcode_skip();
                printf("%s%s%s\n", ESCAPE_LBLUE, String, ESCAPE_DEFAULT);
            } else {
                val = eval();
                printf("%s%d%s\n", ESCAPE_LBLUE, val, ESCAPE_DEFAULT);
            }
            continue;
        } else if (b.type == BCODE_TYPE_KEYWORD) {
            if (b.inst == BCODE_END)                // END
                break;                              // --- exit loop
            else if (is_command(b.inst)) {          // BASIC commands
                basic_command(b.inst);              // --- execute it
                continue;
            }
        }                                           // Direct statement execution
        prog_cmdline(cmd, size);                    // --- store one liner program
        basic_exec();                               // --- execute it
    }
#ifdef PI
    gpio_term();
#endif // PI
    return 0;
}
