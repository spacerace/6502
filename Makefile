OUTFILE=6502
TARGET_OS=linux
CC=gcc
LD=gcc
CFLAGS=-march=native -pipe
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

clean:
	rm 6502.o
	rm main.o
	rm tables.o
	rm mmio.o
	rm 6502
