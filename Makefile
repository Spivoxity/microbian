# microbian/Makefile
# Copyright (c) 2020 J. M. Spivey

include config.mk

EXAMPLES = ex-level.hex ex-valentine.hex ex-echo.hex ex-remote.hex \
	ex-timeout.hex

all: microbian.a startup.o

examples: $(EXAMPLES)

ex-level.elf: accel.o

CPU = -mcpu=$(CHIP) -mthumb
CFLAGS = -O -g -Wall -ffreestanding -I $(BOARD)
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar

vpath %.c $(BOARD)

DRIVERS = timer.o serial.o i2c.o radio.o display.o adc.o

MICROBIAN = microbian.o $(MPX).o $(DRIVERS) lib.o

microbian.a: $(MICROBIAN)
	$(AR) cr $@ $^

%.hex: %.elf
	arm-none-eabi-objcopy -O ihex $< $@

%.elf: %.o startup.o microbian.a
	$(CC) $(CPU) $(CFLAGS) -T $(LSCRIPT) -nostdlib $^ -lc -lgcc \
		-o $@ -Wl,-Map,$*.map

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) -c $< -o $@ 

%.o: %.s
	$(AS) $(CPU) $< -o $@

%/hardware.h: %/hardware.f hwdesc
	./hwdesc $< >$@

clean: force
	rm -f microbian.a *.o *.elf *.hex *.map

force:

###

$(MICROBIAN) startup.o: \
	microbian.h lib.h $(BOARD)/hardware.h
