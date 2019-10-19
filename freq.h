#ifndef FREQ_H_
#define FREQ_H_

#define SYS_FREQ 48000000UL

#define FREQ_1500 1500 // frequency at 1500kHz
#define FREQ_3000 3000 // frequency at 300kHz
#define FREQ_6000 6000 // frequency at 6000kHz
#define FREQ_12000 12000 // frequency at 12000kHz
#define FREQ_24000 24000 // frequency at 24000kHz
#define FREQ_48000 48000 // frequency at 48000kHz

// Function in freq.c
void setDCO(int freq);

#endif /* FREQ_H_ */
