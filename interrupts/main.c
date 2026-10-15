#include <avr/common.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile int int_flag = 0;

ISR(INT0_vect)
{
    int_flag = 1;
    _delay_ms(200);
}

int main()
{

    /* enabling the pull up resistor on the port d pin 2 */
    PORTD |= (1 << PD2);

    /* setting the gpio pin d10 as the output */
    DDRB |= (1 << PB2);

    /* This selects to generate the interrupt on the falling edge
    of clock
    */
    EICRA |= (1 << ISC01);
    /* This is selcting the interrupt for the int0 */
    EIMSK |= (1 << INT0);

    /* enables the interrupt or the global enable interrupt bit */
    sei();

    while (1)
    {
        if (int_flag == 1)
        {   
            /* toggling the d10 pin output on interrupt */
            PORTB ^= (1 << PB2);
            int_flag = 0;
        }
    }
}