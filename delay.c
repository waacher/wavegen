/* Delay functions */

#include "delay.h"
#include "msp.h"

// Delay input microseconds
void delay_us(int x, int sys_freq)
{
    uint32_t i, j, scale;

    if (x <= 4) {
        ;
    }
    else {
        scale = sys_freq / 1000000;
        i = x * scale;
        for (j = i; j > 0; j--) {
            __delay_cycles(1);
        }
    }
}

// x (ms), sys_freq(Hz)
void delay_ms(int x, int sys_freq)
{
    uint32_t i, j, scale;

    if (x <= 4) {
        ;
    }
    else {
        scale = sys_freq / 10000;
        i = x * scale;
        for (j = i; j > 0; j--) {
            __delay_cycles(1);
        }
    }
}
