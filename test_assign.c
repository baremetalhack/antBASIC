#include <stdio.h>      // fprintf()
#include <setjmp.h>     // setjmp()

#include "bcode.h"
#include "eval.h"

FILE *Debug;
jmp_buf Recovery;

int main() {
    char buffer[ 256 ], *ptr;
    byte_t bytes[ 256 ];
    int ret;
    word_t val;
    bcode_t b;

    Debug = stderr;
    eval_init(&Recovery);
    while (1) {
        printf("\n");
        fprintf(Debug, "\n");
        if ((ret = setjmp(Recovery)) != 0) {
            printf("*** eval() error: %s\n", eval_result(ret));
            continue;
        }
        ptr = fgets(buffer, 256, stdin);
        if (ptr == 0 || *buffer == 0 || *buffer == '\n')
            break;
        token_source(buffer);
        ret = bcode_compile(bytes);
        if (ret < 0) {
            printf("*** bcode_compile() error: %s\n", bcode_result(-ret));
            continue;
        }
        bcode_start(bytes);
        bcode_read(&b);
        if (b.inst == BCODE_INQ) {
            val = eval();
            printf("eval() returns %d\n", val);
        } else if (b.type == BCODE_TYPE_VARIABLE)
            eval_assign(b.idx + 1);
        else if (b.inst == BCODE_AT)
            eval_assign(0);
        else
            printf("*** Syntax error\n");
    }
    return 0;
}
