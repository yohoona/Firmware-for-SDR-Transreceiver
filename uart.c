#include "uart.h"

void uart_init() {
    // step 1: set baud rate
    UBRR0L = (uint8_t)(BAUD_RATE);
    UBRR0H = (uint8_t)(BAUD_RATE >> 8);
    
    // step 2: set frame format (8 bits, 1 stop bit, no parity), mode select (async)
    UCSR0B &= ~(1 << UCSZ2);
    UCSR0C |= (1 << UCSZ1) | (1 << UCSZ0); // 8-bit frame
    
    UCSR0C &= ~((1 << UMSEL1) | (1 << UMSEL0)); // async mode
    
    UCSR0C &= ~((1 << UPM1) | (1 << UPM0)); // disable parity
    
    UCSR0C &= ~(1 << USBS); // 1 stop bit
    
    // step 3: enable RX, TX
    UCSR0B |= (1 << TXEN) | (1 << RXEN);
}

void uart_transmit(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE))); // infinite loop to wait until data buffer is empty
    UDR0 = data;
}

uint8_t uart_receive() {
    while (!(UCSR0A & (1 << RXC))); // infinite loop to wait until receive is complete
    return UDR0;
}

void uart_transmit_str(uint8_t* str) {
    while (*str)
        uart_transmit(*str++);
}