AS=arm-unknown-eabi-as
LD=arm-unknown-eabi-ld
CC=arm-unknown-eabi-gcc

CFLAGS=-I$(BASEDIR)/include

PLATFORM_O := uart.o platform.o

platform.a : $(PLATFORM_O)
	ar q $@ $^

clean:
	rm -f $(PLATFORM_O) platform.a
