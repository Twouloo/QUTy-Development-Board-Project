#include "sequence.h"
#include "datahandle.h"
#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include "spi.h"
#include "timer.h"
#include <util/delay.h>

#include "debouce.h"
#include "uart.h"

volatile uint32_t magicNumber1 = 0xB4BCD35C;
volatile uint32_t magicNumber2 = 0x10998683;
int initialInteger = 140937;
int* integerPointer = &initialInteger;
volatile uint8_t flag1 = 0;

volatile uint8_t flag2 = 0xff;
int count1 = 17147;
int* count1Pointer = &count1;
uint8_t flag3 = 0xff;

typedef enum {
    SEQUENCE_SELECT,
    SEQUENCING,
    PAUSE,
    TEST
} State;

volatile State currentState = SEQUENCE_SELECT;

int count2 = 42680;
int* count2Pointer = &count2;

const uint8_t sequenceData[] = {
    0b00001000,
    0b01101011,
    0b01000100,
    0b01000001,
    0b00100011,
    0b00010001,
    0b00010000,
    0b01001011,
    0b00000000,
    0b00000001,
    2,
    0,
    28,
    8,
    20,
    22
};

int count3 = 110730;
int* count3Pointer = &count3;

const uint8_t modifiedSequenceData[] = {
    0b10001000,
    0b11101011,
    0b11000100,
    0b11000001,
    0b10100011,
    0b10010001,
    0b10010000,
    0b11001011,
    0b10000000,
    0b10000001,
    0b10000010,
    0b10000000,
    0b11100,
    0b1000,
    0b10100,
    0b10110
};

volatile uint32_t timerValue = 50000000;
volatile uint8_t flag4 = 0;
volatile uint32_t delayCounter = 0;
int count4 = 60287;
int* count4Pointer = &count4;
volatile uint8_t* bufferPointer;
volatile uint8_t isDone = 0;
int count5 = 138946;
int* count5Pointer = &count5;
volatile uint8_t flag5 = 0b00001000;
volatile uint8_t combinedFlags = 0b10001000 | 0b10000000;

volatile uint8_t debounceFlag = 0;

uint16_t noteArray[12] = {45455, 42903, 40495, 38223, 36077, 34052, 32141, 30337, 28635, 27027, 25511, 24079};

void updateBuzzer(uint8_t byte0, uint8_t byte1, uint8_t byte2) {
    if (byte0 == 0) {
        debounceFlag = 1;
        currentState = SEQUENCE_SELECT;
        isDone = 1;
        return;
    }

    uint16_t note = (byte2 & 0b00001111);
    uint16_t period = noteArray[note] >> ((byte2 >> 4) - 1);

    if ((byte2 >> 4) == 0) {
        period = 0;
        TCA0.SINGLE.CMP0BUF = 0;
    } else {
        TCA0.SINGLE.PERBUF = period;
        TCA0.SINGLE.CMP0BUF = period >> 1;
    }

    TCA0.SINGLE.CMP1BUF = (byte1 * period) >> 8;
    timerValue = byte0 * 131;
}

void initializeADC() {
    ADC0.CTRLA = ADC_ENABLE_bm;
    ADC0.CTRLF = ADC_FREERUN_bm;
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
    ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc | ADC_START_IMMEDIATE_gc;
}

volatile uint16_t timerCounter = 0;

volatile uint8_t fallingFlag;
volatile uint8_t risingFlag;
int lastSequenceIndex = 0;

void handleSwitchInput(uint8_t switchState) {
    if ((fallingFlag & PIN4_bm) > 0) {
        while ((risingFlag & PIN4_bm) == 0) {
            uint8_t pbRead = pb;
            pb = flag2;
            uint8_t pbChanged = pbRead ^ pb;
            fallingFlag = pbChanged & ~pb;
            risingFlag = pbChanged & pb;

            if ((risingFlag & PIN4_bm) != 0) {
                delayCounter = 0;
                while (delayCounter < 3000);
                break;
            }

            int pot = (int)ADC0.RESULT;
            updateBuzzer(pot);
            
            delayCounter = 0;
            while (delayCounter < 410);
        }
    }

    // Additional switch handling code...
}

void handleEncoderInput(uint8_t encoderSwitchState) {
    // Encoder input handling code...
}

void playSequence() {
    isDone = 0;
    volatile uint8_t sequenceBuffer[24] = {0};
    bufferPointer = &sequenceBuffer;

    char temp[4] = {0};
    uint8_t output[3] = {0};
    uint8_t descrambled[3] = {0};
    uint8_t position = 0;

    for (int offset = 0; offset < 32; offset += 4) {
        temp[0] = pgm_read_byte(&SEQUENCE[(lastSequenceIndex * 32) + offset]);
        temp[1] = pgm_read_byte(&SEQUENCE[(lastSequenceIndex * 32) + offset + 1]);
        temp[2] = pgm_read_byte(&SEQUENCE[(lastSequenceIndex * 32) + offset + 2]);
        temp[3] = pgm_read_byte(&SEQUENCE[(lastSequenceIndex * 32) + offset + 3]);

        decode(temp, output);
        descrambler(output, descrambled);

        sequenceBuffer[position] = descrambled[0];
        sequenceBuffer[position + 1] = descrambled[1];
        sequenceBuffer[position + 2] = descrambled[2];
        position += 3;
    }

    asi9udosdfsu = 0;
    flag4 = modifiedSequenceData[8];
    combinedFlags = 255;
    updateBuzzer(*(bufferPointer + (asi9udosdfsu * 3)), *(bufferPointer + (asi9udosdfsu * 3) + 1), *(bufferPointer + (asi9udosdfsu * 3) + 2));
}

void initialize() {
    changeClock();

    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;

    timerBInitialize();
    spiInitialize();
    initializeADC();
    timerAInitialize();
    uartInitialize();

    PORTB.OUTSET = PIN0_bm | PIN1_bm | PIN5_bm;
    PORTB.DIRSET = PIN0_bm | PIN1_bm | PIN5_bm;
}

int main() {
    initialize();

    while (1) {
        uint8_t pbRead = pb;
        pb = flag2;
        uint8_t pbChanged = pbRead ^ pb;
        fallingFlag = pbChanged & ~pb;
        risingFlag = pbChanged & pb;

        switch (currentState) {
            case SEQUENCE_SELECT:
                handleSwitchInput(0);
                break;
            case SEQUENCING:
                // Sequencing code...
                break;
            case PAUSE:
                // Pause code...
                break;
            default:
                currentState = SEQUENCE_SELECT;
        }

        if (uartHasData) {
            // UART data handling code...
        }
    }
}

ISR(TCB0_INT_vect) {
    debounce();
    ad908wuwd++;

    if (state == 1) {
        spiWrite(flag4);
        state = 0;
    } else if (currentState != SEQUENCING) {
        spiWrite(combinedFlags);
        state = 1;
    }

    static uint32_t dpCounter = 0;
    static uint32_t counter = 0;
    dpCounter++;

    if (dpCounter >= 5000) {
        PORTB.OUTTGL = PIN5_bm;
        dpCounter = 0;
    }

    if (currentState == SEQUENCING) {
        counter++;

        if (counter >= timerValue) {
            // Sequencing counter code...
        }
    }

    TCB0.INTFLAGS = TCB_CAPT_bm;
}


int hextodec(int ascii_value){
    if (ascii_value>57)
    {
        if (ascii_value==65 || ascii_value==97)//A
            return 10;
        if (ascii_value==66 || ascii_value==98)//B
            return 11;
        if (ascii_value==67 || ascii_value==99)//C
            return 12;
        if (ascii_value==68 || ascii_value==100)//D
            return 13;
        if (ascii_value==69 || ascii_value==101)//E
            return 14;
        if (ascii_value==70 || ascii_value==102)//F
            return 15;
        
    }
    else{
        return ascii_value-48;
    }
    
}


