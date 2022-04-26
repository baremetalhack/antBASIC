#include <stdio.h>      // fprintf()
#include <setjmp.h>     // setjmp()

#include "token.h"
#include "bcode.h"
#include "eval.h"

FILE *Debug;
jmp_buf Recovery;

int main() {
    char buffer[ 256 ], *ptr;
    byte_t bytes[ 256 ];
    int ret;
    word_t val;

    Debug = stderr;
    token_init();
    eval_init(&Recovery);
    while (1) {
        fprintf(stdout, "\n");
        fprintf(Debug, "\n");
        if ((ret = setjmp(Recovery)) != 0) {
            fprintf(stdout, "*** eval() error: %s\n", eval_result(ret));
            continue;
        }
        ptr = fgets(buffer, 256, stdin);
        if (ptr == 0 || *buffer == 0 || *buffer == '\n')
            break;
        token_source(buffer);
        ret = bcode_compile(bytes);
        if (ret < 0) {
            fprintf(stdout, "*** bcode_compile() error: %s ***\n", bcode_result(-ret));
            continue;
        }
        bcode_start(bytes);
        val = eval();
        fprintf(stdout, "eval() returns %d\n", val);
    }
    return 0;
}
