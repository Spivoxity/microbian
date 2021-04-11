# microbian/Makefile
# Copyright (c) 2020 J. M. Spivey

BOARD = ubit-v1
CHIP = cortex-m0
MPX = mpx-m0

all: microbian.a startup.o

CPU = -mcpu=$(CHIP) -mthumb
CFLAGS = -O -g -Wall -ffreestanding -I $(BOARD)
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar

vpath %.h $(BOARD)

DRIVERS = timer.o serial.o i2c.o radio.o display.o adc.o

MICROBIAN = microbian.o $(MPX).o $(DRIVERS) lib.o

microbian.a: $(MICROBIAN)
	$(AR) cr $@ $^

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) -c $< -o $@ 

%.o: %.s
	$(AS) $(CPU) $< -o $@

clean: force
	rm -f microbian.a *.o

force:

###

$(MICROBIAN) startup.o: microbian.h hardware.h lib.h
