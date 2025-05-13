#include "lcd.h"

uint8_t isolate_bit(uint8_t data, uint8_t bit) { // bit is strictly a number between 0 and 7
    return (data & (1 << bit));
}

void lcd_command(uint8_t cmd) {
    CTRL_PORT &= ~(1 << RS);                // drive RS bit to 0 -> set up command mode
                                            // RW bit is already 0 (grounded) -> write mode
    /* writing data into registers */
    for (int i = 0; i < 6; ++i) {
        if (isolate_bit(cmd, i))            // if the particular data bit is 1
            DATA_HI_PORT |= (1 << (5 - i));
        else
            DATA_HI_PORT &= ~(1 << (5 - i));
    }

    if (isolate_bit(cmd, 6))
        DATA_LO_PORT |= (1 << D6);
    else
        DATA_LO_PORT &= ~(1 << D6);

    if (isolate_bit(cmd, 7))
        DATA_LO_PORT |= (1 << D7);
    else
        DATA_LO_PORT &= ~(1 << D7);
    
    /* sending data to LCD */
    CTRL_PORT |= (1 << EN);                 // drive enable high -> send data
    _delay_us(1);                           // pulse width > 300ns
    CTRL_PORT &= ~(1 << EN);;               // drive enable back low
    
    _delay_ms(2);                           // maximum execution time for command = 1.64ms
}


void lcd_data(uint8_t data) {
    CTRL_PORT |= (1 << RS);                 // drive RS bit to 1 -> set up data mode
                                            // RW bit is already 0 (grounded) -> write mode
    // writing data into registers
    for (int i = 0; i < 6; ++i) {
        if (isolate_bit(data, i))            // if the particular data bit is 1
            DATA_HI_PORT |= (1 << (5 - i));
        else
            DATA_HI_PORT &= ~(1 << (5 - i));
    }

    if (isolate_bit(data, 6))
        DATA_LO_PORT |= (1 << D6);
    else
        DATA_LO_PORT &= ~(1 << D6);

    if (isolate_bit(data, 7))
        DATA_LO_PORT |= (1 << D7);
    else
        DATA_LO_PORT &= ~(1 << D7);
    
    // sending data to LCD
    CTRL_PORT |= (1 << EN);                 // drive enable high -> send data
    _delay_us(1);                           // pulse width > 300ns
    CTRL_PORT &= ~(1 << EN);                // drive enable back low
    
    _delay_ms(2);                           // maximum execution time for command = 1.64ms
}


void lcd_string(uint8_t* str) {
    while (*str) {
        lcd_data(*str++);
    }
}

void lcd_init() {
    // set used pins as outputs (maybe put in init sequence?)
    CTRL_DDR |= (1 << RS) | (1 << EN);
    DATA_HI_DDR |= 0b001111111;
    DATA_LO_DDR |= (1 << D6) | (1 << D7);
    
    CTRL_PORT &= ~(1 << EN);                // why ?
    _delay_ms(50);                          // wait >40ms after power applied;
    lcd_command(CMD_1LINE_8BIT);            // wake command = 0x30?
    _delay_ms(10);                          // wait >5ms
    lcd_command(CMD_1LINE_8BIT);            // wake command # 2
    _delay_us(200);                         // wait >160us
    lcd_command(CMD_1LINE_8BIT);            // wake command # 3
    _delay_us(200);                         // wait >160us
    lcd_command(CMD_2LINE_8BIT);            // set to 2-line, 8-bit
    lcd_command(CMD_CURSOR_LEFT);           // setup cursor
    lcd_command(CMD_CURSOR_OFF);            // idk (0x0C) supposed to turn disp and cursor on
    lcd_command(CMD_LEFT_TO_RIGHT);         // input mode: left to right
    //lcd_command(CMD_CURSOR_HOME);           // might get rid of this wasn't in sample code
}
