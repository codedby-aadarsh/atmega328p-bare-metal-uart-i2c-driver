#include <avr/io.h>
#include <avr/common.h>

#define START 0x08
#define MT_SLA_W_ACK 0x18
#define MT_DATA_ACK 0x28
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

void twi_init()
{
    TWBR = 72;          // bit rate
    TWSR = 0x00;        // choosing the prescalar to the 1.
    TWCR = (1 << TWEN); // twi enable bit
}

int twi_start()
{
    TWCR |= (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)))
        ;
    if ((TWSR & 0xF8) != START)
        return 1;
    return 0;
}

int twi_write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    if ((TWSR & 0xF8) != MT_SLA_W_ACK && (TWSR & 0xF8) != MT_DATA_ACK)
        return 1;
    return 0;
}
void twi_stop()
{
    TWCR |= (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
}

// SSD1306 minimal init sequence - sends command bytes to light up the display
// Uses your existing twi_start/twi_write/twi_stop functions

#define OLED_ADDR 0x3C

void oled_send_command(uint8_t cmd)
{
    twi_start();
    twi_write((OLED_ADDR << 1) | 0); // address + write
    twi_write(0x00);                 // control byte: 0x00 = command follows
    twi_write(cmd);
    twi_stop();
}

void oled_init(void)
{
    oled_send_command(0xAE); // display off
    oled_send_command(0xD5); // set display clock div
    oled_send_command(0x80);
    oled_send_command(0xA8); // set multiplex
    oled_send_command(0x3F); // 0x3F for 128x64, use 0x1F for 128x32
    oled_send_command(0xD3); // set display offset
    oled_send_command(0x00);
    oled_send_command(0x40); // set start line = 0
    oled_send_command(0x8D); // charge pump
    oled_send_command(0x14); // enable charge pump
    oled_send_command(0x20); // memory addressing mode
    oled_send_command(0x00); // horizontal addressing mode
    oled_send_command(0xA1); // segment re-map
    oled_send_command(0xC8); // COM output scan direction
    oled_send_command(0xDA); // set COM pins
    oled_send_command(0x12);
    oled_send_command(0x81); // set contrast
    oled_send_command(0xCF);
    oled_send_command(0xD9); // set precharge
    oled_send_command(0xF1);
    oled_send_command(0xDB); // set VCOM detect
    oled_send_command(0x40);
    oled_send_command(0xA4); // entire display on (resume from RAM)
    oled_send_command(0xA6); // normal display (not inverted)
    oled_send_command(0xAF); // display ON - this is the payoff
}

// Fill the whole screen with lit pixels, so you see something without
// needing full framebuffer/addressing logic yet.
void oled_fill_screen(void)
{
    // set column address range: 0 to 127
    oled_send_command(0x21);
    oled_send_command(0);
    oled_send_command(127);
    // set page address range: 0 to 7 (for 128x64 display)
    oled_send_command(0x22);
    oled_send_command(0);
    oled_send_command(7);

    // now send data bytes (0xFF = all 8 pixels in this byte lit)
    for (int page = 0; page < 8; page++)
    {
        twi_start();
        twi_write((OLED_ADDR << 1) | 0);
        twi_write(0x40); // control byte: 0x40 = data follows
        for (int col = 0; col < 128; col++)
        {
            twi_write(0xFF);
        }
        twi_stop();
    }
}

// In main(), after your existing setup:
//   twi_init();
//   oled_init();
//   oled_fill_screen();

int main()
{
    twi_init();
    uart_init(UBRR);
    oled_init();
    uart_send_string("setup  completed\r\n");
    oled_fill_screen();


    while (1)
    {
    }
}