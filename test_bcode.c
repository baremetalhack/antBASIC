#include <stdio.h>      // FILE{}, fgets()
#include "bcode.h"

FILE *Debug;

int
main()
{
    char buffer[ 256 ];
    byte_t bytes[ 256 ];
    int i, ret;

    Debug = stderr;
    token_init();
    while (fgets(buffer, 256, stdin) != NULL) {
        if (*buffer == 0 || *buffer == '\n')
            break;
        fprintf(Debug, "===== %s", buffer);
        token_source(buffer);
        ret = bcode_compile(bytes);
        if (ret < 0) {
            ret = -ret;
            fprintf(Debug, "***** Error: %s\n", bcode_result(ret));
            break;
        }
        fprintf(Debug, ">>>>> Compiled bytecodes [ %d bytes ]\n", ret);
        for (i = 0; i < ret; i++) {
            if ((i % 16) == 0)
                fprintf(Debug, "    ");
            fprintf(Debug, "%02X ", bytes[ i ]);
            if ((i % 16) == 15)
                fprintf(Debug, "\n");
        }
        if ((i % 16) != 15)
            fprintf(Debug, "\n");
#ifdef DEBUG
            bcode_dump(bytes);
#endif // DEBUG
    }
    return ret;
}
