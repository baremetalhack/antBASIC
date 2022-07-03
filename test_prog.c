#include <stdio.h>      // FILE{}, fgets()
#include <unistd.h>     // STD*_FILENO
#include "token.h"
#include "bcode.h"
#include "program.h"

FILE *Debug;

int
main()
{
    char buffer[ 256 ];
    byte_t bytes[ 256 ];
    int bsize, ret, num;
    bcode_t b;

    Debug = stderr;
    token_init();
    prog_init();

    while (fgets(buffer, 256, stdin) != NULL) {
        if (*buffer == 0 || *buffer == '\n')
            break;
        fprintf(Debug, "===== %s", buffer);
        token_source(buffer);
        ret = bsize = bcode_compile(bytes);
        if (bsize < 0) {
            fprintf(Debug, "***** Error: %s\n", bcode_result(ret));
            break;
        }
        fprintf(Debug, ">>>>> Compiled bytecodes [ %d bytes ]\n", bsize);
#ifdef DEBUG
            bcode_dump(bytes);
#endif // DEBUG
        
        bcode_start(bytes);
        bcode_read(&b);
        if (b.inst == BCODE_LIST) {
            fprintf(Debug, "-----------------------------------------\n");
            ret = prog_list(STDOUT_FILENO, 1, Lines[1].num, Lines[Lsize].num);
            fprintf(Debug, "-----------------------------------------\n");
        } else if (b.inst == BCODE_NEW) {
            prog_init();
            continue;
        } else if (b.type == BCODE_TYPE_NUMBER10) {
            num = b.num;
            if (bcode_next() == BCODE_EOL) {
                ret = prog_delete(num,num);
                if (ret) {
                    fprintf(Debug, "***** Error: %s\n", prog_result(ret));
                    continue;
                }
                fprintf(Debug, ">>>>> Line %d deleted\n", num);
            } else {
                bcode_read(&b);
                ret = prog_insert(num, &bytes[ b.pos ], bsize - 3);
                if (ret) {
                    fprintf(Debug, "***** Error: %s\n", prog_result(ret));
                    continue;
                }
                fprintf(Debug, ">>>>> Line %d inserted\n", num);
            }
        } else {
            fprintf(Debug, "***** Error: illegal program line\n");
            continue;
        }
        prog_dumpbytes();
        printf("\n");
        prog_dumplines();
        ret = 0;
    }
    return 0;
}
