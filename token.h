//
//  t o k e n . h
//
//  BAREMETALHACK.COM --> public domain
//

#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "byteword.h"           // word_t

//
// Token constants
//

#define TOKEN_MAXBUF            512     // Source text buffer capacity
#define TOKEN_MAXSTR            256     // Maximum length of string
#define TOKEN_MAXKEY            10      // Maximum length of keyword
#define TOKEN_MAXDLM            2       // Maximum length of delimiter

//
// Token types
//

#define TOKEN_TYPE_EOL          0       // End Of Line
#define TOKEN_TYPE_NUMBER10     1       // Decimal number
#define TOKEN_TYPE_NUMBER16     2       // Hexa-decimal number
#define TOKEN_TYPE_STRING       3       // String
#define TOKEN_TYPE_VARIABLE     4       // Variable or array name
#define TOKEN_TYPE_DELIMITER    5       // Delimiter
#define TOKEN_TYPE_KEYWORD      6       // Keyword

//
// Index codes for variable
//

#define TOKEN_VAR_A     0
#define TOKEN_VAR_B     1
#define TOKEN_VAR_C     2
#define TOKEN_VAR_D     3
#define TOKEN_VAR_E     4
#define TOKEN_VAR_F     5
#define TOKEN_VAR_G     6
#define TOKEN_VAR_H     7
#define TOKEN_VAR_I     8
#define TOKEN_VAR_J     9
#define TOKEN_VAR_K     10
#define TOKEN_VAR_L     11
#define TOKEN_VAR_M     12
#define TOKEN_VAR_N     13
#define TOKEN_VAR_O     14
#define TOKEN_VAR_P     15
#define TOKEN_VAR_Q     16
#define TOKEN_VAR_R     17
#define TOKEN_VAR_S     18
#define TOKEN_VAR_T     19
#define TOKEN_VAR_U     20
#define TOKEN_VAR_V     21
#define TOKEN_VAR_W     22
#define TOKEN_VAR_X     23
#define TOKEN_VAR_Y     24
#define TOKEN_VAR_Z     25

//
// Index codes for delimiter
//

#define TOKEN_EQUAL     0       // ==
#define TOKEN_ASSIGN    1       // =
#define TOKEN_GEQUAL    2       // >=
#define TOKEN_GTHAN     3       // >
#define TOKEN_LEQUAL    4       // <=
#define TOKEN_LTHAN     5       // <
#define TOKEN_NEQUAL    6       // !=
#define TOKEN_AT        7       // @
#define TOKEN_LBRACK    8       // [
#define TOKEN_RBRACK    9       // ]
#define TOKEN_LPAREN    10      // (
#define TOKEN_RPAREN    11      // )
#define TOKEN_COLON     12      // :
#define TOKEN_SCOLON    13      // ;
#define TOKEN_COMMA     14      // ,
#define TOKEN_MUL       15      // *
#define TOKEN_DIV       16      // /
#define TOKEN_MOD       17      // %
#define TOKEN_ADD       18      // +
#define TOKEN_SUB       19      // -
#define TOKEN_AND       20      // &
#define TOKEN_OR        21      // |
#define TOKEN_INQ       22      // ?

//
// Index codes for reserved keyword
//

#define TOKEN_CLEAR     0
#define TOKEN_CLS       1
#define TOKEN_COLOR     2
#define TOKEN_DELETE    3
#define TOKEN_DIM       4
#define TOKEN_DUMP      5
#define TOKEN_EDIT      6
#define TOKEN_END       7
#define TOKEN_FOR       8
#define TOKEN_FREE      9
#define TOKEN_GOSUB     10
#define TOKEN_GOTO      11
#define TOKEN_HELP      12
#define TOKEN_HEX2      13
#define TOKEN_HEX4      14
#define TOKEN_IF        15
#define TOKEN_INPUT     16
#define TOKEN_LIST      17
#define TOKEN_LOCATE    18
#define TOKEN_NEW       19
#define TOKEN_NEXT      20
#define TOKEN_PRINT     21
#define TOKEN_REM       22
#define TOKEN_RENUM     23
#define TOKEN_RETURN    24
#define TOKEN_RND       25
#define TOKEN_RUN       26
#define TOKEN_TO        27

// --- OS dependent section ---

#define TOKEN_FILES     28
#define TOKEN_LOAD      29
#define TOKEN_MERGE     30
#define TOKEN_MSLEEP    31
#define TOKEN_SAVE      32
#define TOKEN_USLEEP    33

#ifdef PI
#define TOKEN_IN        34
#define TOKEN_OUT       35
#define TOKEN_OUTHZ     36
#endif // PI

//
// Result codes
//

#define TOKEN_SUCCESS           0
#define TOKEN_MISSINGQUOTE      1
#define TOKEN_TOOLONGSTR        2
#define TOKEN_TOOLONGKEY        3
#define TOKEN_ILLEGALCHAR       4
#define TOKEN_ILLNUMBER         5
#define TOKEN_ILLSTRINRG        6
#define TOKEN_KEYNOTFOUND       7
#define TOKEN_DLMNOTFOUND       8
#define TOKEN_UNKNOWN           9

//
// Type definition
//

typedef struct {
    int type;
    union {
        int idx;
     word_t num;
       char str[ TOKEN_MAXSTR + 1 ]; 
    } body;
} token_t;

//
// Function prototypes
//

  void token_init(void);
  void token_source(char *);
   int token_read(token_t *);
char * token_keyword(int);
char * token_delimiter(int);
char * token_result(int);

#ifdef DEBUG
void token_dump(token_t *);
#endif // DEBUG

#endif // _TOKEN_H_
