//
//  c o n t a i n e r . c
//
//  BAREMETALHACK.COM --> public domain
//

#include <stdio.h>          // printf()
#include "container.h"
#include "escape.h"

//
// Global variables
//

word_t Var[ 26 ];
word_t Array[ 26 ][ CONT_MAXARRAY ];
 dim_t Asize[ 26 ];
byte_t String[ CONT_MAXSTRING ];

//
// Container functions
//

void
cont_init(void)
{
    int i, j;

    for (i = 0; i < 26; i++)
        Var[ i ] = 0;

    for (i = 0; i < 26; i++)
        for (j = 0; j < CONT_MAXARRAY; j++)
            Array[ i ][ j ] = 0;

    for (i = 0; i < 26; i++) {
        Asize[ i ].row = 1;
        Asize[ i ].col = CONT_DEFARRAY;
    }

    for (i = 0; i < CONT_MAXSTRING; i++)
        String[ i ] = 0;
}

void
cont_dumpvar(void)
{
    int i;

    printf("%sVariables%s\n", ESCAPE_LGREEN, ESCAPE_DEFAULT);
    for (i = 0; i < 26; i++)
        printf("%c = %6d (0x%04hX)\n", 'A' + i, Var[ i ], Var[ i ]);  
}

void
cont_dumparr(void)
{
    int i, j, k;

    printf("%sArrays%s\n", ESCAPE_LGREEN, ESCAPE_DEFAULT);
    for (i = 0; i < 26; i++) {
        printf("DIM %c[%d,%d] = { ", 'A' + i, Asize[ i ].row, Asize[ i ].col);
        for (j = 0; j < Asize[ i ].row; j++) {
            if (Asize[ i ].row > 1)
                printf("{ ");
            for (k = 0; k < Asize[ i ].col; k++) {
                printf("%d", Array[ i ][ Asize[ i ].col * j + k ]);
                if (k != (Asize[ i ].col - 1))
                    printf(",");
            }
            if (Asize[ i ].row > 1)
                printf(" }");
            if (j != (Asize[ i ].row - 1))
                printf(", ");
        }
        printf(" }\n");
    }
}

#define CHARS_PERLINE    20
void
cont_dumpstr(void)
{
    int i = 0, len = 0, cnt, idx;
    char ch;

    printf("%sString @%s\n", ESCAPE_LGREEN, ESCAPE_DEFAULT);
    while(String[ i++ ])
        len++;
    len++;

    idx = 0;
    cnt = len;
    while (len) {
        cnt = (len > CHARS_PERLINE) ? CHARS_PERLINE : len;
        for (i = 0; i < cnt; i++)
            printf("%02hhX ", String[ idx + i ]);
        printf("| ");
        for (i = 0; i < cnt; i++) {
            ch = String[ idx + i ];
            printf("%c", (ch < ' ') ? '?' : ch);
        }
        printf("\n");
        idx += cnt;
        len -= cnt;
    }
}
