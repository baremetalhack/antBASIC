//
// f u n c t i o n . c
//
// BAREMETALHACK.COM --> public domain
//

#include <sys/time.h>           // timeval{}, gettimeofday()
#include <stdlib.h>             // srandom(), random()

#ifdef PI
#include "pi_gpio.h"
#endif // PI

//
// BASIC functions
//

static int Seed = 0;

int
func_rnd(void)
{
/*** Classical version using modullo
    static word_t x = 1025;

    x = (257 * x + 1) % 0x8000;
    return (word_t) x;
***/
    struct timeval t;

    if (Seed == 0) {
        gettimeofday(&t, NULL);
        Seed = t.tv_sec;
        srandom(Seed);;
    }
    return random() & 0x7FFF;
}

#ifdef PI
int
func_in(int bit)
{
    return gpio_in(bit);
}
#endif // PI
