//
//  p i _ g p i o . c
//
//  NOTICE: Supported SOCs are BCM2835, BCM2836, BCM2837, BCM2711
//
//  BAREMETALHACK.COM --> public domain
//

#include <fcntl.h>          // open()
#include <unistd.h>         // close()
#include <sys/mman.h>       // mmap(), munmap()
#include <stdint.h>         // uint32_t
#include <stdlib.h>         // atoi()
#include <unistd.h>         // usleep();
#include "pi_gpio.h"

//
// File scope variables
//

static int Soc;
static volatile uint32_t *Gpio_base;
static uint32_t Save_gpio_fsel0, Save_gpio_fsel1, Save_gpio_fsel2;
static uint32_t Save_gpio_set0, Save_gpio_clr0;
static uint32_t Save_gpio_updn0, Save_gpio_updn1;
static int States[ GPIO_BITS ];
static int Microsec;

// Bitmapping array: converts from re-arranged BMH (Bare Metal Hacking) number to
// original BCM (Broadcom) number

static int Bitmap[] = {
    0,
    2,      // BMH1  --> GPIO2
    3,      // BMH2  --> GPIO3
    4,      // BMH3  --> GPIO4
    17,     // BMH4  --> GPIO17
    27,     // BMH5  --> GPIO27
    22,     // BMH6  --> GPIO22
    10,     // BMH7  --> GPIO10
    9,      // BMH8  --> GPIO9
    11,     // BMH9  --> GPIO11
    5,      // BMH10 --> GPIO5
    6,      // BMH11 --> GPIO6
    13,     // BMH12 --> GPIO13
    19,     // BMH13 --> GPIO19
    26      // BMH14 --> GPIO26
};

static char *Results[] = {
    "GPIO ok",                      // GPIO_SUCCESS
    "GPIO unknown SOC",             // GPIO_SOCUNKNOWN
    "GPIO open failure",            // GPIO_OPENFAIL
    "GPIO illegal bit number",      // GPIO_ILLBIT
    "GPIO illegal direction",       // GPIO_ILLDIR
    "GPIO illegal register mode",   // GPIO_ILLREG
};

//
// GPIO control functions
//

int
gpio_init(void)
{
    int fd, ret, i;
    uint8_t rev[ 4 ];
    char *ptr;

    fd = open(GPIO_REVISIONPATH, O_RDONLY);
    if (fd < 0)
        return GPIO_SOCUNKNOWN;
    ret = read(fd, rev, sizeof(rev));
    close(fd);
    if (ret != sizeof(rev))
        return GPIO_SOCUNKNOWN;
    switch ((rev[ 2 ] & 0xF0) >> 4) {
        case 0:
        case 1:
        case 2:
            Soc = GPIO_BCM283x;
            break;

        case 3:
            Soc = GPIO_BCM2711;
            break;

        default:
            return GPIO_SOCUNKNOWN;
    };

    fd = open(GPIO_PAGEPATH, O_RDWR, O_SYNC, O_CLOEXEC);
    if (fd < 0)
        return GPIO_OPENFAIL;

    Gpio_base = (uint32_t *) mmap(NULL, GPIO_PAGESIZE, PROT_READ | PROT_WRITE, \
            MAP_SHARED, fd, 0);
    close(fd);
    if (Gpio_base == MAP_FAILED)
        return GPIO_OPENFAIL;

    for (i = 0; i < GPIO_BITS; i++)
        States[ i ] = GPIO_UNSPECIFIED;

    Save_gpio_fsel0 = Gpio_base[ FSEL0 ];
    Save_gpio_fsel1 = Gpio_base[ FSEL1 ];
    Save_gpio_fsel2 = Gpio_base[ FSEL2 ];
    Save_gpio_set0  = Gpio_base[ SET0 ];
    Save_gpio_clr0  = Gpio_base[ CLR0 ];
    Save_gpio_updn0 = Gpio_base[ UPDN0 ];
    Save_gpio_updn1 = Gpio_base[ UPDN1 ];
    
    ptr = getenv(GPIO_MICROENV);
    if (ptr)
        Microsec = atoi(ptr);
    else
        Microsec = 0;

    return GPIO_SUCCESS;
}

void
gpio_term(void)
{
    Gpio_base[ FSEL0 ] = Save_gpio_fsel0;
    Gpio_base[ FSEL1 ] = Save_gpio_fsel1;
    Gpio_base[ FSEL2 ] = Save_gpio_fsel2;
    Gpio_base[ SET0 ]  = Save_gpio_set0;
    Gpio_base[ CLR0 ]  = Save_gpio_clr0;
    Gpio_base[ UPDN0 ] = Save_gpio_updn0;
    Gpio_base[ UPDN1 ] = Save_gpio_updn1;

    munmap((void*) Gpio_base, GPIO_PAGESIZE);
}

int
gpio_bitmode(int bmh, int dir, int reg)
{
    int bcm, idx;
    uint32_t val, mask;

    if (bmh < 1 || bmh > GPIO_BITS)
        return GPIO_ILLBIT;
    if (dir != GPIO_INPUT && dir != GPIO_OUTPUT)
        return GPIO_ILLDIR;
    if (reg != GPIO_REGNONE && reg != GPIO_REGUP && reg != GPIO_REGDOWN)
        return GPIO_ILLREG;

    bcm = Bitmap[ bmh ];
    idx = FSEL0 + (bcm / 10);
    val = Gpio_base[ idx ];
    mask = ~(0b111 << (3 * (bcm % 10)));
    val &= mask;
    if (dir == GPIO_OUTPUT)
        val |= (0b001 << (3 * (bcm % 10)));
    Gpio_base[ idx ] = val;

    if (Soc == GPIO_BCM2711) {
        idx = UPDN0 + (bcm / 16);
        val = Gpio_base[ idx ];
        mask = ~(0b11 << (2 * (bcm % 16)));
        val &= mask;
        if (reg == GPIO_REGUP)
            val |= (0b01 << (2 * (bcm % 16)));
        else if (reg == GPIO_REGDOWN)
            val |= (0b10 << (2 * (bcm % 16)));
        Gpio_base[ idx ] = val;
    } else {    // BMC283x
        switch (reg) {
            case GPIO_REGNONE:
                val = 0b00;
                break;

            case GPIO_REGUP:
                val = 0b10;
                break;

            case GPIO_REGDOWN:
                val = 0x01;
                break;
        };
        // The following protocol is adopted from "BCM2835 ARM Peripherals"

        Gpio_base[ GPPUD ] = val;               // Update GPPUD
        gpio_usleep(GPIO_UPDN_WAIT);            // Wait more than 150 cycles
        Gpio_base[ GPPUDCLK0 ] = 1 << bcm;      // Assert clock
        gpio_usleep(GPIO_UPDN_WAIT);            // Wait more than 150 cycles
        Gpio_base[ GPPUD ] = 0;                 // Remove GPPUD control signal
        Gpio_base[ GPPUDCLK0 ] = 0;             // Remove GPPUDCLK0 clock signal
    }

    return GPIO_SUCCESS;
}

int
gpio_bitread(int bmh)
{
    int bcm;

    if (bmh < 1 || bmh > GPIO_BITS)
        return GPIO_ILLBIT;

    bcm = Bitmap[ bmh ];
    if (Gpio_base[ LEV0 ] & (1 << bcm))
        return GPIO_HIGH;
    else
        return GPIO_LOW;
}

int
gpio_bitwrite(int bmh, int value)
{
    int bcm, idx;

    if (bmh < 1 || bmh > GPIO_BITS)
        return GPIO_ILLBIT;

    bcm = Bitmap[ bmh ];
    if (value)
        idx = SET0;
    else
        idx = CLR0;
    Gpio_base[ idx ] = (1 << bcm);

    return GPIO_SUCCESS;
}

int
gpio_in(int bmh)
{
    int ret;

    if (States[ bmh ] == GPIO_INPUT)
        return gpio_bitread(bmh);
    else {
        ret = gpio_bitmode(bmh, GPIO_INPUT, GPIO_REGUP);
        if (ret < 0)
            return ret;
        else {
            States[ bmh ] = GPIO_INPUT;
            return gpio_bitread(bmh);
        }
    }
}

int
gpio_out(int bmh, int val)
{
    int ret;

    if (States[ bmh ] == GPIO_OUTPUT)
        return gpio_bitwrite(bmh, val);
    else {
        ret = gpio_bitmode(bmh, GPIO_OUTPUT, GPIO_REGNONE);
        if (ret < 0)
            return ret;
        else {
            States[ bmh ] = GPIO_OUTPUT;
            return gpio_bitwrite(bmh, val);
        }
    }
}

int
gpio_outhz(int bmh, int val, int pull)
{
    int ret, p;

    if (val) {
        p = pull ? GPIO_REGUP : GPIO_REGNONE;
        ret = gpio_bitmode(bmh, GPIO_INPUT, p);
        if (ret)
            return ret;
    } else {
        ret = gpio_bitmode(bmh, GPIO_OUTPUT, GPIO_REGNONE);
        if (ret)
            return ret;
        ret = gpio_bitwrite(bmh, 0);
        if (ret)
            return ret;
    }
    States[ bmh ] = GPIO_OUTPUTHZ;
    return GPIO_SUCCESS;
}

char *
gpio_result(int code)
{
    return Results[ code ];
}

void
gpio_usleep(int utime)
{
    int loops;
    volatile int i;     // We need "volatile" for prevention of code optimization

    if (Microsec == 0) {
        usleep(utime);
        return;
    } else {
        loops = utime * Microsec;
        for (i = 0; i < loops; i++)
            ;
    }
}
