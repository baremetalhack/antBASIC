//
//  p i _ g p i o . h
//
//  BAREMETALHACK.COM --> public domain
//

#ifndef _PI_GPIO_H_
#define _PI_GPIO_H_

// Raspberry Pi SOC type extracted from revision code
// NOTE: pull-up/dowun register handling is different between BCM283x and BCM2711

#define GPIO_REVISIONPATH   "/proc/device-tree/system/linux,revision"
#define GPIO_BCM283x        0       // BCM2835: Pi original, Pi Zero
                                    // BCM2836: Pi 2, Pi 3
#define GPIO_BCM2711        1       // Pi 4, Pi 400
#define GPIO_UPDN_WAIT      1       // Microseconds to wait while pull-up/down
                                    // control in BCM283x
                                    // "BCM2835 ARM Peripheral" says "150cycles"
                                    //    GPIO clock = 250MHz (GPU)
                                    //    150/250M = 0.6 microseconds

// Linux ROOTLESS access to GPIOMEM 

#define GPIO_PAGEPATH       "/dev/gpiomem"
#define GPIO_PAGESIZE       4096

// Re-arranged BMH (Bare Metal Hacking) bits: BMH1-BMH14
//      GPIO2, GPIO3, GPIO4, GPIO17, GPIO27, GPIO22, GPIO10, GPIO9, GPIO11,
//      GPIO5, GPIO6, GPIO13, GPIO19, GPIO26

#define GPIO_BITS           14      // BMH1 to BMH14

// GPIO control register index (32bit WORD offset address)

#define FSEL0               0       // Function select 0 (GPIO0 to GPIO9)
#define FSEL1               1       // Function select 1 (GPIO10 to GPIO19)
#define FSEL2               2       // Function select 2 (GPIO20 to GPIO29)
#define SET0                7       // Pin output set 0 (GPIO0 to GPIO31)
#define CLR0                10      // Pin output clear 0 (GPIO0 to GPIO31)
#define LEV0                13      // Pin level 0 (GPIO0 to GPIO31)

// GPIO pull-up/down control registers in BCM2711

#define UPDN0               57      // Pull-up pull-down 0 (GPIO0 to GPIO15)
#define UPDN1               58      // Pull-up pull-down 0 (GPIO16 to GPIO31)

// GPIO pull-up/down control registers in BCM283x

#define GPPUD               37      // Gpio Pull-Up/Down enable register
#define GPPUDCLK0           38      // Gpio Pull-Up/Down enable CLocK 0 register

// Bit values

#define GPIO_HIGH           1
#define GPIO_LOW            0

// Bit definition

#define GPIO_INPUT          0
#define GPIO_OUTPUT         1
#define GPIO_OUTPUTHZ       2
#define GPIO_UNSPECIFIED    (-1)

// Register mode

#define GPIO_REGNONE        0       // NO register is connected
#define GPIO_REGUP          1       // Pull-UP register is connected
#define GPIO_REGDOWN        2       // Pull-DOWN register is connected

// Result codes

#define GPIO_SUCCESS        0
#define GPIO_SOCUNKNOWN     (-1)
#define GPIO_OPENFAIL       (-2)
#define GPIO_ILLBIT         (-3)
#define GPIO_ILLDIR         (-4)
#define GPIO_ILLREG         (-5)

//
// Environment variable which holds calibrated loop count for MICROSECOND wait
//

#define GPIO_MICROENV       "ANT_MICROWAIT"

// 
// Function prototypes
//

   int gpio_init(void);
  void gpio_term(void);
   int gpio_bitmode(int, int, int);
   int gpio_bitread(int);
   int gpio_bitwrite(int, int);
   int gpio_in(int);
   int gpio_out(int, int);
   int gpio_outhz(int, int, int);
char * gpio_result(int);
  void gpio_usleep(int);

#endif // _PI_GPIO_H
