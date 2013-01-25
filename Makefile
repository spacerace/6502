OUTFILE=6502

CC=gcc
LD=gcc
CFLAGS=-march=native -pipe
WARN=-Wall
ERR=-Werror
OPT=-O3
DBG=-g
INC=-I./include/
LINK=
SRC=main.c
SRC+=tables.c
SRC+=6502.c

6502: 6502.o tables.o main.o
	$(LD) -o $(OUTFILE) 6502.o tables.o main.o

main.o: main.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c main.c

tables.o: tables.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c tables.c

6502.o: 6502.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c 6502.c	

clean:
	rm 6502.o
	rm main.o
	rm tables.o
	rm 6502
