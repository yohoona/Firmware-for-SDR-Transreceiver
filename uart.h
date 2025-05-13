#ifndef UART_H
#define UART_H

#define F_CPU 1000000UL
#define BAUD_RATE 115200

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* FT232 Pins:
    PIN8 E1 // not used since in async mode (uart not usart)
    PIN9 D0 RXD
    PIN10 D1 TXD
*/

/* Registers:
    tx/rx data buffer "UDR"

    USART control/status register A "UCSR0A"
        b7 - receive complete
        b6 - transmit complete
        b5 - data reg empty

    USART control/status register B "UCSR0B"
        b7/6/5 - interrupt enable bits
        b4/3 - rx/tx enable -> drive both to 1
        b2 - data size - use "0" for 8-bit
        b1/0 - bit 8

    USART control/status register C "UCSR0C"
        b7/6 - mode select - "00" is async
        b5/4 - parity - "00" to disable
        b3 - stop bit select - "0" is 1-bit
        b2/1 - data size - use "11" for 8-bit

    baud rate registers UBRRH/L
        b11-0 - USART baud rate        
*/

void uart_init();
void uart_transmit(uint8_t data);
uint8_t uart_receive();
void uart_transmit_str(uint8_t* str);

#endif /*UART_H*/