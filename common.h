#ifndef COMMON_H
#define COMMON_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "Si5351.h"
#include "twi.h"
#include "lcd.h"
#include "uart.h"

extern float output;
extern int output_whole;
extern int output_dec;
extern const uint8_t multiplier;
extern uint8_t divisor;
extern uint32_t numerator;
extern uint32_t denominator;
extern int current_state;
extern int last_state;
extern int dt_state;
extern uint8_t output_f_str[3];
extern uint8_t mode[2];
extern float digit;
extern bool lcd_update;
extern uint8_t CAT_string[20];

extern volatile uint8_t buffer[20];
extern volatile int index = 0;

void calculate_divisor();
void decimal_to_fraction();
void float_to_ints();
int float_to_int();
void int_to_string(int num);
void int_to_string_cat(int num);
void handle_TX();
void handle_FA();
void handle_FB();
void handle_IF();
void handle_string();
void interrupt_setup();
ISR(INT0_vect);
ISR(USART0_RX_vect);
void mode_select();
void init_pins();

#endif /*COMMON_H*/