//
//  e v a l . c
//
//  BAREMETALHACK.COM --> public domain
//

#include <setjmp.h>         // jmp_buf{}, longjmp()
#include "bcode.h"
#include "container.h"
#include "function.h"
#include "eval.h"
#include "debug.h"

//
// File scope variables
//

static jmp_buf *Exit;
static bcode_t Bcode;

static char * Results[] = {
    "OK",                   // EVAL_SUCCESS
    "illegal array",        // EVAL_ILLARRAY
    "illegal factor",       // EVAL_ILLFACTOR
    "illegal index",        // EVAL_ILLINDEX
    "index OVER",           // EVAL_INDEXOVER
    "no assignment",        // EVAL_NOASSIGN
    "no comma",             // EVAL_NOCOMMA
    "no left bracket",      // EVAL_NOLBRAK
    "no right bracket",     // EVAL_NOLBRAK
    "no left parenthesis",  // EVAL_NOLPAREN
    "no right parenthesis", // EVAL_NORPAREN
    "no string",            // EVAL_NOSTRING
    "divided by ZERO"       // EVAL_DIVBYZERO
};

//
// Error handler
//

void
eval_error(int code)
{
    longjmp(*Exit, code);
}

//
// Initializer
//

void
eval_init(jmp_buf *env)
{
    Exit = env;
}

//
// Dimension evaluator
//

void
eval_dim(dim_t *dim)
{
    word_t row = 0, col = 0;

    if (bcode_next() != BCODE_LBRACK)
        eval_error(EVAL_NOLBRACK);
    bcode_skip();
    row = eval();
    if (bcode_next() == BCODE_RBRACK) {
        bcode_skip();
        if (row < 0 || row > CONT_MAXARRAY)
            eval_error(EVAL_ILLINDEX);
        col = row;
        row = 0;
    } else if (bcode_next() == BCODE_COMMA) {
        bcode_skip();
        col = eval();
        if (col < 0 || col > CONT_MAXARRAY)
            eval_error(EVAL_ILLINDEX);
        if (bcode_next() != BCODE_RBRACK)
            eval_error(EVAL_NORBRACK);
        bcode_skip();
    } else
        eval_error(EVAL_ILLARRAY);
    dim->row = row;
    dim->col = col;
}

// Assign value to variable or array
//     var == 0 -> assign to string holder @[]
//     var != 0 -> assign to variable/array (index = var - 1) 

void
eval_assign(int var) {
    bcode_t b;
    byte_t *src, *dst;
    word_t val, idx;
    int array, row, col;
    dim_t dim;

    // String array (@[])

    if (var == 0) {
        if (bcode_next() == BCODE_ASSIGN) {
            bcode_skip();
            if (! bcode_nextisstr())
                eval_error(EVAL_NOSTRING);
            bcode_read(&b);
            src = b.str;
            dst = String;
            while (*src)
                *dst++ = *src++;
            *dst = 0;
            return;
        } else if (bcode_next() == BCODE_LBRACK) {
            bcode_skip();
            idx = eval();
            if (idx < 0 || idx >= CONT_MAXSTRING)
                eval_error(EVAL_ILLINDEX);
            if (bcode_next() != BCODE_RBRACK)
                eval_error(EVAL_NORBRACK);
            bcode_skip();

            if (bcode_next() != BCODE_ASSIGN)
                eval_error(EVAL_NOASSIGN);
            bcode_skip();

            val = eval();
            String[ idx ] = (byte_t) (val & 0xFF);
            DMSG("@[%d] = %d", idx, String[idx]);
            return;
        } else
            eval_error(EVAL_NOLBRACK);
    }

    // Variable or standard array

    var--;  // Recover right index
    if (bcode_next() == BCODE_LBRACK) {
        eval_dim(&dim);
        row = dim.row;
        col = dim.col;
        if (dim.row >= Asize[ var ].row || dim.col >= Asize[ var ].col)
            eval_error(EVAL_ILLINDEX);
        array = 1;
        DMSG("rowsiz = %d : colsiz = %d : row = %d : col = %d", \
                Asize[ var ].row, Asize[ var ].col, row, col);
    } else
        array = 0;

    if (bcode_next() != BCODE_ASSIGN)
        eval_error(EVAL_NOASSIGN);
    bcode_skip();

    // Variable assignment

    if (! array) {
        val = eval();
        Var[ var ] = val;
        DMSG("variable %c = %d", var+'A', val);
        return;
    }

    // Array assignment

    while (1) {
        val = eval();
        idx = Asize[ var ].col * row + col;
        Array[ var ][ idx ] = val;
        DMSG("array %c[%d,%d] = %c[%d] = %d", var+'A', row, col, var+'A', idx, val);
        if (bcode_next() == BCODE_COMMA) {
            bcode_skip();
            col++;
            if (col >= Asize[ var ].col) {
		col = 0;
		row++;
                if (row >= Asize[ var ].row)
                    eval_error(EVAL_INDEXOVER);
	    }
        } else
            break;
    }
    return;
}

//
// Recursive descent evaluator
//

static word_t
level_zero(void)
{
    word_t res = 0, v, idx;
    dim_t dim;

    if (Bcode.type == BCODE_TYPE_NUMBER10) {
        res = Bcode.num;
        DMSG("number %d", Bcode.num);
    } else if (Bcode.type == BCODE_TYPE_NUMBER16) {
        res = Bcode.num;
        DMSG("number 0x%04hX", Bcode.num);
    } else if (Bcode.inst == BCODE_LPAREN) {
        res = eval();
        if (bcode_next() != BCODE_RPAREN)
            eval_error(EVAL_NORPAREN);
        bcode_skip();
        DMSG("( %d )", res);
    } else if (Bcode.type == BCODE_TYPE_VARIABLE) {
        v = Bcode.idx;
        if (bcode_next() == BCODE_LBRACK) {
            eval_dim(&dim);
            if (dim.row >= Asize[ v ].row || dim.col >= Asize[ v ].col)
                eval_error(EVAL_ILLINDEX);
            idx = Asize[ v ].col * dim.row + dim.col;
            res = Array[ v ][ idx ];
            DMSG("array %c[%d,%d] = %c[%d] = %d",
                 v+'A', dim.row, dim.col, v+'A', idx, res);
        } else {
            res = Var[ v ];
            DMSG("variable %c = %d", v+'A', res);
        }
    } else if (Bcode.inst == BCODE_AT) {
        if (bcode_next() != BCODE_LBRACK)
            eval_error(EVAL_NOLBRACK);
        bcode_skip();
        idx = eval();
        if (bcode_next() != BCODE_RBRACK)
            eval_error(EVAL_NORBRACK);
        bcode_skip();
        res = (word_t) String[ idx ];
        DMSG("@[ %d ] = %d", idx, res);
    } else if (Bcode.inst == BCODE_RND) {
        if (bcode_next() != BCODE_LPAREN)
            eval_error(EVAL_NOLPAREN);
        bcode_skip();
        if (bcode_next() != BCODE_RPAREN)
            eval_error(EVAL_NORPAREN);
        bcode_skip();
        res = func_rnd(); 
        DMSG("rnd() = %d", res);
#ifdef PI
    } else if (Bcode.inst == BCODE_IN) {
        if (bcode_next() != BCODE_LPAREN)
            eval_error(EVAL_NOLPAREN);
        bcode_skip();
        idx = eval();
        if (bcode_next() != BCODE_RPAREN)
            eval_error(EVAL_NORPAREN);
        bcode_skip();
        res = func_in((int) idx);
        DMSG("in(%d) = %d", idx, res);
#endif // PI
    } else
        eval_error(EVAL_ILLFACTOR);
    return res;
}

static word_t
level_one(void)
{
    word_t unary, res;
    
    if (Bcode.inst == BCODE_SUB) {
        bcode_read(&Bcode);
        unary = level_zero();
        DMSG("-%d", unary);
        res = -unary;
    } else if (Bcode.inst == BCODE_ADD) {
        bcode_read(&Bcode);
        unary = level_zero();
        DMSG("+%d", unary);
        res = unary;
    } else
        res = level_zero();
    return res;
}

static word_t
level_two(void)
{
    int op;
    word_t first, second;
    
    first = level_one();
    while (1) {
        op = bcode_next();
        if (op == BCODE_MUL || op == BCODE_DIV || op == BCODE_MOD) {
            bcode_skip();
            bcode_read(&Bcode);
            second = level_one(); 
            switch (op) {
                case BCODE_MUL:
                    DMSG("%d * %d", first, second);
                    first = first * second;
                    break;

                case BCODE_DIV:
                    if (second == 0)
                        eval_error(EVAL_DIVBYZERO);
                    DMSG("%d / %d", first, second);
                    first = first / second;
                    break;

                case BCODE_MOD:
                    DMSG("%d %% %d", first, second);
                    first = first % second;
                    break;
            }
        } else
            break;
    }
    return first;
}

static word_t
level_three(void)
{
    int op;
    word_t first, second;

    first = level_two();
    while (1) {
        op = bcode_next();
        if (op == BCODE_ADD || op == BCODE_SUB) {
            bcode_skip();
            bcode_read(&Bcode);
            second = level_two(); 
            switch (op) {
                case BCODE_ADD:
                    DMSG("%d + %d", first, second);
                    first = first + second;
                    break;

                case BCODE_SUB:
                    DMSG("%d - %d", first, second);
                    first = first - second;
                    break;
            }
        } else
            break;
    }
    return first;
}

static word_t
level_four(void)
{
    int op;
    word_t first, second;

    first = level_three();
    while (1) {
        op = bcode_next();
        if (op == BCODE_EQUAL || op == BCODE_GEQUAL ||
            op == BCODE_GTHAN || op == BCODE_LEQUAL ||
            op == BCODE_LTHAN || op == BCODE_NEQUAL) {
            bcode_skip();
            bcode_read(&Bcode);
            second = level_three(); 
            switch (op) {
                case BCODE_EQUAL:
                    DMSG("%d == %d", first, second);
                    first = (first == second) ? 1 : 0;
                    break;

                case BCODE_GEQUAL:
                    DMSG("%d >= %d", first, second);
                    first = (first >= second) ? 1 : 0;
                    break;

                case BCODE_GTHAN:
                    DMSG("%d > %d", first, second);
                    first = (first > second) ? 1 : 0;
                    break;

                case BCODE_LEQUAL:
                    DMSG("%d <= %d", first, second);
                    first = (first <= second) ? 1 : 0;
                    break;

                case BCODE_LTHAN:
                    DMSG("%d < %d", first, second);
                    first = (first < second) ? 1 : 0;
                    break;

                case BCODE_NEQUAL:
                    DMSG("%d != %d", first, second);
                    first = (first != second) ? 1 : 0;
                    break;
            }
        } else
            break;
    }
    return first;
}

static word_t
level_five(void)
{
    int op;
    word_t first, second;

    first = level_four();
    while (1) {
        op = bcode_next();
        if (op == BCODE_AND || op == BCODE_OR) {
            bcode_skip();
            bcode_read(&Bcode);
            second = level_four(); 
            switch (op) {
                case BCODE_AND:
                    DMSG("%d & %d", first, second);
                    first = first & second;
                    break;

                case BCODE_OR:
                    DMSG("%d | %d", first, second);
                    first = first | second;
                    break;
            }
        } else
            break;
    }
    return first;
}

word_t eval(void) {
    word_t res;

    bcode_read(&Bcode);
    if (Bcode.inst == BCODE_EOL)
        return 0;
    res = level_five();
    DMSG("--> %d (0x%04X)", res, res);
    return res;
}

char *
eval_result(int idx)
{
    return Results[ idx ];
}
