#include "msp.h"

#ifndef DELAY_H_
#define DELAY_H_

// Defines the Frequencies
#define FREQ_1_5_MHz CS_CTL0_DCORSEL_0
#define FREQ_3_MHz CS_CTL0_DCORSEL_1
#define FREQ_6_MHz CS_CTL0_DCORSEL_2
#define FREQ_12_MHz CS_CTL0_DCORSEL_3
#define FREQ_24_MHz CS_CTL0_DCORSEL_4
#define FREQ_48_MHz CS_CTL0_DCORSEL_5

// Functions in delay.c
void delay_us(int us, int x);
void delay_ms(int ms, int x);

#endif /* DELAY_H_ */

