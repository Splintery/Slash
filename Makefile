
CC ?= gcc
CCFLAGS ?= -g -Wall -Wextra -std=c99

all: slash

slash: slash.c mystring.o
	$(CC) $(CCFLAGS) -o slash slash.c mystring.o

mystring.o: mystring.c
	$(CC) $(CCFLAGS) -o mystring.o -c mystring.c

clean:
	rm -f slash mystring.o
