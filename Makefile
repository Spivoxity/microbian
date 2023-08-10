# microbian/Makefile
# Copyright (c) 2020 J. M. Spivey

include config.mk

EXAMPLES = examples/ex-level.hex examples/ex-valentine.hex examples/ex-echo.hex examples/ex-remote.hex \
	examples/ex-timeout.hex examples/ex-rng.hex

INCLUDE = incl

all: microbian.a startup.o

examples: $(EXAMPLES)

ex-level.elf: drivers/accel.o examples/ex-level.o examples/ex-level.elf

CPU = -mcpu=$(CHIP) -mthumb
CFLAGS = -O -g -Wall -ffreestanding -I $(INCLUDE) -I $(BOARD)
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar

vpath %.c $(BOARD)

DRIVERS = drivers/accel.o drivers/timer.o drivers/serial.o drivers/i2c.o drivers/radio.o drivers/display.o drivers/adc.o drivers/rng.o

MICROBIAN = microbian.o $(MPX).o defines.o process.o lib.o $(DRIVERS)

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

%.h: %.f
	./hwdesc $< >$@

clean: force
	rm -f microbian.a *.o *.elf *.hex drivers/*.o examples/*.o

force:

###

$(MICROBIAN) startup.o: $(INCLUDE)/microbian.h $(INCLUDE)/lib.h $(BOARD)/hardware.h
