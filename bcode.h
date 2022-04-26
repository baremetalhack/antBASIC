//
//  b c o d e . h
//
//  BAREMETALHACK.COM --> public domain
//

#ifndef _BCODE_H_
#define _BCODE_H_

#include "token.h"

//
// Bytecode constants
//

#define BCODE_MAXSIZE           512     // Maximum byte size in single line

//-------------------------------------------------
//
// Bytecode instruction codes
//
//   Type 0       EOL: 0x00
//   Type 1  Number10: 0x01, low, high
//   Type 2  Number16: 0x02, low, high
//   Type 3    String: 0x03, len (includes null terminator), ch1, ch2, ..., 0
//   Type 4  Variable: 0x10 - 0x29
//   Type 5 Delimiter: 0x40 - 0x56
//   Type 6   Keyword: 0x80 - 0xA1
//
//-------------------------------------------------

#define BCODE_TYPE_EOL          TOKEN_TYPE_EOL
#define BCODE_TYPE_NUMBER10     TOKEN_TYPE_NUMBER10
#define BCODE_TYPE_NUMBER16     TOKEN_TYPE_NUMBER16
#define BCODE_TYPE_STRING       TOKEN_TYPE_STRING
#define BCODE_TYPE_VARIABLE     TOKEN_TYPE_VARIABLE
#define BCODE_TYPE_DELIMITER    TOKEN_TYPE_DELIMITER
#define BCODE_TYPE_KEYWORD      TOKEN_TYPE_KEYWORD

#define BCODE_EOL               BCODE_TYPE_EOL
#define BCODE_NUMBER10          BCODE_TYPE_NUMBER10
#define BCODE_NUMBER16          BCODE_TYPE_NUMBER16
#define BCODE_STRING            BCODE_TYPE_STRING
#define BCODE_VARIABLE          0x10
#define BCODE_DELIMITER         0x40
#define BCODE_KEYWORD           0x80

#define BCODE_VAR_A             (BCODE_VAIRABLE | ('A' - 'A'))
#define BCODE_VAR_B             (BCODE_VAIRABLE | ('B' - 'A'))
#define BCODE_VAR_C             (BCODE_VAIRABLE | ('C' - 'A'))
#define BCODE_VAR_D             (BCODE_VAIRABLE | ('D' - 'A'))
#define BCODE_VAR_E             (BCODE_VAIRABLE | ('E' - 'A'))
#define BCODE_VAR_F             (BCODE_VAIRABLE | ('F' - 'A'))
#define BCODE_VAR_G             (BCODE_VAIRABLE | ('G' - 'A'))
#define BCODE_VAR_H             (BCODE_VAIRABLE | ('H' - 'A'))
#define BCODE_VAR_I             (BCODE_VAIRABLE | ('I' - 'A'))
#define BCODE_VAR_J             (BCODE_VAIRABLE | ('J' - 'A'))
#define BCODE_VAR_K             (BCODE_VAIRABLE | ('K' - 'A'))
#define BCODE_VAR_L             (BCODE_VAIRABLE | ('L' - 'A'))
#define BCODE_VAR_M             (BCODE_VAIRABLE | ('M' - 'A'))
#define BCODE_VAR_N             (BCODE_VAIRABLE | ('N' - 'A'))
#define BCODE_VAR_O             (BCODE_VAIRABLE | ('O' - 'A'))
#define BCODE_VAR_P             (BCODE_VAIRABLE | ('P' - 'A'))
#define BCODE_VAR_Q             (BCODE_VAIRABLE | ('Q' - 'A'))
#define BCODE_VAR_R             (BCODE_VAIRABLE | ('R' - 'A'))
#define BCODE_VAR_S             (BCODE_VAIRABLE | ('S' - 'A'))
#define BCODE_VAR_T             (BCODE_VAIRABLE | ('T' - 'A'))
#define BCODE_VAR_U             (BCODE_VAIRABLE | ('U' - 'A'))
#define BCODE_VAR_V             (BCODE_VAIRABLE | ('V' - 'A'))
#define BCODE_VAR_W             (BCODE_VAIRABLE | ('W' - 'A'))
#define BCODE_VAR_X             (BCODE_VAIRABLE | ('X' - 'A'))
#define BCODE_VAR_Y             (BCODE_VAIRABLE | ('Y' - 'A'))
#define BCODE_VAR_Z             (BCODE_VAIRABLE | ('Z' - 'A'))

#define BCODE_EQUAL             (BCODE_DELIMITER | TOKEN_EQUAL)
#define BCODE_ASSIGN            (BCODE_DELIMITER | TOKEN_ASSIGN)
#define BCODE_GEQUAL            (BCODE_DELIMITER | TOKEN_GEQUAL)
#define BCODE_GTHAN             (BCODE_DELIMITER | TOKEN_GTHAN)
#define BCODE_LEQUAL            (BCODE_DELIMITER | TOKEN_LEQUAL)
#define BCODE_LTHAN             (BCODE_DELIMITER | TOKEN_LTHAN)
#define BCODE_NEQUAL            (BCODE_DELIMITER | TOKEN_NEQUAL)
#define BCODE_EXCL              (BCODE_DELIMITER | TOKEN_EXCL)
#define BCODE_AT                (BCODE_DELIMITER | TOKEN_AT)
#define BCODE_LBRACK            (BCODE_DELIMITER | TOKEN_LBRACK)
#define BCODE_RBRACK            (BCODE_DELIMITER | TOKEN_RBRACK)
#define BCODE_LPAREN            (BCODE_DELIMITER | TOKEN_LPAREN)
#define BCODE_RPAREN            (BCODE_DELIMITER | TOKEN_RPAREN)
#define BCODE_COLON             (BCODE_DELIMITER | TOKEN_COLON)
#define BCODE_SCOLON            (BCODE_DELIMITER | TOKEN_SCOLON)
#define BCODE_COMMA             (BCODE_DELIMITER | TOKEN_COMMA)
#define BCODE_MUL               (BCODE_DELIMITER | TOKEN_MUL)
#define BCODE_DIV               (BCODE_DELIMITER | TOKEN_DIV)
#define BCODE_MOD               (BCODE_DELIMITER | TOKEN_MOD)
#define BCODE_ADD               (BCODE_DELIMITER | TOKEN_ADD)
#define BCODE_SUB               (BCODE_DELIMITER | TOKEN_SUB)
#define BCODE_AND               (BCODE_DELIMITER | TOKEN_AND)
#define BCODE_OR                (BCODE_DELIMITER | TOKEN_OR)
#define BCODE_INQ               (BCODE_DELIMITER | TOKEN_INQ)

#define BCODE_CLEAR             (BCODE_KEYWORD | TOKEN_CLEAR)
#define BCODE_CLS               (BCODE_KEYWORD | TOKEN_CLS)
#define BCODE_DELETE            (BCODE_KEYWORD | TOKEN_DELETE)
#define BCODE_DIM               (BCODE_KEYWORD | TOKEN_DIM)
#define BCODE_DUMP              (BCODE_KEYWORD | TOKEN_DUMP)
#define BCODE_END               (BCODE_KEYWORD | TOKEN_END)
#define BCODE_FOR               (BCODE_KEYWORD | TOKEN_FOR)
#define BCODE_FREE              (BCODE_KEYWORD | TOKEN_FREE)
#define BCODE_GOSUB             (BCODE_KEYWORD | TOKEN_GOSUB)
#define BCODE_GOTO              (BCODE_KEYWORD | TOKEN_GOTO)
#define BCODE_HELP              (BCODE_KEYWORD | TOKEN_HELP)
#define BCODE_HEX2              (BCODE_KEYWORD | TOKEN_HEX2)
#define BCODE_HEX4              (BCODE_KEYWORD | TOKEN_HEX4)
#define BCODE_IF                (BCODE_KEYWORD | TOKEN_IF)
#define BCODE_INPUT             (BCODE_KEYWORD | TOKEN_INPUT)
#define BCODE_LIST              (BCODE_KEYWORD | TOKEN_LIST)
#define BCODE_NEW               (BCODE_KEYWORD | TOKEN_NEW)
#define BCODE_NEXT              (BCODE_KEYWORD | TOKEN_NEXT)
#define BCODE_PRINT             (BCODE_KEYWORD | TOKEN_PRINT)
#define BCODE_REM               (BCODE_KEYWORD | TOKEN_REM)
#define BCODE_RENUM             (BCODE_KEYWORD | TOKEN_RENUM)
#define BCODE_RETURN            (BCODE_KEYWORD | TOKEN_RETURN)
#define BCODE_RND               (BCODE_KEYWORD | TOKEN_RND)
#define BCODE_RUN               (BCODE_KEYWORD | TOKEN_RUN)
#define BCODE_TO                (BCODE_KEYWORD | TOKEN_TO)

#define BCODE_FILES             (BCODE_KEYWORD | TOKEN_FILES)
#define BCODE_LOAD              (BCODE_KEYWORD | TOKEN_LOAD)
#define BCODE_MERGE             (BCODE_KEYWORD | TOKEN_MERGE)
#define BCODE_MSLEEP            (BCODE_KEYWORD | TOKEN_MSLEEP)
#define BCODE_SAVE              (BCODE_KEYWORD | TOKEN_SAVE)
#define BCODE_USLEEP            (BCODE_KEYWORD | TOKEN_USLEEP)

#ifdef PI
#define BCODE_IN                (BCODE_KEYWORD | TOKEN_IN)
#define BCODE_OUT               (BCODE_KEYWORD | TOKEN_OUT)
#define BCODE_OUTHZ             (BCODE_KEYWORD | TOKEN_OUTHZ)
#endif // PI

//
// Result codes
//

#define BCODE_SUCCESS           TOKEN_SUCCESS
#define BOCDE_MISSINGQUOTE      TOKEN_MISSINGQUOTE
#define BCODE_TOOLONGSTR        TOKEN_TOOLONGSTR
#define BCODE_TOOLONGKEY        TOKEN_TOOLONGKEY
#define BCODE_ILLEGALCHAR       TOKEN_ILLEGALCHAR
#define BCODE_ILLNUMBER         TOKEN_ILLNUMBER
#define BCODE_ILLSTRINRG        TOKEN_ILLSTRING
#define BCODE_KEYNOTFOUND       TOKEN_KEYNOTFOUND
#define BCODE_DLMNOTFOUND       TOKEN_DLMNOTFOUND
#define BCODE_UNKNOWN           TOKEN_UNKNOWN
#define BCODE_SIZEOVER          (BCODE_UNKNOWN + 1)

//
// Type definition
//

typedef struct {
      int pos;
      int inst;
      int type;
      int idx;
   word_t num;
 byte_t * str;
} bcode_t;

//
// Function prototypes
//

   void bcode_start(byte_t *);
   void bcode_setpc(int);
    int bcode_getpc(void);
   void bcode_skip(void);
    int bcode_next(void);
    int bcode_nextiseol(void);
    int bcode_nextisnum(void);
    int bcode_nextisstr(void);
    int bcode_nextisvar(void);
    int bcode_nextiskey(void);
    int bcode_nextisdlm(void);
    int bcode_twoahead(void);
   void bcode_read(bcode_t *);
    int bcode_compile(byte_t *);
 char * bcode_result(int);

#ifdef DEBUG
   void bcode_display(bcode_t *);
   void bcode_dump(byte_t *);
#endif // DEBUG

#endif // _BCODE_H_
