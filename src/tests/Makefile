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
LINK_CURSES_PANEL=-lpanel
LINK_CURSES_MENU=-lmenu

all:	ncurses_color_test.o ncurses_panel_test.o ncurses_menu_test.o ncurses_menu2_test.o
	$(LD) $(LINK) $(LINK_CURSES) -o ncurses_color_test ncurses_color_test.o
	$(LD) $(LINK) $(LINK_CURSES) $(LINK_CURSES_PANEL) -o ncurses_panel_test ncurses_panel_test.c
	$(LD) $(LINK) $(LINK_CURSES) $(LINK_CURSES_MENU) -o ncurses_menu_test ncurses_menu_test.c
	$(LD) -lmenu -lpanel -o ncurses_menu2_test ncurses_menu2_test.c

ncurses_color_test.o:	ncurses_color_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_color_test.c -o ncurses_color_test.o

ncurses_panel_test.o:	ncurses_panel_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_panel_test.c -o ncurses_panel_test.o

ncurses_menu_test.o:	ncurses_menu_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_menu_test.c -o ncurses_menu_test.o

ncurses_menu2_test.o:	ncurses_menu2_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_menu2_test.c -o ncurses_menu2_test.o

clean:
	rm -f *.o
	rm -f ncurses_color_test
	rm -f ncurses_panel_test
	rm -f ncurses_menu_test
	rm -f ncurses_menu2_test
