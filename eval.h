//
//  e v a l . h
//
//  BAREMETALHACK.COM --> public domain
//

#ifndef _EVAL_H_
#define _EVAL_H_

#include <setjmp.h>         // jmp_buf{}
#include "container.h"      // dim_t{}

//
// Result codes
//

#define EVAL_SUCCESS        0
#define EVAL_ILLARRAY       1
#define EVAL_ILLFACTOR      2
#define EVAL_ILLINDEX       3
#define EVAL_INDEXOVER      4
#define EVAL_NOASSIGN       5
#define EVAL_NOCOMMA        6
#define EVAL_NOLBRACK       7
#define EVAL_NORBRACK       8
#define EVAL_NOLPAREN       9
#define EVAL_NORPAREN       10
#define EVAL_NOSTRING       11
#define EVAL_DIVBYZERO      12

//
// Function prototype
//

  void eval_init(jmp_buf *);
  void eval_dim(dim_t *);
  void eval_assign(int);
word_t eval(void);
char * eval_result(int);

#endif // _EVAL_H_
