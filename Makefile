OUTFILE=6502
TARGET_OS=linux
CC=gcc
LD=gcc
CFLAGS=-march=native -pipe
# march should be set to your CPU
WARN=-Wall
ERR=-Werror
OPT=-O3
DBG=-g

INC=-I./include/

LINK=-lm
LINK+=-lpthread

SRC=main.c
SRC+=tables.c
SRC+=6502.c
SRC+=mmio.c

6502: 6502.o tables.o main.o mmio.o 
	$(LD) $(LINK) -o $(OUTFILE) 6502.o tables.o main.o mmio.o
	size *.o 6502

main.o: main.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c main.c

tables.o: tables.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c tables.c

6502.o: 6502.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c 6502.c	

mmio.o: mmio.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c mmio.c

examples:
	make -f ROM/src/Makefile

clean:
	rm -f 6502.o
	rm -f main.o
	rm -f tables.o
	rm -f mmio.o
	rm -f 6502
