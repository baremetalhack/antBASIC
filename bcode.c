//
//  b c o d e . c
//
//  BAREMETALHACK.COM --> public domain
//

#ifdef DEBUG                // bcode_dump()
#include <stdio.h>          // FILE{}, fprintf()
#endif

#include "bcode.h"

//
// Filescope variables
//

static byte_t *Code;        // points start of code area
static int Pc;              // program counter

//
// Bytecode functions
//

void
bcode_start(byte_t *ptr)
{
    Code = ptr;
    Pc = 0;
}

void
bcode_setpc(int where)
{
    Pc = where;
}

int
bcode_getpc(void)
{
    return Pc;
}

void
bcode_skip(void)
{
    bcode_t b;

    bcode_read(&b);
}

int
bcode_next(void)
{
    return (int) Code[ Pc ];
}

int
bcode_nextiseol(void)
{
    return Code[ Pc ] == BCODE_EOL ? 1 : 0;
}

int
bcode_nextisnum(void)
{
    if ((Code[ Pc ] == BCODE_NUMBER10) || (Code[ Pc ] == BCODE_NUMBER16))
        return 1;
    else
        return 0;
}

int
bcode_nextisstr(void)
{
    return Code[ Pc ] == BCODE_STRING ? 1 : 0;
}

int
bcode_nextisvar(void)
{
    if ((Code[ Pc ] >= BCODE_VARIABLE) && (Code[ Pc ] < BCODE_DELIMITER))
        return 1;
    else
        return 0;
}

int
bcode_nextisdlm(void)
{
    if ((Code[ Pc ] >= BCODE_DELIMITER) && (Code[ Pc ] < BCODE_KEYWORD))
        return 1;
    else
        return 0;
}

int
bcode_nextiskey(void)
{
    return Code[ Pc ] >= BCODE_KEYWORD ? 1 : 0;
}

int
bcode_twoahead(void)
{
    return (int) Code[ Pc + 1 ];
}

void
bcode_read(bcode_t *b)
{
    int len;
    word_t *ptr;

    b->pos = Pc;
    b->inst = Code[ Pc ];
    if (Code[ Pc ] < BCODE_VARIABLE)
        b->type = b->idx = Code[ Pc ];
    else if (Code[ Pc ] >= BCODE_VARIABLE && Code[ Pc ] < BCODE_DELIMITER) {
        b->type = BCODE_TYPE_VARIABLE;
        b->idx = Code[ Pc ] - BCODE_VARIABLE;
    } else if (Code[ Pc ] >= BCODE_DELIMITER && Code[ Pc ] < BCODE_KEYWORD) {
        b->type = BCODE_TYPE_DELIMITER;
        b->idx = Code[ Pc ] - BCODE_DELIMITER;
    } else {
        b->type = BCODE_TYPE_KEYWORD;
        b->idx = Code[ Pc ] - BCODE_KEYWORD;
    }
    b->num = 0;
    b->str = 0;
    Pc++;
    switch (b->type) {
        case BCODE_TYPE_NUMBER10:
        case BCODE_TYPE_NUMBER16:
            ptr = (word_t*) &Code[ Pc ];
            b->num = *ptr;
            Pc += 2;
            break;

        case BCODE_TYPE_STRING:
            b->idx = len = Code[ Pc++ ];
            b->str = &Code[ Pc ];
            Pc += len;
            break;
    }
}

int bcode_compile(byte_t *start) {
    byte_t *bptr, *lptr, *sptr;
    int len, ret;
    token_t tok;

    bptr = start;
    while (1) {
        ret = token_read(&tok);
        if (ret != TOKEN_SUCCESS)
            return -ret;
        switch (tok.type) {
            case TOKEN_TYPE_EOL:
                *bptr++ = BCODE_EOL;
                return bptr - start;

            case TOKEN_TYPE_NUMBER10:
                *bptr++ = BCODE_NUMBER10;
                *bptr++ = tok.body.num & 0xFF;
                *bptr++ = (tok.body.num >> 8) & 0xFF;
                break;

            case TOKEN_TYPE_NUMBER16:
                *bptr++ = BCODE_NUMBER16;
                *bptr++ = tok.body.num & 0xFF;
                *bptr++ = (tok.body.num >> 8) & 0xFF;
                break;

            case TOKEN_TYPE_STRING:
                *bptr++ = BCODE_STRING;
                lptr = bptr++;
                sptr = (byte_t *) tok.body.str;
                len = 0;
                while (*sptr) {
                    *bptr++ = *sptr++;
                    len++;
                }
                *bptr++ = 0;
                len++;
                *lptr = len;
                break;

            case TOKEN_TYPE_VARIABLE:
                *bptr++ = BCODE_VARIABLE + tok.body.idx;
                break;

            case TOKEN_TYPE_DELIMITER:
                *bptr++ = BCODE_DELIMITER + tok.body.idx;
                break;

            case TOKEN_TYPE_KEYWORD:
                *bptr++ = BCODE_KEYWORD + tok.body.idx;
                break;
        }
        if ((bptr - start + 3) > BCODE_MAXSIZE)
            return -BCODE_SIZEOVER;
    }
    return bptr - start;
}

char *
bcode_result(int idx)
{
    static char
        msg_ok[] = "BCODE \"ok\"",
        msg_sizeover[] = "BCODE \"size over\"";

    if (idx == BCODE_SUCCESS)
        return msg_ok;
    else if (idx == BCODE_SIZEOVER)
        return msg_sizeover;
    else
        return token_result(idx);
}

#ifdef DEBUG
extern FILE *Debug;

void
bcode_display(bcode_t *b)
{
    fprintf(Debug, "    0d%05d : ", b->pos);
    fprintf(Debug, "bcode 0x%02X : ", b->inst);
    fprintf(Debug, "type %1d : ", b->type);
    fprintf(Debug, "idx %2d | ", b->idx);
    if (b->type == BCODE_TYPE_NUMBER10)
        fprintf(Debug, "%d (0x%04hX)\n", b->num, b->num);
    else if (b->type == BCODE_TYPE_NUMBER16)
        fprintf(Debug, "0x%04hX (%d)\n", b->num, b->num);
    else if (b->type == BCODE_TYPE_STRING)
        fprintf(Debug, "\"%s\"\n", (char*) b->str);
    else if (b->type == BCODE_TYPE_VARIABLE)
        fprintf(Debug, "%c\n", 'A' + b->idx);
    else if (b->type == BCODE_TYPE_KEYWORD)
        fprintf(Debug, "%s\n", token_keyword(b->idx));
    else if (b->type == BCODE_TYPE_DELIMITER)
        fprintf(Debug, "%s\n", token_delimiter(b->idx));
    else if (b->type == BCODE_TYPE_EOL)
        fprintf(Debug, "EOL\n");
}

void
bcode_dump(byte_t *bytes)
{
    bcode_t b;

    fprintf(Debug, "[Compiled bytecodes]\n");
    bcode_start(bytes);
    while (1) {
        bcode_read(&b);
        bcode_display(&b);
        if (b.type == BCODE_TYPE_EOL)
            break;
    }
}
#endif
