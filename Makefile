CROSS_COMPILE?=avr-
CC:=$(CROSS_COMPILE)gcc
CHIP?=atmega328p
OBJCOPY=$(CROSS_COMPILE)objcopy

CFLAGS = -mmcu=$(CHIP) -DF_CPU=16000000UL -Os

all : none

none: 
	@echo "Please specify the target to make"
	@echo "eg. make interrupt; make flash"

% : %/main.c
	$(CC) $(CFLAGS) -o main.elf $<
	$(OBJCOPY) -O ihex -R .eeprom main.elf main.hex

flash:
	avrdude -c arduino -p m328p -P /dev/ttyUSB0 -b 115200 -U flash:w:main.hex:i

clean: 
	rm -f main.hex main.elf