
CC ?= gcc
CCFLAGS ?= -Wall -pedantic -g -Wextra -std=c99

all: slash

slash: slash.c mystring.o command.o
	$(CC) $(CCFLAGS) -o slash slash.c mystring.o command.o -lreadline

mystring.o: mystring.c
	$(CC) $(CCFLAGS) -o mystring.o -c mystring.c -lreadline

command.o: command.c
	$(CC) $(CCFLAGS) -o command.o -c command.c -lreadline

clean:
	rm -f slash mystring.o
