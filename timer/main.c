#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t tick_ms = 0;
volatile uint32_t last_ms = 0;

ISR(TIMER0_COMPA_vect)
{
    tick_ms++;
}

/* This function supposedly configures the 8 bit timere to the ctc mode */
void timer_init(uint8_t value)
{
    /* This selects the mode of timer to be ctc */
    TCCR0A |= (1 << WGM01);

    /* This one selects the prescalar value to be 64,
    allows you to select the external or internal clock
    options too
    */
    TCCR0B |= ((1 << CS00) | (1 << CS01));

    /* To generate the tick of the 1ms , this is the value of the register we have to keep */
    OCR0A = value;

    /* enabling the interrupt bit, so the global interrupt register works */
    TIMSK0 |= (1 << OCIE0A);
}

uint32_t get_millis()
{
    uint32_t val;
    cli();
    val = tick_ms;
    sei();
    return val;
}

volatile uint32_t int_flag = 0;

ISR(INT0_vect)
{
    last_ms = tick_ms;
    int_flag = 1;
}

int main()
{

    /* do the setup thing  here */
    timer_init(50);

    /* setting the portb pin pb5 as output */
    DDRB |= (1 << PB5);

    /* enabling the pull up resistor on the port d pin 2 */
    PORTD |= (1 << PD2);

    /* This selects to generate the interrupt on the falling edge
    of clock
    */
    EICRA |= (1 << ISC01);
    /* This is selcting the interrupt for the int0 */
    EIMSK |= (1 << INT0);

    sei();
    /* This is your main loop */
    while (1)
    {
        if (int_flag)
        {
            uint32_t now = get_millis();
            if (now - last_ms > 200)
            {
                PORTB ^= (1 << PB5);
            }
            int_flag = 0;
        }
    }

    return 0;
}
