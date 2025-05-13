#define F_CPU 1000000UL
#define VCO_F 600 // MHz

#include "common.h"

int main() {
    /* initialization sequence */
    lcd_init();
    
    lcd_command(CMD_CLEAR);

    init_pins();
    
    bool enabled = true;

    twi_init();
    si5351_init();
    reset_pll();

    setup_PLL(SI5351_PLL_A, multiplier, 0, 1);
    setup_clock(SI5351_PLL_A, SI5351_PORT0, divisor, numerator, denominator);
    setup_clock(SI5351_PLL_A, SI5351_PORT1, divisor, numerator, denominator);
    
    set_phase(divisor); // flipped offset?

    enable_clocks(enabled);

    last_state = (DDRD & (1 << DDD2)) >> DDD2; // read value of pin D2
    
    uart_init();

    interrupt_setup();
    
    float_to_ints();
    
    int_to_string(output_whole);
    
    lcd_string(output_f_str);
    lcd_string(".");
    
    int_to_string(output_dec);
    lcd_string(output_f_str);
    
    lcd_command(CMD_CURSOR_RIGHT);
    lcd_string("MHz");
    /* end of initialization sequence */
    
    while(1) {
        mode_select(); // this line determine manual vs remote tx/rx
        if (lcd_update) {
            set_phase(divisor);
            lcd_command(CMD_CLEAR);
            float_to_ints();

            int_to_string(output_whole);

            lcd_string(output_f_str);
            lcd_string(".");

            int_to_string(output_dec);
            lcd_string(output_f_str);
            lcd_command(CMD_CURSOR_RIGHT);
            lcd_string("MHz");
            lcd_update = false;
        }
    }
}