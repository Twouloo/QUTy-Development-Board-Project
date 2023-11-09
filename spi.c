#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>
void spi_init(void) {
    cli();
    PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc; // SPI pins on PC0-3
    PORTC.DIR |= (PIN0_bm | PIN2_bm); // Set SCK (PC0) and MOSI (PC2) as outputs
    SPI0.CTRLA = SPI_MASTER_bm; // Master, /4 prescaler, MSB first
    SPI0.CTRLB = SPI_SSD_bm; // Mode 0, client select disable, unbuffered
    SPI0.INTCTRL|= 0b00000001;
    PORTB_OUTSET= PIN1_bm ;//disp en
    PORTB.DIRSET= PIN1_bm ;
    PORTA_OUTSET= PIN1_bm;  // &   set bit  
    PORTA.DIRSET= PIN1_bm;  //disp latch 
    SPI0.CTRLA |= SPI_ENABLE_bm; // Enable
    sei();
}
void spi_write(uint8_t data) {
    SPI0.DATA = data; // Note DATA register used for both Tx and Rx
}


ISR(SPI0_INT_vect){//SPI->shift register  ISR triggers it means the transimition of the data is done 
    PORTA_OUT&=0b11111101;//or 
    PORTA_OUT|=PIN1_bm;
};