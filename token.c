//
//  t o k e n . c
//
//  BAREMETALHACK.COM --> public domain
//

#ifdef DEBUG            // token_dump()
#include <stdio.h>      // FILE{}, fprintf()
#include <string.h>     // strlen()
#endif

#include "token.h"
#include "debug.h"

//
// Constants for character type
//

#define CT_EOL              1       // end of line: NULL | '\n'
#define CT_SPACE            2       // ' ' | '\t'
#define CT_DELIMITER        4       // !()*+,-/:;<=>@[]&|?
#define CT_ALPHABET         8       // A-Z | a-z
#define CT_BINARY           16      // 0-1
#define CT_DECIMAL          32      // 0-9
#define CT_HEXADECIMAL      64      // 0-9 | A-F | a-f

//
// Local type definition
//

typedef struct {
    char * string;
       int index;
} table_t;

//
// File scope variables
//

static unsigned char Ctype[ 256 ];
static char *Srcptr;
static char Next;

static table_t
Keywords[] = {
    { "CLEAR",  TOKEN_CLEAR },
    { "CLS",    TOKEN_CLS },
    { "COLOR",  TOKEN_COLOR },
    { "DELETE", TOKEN_DELETE },
    { "DIM",    TOKEN_DIM },
    { "DUMP",   TOKEN_DUMP },
    { "EDIT",   TOKEN_EDIT },
    { "END",    TOKEN_END },
    { "FOR",    TOKEN_FOR },
    { "FREE",   TOKEN_FREE },
    { "GOSUB",  TOKEN_GOSUB },
    { "GOTO",   TOKEN_GOTO },
    { "HELP",   TOKEN_HELP },
    { "HEX2",   TOKEN_HEX2 },
    { "HEX4",   TOKEN_HEX4 },
    { "IF",     TOKEN_IF },
    { "INPUT",  TOKEN_INPUT },
    { "LIST",   TOKEN_LIST },
    { "LOCATE", TOKEN_LOCATE },
    { "NEW",    TOKEN_NEW },
    { "NEXT",   TOKEN_NEXT },
    { "PRINT",  TOKEN_PRINT },
    { "REM",    TOKEN_REM },
    { "RENUM",  TOKEN_RENUM },
    { "RETURN", TOKEN_RETURN },
    { "RND",    TOKEN_RND },
    { "RUN",    TOKEN_RUN },
    { "TO",     TOKEN_TO },

    { "FILES",  TOKEN_FILES },
    { "LOAD",   TOKEN_LOAD },
    { "MERGE",  TOKEN_MERGE },
    { "MSLEEP", TOKEN_MSLEEP },
    { "SAVE",   TOKEN_SAVE },
    { "USLEEP", TOKEN_USLEEP },

#ifdef PI
    { "IN",     TOKEN_IN },
    { "OUT",    TOKEN_OUT },
    { "OUTHZ",  TOKEN_OUTHZ },
#endif // PI
    { 0, 0 }                        // table end mark
};

static table_t
Delimiters[] = {
    { "==", TOKEN_EQUAL },
    { "=",  TOKEN_ASSIGN },
    { ">=", TOKEN_GEQUAL },
    { ">",  TOKEN_GTHAN },
    { "<=", TOKEN_LEQUAL },
    { "<",  TOKEN_LTHAN },
    { "!=", TOKEN_NEQUAL },
    { "@",  TOKEN_AT },
    { "[",  TOKEN_LBRACK },
    { "]",  TOKEN_RBRACK },
    { "(",  TOKEN_LPAREN },
    { ")",  TOKEN_RPAREN },
    { ":",  TOKEN_COLON },
    { ";",  TOKEN_SCOLON },
    { ",",  TOKEN_COMMA },
    { "*",  TOKEN_MUL },
    { "/",  TOKEN_DIV },
    { "%",  TOKEN_MOD },
    { "+",  TOKEN_ADD },
    { "-",  TOKEN_SUB },
    { "&",  TOKEN_AND },
    { "|",  TOKEN_OR },
    { "?",  TOKEN_INQ },
    { 0, 0 }                        // table end mark
};

static char *
Results[] = {
    "TOKEN \"ok\"",                     // TOKEN_SUCCESS
    "TOKEN \"missing quotation\"",      // TOKEN_MISSINGQUOTE
    "TOKEN \"too long string\"",        // TOKEN_TOOLONGSTR
    "TOKEN \"too long keyword\"",       // TOKEN_TOOLONGKEY
    "TOKEN \"illegal character\"",      // TOKEN_ILLCHAR
    "TOKEN \"illegal number\"",         // TOKEN_ILLNUMBER
    "TOKEN \"illegal string\"",         // TOKEN_ILLSTRING
    "TOKEN \"keyword not found\"",      // TOKEN_KEYNOTFOUND
    "TOKEN \"delimiter not found\"",    // TOKEN_DLMNOTFOUND
    "TOKEN \"unknown token\""           // TOKEN_UNKNOWN
};

//
// Procedures and functions
//

void
token_init(void)
{
    int i;

    for (i = 0; i < 256; i++) Ctype[ i ] = 0;

    Ctype[ 0 ] = Ctype[ '\n' ] = CT_EOL;

    Ctype[ ' ' ] = Ctype[ '\t' ] = CT_SPACE;

    Ctype[ '!' ] = Ctype[ '%' ] = Ctype[ '&' ] = Ctype[ '(' ] = \
    Ctype[ ')' ] = Ctype[ '*' ] = Ctype[ '+' ] = Ctype[ ',' ] = \
    Ctype[ '-' ] = Ctype[ '/' ] = Ctype[ ':' ] = Ctype[ ';' ] = \
    Ctype[ '<' ] = Ctype[ '=' ] = Ctype[ '>' ] = Ctype[ '@' ] = \
    Ctype[ '[' ] = Ctype[ ']' ] = Ctype[ '?' ] = Ctype[ '|' ] = CT_DELIMITER;

    Ctype[ 'A' ] = Ctype[ 'B' ] = Ctype[ 'C' ] = Ctype[ 'D' ] = \
    Ctype[ 'E' ] = Ctype[ 'F' ] = Ctype[ 'G' ] = Ctype[ 'H' ] = \
    Ctype[ 'I' ] = Ctype[ 'J' ] = Ctype[ 'K' ] = Ctype[ 'L' ] = \
    Ctype[ 'M' ] = Ctype[ 'N' ] = Ctype[ 'O' ] = Ctype[ 'P' ] = \
    Ctype[ 'Q' ] = Ctype[ 'R' ] = Ctype[ 'S' ] = Ctype[ 'T' ] = \
    Ctype[ 'U' ] = Ctype[ 'V' ] = Ctype[ 'W' ] = Ctype[ 'X' ] = \
    Ctype[ 'Y' ] = Ctype[ 'Z' ] = CT_ALPHABET;

    Ctype[ 'a' ] = Ctype[ 'b' ] = Ctype[ 'c' ] = Ctype[ 'd' ] = \
    Ctype[ 'e' ] = Ctype[ 'f' ] = Ctype[ 'g' ] = Ctype[ 'h' ] = \
    Ctype[ 'i' ] = Ctype[ 'j' ] = Ctype[ 'k' ] = Ctype[ 'l' ] = \
    Ctype[ 'm' ] = Ctype[ 'n' ] = Ctype[ 'o' ] = Ctype[ 'p' ] = \
    Ctype[ 'q' ] = Ctype[ 'r' ] = Ctype[ 's' ] = Ctype[ 't' ] = \
    Ctype[ 'u' ] = Ctype[ 'v' ] = Ctype[ 'w' ] = Ctype[ 'x' ] = \
    Ctype[ 'y' ] = Ctype[ 'z' ] = CT_ALPHABET;

    Ctype[ '0' ] = Ctype[ '1' ] = Ctype[ '2' ] = Ctype[ '3' ] = \
    Ctype[ '4' ] = Ctype[ '5' ] = Ctype[ '6' ] = Ctype[ '7' ] = \
    Ctype[ '8' ] = Ctype[ '9' ] = CT_DECIMAL;

    Ctype[ '0' ] |= Ctype[ '1' ] |= CT_BINARY;

    Ctype[ '0' ] |= Ctype[ '1' ] |= Ctype[ '2' ] |= Ctype[ '3' ] |= \
    Ctype[ '4' ] |= Ctype[ '5' ] |= Ctype[ '6' ] |= Ctype[ '7' ] |= \
    Ctype[ '8' ] |= Ctype[ '9' ] |= CT_HEXADECIMAL;

    Ctype[ 'A' ] |= Ctype[ 'B' ] |= Ctype[ 'C' ] |= Ctype[ 'D' ] |= \
    Ctype[ 'E' ] |= Ctype[ 'F' ] |= CT_HEXADECIMAL;

    Ctype[ 'a' ] |= Ctype[ 'b' ] |= Ctype[ 'c' ] |= Ctype[ 'd' ] |= \
    Ctype[ 'e' ] |= Ctype[ 'f' ] |= CT_HEXADECIMAL;
}

static void
read_char(void)
{
    Next = *Srcptr++;
    return;
}

void
token_source(char *ptr)
{
    Srcptr = ptr;
    read_char();        // do pre-fetch first
    return;
}

static int
check_digit(int base)
{
    if (base == 16) {
        if (Ctype[ (int) Next ] & CT_HEXADECIMAL) {
            if (Next >= 'a') 
                return (Next - 'a' + 10);
            else if (Next >= 'A')
                return (Next - 'A' + 10);
            else 
                return (Next - '0');
        } else
            return -1;
    } else if (base == 10) {
        if (Ctype[ (int) Next ] & CT_DECIMAL)
            return (Next - '0');
        else
            return -1;
    } else if (base == 2) {
        if (Ctype[ (int) Next ] & CT_BINARY)
            return (Next - '0');
        else
            return -1;
    } else
        return -1;
}

static word_t
number(token_t *tok)
{
    word_t base = 10, digit, val = 0;

    if (Next == '0') {
        read_char();
        if (Next == 'x' || Next == 'X') {
            base = 16;
            read_char();
        } else
            base = 10;
    }
    while ((digit = check_digit(base)) >= 0) {
        val = (val * base) + digit;
        read_char();
    };
   
    if (base == 16)
        tok->type = TOKEN_TYPE_NUMBER16;
    else
        tok->type = TOKEN_TYPE_NUMBER10;
    tok->body.num = val;
    return TOKEN_SUCCESS;
}
    
static int
string(token_t *tok)
{
    int cnt, high, low;
    char *str;

    str = tok->body.str;
    cnt = 0;
    read_char();
    while (Next && Next != '\"' && cnt < TOKEN_MAXSTR) {
        if (Next == '\\') {
            read_char();
            switch(Next) {
                case 'a':
                    Next = '\007';
                    break;

                case 'b':
                    Next = '\010';
                    break;

                case 't':
                    Next = '\011';
                    break;

                case 'n':
                    Next = '\012';
                    break;

                case 'r':
                    Next = '\015';
                    break;

                case 'e':
                    Next = '\033';
                    break;

                case 'x':
                    read_char();
                    if (! (Ctype[ (int) Next ] & CT_HEXADECIMAL))
                        return TOKEN_ILLNUMBER;
                    high = check_digit(16);
                    read_char();
                    if (! (Ctype[ (int) Next ] & CT_HEXADECIMAL))
                        return TOKEN_ILLNUMBER;
                    low = check_digit(16);
                    Next = high * 16 + low;;
                    break;

                case '\\':
                    Next = '\\';
                    break;

                default:                // unknown character
                    Next = '?';
                    break;
            }
        }
        *str++ = Next;
        cnt++;
        read_char();
    }
    *str = 0;

    if (Next == '\"')
        read_char();
    else if (cnt == TOKEN_MAXSTR)
        return TOKEN_TOOLONGSTR;
    else
        return TOKEN_MISSINGQUOTE;

    tok->type = TOKEN_TYPE_STRING;
    return TOKEN_SUCCESS;
}

static int
delimiter(token_t *tok)
{
    int idx;

    switch(Next) {
        case '=':
            read_char();
            if (Next == '=') {
                read_char();
                idx = TOKEN_EQUAL;
            } else
                idx = TOKEN_ASSIGN;
            break;

        case '>':
            read_char();
            if (Next == '=') {
                read_char();
                idx = TOKEN_GEQUAL;
            } else
                idx = TOKEN_GTHAN;
            break;

        case '<':
            read_char();
            if (Next == '=') {
                read_char();
                idx = TOKEN_LEQUAL;
            } else
                idx = TOKEN_LTHAN;
            break;

        case '!':
            read_char();
            if (Next == '=') {
                read_char();
                idx = TOKEN_NEQUAL;
            } else
                return TOKEN_UNKNOWN;
            break;

        case '@':
            read_char();
            idx = TOKEN_AT;
            break;

        case '[':
            read_char();
            idx = TOKEN_LBRACK;
            break;

        case ']':
            read_char();
            idx = TOKEN_RBRACK;
            break;

        case '(':
            read_char();
            idx = TOKEN_LPAREN;
            break;

        case ')':
            read_char();
            idx = TOKEN_RPAREN;
            break;

        case ':':
            read_char();
            idx = TOKEN_COLON;
            break;

        case ';':
            read_char();
            idx = TOKEN_SCOLON;
            break;

        case ',':
            read_char();
            idx = TOKEN_COMMA;
            break;

        case '*':
            read_char();
            idx = TOKEN_MUL;
            break;

        case '/':
            read_char();
            idx = TOKEN_DIV;
            break;

        case '%':
            read_char();
            idx = TOKEN_MOD;
            break;

        case '+':
            read_char();
            idx = TOKEN_ADD;
            break;

        case '-':
            read_char();
            idx = TOKEN_SUB;
            break;
        case '&':
            read_char();
            idx = TOKEN_AND;
            break;

        case '|':
            read_char();
            idx = TOKEN_OR;
            break;

        case '?':
            read_char();
            idx = TOKEN_INQ;
            break;
    }
    tok->type = TOKEN_TYPE_DELIMITER;
    tok->body.idx = idx;
    return TOKEN_SUCCESS;
}

static int
search_table(table_t *tbl, char *key)
{
    int idx = 0;
    char *src, *dst;

    while (tbl->string) {
        src = key;
        dst = tbl->string;
        while (*src && *dst) {
            if (*dst != *src)
                break;
            src++;
            dst++;
        }
        if ((*src == 0) && (*dst == 0))
            return idx;
        idx++;
        tbl++;
    }
    return -1;
}

static int keyword(token_t *tok)
{
    int cnt, idx;
    char key[ TOKEN_MAXKEY + 1 ], *ptr;

    ptr = key;
    cnt = 0;
    while (Ctype[ (int) Next ] & CT_ALPHABET || \
           Ctype[ (int) Next ] & CT_DECIMAL) {
        if (Next >= 'a' && Next <= 'z')
            Next -= 0x20;
        cnt++;
        if (cnt == (TOKEN_MAXKEY + 1))
            return TOKEN_TOOLONGKEY;
        *ptr++ = Next;
        read_char();
    }
    *ptr = 0;

    if (cnt == 1) {
        tok->type = TOKEN_TYPE_VARIABLE;
        tok->body.idx = key[ 0 ] - 'A';
        return TOKEN_SUCCESS;
    }

    idx = search_table(Keywords, key);
    if (idx < 0)
        return TOKEN_KEYNOTFOUND;
    
    tok->type = TOKEN_TYPE_KEYWORD;
    tok->body.idx = Keywords[ idx ].index;
    return TOKEN_SUCCESS;
}

int
token_read(token_t *tok)
{
    while (Ctype[ (int) Next ] & CT_SPACE)      // Skip blank
        read_char();
    if (Ctype[ (int) Next ] & CT_EOL) {
        tok->type = TOKEN_TYPE_EOL;
        return TOKEN_SUCCESS;
    }

    if (Ctype[ (int) Next ] & CT_DECIMAL)
        return number(tok);
    else if (Ctype[ (int) Next ] & CT_ALPHABET) 
        return keyword(tok);
    else if (Ctype[ (int) Next ] & CT_DELIMITER)
        return delimiter(tok);
    else if (Next == '\"')
        return string(tok);
    else
        return TOKEN_UNKNOWN;
}

char *
token_keyword(int idx)
{
    return Keywords[ idx ].string;
}

char *
token_delimiter(int idx)
{
    return Delimiters[ idx ].string;
}

char *
token_result(int idx)
{
    return Results[ idx ];
}

#ifdef DEBUG
void
token_dump(token_t *tok)
{
    int i, len;

    fprintf(Debug, "    token type %d : ", tok->type);
    switch (tok->type) {
        case TOKEN_TYPE_EOL:
            fprintf(Debug, "EOL\n");
            break;

        case TOKEN_TYPE_NUMBER10:
            fprintf(Debug, "number %d (0x%0hX)\n", tok->body.num, tok->body.num);
            break;

        case TOKEN_TYPE_NUMBER16:
            fprintf(Debug, "number 0x%0hX (%d)\n", tok->body.num, tok->body.num);
            break;

        case TOKEN_TYPE_STRING:
            len = strlen(tok->body.str);
            fprintf(Debug, "string (strlen = %d)\n", len);
            for (i = 0; i < (len + 1); i++) {
                if ((i % 16) == 0)
                    fprintf(Debug, "        ");
                fprintf(Debug, "%02hhX ", (unsigned char) tok->body.str[ i ]);
                if ((i % 16) == 15)
                    fprintf(Debug, "\n");
            }
            if ((i % 16) != 15)
                fprintf(Debug, "\n");
            break;

        case TOKEN_TYPE_VARIABLE:
            fprintf(Debug, "variable %c\n", tok->body.idx + 'A');
            break;

        case TOKEN_TYPE_KEYWORD:
            fprintf(Debug, "keyword \"%s\"\n", Keywords[ tok->body.idx ].string);
            break;

        case TOKEN_TYPE_DELIMITER:
            fprintf(Debug, "delimiter \"%s\"\n", Delimiters[ tok->body.idx ].string);
            break;
    }
}
#endif // DEBUG
