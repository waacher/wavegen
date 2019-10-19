/* Keypad Driver
 * Rows:    Pin 4
 * Columns: Pin 6
 */

#include <msp.h>
#include "keypad.h"
//#include "delay.h"

// Set rows as inputs, columns as outputs
void initKeypad() {
    P6->DIR |= (COL1 | COL2 | COL3);
    P4->DIR &= ~(ROW1 | ROW2 | ROW3 | ROW4);
    P4->REN |= (ROW1 | ROW2 | ROW3 | ROW4);
    P4->OUT &= ~(ROW1 | ROW2 | ROW3 | ROW4);
}

// Read from keypad and written selected character
uint8_t get_key() {

    uint8_t rows, col, key;

    // Send voltage to all columns, grab input
    P6->OUT |= (COL1 | COL2 | COL3);
    __delay_cycles(20);
    rows = (P4->IN & 0x0F);
    if (rows == 0) {
        return 0xFF;
    }

    // Loop through columns until selected row
    for (col = 0; col < 3; col++) {
        P6->OUT &= ~(COL1 | COL2 | COL3);
        if (col == 0) {
            P6->OUT |= COL1;
        }
        else if (col == 1) {
            P6->OUT |= COL2;
        }
        else {
            P6->OUT |= COL3;
        }

        __delay_cycles(20);
        rows = P4->IN & 0x0F;
        if (rows != 0) {
            break;
        }
    }
    // None
    if (col == 3) return 0xFF;

    // Convert to correct row (for weird keypad layout)
    if (rows == 1) {
        rows = 0x01;
    }
    else if (rows == 2) {
        rows = 0x04;
    }
    else if (rows == 4) {
        rows = 0x03;
    }
    else if (rows == 8) {
        rows = 0x02;
    }

    // Calculate character
    key = rows * 3 + col - 2;
    if (key == 11) {
        key = 0;
    }
    if (key == 10) {
        return '*';
    }
    if (key == 12) {
        return '#';
    }

    // Convert to ascii number
    key += '0';

    return key;
}
