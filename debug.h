//
//  d e b u g . h
//
//  BAREMETALHACK.COM --> public domain
//

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>      // FILE{}

//
// Global variable
//

extern FILE *Debug;

//
// Macro definition for debugging
//

#ifdef DEBUG
    #define DMSG(...) { \
        fprintf(Debug, "%s() ", __FUNCTION__); \
        fprintf(Debug, __VA_ARGS__); \
        fprintf(Debug, "\n"); \
    }
#else
    #define DMSG(...) {}
#endif

#endif // _DEBUG_H_
