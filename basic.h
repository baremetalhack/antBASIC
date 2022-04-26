//
//  b a s i c . h
//
//  BAREMETALHACK.COM --> public domain
//

#ifndef _BASIC_H_
#define _BASIC_H_

#include <setjmp.h>             // jmp_buf{}

//
// Program information
//

#ifdef PI
#define BASIC_NAME              "antBASIC for Pi"
#else
#define BASIC_NAME              "antBASIC for Unix"
#endif // PI

// Version number

#define BASIC_MAJOR             1
#define BASIC_MINOR             0
#define BASIC_PATCH             0

//
// BASIC constants
//

#define BASIC_MAXLINECHAR       512     // Maximum characters on command line
#define BASIC_MAXSTACKDEPTH     8       // Maximum depth of GOSUB/FOR stack
#define BASIC_LINESTART         100     // RENUM default start line number
#define BASIC_LINESTEP          10      // RENUM default line step

#define BASIC_MODERUN           0       // Normal execution mode (RUN)
#define BASIC_MODEDIRECT        1       // Direct execution from command line
#define BASIC_MODELOAD          2       // Program load mode (LOAD/MERGE)
#define BASIC_MODESHELL         4       // Execution from the shell

//
// Return codes
//

#define BASIC_SUCCESS           0
#define BASIC_DIRECTDENY        1
#define BASIC_DIRNOTFOUND       2
#define BASIC_FILEIOERROR       3
#define BASIC_ILLARRAY          4
#define BASIC_ILLRANGE          5
#define BASIC_NOARRAY           6
#define BASIC_NOASSIGN          7
#define BASIC_NOCONTROLVAR      8
#define BASIC_NODIRNAME         9
#define BASIC_NOLINENUM         10
#define BASIC_NOFILENAME        11
#define BASIC_NOSTACK           12
#define BASIC_NOVARIABLE        13
#define BASIC_STACKOVER         14
#define BASIC_SYNTAXERROR       15

// Error code group BIT

#define BASIC_BCODE_ERROR       0b00100000
#define BASIC_PROG_ERROR        0b01000000
#define BASIC_GPIO_ERROR        0b10000000

//
// Type definitions
//

typedef struct {
    int lidx;                   // Line index number
    int radd;                   // Return address
} gstack_t;

typedef struct {
    int lidx;                   // Line index number
    int ladd;                   // Looping entry address
    int vidx;                   // Variable index number
    int ecnt;                   // End count
} fstack_t;

//
// Global variable
//

extern int Lnum;                // Line number currently executing
extern int Mode;                // Current mode (NORMAL/DIRECT/LOAD/SHELL)
extern int Status;              // Exit status (in the case of SHELL mode)
extern char Text[];             // Source text line buffer

//
// Function prototype
//

  void basic_init(jmp_buf *);
  void basic_error(int);
   int basic_readline(char *, char *, int);
   int basic_command(int);
   int basic_load(char *);
   int basic_exec(void);
char * basic_result(int);

#endif  // _BASIC_H_
