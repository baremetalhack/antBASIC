//
//  p r o g r a m . h
//
//  BAREMETALHACK.COM --> public domain
//

#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <stdio.h>               // FILE{}
#include "bcode.h"

//
// Program constants
//

#define PROG_MAXSIZE        30000
#define PROG_MAXLINE        2000
#define PROG_MAXRELOC       100
#define PROG_BLANKLINE      0x7FFF
#define PROG_BLANKDATA      0xFF
#define PROG_CMDLINE        PROG_MAXSIZE

#define PROG_LISTCOLOR      1
#define PROG_LISTPLAIN      0

//
// Result codes
//

#define PROG_SUCCESS        0
#define PROG_LINENOTFOUND   1
#define PROG_TOOMANYLINES   2
#define PROG_SIZEOVER       3
#define PROG_TOOMANYRELOCS  4

//
// Type definitions
//

typedef struct {
    int num;
    int add;
    int len;
} line_t;

typedef struct {
    int idx;
    int add;
} reloc_t;

//
// Global variables
//

extern byte_t Program[];
extern int Psize;
extern line_t Lines[];
extern int Lsize;
extern int Pnum;

//
// Function prototypes
//

  void prog_init(void);
   int prog_list(int, int, int, int);
   int prog_search(int);
   int prog_delete(int, int);
   int prog_insert(int, byte_t *, int);
  void prog_cmdline(byte_t *, int);
   int prog_renum(int, int);
char * prog_result(int);
  void prog_dumpbytes(void);
  void prog_dumplines(void);

#endif // _PROGRAM_H_
