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

INC=-I./src/include/

LINK=-lm
LINK+=-lpthread
LINK+=-lSDL -lSDL_gfx
LINK+=-lncurses

6502:	6502.o tables.o main.o mmio.o random.o nc_ui.o nc_io.o 
	$(LD) $(LINK) -o $(OUTFILE) 6502.o tables.o main.o mmio.o random.o nc_ui.o nc_io.o
	size 6502

random.o: src/random.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/random.c -o random.o

main.o: src/main.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/main.c -o main.o

tables.o: src/tables.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/tables.c -o tables.o

6502.o: src/6502.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/6502.c -o 6502.o

mmio.o: src/mmio.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio.c -o mmio.o

nc_ui.o: src/nc_ui.o
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/nc_ui.c -o nc_ui.o

nc_io.o: src/nc_io.o
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/nc_io.c -o nc_io.o

examples:
	echo "building 6502 binary examples..."
	echo " you'll need the xa65 assembler to do this"

clean:
	rm -f 6502.o
	rm -f main.o
	rm -f tables.o
	rm -f random.o
	rm -f mmio.o
	rm -f nc_ui.o
	rm -f nc_io.o
	rm -f 6502

mrproper:
	rm -f 6502.o
	rm -f main.o
	rm -f tables.o
	rm -f random.o
	rm -f mmio.o
	rm -f nc_ui.o
	rm -f nc_io.o
	rm -f 6502
	rm -f .*.c.swp
	rm -f .*.h.swp
	rm -f .Makefile.swp
	rm -f *.c~
	rm -f *.h~
	rm -f Makefile~
	rm -f src/.*.c.swp
	rm -f src/.*.h.swp
	rm -f src/*.c~
	rm -f src/*.h~


