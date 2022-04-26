#include <stdio.h>      // FILE{}, stdin, stderr, fgets(), printf()
#include "token.h"

FILE *Debug;

int
main()
{
    char buffer[ 256 ];
    token_t tok;
    int ret;

    Debug = stderr;
    token_init();
    while (fgets(buffer, 256, stdin) != NULL) {
        if (*buffer == 0 || *buffer == '\n')
            break;
        printf("===== %s", buffer);
        token_source(buffer);
        while (1) {
            ret = token_read(&tok);
            if (ret != TOKEN_SUCCESS) {
                fprintf(Debug, "*** Token error: %s\n", token_result(ret));
                break;
            }
#ifdef DEBUG
            token_dump(&tok);
#endif
            if (tok.type == TOKEN_TYPE_EOL)
                break;
        }
        fprintf(Debug, "\n");
    }

    return 0;
}
