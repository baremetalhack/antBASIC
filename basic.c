//
//  b a s i c . c
//
//  BAREMETALHACK.COM --> public domain
//

#include <stdio.h>              // FILE{}, stdin, fgets(), fflush()
#include <setjmp.h>             // jmp_buf{}, longjmp()
#include <string.h>             // strlen(), strncpy()
#include <stdlib.h>             // atoi(), free()
#include <dirent.h>             // DIR{}, dirent{}, scandir()
#include <unistd.h>             // usleep()

#ifdef READLINE
#include <readline/readline.h>  // readline()
#include <readline/history.h>   // add_history()
#endif // READLINE

#include "basic.h"
#include "token.h"
#include "bcode.h"
#include "eval.h"
#include "program.h"
#include "container.h"
#include "escape.h"
#include "debug.h"

#ifdef PI
#include "pi_gpio.h"
#endif // PI

//
// Global variable
//

 int Lnum;                  // Current executing line number
 int Mode;                  // Current executing mode (NORMAL/DIRECT/LOAD/SHELL)
 int Status = 0;            // Exit status which will be returned to the shell
char Text[ TOKEN_MAXBUF ];  // Source text line buffer

//
// File scope variables
//

static jmp_buf * Exit;
      static int Lidx;
      static int Gsize, Fsize;
 static gstack_t Gstack[ BASIC_MAXSTACKDEPTH ];
 static fstack_t Fstack[ BASIC_MAXSTACKDEPTH ];

static char * Results[] = {
    "BASIC \"ok\"",                     // BASIC_SUCCESS
    "BASIC \"direct exec is denied\"",  // BASIC_DIRECTDENY
    "BASIC \"directory not found\"",    // BASIC_DIRNOTFOUND
    "BASIC \"file I/O error\"",         // BASIC_FILEIOERROR
    "BASIC \"illegal array\"",          // BASIC_ILLARRAY
    "BASIC \"illegal range\"",          // BASIC_ILLRANGE
    "BASIC \"no array\"",               // BASIC_NOARRAY
    "BASIC \"no assignment\"",          // BASIC_NOASSIGN
    "BASIC \"no control variable\"",    // BASIC_NOCONTROLVAR
    "BASIC \"no directroy name\"",      // BASIC_NODIRNAME
    "BASIC \"no line number\"",         // BASIC_NOLINENUM
    "BASIC \"no filename\"",            // BASIC_NOFILENAME
    "BASIC \"no stack\"",               // BASIC_NOSTACK
    "BASIC \"no variable\"",            // BASIC_NOVARIABLE
    "BASIC \"stack is overflowed\"",    // BASIC_STACKOVER
    "BASIC \"syntax error\""            // BASIC_SYNTAXERROR
};

static char* help_container[] = {
    "CONTAINERS",     "",
    "Numbers",        "Signed 16bit integer (range from -32768 to 32767)$" \
                      "Decimal or hexadecimal (0x prefix is needed)$" \
                      "    ex. 1234, -1234, 0xABCD, 0xEF",
    "Strings",        "8bit ASCII characters surrounded by double quotation$" \
                      "Escaped special characters are as follows:$" \
                      "    \\a BEL, \\b BS, \\t TAB, \\n LF, \\r CR, \\e ESC, \\\\ backslash$" \
                      "Special array @ holds string$" \
                      "    ex. @=\"hello!\":@[0]=@[0]-0x20:print @ -> Hello!$" \
                      "        @[0]=33:@[1]=7:@[2]=0:print @ -> ! with alarm",
    "Variables",      "Vaiable A to Z holds integer: ex. A=123:B=A+0x1234",
    "Arrays",         "Array A[] to Z[] holds integer (index starts from ZERO)$" \
                      "Two-dimensional array form is X[column,row]$" \
                      "    ex. DIM A[1],B[2,3]:A[0]=1:B[0,0]=0,1,2,3,4,5$" \
                      "        A[0] -> 1, B[0,2] -> 2, B[1,0] -> 3, B[1,2] -> 5",
    0
};

static char* help_operator[] = {
    "OPERATORS",      "Precedence (high to low)",
    "Unary",          "-xxx, +xxx",
    "Mul/Div/Mod",    "*, /, %",
    "Add/Sub",        "+, -",
    "Condition",      "==, !=, <, <=, >, >=",
    "Bitwise",        "&, |",
    0
};

static char* help_statement[] = {
    "STATEMENTES",    "",
    "CLS",            "Clear screen",
    "DIM",            "Define array size: DIM[ col, row ]$" \
                      "    NOTE: array size limitation, col * row <= 512",
    "END",            "Terminate program",
    "FOR/NEXT",       "Iterate statements between FOR and NEXT$" \
                      "    ex. S=0:FOR A=1 TO 10:S=S+A:NEXT$" \
                      "    NOTE: increment step is fixed to ONE",
    "GOTO",           "Jump to specified line number: ex. GOTO 100, GOTO X",
    "GOSUB/RETURN",   "Call subroutine / return to caller: ex. GOSUB 200, GOSUB Y",
    "IF",             "Conditional execution: if expression is ZERO then goto next line$" \
                      "    ex. IF A<B GOTO 100, IF A&0x80 B=128:PRINT \"MSB is on\"",
#ifdef PI
    "IN",             "Read GPIO (B 1-14) bit level: IN(B) -> 0|1",
#endif // PI
    "INPUT",          "Input data from user: number) INPUT A, string) INPUT @",
#ifdef PI
    "OUT",            "Set GPIO (B 1-14) bit level (L 0 GND|1 Vdd): OUT(B,L)",
    "OUTHZ",          "Set GPIO (B 1-14) bit level (L 0 GND|1 HiZ),$" \
                      "    Internal pull-up (P 0 None|1 Pull): OUTHZ(B,S,P)",
#endif // PI
    "PRINT",          "Print data$" \
                      "    integer: immediate value, variable, array$" \
                      "    string: @$" \
                      "    separator: semicolon=no spacing, comma=tabulation$" \
                      "      PRINT \"H\";\"I\";\"!\" -> HI!$" \
                      "    hexadecimal format: HEX2(xxx), HEX4(xxxxx)",
    "REM",            "Remark: REM, REM \"This is comment string\"",
    "RND",            "Returns random number (0 to 32767): RND()",
    "MSLEEP",         "Suspend execution for MILLI-seconds: SLEEP(1000) -> 1sec",
    "USLEEP",         "Suspend execution for MICRO-seconds: MLEEP(1000) -> 1usec",
    0
};

static char* help_command[] = {
    "COMMANDS",       "",
    "CLEAR",          "Clear containers",
    "CLS",            "Clear screen",
    "DELETE",         "Delete statement(s): DELETE 100, DELETE 210,290",
    "DUMP",           "Dump containers: DUMP (all), DUMP V (variables), DUMP A (arrays),$" \
                      "     DUMP S (string), DUMP L (lines), DUMP B (bytecodes)",
    "END",            "Quit antBASIC",
    "FILES",          "List files in current working directory",
    "FREE",           "Display memory usage",
    "HELP",           "Display help information",
    "LIST",           "List all or part of program: LIST, LIST 100, LIST 210,330",
    "LOAD",           "Load a source file into memory: LOAD \"example/hello.bas\"",
    "MERGE",          "Merge an additional file into memory: MERGE \"mylib/addon.bas\"",
    "NEW",            "Clear program",
    "RENUM",          "Renumber program: RENUM, RENUM start, RENUM start,step",
    "RUN",            "Start-up program: CONTROL-C aborts the program",
    "SAVE",           "Save program list to a file: SAVE \"myprogram.bas\"",
    0
};

//
// Initializer
//

void
basic_init(jmp_buf *env)
{
    Exit = env;
    Lnum = Lidx = Gsize = Fsize = 0;
}

//
// Helper functions
//

void
basic_error(int code)
{
    longjmp(*Exit, code);
}

int
basic_readline(char *msg, char *buf, int size)
{
    char *ptr;
    int len;

#ifdef READLINE
    ptr = readline(msg);
    if (ptr) {
        len = strlen(ptr);
        strncpy(buf, ptr, size);
        add_history(ptr);
        free(ptr);
    }
    else
        len = 0;
#else
    printf("%s", msg);
    ptr = fgets(buf, size, stdin);
    if (ptr == NULL)
        len = 0;
    else {
        while (*ptr) {
            if (*ptr == '\n') {
                *ptr = 0;
                break;
            }
        }
        len = strlen(ptr);
    }
#endif // READLINE
    return len;
}

//
// BASIC statements
//

static void
stat_rem(void)
{
    bcode_t b;

    while (bcode_next() != BCODE_EOL) {
        bcode_read(&b);
    }
    DMSG(">>> %05d REM", Lnum);
}

static int
stat_end(void)
{
    bcode_t b;
    word_t num;

    if (bcode_nextiseol())
        num = 0;
    else if (bcode_nextisvar()) {
        bcode_read(&b);
        num = Var[ b.idx ];
    } else
        num = eval();

    return num;
}

static void
stat_dim(void)
{
    bcode_t b;
    int v;
    dim_t size;

    while (1) {
        bcode_read(&b);
        if (b.type != BCODE_TYPE_VARIABLE)
            basic_error(BASIC_NOARRAY);
        v = b.idx;

        eval_dim(&size);
        if (size.row < 0 || size.col <= 0)
            basic_error(BASIC_ILLRANGE);
        if (size.row == 0) {
            size.row = 1;
            if (size.col > CONT_MAXARRAY)
                basic_error(BASIC_ILLRANGE);
        } else if (size.row * size.col > CONT_MAXARRAY)
            basic_error(BASIC_ILLRANGE);
        Asize[ v ].row = size.row;
        Asize[ v ].col = size.col;
        DMSG(">>> %05d DIM %c[%d,%d]\n", Lnum, v+'A', Asize[ v ].row, Asize[ v ].col);
        if (bcode_next() == BCODE_COLON || bcode_next() == BCODE_EOL)
            return;
        else if (bcode_next() == BCODE_COMMA)
            bcode_skip();
        else
            basic_error(BASIC_ILLARRAY);
    }
}

static void
stat_print(void)
{
    bcode_t b;
    int pos = 0, len, sp, newline = 1;
    word_t res;

    while (1) {
        if (bcode_next() == BCODE_COLON || bcode_next() == BCODE_EOL)
            break;
        else if (bcode_next() == BCODE_STRING) {
            bcode_read(&b);
            len = printf("%s", b.str);
            pos += len;
            newline = 1;
        } else if (bcode_next() == BCODE_AT && \
                   bcode_twoahead() != BCODE_LBRACK) {
            bcode_skip();
            len = printf("%s", String);
            pos += len;
            newline = 1;
        } else if (bcode_next() == BCODE_HEX2) {
            bcode_skip();
            if (bcode_next() != BCODE_LPAREN)
                basic_error(BASIC_SYNTAXERROR);
            bcode_skip();
            res = eval();
            if (bcode_next() != BCODE_RPAREN)
                basic_error(BASIC_SYNTAXERROR);
            bcode_skip();
            len = printf("%02hhX", (unsigned char) res);
            pos += len;
            newline = 1;
        } else if (bcode_next() == BCODE_HEX4) {
            bcode_skip();
            if (bcode_next() != BCODE_LPAREN)
                basic_error(BASIC_SYNTAXERROR);
            bcode_skip();
            res = eval();
            if (bcode_next() != BCODE_RPAREN)
                basic_error(BASIC_SYNTAXERROR);
            bcode_skip();
            len = printf("%04hX", res);
            pos += len;
            newline = 1;
        } else {
            res = eval();
            len = printf("%d", (int) res);
            pos += len;
            newline = 1;
        }
        if (bcode_next() == BCODE_COMMA) {
            bcode_skip();
            sp = 8 - (pos % 8);
            if (sp == 8)
                sp = 0;
            pos += sp;
            while (sp--)
                printf(" ");
            newline = 1;
        } else if (bcode_next() == BCODE_SCOLON) {
            bcode_skip();
            newline = 0;
        }
    }
    if (newline)
        printf("\n");
    fflush(stdout);
    DMSG(">>> %05d PRINT <<<", Lnum);
}

static void
stat_input(void)
{
    int len;
    bcode_t b;
    word_t val;
    char buff[ BASIC_MAXLINECHAR ];

    if (bcode_next() == BCODE_AT) {
        bcode_skip();
        basic_readline("? ", (char *) String, BASIC_MAXLINECHAR);
        DMSG(">>> %05d INPUT @", Lnum);
    } else if (bcode_nextisvar()) {
        bcode_read(&b);
        len = basic_readline("? ", buff, BASIC_MAXLINECHAR);
        if (len == 0)
            val = 0; 
        else
            val = (word_t) atoi(buff);
        Var[ b.idx ] = val;
        DMSG(">>> %05d INPUT %c = %d\n", Lnum, 'A' + b.idx, val);
    } else
        basic_error(BASIC_NOVARIABLE);
}

static int
stat_goto(void)
{
    bcode_t b;
    word_t num = 0;

    if (bcode_nextisvar()) {
        bcode_read(&b);
        num = Var[ b.idx ];
    } else
        num = eval();
    DMSG(">>> %05d GOTO %d <<<", Lnum, num);
    return num;
}

static int
stat_gosub(void)
{
    bcode_t b;
    word_t num = 0;

    if (bcode_nextisvar()) {
        bcode_read(&b);
        num = Var[ b.idx ];
    } else
        num = eval();

    if (Gsize == BASIC_MAXSTACKDEPTH)
        basic_error(BASIC_STACKOVER);
    Gstack[ Gsize ].lidx = Lidx;
    Gstack[ Gsize ].radd = bcode_getpc();
    Gsize++;
    DMSG(">>> %05d GOSUB %d", Lnum, num);
    return num;
}

static void
stat_return(void)
{
    if (Gsize == 0)
        basic_error(BASIC_NOSTACK);
    else
        Gsize--;
    DMSG(">>> %05d RETURN", Lnum);
}

static int
stat_if(void)
{
    word_t res;

    res = eval();
    res = (res != 0) ? 1 : 0;
    DMSG(">>> %05d IF %d", Lnum, res);
    return res;
}

static void
stat_for(void)
{
    bcode_t b;
    int var;
    word_t start, end;

    if (! bcode_nextisvar())
        basic_error(BASIC_NOCONTROLVAR);
    bcode_read(&b);
    var = b.idx;

    if (bcode_next() != BCODE_ASSIGN)
        basic_error(BASIC_NOASSIGN);
    bcode_skip();
    start = eval();
    
    if (bcode_next() != BCODE_TO)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
    end = eval();

    if (start > end)
        basic_error(BASIC_ILLRANGE);
    if (Fsize == BASIC_MAXSTACKDEPTH)
        basic_error(BASIC_STACKOVER);

    Var[ var ] = start;
    Fstack[ Fsize ].lidx = Lidx;
    Fstack[ Fsize ].ladd = bcode_getpc();
    Fstack[ Fsize ].vidx = var;
    Fstack[ Fsize ].ecnt = (int) end;
    Fsize++;
    DMSG(">>> %05d FOR lidx=%d : ladd=%d : var '%c' : start=%d : end=%d\n", \
            Lnum, Lidx, bcode_getpc(), var + 'A', start, end);
}

static void
stat_next(void)
{
    if (Fsize == 0)
        basic_error(BASIC_NOSTACK);
    DMSG(">>> %05d NEXT", Lnum);
}

static void
stat_cls(void)
{
    printf(ESCAPE_HOME ESCAPE_CLS);
    DMSG(">>> %05d CLS", Lnum);
}

static void
stat_usleep(void)
{
    word_t val;

    if (bcode_next() != BCODE_LPAREN)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
    val = eval();
    if (bcode_next() != BCODE_RPAREN)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
#ifdef PI
    gpio_usleep(val);
#else
    usleep(val);
#endif // PI
    DMSG(">>> %05d USLEEP(%d)", Lnum, val);
}

static void
stat_msleep(void)
{
    word_t val;

    if (bcode_next() != BCODE_LPAREN)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
    val = eval();
    if (bcode_next() != BCODE_RPAREN)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
    usleep(val * 1000);
    DMSG(">>> %05d MSLEEP(%d)", Lnum, val);
}

#ifdef PI
static void
stat_out(void)
{
    word_t bit, val;
    int res;

    if (bcode_next() != BCODE_LPAREN)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
    bit = eval();
    if (bcode_next() != BCODE_COMMA)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
    val = eval();
    if (bcode_next() != BCODE_RPAREN)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
    res = gpio_out(bit, val);
    if (res < 0) {
        res = -res;
        basic_error(res | BASIC_GPIO_ERROR);
    }
    DMSG(">>> %05d OUT(%d,%d)", Lnum, bit, val);
}

static void
stat_outhz(void)
{
    word_t bit, val, pull;
    int res;

    if (bcode_next() != BCODE_LPAREN)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
    bit = eval();
    if (bcode_next() != BCODE_COMMA)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
    val = eval();
    if (bcode_next() != BCODE_COMMA)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
    pull = eval();
    if (bcode_next() != BCODE_RPAREN)
        basic_error(BASIC_SYNTAXERROR);
    bcode_skip();
    res = gpio_outhz(bit, val, pull);
    if (res < 0) {
        res = -res;
        basic_error(res | BASIC_GPIO_ERROR);
    }
    DMSG(">>> %05d OUTHZ(%d,%d,%d)", Lnum, bit, val, pull);
}
#endif // PI

//
// BASIC commands
//

#define FIRSTCOLSIZE  14
static void
display_help(char **msg)
{
    char *ptr;
    int cnt = 0, sp, i;

    ptr = *msg++;
    while (ptr) {
        if (cnt == 0)
            printf("%s%s%s", ESCAPE_LRED, ptr, ESCAPE_DEFAULT);
        else
            printf("%s%s%s", ESCAPE_LGREEN, ptr, ESCAPE_DEFAULT);
        sp = FIRSTCOLSIZE - strlen(ptr);
        while (sp--)
            printf(" ");
        if (cnt == 0)
            printf("%s", ESCAPE_LRED);
        ptr = *msg++;
        while (*ptr) {
            if (*ptr == '$') {
                printf("\n");
                for (i = 0; i < FIRSTCOLSIZE; i++)
                    printf(" ");
            } else
                printf("%c", *ptr);
            ptr++;
        }
        if (cnt == 0)
            printf("%s", ESCAPE_DEFAULT);
        printf("\n");
        ptr = *msg++;
        cnt++;
    }
}

static void
cmd_help(void)
{
    bcode_t b;
    int mode = 0;
    char buf[ 3 ], *ptr;

    if (bcode_nextisvar()) {
        bcode_read(&b);
        switch(b.idx) {
            case 'C' - 'A':     // Containers
                mode = 1;
                break;
            
            case 'O' - 'A':     // Operators
                mode = 2;
                break;
            
            case 'S' - 'A':     // Statements
                mode = 3;
                break;
            
            case 'B' - 'A':     // BASIC commands
                mode = 4;
                break;
            
            default:            // All
                mode = 5;
                break;
        }
    }

    if (mode == 0) {
        printf("---[ Help ]-----------------------------------------------------------------\n");
        printf("C)ontainers, O)perators, S)tatements, B)ASIC commands, A)ll (c/o/s/b/a) ? ");
        ptr = fgets(buf, 3, stdin);;
        printf("----------------------------------------------------------------------------\n");
        if (*ptr == 'C' || *ptr == 'c')
            mode = 1;
        else if (*ptr == 'O' || *ptr == 'o')
            mode = 2;
        else if (*ptr == 'S' || *ptr == 's')
            mode = 3;
        else if (*ptr == 'B' || *ptr == 'b')
            mode = 4;
        else
            mode = 5;
    }

    switch (mode) {
        case 1:
            display_help(help_container);
            break;

        case 2:
            display_help(help_operator);
            break;

        case 3:
            display_help(help_statement);
            break;

        case 4:
            display_help(help_command);
            break;

        default:
            display_help(help_container);
            printf("\n");
            display_help(help_operator);
            printf("\n");
            display_help(help_statement);
            printf("\n");
            display_help(help_command);
            break;
    }
}

static void
cmd_free(void)
{
    printf(ESCAPE_LBLUE);
    printf("%d lines, %d lines free\n", Lsize, PROG_MAXLINE - Lsize);
    printf("%d bytes used, %d bytes free\n", Psize, PROG_MAXSIZE - Psize);
    printf(ESCAPE_DEFAULT);
}

static void
cmd_dump(void)
{
    bcode_t b;

    if (bcode_nextisvar()) {
        bcode_read(&b);
        switch(b.idx) {
            case 'V' - 'A':
                cont_dumpvar();
                return;
            
            case 'A' - 'A':
                cont_dumparr();
                return;
            
            case 'S' - 'A':
                cont_dumpstr();
                return;
            
            case 'B' - 'A':
                prog_dumpbytes();
                return;
            
            case 'L' - 'A':
                prog_dumplines();
                return;
        }
    }
    cont_dumpvar();
    printf("\n");
    cont_dumparr();
    printf("\n");
    cont_dumpstr();
    printf("\n");
    prog_dumpbytes();
    printf("\n");
    prog_dumplines();
}

static void
cmd_new(void)
{
    prog_init();
    Lnum = Lidx = Gsize = Fsize = 0;
}

static void
cmd_clear(void)
{
    cont_init();
}

static void
cmd_list(void)
{
    bcode_t b;
    int start, end, ret;

    start = Lines[ 1 ].num;
    end = Lines[ Lsize ].num;
    if (bcode_next() == BCODE_NUMBER10) {
        bcode_read(&b);
        start = b.num;
        if (start < 1 || start >= PROG_BLANKLINE)
            basic_error(BASIC_ILLRANGE);
        end = start;
        if (bcode_next() == BCODE_COMMA) {
            bcode_skip();
            if (bcode_next() == BCODE_NUMBER10) {
                bcode_read(&b);
                end = b.num;
                if (end < 1 || end >= PROG_BLANKLINE || start > end)
                    basic_error(BASIC_ILLRANGE);
            } else
                basic_error(BASIC_ILLRANGE);
        }
    } else if (bcode_next() != BCODE_EOL)
        basic_error(BASIC_ILLRANGE);
    ret = prog_list(stdout, PROG_LISTCOLOR, start, end);
    if (ret)
        basic_error(ret | BASIC_PROG_ERROR);
}

static void
cmd_renum(void)
{
    bcode_t b;
    int start = 0, step = 0, ret;

    if (bcode_next() == BCODE_EOL) {
        start = BASIC_LINESTART;
        step = BASIC_LINESTEP;
    } else if (bcode_nextisnum()) {
        bcode_read(&b);
        start = b.num;
        if (bcode_next() == BCODE_COMMA) {
            bcode_skip();
            if (! bcode_nextisnum())
                basic_error(BASIC_ILLRANGE);
            else {
                bcode_read(&b);
                step = b.num;
            }
        } else if (bcode_next() == BCODE_EOL)
            step = BASIC_LINESTEP;
        else
            basic_error(BASIC_ILLRANGE);
    } else
        basic_error(BASIC_ILLRANGE);

    if (start < 1 || step < 1 || (start + step * Lsize) >= PROG_BLANKLINE)
        basic_error(BASIC_ILLRANGE);

    ret = prog_renum(start, step);
    if (ret)
        basic_error(ret | BASIC_PROG_ERROR);
}

static void
cmd_delete(void)
{
    bcode_t b;
    int start = 0, end = 0, ret;

    if (bcode_next() == BCODE_NUMBER10) {
        bcode_read(&b);
        start = b.num;
        if (start < 1 || start >= PROG_BLANKLINE)
            basic_error(BASIC_ILLRANGE);
        end = start;
        if (bcode_next() == BCODE_COMMA) {
            bcode_skip();
            if (bcode_next() == BCODE_NUMBER10) {
                bcode_read(&b);
                end = b.num;
                if (end < 1 || end >= PROG_BLANKLINE || start > end)
                    basic_error(BASIC_ILLRANGE);
            } else
                basic_error(BASIC_ILLRANGE);
        }
    } else
        basic_error(BASIC_NOLINENUM);
    ret = prog_delete(start, end);
    if (ret)
        basic_error(ret | BASIC_PROG_ERROR);
}

static void
cmd_files(void)
{
    bcode_t b;
    struct dirent **list;
    char *dir;
    int i, cnt;

    if (bcode_nextiseol())
        dir = "./";
    else if (! bcode_nextisstr())
        basic_error(BASIC_NODIRNAME);
    else {
        bcode_read(&b);
        dir = (char *) b.str;
    }
    cnt = scandir(dir, &list, NULL, alphasort);
    if (cnt == 0)
        basic_error(BASIC_DIRNOTFOUND);

    for (i = 0; i < cnt; i++) {
        if (list[ i ]->d_name[0] == '.')
            continue;
        if (list[ i ]->d_type == DT_DIR) {
            printf(ESCAPE_LGREEN);
            printf("%s/\n", list[ i ]->d_name);
        } else {
            printf(ESCAPE_LBLUE);
            printf("%s\n", list[ i ]->d_name);
        }
    }
    printf(ESCAPE_DEFAULT);
    free(list);
}

static void
cmd_load(void)
{
    bcode_t b;
    FILE *src;
    char *ptr;
    byte_t code[ BCODE_MAXSIZE ];
    int size, line, ret;

    if (! bcode_nextisstr())
        basic_error(BASIC_NOFILENAME);
    bcode_read(&b);
    printf("Loading file \"%s\"\n", b.str);
    src = fopen((char *) b.str, "r");
    if (src == NULL)
        basic_error(BASIC_FILEIOERROR);

    prog_init();
    Mode = BASIC_MODELOAD;
    while (1) {
        ptr = fgets(Text, TOKEN_MAXBUF, src);
        if (ptr == NULL)
            break;
        while (*ptr) {
            if (*ptr == '\n')
                *ptr = 0;
            ptr++;
        }
        token_source(Text);
        size = bcode_compile(code);
        if (size < 0) {
            fclose(src);
            size = -size;
            basic_error(size | BASIC_BCODE_ERROR);
        }
        bcode_start(code);
        if (! bcode_nextisnum()) {
            fclose(src);
            basic_error(BASIC_NOLINENUM);
        }
        bcode_read(&b);
        line = b.num;
        bcode_read(&b);
        ret = prog_insert(line, &code[ b.pos ], size - 3);
        if (ret) {
            fclose(src);
            basic_error(ret | BASIC_PROG_ERROR);
        }
    }
    fclose(src);
    printf("%d lines loaded\n", Lsize);
}

static void
cmd_merge(void)
{
    bcode_t b;
    FILE *src;
    char *ptr;
    byte_t code[ BCODE_MAXSIZE ];
    int cnt, size, line, ret;

    if (! bcode_nextisstr())
        basic_error(BASIC_NOFILENAME);
    bcode_read(&b);
    printf("Merging file \"%s\"\n", b.str);
    src = fopen((char *) b.str, "r");
    if (src == NULL)
        basic_error(BASIC_FILEIOERROR);

    cnt = 0;
    Mode = BASIC_MODELOAD;
    while (1) {
        ptr = fgets(Text, TOKEN_MAXBUF, src);
        if (ptr == NULL)
            break;
        while (*ptr) {
            if (*ptr == '\n')
                *ptr = 0;
            ptr++;
        }
        token_source(Text);
        size = bcode_compile(code);
        if (size < 0) {
            fclose(src);
            size = -size;
            basic_error(size | BASIC_BCODE_ERROR);
        }
        bcode_start(code);
        if (! bcode_nextisnum()) {
            fclose(src);
            basic_error(BASIC_NOLINENUM);
        }
        bcode_read(&b);
        line = b.num;
        bcode_read(&b);
        ret = prog_insert(line, &code[ b.pos ], size - 3);
        if (ret) {
            fclose(src);
            basic_error(ret | BASIC_PROG_ERROR);
        }
        cnt++;
    }
    fclose(src);
    printf("%d lines merged\n", cnt);
}

static void
cmd_save(void)
{
    bcode_t b;
    FILE *dst;
    int ret;

    if (! bcode_nextisstr())
        basic_error(BASIC_NOFILENAME);
    bcode_read(&b);
    DMSG("Saving file \"%s\"\n", b.str);
    dst = fopen((char *) b.str, "w");
    if (dst == NULL)
        basic_error(BASIC_FILEIOERROR);
    ret = prog_list(dst, PROG_LISTPLAIN, Lines[ 1 ].num, Lines[ Lsize ].num);
    if (ret) {
        fclose(dst);
        basic_error(ret | BASIC_PROG_ERROR);
    }
    fclose(dst);
    printf("%d lines saved\n", Lsize);
}

static void
deny_direct(void)
{
    if (Mode == BASIC_MODEDIRECT)
        basic_error(BASIC_DIRECTDENY);
}

int
basic_load(char *fname)
{
    bcode_t b;
    FILE *src;
    char *ptr;
    byte_t code[ BCODE_MAXSIZE ];
    int size, line, ret;

    src = fopen((char *) fname, "r");
    if (src == NULL)
        return 1;           // 1: File open failure

    prog_init();
    Mode = BASIC_MODELOAD;
    while (1) {
        ptr = fgets(Text, TOKEN_MAXBUF, src);
        if (ptr == NULL)
            break;
        while (*ptr) {
            if (*ptr == '\n')
                *ptr = 0;
            ptr++;
        }
        token_source(Text);
        size = bcode_compile(code);
        if (size < 0) {
            fclose(src);
            size = -size;
            return 2;       // 2: Bytecode compile error
        }
        bcode_start(code);
        if (! bcode_nextisnum()) {
            fclose(src);
            return 3;       // 3: Line number undefined
        }
        bcode_read(&b);
        line = b.num;
        bcode_read(&b);
        ret = prog_insert(line, &code[ b.pos ], size - 3);
        if (ret) {
            fclose(src);
            return 4;       // 4: Program error
        }
    }
    fclose(src);
    return 0;
}

int
basic_exec(void)
{
    bcode_t b;
    int line, jump = 0, back = 0, skip = 0, newpc, var;

    if (Mode == BASIC_MODEDIRECT)
        line = 0;
    else if (Mode == BASIC_MODERUN) {
        if (Lsize == 0)
            return 0;
        cont_init();
        line = 1;
    } else if (Mode == BASIC_MODESHELL) {
        if (Lsize == 0)
            return 0;
        line = 1;
    }

    Gsize = Fsize = 0;
    bcode_start(Program);
    while (Lines[ line ].len) {
        Lidx = line;
        Lnum = Lines[ line ].num;
#ifdef DEBUG
        if (Lnum)
            DMSG("Executing line %d", Lnum)
        else
            DMSG("Executing COMMAND line")
#endif // DEBUG
        if (back)
            bcode_setpc(newpc);
        else
            bcode_setpc(Lines[ line ].add);
        jump = back = skip = 0;
        while (1) {
            bcode_read(&b);
            if (b.type == BCODE_TYPE_EOL)
                break;
            else if (b.inst == BCODE_COLON)
                continue;
            else if (b.type == BCODE_TYPE_KEYWORD) {
                switch (b.inst) {
                    case BCODE_END:
                        return(stat_end());

                    case BCODE_CLS:
                        stat_cls();
                        break;

                    case BCODE_DIM:
                        stat_dim();
                        break;

                    case BCODE_FOR:
                        deny_direct();
                        stat_for();
                        break;

                    case BCODE_GOSUB:
                        jump = stat_gosub();
                        break;

                    case BCODE_GOTO:
                        jump = stat_goto();
                        break;

                    case BCODE_IF:
                        skip = stat_if() ? 0 : 1;
                        break;

                    case BCODE_INPUT:
                        stat_input();
                        break;

                    case BCODE_NEXT:
                        deny_direct();
                        stat_next();
                        var = Fstack[ Fsize - 1 ].vidx;
                        Var[ var ]++;
                        if (Var[ var ] <= Fstack[ Fsize - 1 ].ecnt) {
                            line = Fstack[ Fsize - 1 ].lidx;
                            newpc = Fstack[ Fsize - 1 ].ladd;
                            back = 1;
                        } else {
                            Fsize--;
                            back = 0;
                        }
                        break;
#ifdef PI
                    case BCODE_OUT:
                        stat_out();
                        break;

                    case BCODE_OUTHZ:
                        stat_outhz();
                        break;
#endif // PI
                    case BCODE_PRINT:
                        stat_print();
                        break;

                    case BCODE_REM:
                        deny_direct();
                        stat_rem();
                        break;

                    case BCODE_RETURN:
                        deny_direct();
                        stat_return();
                        line = Gstack[ Gsize ].lidx;
                        newpc = Gstack[ Gsize ].radd;
                        back = 1;
                        break;

                    case BCODE_MSLEEP:
                        stat_msleep();
                        break;

                    case BCODE_USLEEP:
                        stat_usleep();
                        break;

                    default:
                        basic_error(BASIC_SYNTAXERROR);
                } // switch
            } else if (b.type == BCODE_TYPE_VARIABLE)
                eval_assign(b.idx + 1);
            else if (b.inst == BCODE_AT)
                eval_assign(0);
            else
                basic_error(BASIC_SYNTAXERROR);
            if (jump || back || skip)
                break;
        } // inner while
        if (jump) {
            line = prog_search(jump);
            if (line < 0)
                basic_error(PROG_LINENOTFOUND | BASIC_PROG_ERROR);
        } else if (back)
            continue;
        else {
            if (line == 0)  // Terminate DIRECT mode
                return 0;
            line++;
        }
    } // outer while
    return 0;
}

int
basic_command(int inst)
{
    switch (inst) {
        case BCODE_CLEAR:
            cmd_clear();
            break;

        case BCODE_CLS:
            stat_cls();
            break;

        case BCODE_DELETE:
            cmd_delete();
            break;

        case BCODE_DUMP:
            cmd_dump();
            break;

        case BCODE_FILES:
            cmd_files();
            break;

        case BCODE_FREE:
            cmd_free();
            break;

        case BCODE_HELP:
            cmd_help();
            break;

        case BCODE_LIST:
            cmd_list();
            break;

        case BCODE_LOAD:
            cmd_load();
            break;

        case BCODE_MERGE:
            cmd_merge();
            break;

        case BCODE_NEW:
            cmd_new();
            break;

        case BCODE_RENUM:
            cmd_renum();
            break;

        case BCODE_RUN:
            Mode = BASIC_MODERUN;
            basic_exec();
            break;

        case BCODE_SAVE:
            cmd_save();
            break;

        default:
            return 1;
    }
    return 0;
}

char *
basic_result(int code)
{
    return Results[ code ];
}
