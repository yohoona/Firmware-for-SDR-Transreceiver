#include "common.h"

float output = 14.0735; // MHz
int output_whole;
int output_dec;
const uint8_t multiplier = 24;
uint8_t divisor = 42;
uint32_t numerator = 6333;
uint32_t denominator = 10000;
int current_state;
int last_state;
int dt_state;
uint8_t output_f_str[3];
uint8_t mode[2];
float digit;
bool lcd_update = false;
uint8_t CAT_string[20];

volatile uint8_t buffer[20];
volatile int index = 0;

void calculate_divisor() {
    divisor = (uint8_t) (VCO_F / output);
}

void decimal_to_fraction() { 
    float decimal = (float)(VCO_F / output) - divisor;
    denominator = 100; // Precision of 5 decimal places
    numerator = (uint32_t)(decimal * denominator);
    
    // Simplify the fraction using the GCD (greatest common divisor)
    int gcd = 1;
    for (int i = 1; i <= numerator && i <= denominator; i++) {
        if (numerator % i == 0 && denominator % i == 0) {
            gcd = i;
        }
    }
    
    numerator /= (uint32_t)gcd;
    denominator /= (uint32_t)gcd;
}


void float_to_ints() {
    output_whole = (int)output;
    output_dec = (output - output_whole) * 1000;
}

int float_to_int() {
    return (int)((int)output * 1000 + ((output - (int)output) * 1000));
}

void int_to_string(int num) {
    int i = 0;

    do {
        output_f_str[i] = (num % 10) + '0'; // Convert digit to ASCII
        i++;
        num /= 10;
    } while (num > 0);


    output_f_str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = output_f_str[j];
        output_f_str[j] = output_f_str[k];
        output_f_str[k] = temp;
    }
}

void int_to_string_cat(int num) {
    int i = 0;
    bool small = false;
    
    if (num < 10000)
        small = true;

    do {
        CAT_string[i] = (num % 10) + '0'; // Convert digit to ASCII
        i++;
        num /= 10;
    } while (num > 0);

    if (small)
        CAT_string[i++] = '0';

    CAT_string[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = CAT_string[j];
        CAT_string[j] = CAT_string[k];
        CAT_string[k] = temp;
    }
}

void handle_TX() {
    if (buffer[2] == '\0') {
        if ((PINC & (1 << PINC7)) >> PINC7)
            uart_transmit_str("\r\n>TX0");
        else
            uart_transmit_str("\r\n>TX1");
    } else if (buffer[2] == '0') {
        PORTB |= (1 << PB7);
        PORTB &= ~(1 << PB6);
        PORTC |= (1 << PC7);
    } else if (buffer[2] == '1') {
        PORTB |= (1 << PB6);
        PORTB &= ~(1 << PB7);
        PORTC &= ~(1 << PC7);
    } else {
        uart_transmit_str("\r\n>Invalid Command\r\n");
    }
}

void handle_FA() {
    int out = float_to_int();
    int_to_string_cat(out);
    uart_transmit_str("\r\n>FA");
    uart_transmit_str(CAT_string);
    uart_transmit_str("000\r\n");
    if (buffer[2] == '\0') {
        int out = float_to_int();
        int_to_string_cat(out);
        uart_transmit_str("\r\n>FA");
        uart_transmit_str(CAT_string);
        uart_transmit_str("000\r\n");
    } else {
        char* endptr;
        int f = strtol((char*)(buffer + 2), &endptr, 10);
        output = (float) f / (float) 1000000.0;
        
        float_to_ints();

        int_to_string(output_whole);

        lcd_string(output_f_str);
        lcd_string(".");

        int_to_string(output_dec);
        lcd_string(output_f_str);
        lcd_command(CMD_CURSOR_RIGHT);
        lcd_string("MHz");
    }
}

void handle_FB() {
    return;
}

void handle_IF() {
    int out = float_to_int();
    int_to_string_cat(out);
    
    uart_transmit_str("\r\n>IF");
    uart_transmit_str("000");
    // operating frequency
    uart_transmit_str(CAT_string);
    uart_transmit_str("000");
    
    uart_transmit_str("0000000000000\r\n");
    
}

void handle_string() {
    uint8_t prefix[2];
    prefix[0] = buffer[0];
    prefix[1] = buffer[1];
    prefix[2] = '\0';
    
    if (!strcmp(prefix, "TX")) {
        handle_TX();
    } else if (!strcmp(prefix, "FA")) {
        handle_FA();
    } else if (!strcmp(prefix, "FB")) {
        handle_FB();
    } else if (!strcmp(prefix, "IF")) {
        handle_IF();
    } else {
        uart_transmit_str("\r\n>Invalid Command\r\n");
    }
}

void interrupt_setup() {
    cli();                  // disable global interrupts
    
    DDRD &= ~(1 << DDD2);   // set D2 (INT0) as input
    PORTD |= (1 << PORTD2); // enable pullup on D2

    DDRD &= ~(1 << DDD3);   // set D3 as input (DT)
    PORTD |= (1 << PORTD3); // enable pullup on D3 
    
    EICRA &= ~(1 << ISC01); // bit 1 of EICRA set to 0
    EICRA |= (1 << ISC00);  // bit 0 of EICRA set to 1
                            // ISC0 = 01, any logical change generates interrupt request
    
    UCSR0B |= (1 << RXCIE); // enable UART RX complete interrupts

    EIMSK |= (1 << INT0);   // enable interrupts on INT0
    
    sei();                  // enable global interrupts
}

ISR(INT0_vect) {
    current_state = (PIND & (1 << PIND2)) >> PIND2;
    dt_state = (PIND & (1 << PIND3)) >> PIND3;
    
    // digit selection (is there a better way?)
    if ((PINE & (1 << PINE3)) >> PINE3) {
        digit = 1;
    } else if ((PINE & (1 << PINE2)) >> PINE2) {
        digit = 0.1;
    } else if ((PIND & (1 << PIND7)) >> PIND7) {
        digit = 0.01;
    } else if ((PIND & (1 << PIND6)) >> PIND6) {
        digit = 0.001;
    }
    
    if (current_state != last_state && current_state == 1) {
        if (current_state != dt_state)
            output -= digit;
        else
            output += digit;
    }
    calculate_divisor();
    decimal_to_fraction();
    setup_clock(SI5351_PLL_A, SI5351_PORT0, divisor, numerator, denominator);
    setup_clock(SI5351_PLL_A, SI5351_PORT1, divisor, numerator, denominator);
    last_state = current_state;
    lcd_update = true;
}

ISR(USART0_RX_vect) {
    uint8_t received = UDR0;
    
    //UCSR0A |= (1 << RXC); // clear complete flag
    
    if (received == ';') {
        buffer[index] = '\0'; // null terminate string
        handle_string();
        index = 0;
    } else if (received == 'a') {
        output += 1;
    } else if (received == 'd') {
        output -= 1;
    } else if (received == 'w') {
        output += 0.1;
    } else if (received == 's') {
        output -= 0.1;
    } else {
        buffer[index++] = received;
        if (index >= 20)
            index = 0;
    }
    
    calculate_divisor();
    decimal_to_fraction();
    lcd_update = true;
    
    uart_transmit(received); // echo back for debugging
}

void mode_select() {
    if ((PINE & (1 << PINE0)) >> PINE0) {
        // turn on right led
        PORTB |= (1 << PB7);
        PORTB &= ~(1 << PB6);
        mode[0] = 'R';
        mode[1] = 'X';
        PORTC |= (1 << PC7);
    } else {
        PORTB |= (1 << PB6);
        PORTB &= ~(1 << PB7);
        mode[0] = 'T';
        mode[1] = 'X';
        PORTC &= ~(1 << PC7);
    }
}

void init_pins() {
    /* pin setups */ // maybe put this into an initialization sequence function
    DDRC |= (1 << DDC7); // set pin 26 as output
    
    // pins 18, 17 -> E3, E2 (inputs)
    DDRE &= ~(1 << DDE3) | ~(1 << DDE2);
    PORTE |= (1 << PORTE3) | (1 << PORTE2); // enable pullup
    // pins 16, 15 -> D7, D6 (inputs)
    DDRD &= ~(1 << DDD7) | ~(1 << DDD6);
    PORTD |= (1 << PORTD7) | (1 << PORTD6); // enable pullup
    
    // pin 13 -> D4
    DDRD &= ~(1 << DDD4);
    PORTD |= (1 << PORTD4); // enable pullup
    
    // pin 12, 11 (rotary encoder) -> setup in interrupt enabling
    
    // pin 7 -> E0
    DDRE &= ~(1 << DDE0);
    PORTE |= (1 << PORTE0); // enable pullup
    
    // pin 3, 2 -> B7, B6
    DDRB |= (1 << DDB7) | (1 << DDB6);
    /* end of pin setups*/
}