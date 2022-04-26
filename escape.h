//
//  e s c a p e . h
//
//  BAREMETALHACK.COM --> public domain
//

#ifndef _ESCAPE_H_
#define _ESCAPE_H_

//
// ANSI escape sequences
//

#define ESCAPE_HOME             "\e[1;1H"
#define ESCAPE_CLS              "\e[2J"

#define ESCAPE_DEFAULT          "\e[0m"
#define ESCAPE_BOLD             "\e[1m"
#define ESCAPE_RED              "\e[31m"
#define ESCAPE_GREEN            "\e[32m"
#define ESCAPE_YELLOW           "\e[33m"
#define ESCAPE_BLUE             "\e[34m"
#define ESCAPE_MAGENTA          "\e[35m"
#define ESCAPE_CYAN             "\e[36m"
#define ESCAPE_WHITE            "\e[37m"
#define ESCAPE_LRED             (ESCAPE_BOLD ESCAPE_RED)
#define ESCAPE_LGREEN           (ESCAPE_BOLD ESCAPE_GREEN)
#define ESCAPE_LYELLOW          (ESCAPE_BOLD ESCAPE_YELLOW)
#define ESCAPE_LBLUE            (ESCAPE_BOLD ESCAPE_BLUE)
#define ESCAPE_LMAGENTA         (ESCAPE_BOLD ESCAPE_MAGENTA)
#define ESCAPE_LCYAN            (ESCAPE_BOLD ESCAPE_CYAN)
#define ESCAPE_LWHITE           (ESCAPE_BOLD ESCAPE_WHITE)

#endif  // _ESCAPE_H_
