# Atmega328p bare metal embedded 
-  This repo is basically showing the bare metal manipulation of the register to demonstrate the basic of emebedded system fundamentals with no hal or library. In which we cover the following:
1. gpio : controlling & configuring gpio pins.
2. i2c : using the i2c communication protocol to send the i2c command to the oled screen.
3. uart : using the uart protocol to print the strings on the terminal, also doing the loopback, what you type in the terminal is shown back to you.
4. interrupts: using interrupts to do some action, rather than polling , blocking or wasting the precious clock cycles. 