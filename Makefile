all : main 

uart: uart/main.c 
	avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -o main.elf uart/main.c
	avr-objcopy -O ihex -R .eeprom main.elf main.hex 

i2c: i2c/main.c 
	avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -o main.elf i2c/main.c
	avr-objcopy -O ihex -R .eeprom main.elf main.hex

flash:
	avrdude -c arduino -p m328p -P /dev/ttyUSB0 -b 115200 -U flash:w:main.hex:i

clean: 
	rm -f main.*