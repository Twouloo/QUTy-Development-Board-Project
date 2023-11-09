#include <avr/io.h>
#include <avr/interrupt.h>

void timerB_init() {
    TCB0.CTRLB = TCB_CNTMODE_INT_gc;    // Configure TCB0 in periodic interrupt mode
    TCB0.CCMP = 500;                   // Set interval for 0.1ms (3333 clocks @ 3.3 MHz)
    TCB0.INTCTRL = TCB_CAPT_bm;         // CAPT interrupt enable
    TCB0.CTRLA = TCB_ENABLE_bm;      // Enable
}

void timerA_init(){
    PORTMUX.TCAROUTEA |= PORTMUX_TCA00_DEFAULT_gc|PORTMUX_TCA01_DEFAULT_gc;
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP0EN_bm; // enabled cmp1 and cmp0
    TCA0.SINGLE.PER = 33333;
    TCA0.SINGLE.CMP1 = 33334;
    TCA0.SINGLE.CMP0 = 33334;
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm | TCA_SINGLE_CLKSEL_DIV2_gc;
}

void change_clock(){
    CCP = 0xD8;
    CLKCTRL_MCLKCTRLB = CLKCTRL_PDIV0_bm | CLKCTRL_PEN_bm;
}