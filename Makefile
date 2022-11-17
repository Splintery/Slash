
CC ?= gcc
CCFLAGS ?= -Wall -pedantic -g -Wextra -std=c99

all: slash

slash: slash.c mystring.o
	$(CC) $(CCFLAGS) -o slash slash.c mystring.o -lreadline

mystring.o: mystring.c
	$(CC) $(CCFLAGS) -o mystring.o -c mystring.c -lreadline

clean:
	rm -f slash mystring.o
