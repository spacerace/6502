CC=gcc
LD=gcc
CFLAGS=-march=native -pipe
# march should be set to your CPU
WARN=-Wall
ERR=-Werror
OPT=-O3
DBG=-g

INC=-I.

LINK=-lm
LINK_SDL=-lSDL -lSDL_gfx
LINK_CURSES=-lncurses

all:	ncurses_color_test.o 
	$(LD) $(LINK) $(LINK_CURSES) -o ncurses_color_test ncurses_color_test.o

ncurses_color_test.o: ncurses_color_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_color_test.c -o ncurses_color_test.o


clean:
	rm -f *.o
	rm -f ncurses_color_test


