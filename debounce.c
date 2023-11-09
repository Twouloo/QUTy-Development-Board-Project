#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

extern uint8_t buttonState;

void debounce(void){
    static uint8_t previousButtonState = 0;
    static uint8_t currentButtonState = 0;

    uint8_t currentPortValue = PORTA.IN;
    uint8_t buttonStateChanged = currentPortValue ^ buttonState;

    currentButtonState = (currentButtonState ^ previousButtonState) & buttonStateChanged;
    previousButtonState = ~previousButtonState & buttonStateChanged;

    buttonState ^= (currentButtonState & previousButtonState) | (buttonStateChanged & buttonState);
}
