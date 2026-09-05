#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/common.h>

#define F_OSC 16000000
#define BAUD 9600
#define UBRR (F_OSC / 16 / BAUD - 1)

/* uart initilisation code  */
void uart_init(unsigned int baud)
{

    /* setting the baude rate */
    UBRR0H = (unsigned char)(baud >> 8);
    UBRR0L = (unsigned char)(baud);

    /* setting the frame format to the 8 bit , 1 stop bit, no parity, asynchronus mode */
    UCSR0C |= ((1 << UCSZ00) | (1 << UCSZ01));

    /* enabling the reciver and transmitter */
    UCSR0B |= ((1 << RXEN0) | (1 << TXEN0));
}

void uart_send_char(unsigned char ch)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = ch;
}
char uart_recv_char()
{
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}

void uart_send_string(const char *str)
{
    while (*str)
    {
        uart_send_char(*str);
        str++;
    }
}
int main()
{
    /* setting the port b pin 5 to output  */
    DDRB |= (1 << PB5);

    /* uart intitlisation to the 9600 baud rate */
    uart_init(UBRR);

    uart_send_string("Setup complete\r\n");

    while (1)
    {
        char ch = uart_recv_char();
        uart_send_string("recived string:");
        uart_send_char(ch);
        uart_send_string("\r\n");

        if (ch == '1')
            PORTB ^= (1 << PB5);
    }
}