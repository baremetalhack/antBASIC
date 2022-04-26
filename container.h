//
//  c o n t a i n e r . h
//
//  BAREMETALHACK.COM --> public domain
//

#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include "byteword.h"

//
// Container constants
//

#define CONT_MAXARRAY   512
#define CONT_DEFARRAY   10
#define CONT_MAXSTRING  512

//
// Type definition
//

typedef struct {
    int row;
    int col;
} dim_t;

//
// Global variables
//

extern word_t Var[ 26 ];
extern word_t Array[ 26 ][ CONT_MAXARRAY ];
 extern dim_t Asize[ 26 ];
extern byte_t String[ CONT_MAXSTRING ];

//
// Function prototype
//

void cont_init(void);
void cont_dumpvar(void);
void cont_dumparr(void);
void cont_dumpstr(void);

#endif // _CONTAINER_H_
