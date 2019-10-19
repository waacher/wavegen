/*
 * Function Generator
 *  SPI, Timer, Interrupt setup
 *  Keypad integration
 *  Waveform logic
 */

#include "msp.h"
#include "freq.h"
#include "delay.h"
#include "keypad.h"
#include "sine.h"

#define VDD 4000    // 3.3V
#define V0  255     // +-20mV (couldn't get 0V)
#define V1  1075    // 1V
#define V2  2300    // 2V

// Factors for dividing CCR[0] value
#define HZ100 1
#define HZ200 2
#define HZ300 3
#define HZ400 4
#define HZ500 5

#define SQUARE 0
#define SAWTOOTH 1
#define SINE 2

static uint8_t waveType = SQUARE;

// Iterations for Timer=600 for 50% duty cycle
static uint8_t iterations = 1;
static uint8_t duty = 100;

static uint16_t sineIndex = 0;
static uint16_t data = V0;          // Start ouput at 0V
static uint16_t amp = VDD;          // Default amplitude VDD = 3.3V

// Initialize SPI
void initSPI() {
/*  For testing
    P7->DIR |= BIT0;                                //SMCLK
    P7->SEL1 &= ~BIT0;
    P7->SEL0 |= BIT0;

    P4->SEL1 &= ~BIT3;                              // MCLK
    P4->SEL0 |= BIT3;
    P4->DIR |= BIT3;
*/
    P3->DIR |= BIT6;                                // CS

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_CKPL
                      | EUSCI_B_CTLW0_MSB
                      | EUSCI_B_CTLW0_MST
                      | EUSCI_B_CTLW0_SYNC
                      | EUSCI_B_CTLW0_UCSSEL_2      // Select SMCLK
                      | EUSCI_B_CTLW0_SWRST;

    P1->SEL1 &= ~(BIT5 | BIT6);                     // 1.5 -> SCLK, 1.6 -> SDI
    P1->SEL0 |= (BIT5 | BIT6);

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;        // activate SPI
    P3->OUT |= BIT6;                                // initialize CS high
}

// Initalize Timer
void initTimer() {
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE;        // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 600;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK |       // SMCLK
                    TIMER_A_CTL_MC__UP;             // Up Mode

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);       // Enable Timer_A Interrupts
    __enable_irq();                                 // Enable global interrupt
}

// Change CCR[0] to update frequency
void setFreq(uint8_t freq) {
    TIMER_A0->CCR[0] = 600 / freq;
}

// Every time timer goes off, output a point
void TA0_0_IRQHandler(void) {
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    uint8_t loByte, hiByte;

    if (waveType == SQUARE) {
        if (data == V0) {                           // Low -> High
            if (iterations == (200-duty)) {
                data = amp;                         // VDD
                iterations = 0;
            }
        }
        else if (data) {                            // High -> Low
            if (iterations == duty) {
                data = V0;
                iterations = 0;
            }
        }
        iterations++;
    }

    else if (waveType == SAWTOOTH) {
        data+= 19;
        if (data > amp) {
            data = V0;
        }
    }

    else if (waveType == SINE) {
        // Index into lookup table
        sineIndex = sineIndex % 200;
        // Cases for different amplitude
        if (amp == VDD) {
            data = sineTableVDD[sineIndex];
        }
        else if (amp == V1) {
            data = sineTable1[sineIndex];
        }
        else if (amp == V2) {
            data = sineTable2[sineIndex];
        }
        sineIndex++;
    }

    loByte = data & 0xFF;                           // lower 8 bits of data
    hiByte = (data >> 8) & 0x0F;                    // lower 4 of upper 8 bits
    hiByte |= (BIT4 | BIT5);                        // set Gain and Shutdown config bits
    P3->OUT &= ~BIT6;                               // set CS low
    EUSCI_B0-> RXBUF;
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));   // wait for TXBuf to be empty
    EUSCI_B0->TXBUF = hiByte;
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));   // wait again
    EUSCI_B0->TXBUF = loByte;
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG));
    EUSCI_B0->RXBUF;
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG));
    P3->OUT |= BIT6;                                // Raise CS
}

void main(void){
    uint8_t x;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    // INIT
    setDCO(SYS_FREQ);
    initKeypad();
    initSPI();
    initTimer();

    // Runs forever
    while (1) {
        delay_ms(120, SYS_FREQ);
        x = get_key();

        // Frequencies
        if (x == '1') {
            setFreq(HZ100);
        }
        else if (x == '2') {
            setFreq(HZ200);
        }
        else if (x == '3') {
            setFreq(HZ300);
        }
        else if (x == '4') {
            setFreq(HZ400);
        }
        else if (x == '5') {
            setFreq(HZ500);
        }
        // Change amplitude
        else if (x == '6')  {
            if (amp == V1) {
                amp = V2;
            }
            else if (amp == V2) {
                amp = VDD;
            }
            else if (amp == VDD) {
                amp = V1;
            }
        }
        // Wave Type
        else if (x == '7') {
            waveType = SQUARE;
        }
        else if (x == '8') {
            waveType = SINE;
        }
        else if (x == '9') {
            waveType = SAWTOOTH;
        }
        // Duty cycle
        else if (x == '*' && duty > 20) {
            duty -= 20;
        }
        else if (x == '0') {
            duty = 100;
        }
        else if (x == '#' && duty < 180) {
            duty += 20;
        }
    }
}
