/* Change oscillator speed
 * For 48 MHz, use MCLK and divide SMCLK to 12 MHz
 */

#include <msp.h>
#include "freq.h"

// Change clock speed
void setDCO(int freq) {
    freq = freq / 1000; // convert to kHz

    CS->KEY = CS_KEY_VAL;
    CS->CTL0 = 0;

    switch(freq) {
        case FREQ_1500:
            CS->CTL0 = CS_CTL0_DCORSEL_0; // set DCO = 1.5 MHz
            CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
            // select clock sources
            break;

        case FREQ_3000:
            CS->CTL0 = CS_CTL0_DCORSEL_1; // set DCO = 3.0 MHz
            CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
            // select clock sources
            break;

        case FREQ_6000:
            CS->CTL0 = CS_CTL0_DCORSEL_2; // set DCO = 6.0 MHz
            CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
            // select clock sources
            break;

        case FREQ_12000:
            CS->CTL0 = CS_CTL0_DCORSEL_3; // set DCO = 12.0 MHz
            CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
            // select clock sources
            break;

        case FREQ_24000:
            CS->CTL0 = CS_CTL0_DCORSEL_4; // set DCO = 24.0 MHz
            CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
            // select clock sources
            break;

        case FREQ_48000:
            /* Transition to VCORE Level 1: AM0_LDO --> AM1_LDO */
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
            PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
            while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
            /* Configure Flash wait-state to 1 for both banks 0 & 1 */
            FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL &
            ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
            FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL &
            ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;

            CS->CTL0 = CS_CTL0_DCORSEL_5; // set DCO = 48.0 MHz
            CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK) | CS_CTL1_SELM_3;                    // MCLK = 48 MHz
            CS->CTL1 = CS->CTL1 & ~(CS_CTL1_SELS_MASK | CS_CTL1_DIVS_MASK) | CS_CTL1_SELS_3 | CS_CTL1_DIVS_2;   // SMCLK = 12 MHz
            break;

        default:
            CS->CTL0 = CS_CTL0_DCORSEL_1; // default: set DCO = 3.0 MHz
            CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
            // select clock sources
            break;
    }

    CS->KEY = 0; // lock the CS registers
}
