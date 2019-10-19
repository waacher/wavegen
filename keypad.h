#ifndef keypad_H_
#define keypad_H_

#define ROW1 BIT0
#define ROW2 BIT3
#define ROW3 BIT2
#define ROW4 BIT1
#define COL1 BIT5
#define COL2 BIT0
#define COL3 BIT4

// Functions in keypad.c
void initKeypad();
uint8_t get_key();

#endif /* keypad_H_ */
