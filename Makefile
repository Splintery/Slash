
CC ?= gcc
CCFLAGS ?= -Wall -pedantic -g -Wextra -D_XOPEN_SOURCE=700 -std=c99

all: slash

slash: slash.c mystring.o command.o wildcards.o
	$(CC) $(CCFLAGS) -o slash slash.c mystring.o command.o wildcards.o -lreadline

mystring.o: mystring.c
	$(CC) $(CCFLAGS) -o mystring.o -c mystring.c -lreadline

command.o: command.c
	$(CC) $(CCFLAGS) -o command.o -c command.c -lreadline

wildcards.o: wildcards.c
	$(CC) $(CCFLAGS) -o wildcards.o -c wildcards.c -lreadline

clean:
	rm -f slash mystring.o command.o wildcards.o
