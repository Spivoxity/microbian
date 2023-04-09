# microbian/Makefile
# Copyright (c) 2020 J. M. Spivey

BOARD = ubit-v1
CHIP = cortex-m0
MPX = mpx-m0
LSCRIPT = nRF51822.ld

EXAMPLES = ex-level.hex ex-valentine.hex ex-echo.hex ex-remote.hex

all: microbian.a startup.o

examples: $(EXAMPLES)

ex-level.elf: accel.o

CPU = -mcpu=$(CHIP) -mthumb
CFLAGS = -O -g -Wall -ffreestanding -I $(BOARD)
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar

vpath %.h $(BOARD)
vpath %.c $(BOARD)

DRIVERS = timer.o serial.o i2c.o radio.o display.o adc.o

MICROBIAN = microbian.o $(MPX).o $(DRIVERS) lib.o

%.hex: %.elf
	arm-none-eabi-objcopy -O ihex $< $@

%.elf: %.o startup.o microbian.a
	$(CC) $(CPU) $(CFLAGS) -T $(LSCRIPT) -nostdlib $^ -lc -lgcc -o $@

microbian.a: $(MICROBIAN)
	$(AR) cr $@ $^

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) -c $< -o $@ 

%.o: %.s
	$(AS) $(CPU) $< -o $@

clean: force
	rm -f microbian.a *.o *.elf *.hex

force:

###

$(MICROBIAN) startup.o: microbian.h hardware.h lib.h
