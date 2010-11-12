CC=msp430-gcc
CFLAGS=-Os -Wall -g -mmcu=msp430x2012

OBJS=prog1.o


all: $(OBJS)
	$(CC) $(CFLAGS) -o prog1.elf $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -fr prog1.elf $(OBJS)
