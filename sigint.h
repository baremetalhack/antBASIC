//
//  s i g i n t . h
//
//  BAREMETALHACK.COM --> public domain
//

#ifndef _SIGINT_H_
#define _SIGINT_H_

// SIGINT handler defined in "main.c"

void sigint_register(void);
void sigint_edit_register(void);
int  sigint_edit_abort(void);

#endif  // _SIGINT_H_
