#include <avr/io.h>
#include <stdint.h>
#include "uart.h"


void uart_init(void) {
    PORTB.DIRSET = PIN2_bm; // Enable PB2 as output (USART0 TXD)
    USART0.BAUD = 2083;     // 9600 baud @ 3.3 MHz
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;   // Enable Tx/Rx
}

uint8_t uart_has_data(void) {
    return (USART0.STATUS & USART_RXCIF_bm);
}

uint8_t uart_getc(void) {
    //while (!uart_has_data());  // Wait for data
    return USART0.RXDATAL;
}

void uart_putc(uint8_t c) {
    while (!(USART0.STATUS & USART_DREIF_bm));  // Wait for TXDATA empty
    USART0.TXDATAL = c;
}

void uart_puts(char* string) {
    while (*string != '\0') {
        uart_putc(*string);
        string++;
    }
}

void ack(void) {
    uart_puts("#ACK\n");
}

void nack(void) {
    uart_puts("#NACK\n");
}

void debug(char* string) {
    uart_putc('?');
    uart_puts(string);
    uart_puts("\n");
}

command_state parser(void) {
    static command_state state = START;

    while (uart_has_data()){
        uint8_t c = uart_getc();
    
        switch (state) {
            case START:
                if (c == '\\') {
                    state = ESCAPE;
                }
                break;
            case ESCAPE:
                if (c == '\\') {
                    state = ID;
                }
                else {
                    state = START;
                    nack();
                }
                break;
            case ID:
                if (c == 's') {
                    ack();
                    state=START;
                    return CMD_SEQ;
                }
                else if (c == 't') {
                    state = CMD_TEST;
                    ack();
                }
                else if (c == 'e') {
                    ack();
                    state=START;
                    return CMD_EXIT;
                }
                else if (c == 'p') {
                    ack();
                    state=START;
                    return CMD_PAUSE;
                }
                else if (c == 'n') {
                    ack();
                    state=START;
                    return CMD_STEP;
                }
                else if (c == 'y') {
                    state = CMD_SYNC;
                    ack();
                }
                else if (c == 'i') {
                    ack();
                    state=START;
                    return CMD_SEQIDX;
                }
                else if (c == 'd') {
                    state = CMD_TEST_SEQ;
                    ack();
                }
                else if (c == 'u') {
                    state = CMD_TEST_SEQNS;
                    ack();
                }
                else {
                    state = START;
                    nack();
                }

                
            default:
                state = START;
        }
    }
    return state;
    
}
