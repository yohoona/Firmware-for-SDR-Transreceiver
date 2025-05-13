#ifndef LCD_H
#define LCD_H

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define CTRL_PORT PORTA
#define CTRL_DDR DDRA
#define RS PA7
#define EN PA6

#define DATA_HI_PORT PORTA
#define DATA_HI_DDR DDRA
#define D0 PA5
#define D1 PA4
#define D2 PA3
#define D3 PA2
#define D4 PA1
#define D5 PA0

#define DATA_LO_PORT PORTE
#define DATA_LO_DDR DDRE
#define D6 PE5
#define D7 PE6

#define CMD_CLEAR 0x01
#define CMD_LEFT_TO_RIGHT 0x06
#define CMD_CURSOR_HOME 0x02
#define CMD_CURSOR_OFF 0x0C
#define CMD_CURSOR_BLINK 0x0E
#define CMD_CURSOR_LINE1 0x80
#define CMD_CURSOR_LINE2 0xC0
#define CMD_CURSOR_LEFT 0x10
#define CMD_CURSOR_RIGHT 0x14
#define CMD_DISP_LEFT 0x18
#define CMD_DISP_RIGHT 0x1C
#define CMD_2LINE_8BIT 0x38
#define CMD_2LINE_4BIT 0x28
#define CMD_1LINE_8BIT 0x30
#define CMD_1LINE_4BIT 0x20

uint8_t isolate_bit(uint8_t data, uint8_t bit);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_string(uint8_t* str);
void lcd_init();

#endif /*LCD_H*/