CC=gcc
LD=gcc
CFLAGS=-march=native -pipe
# march should be set to your CPU
WARN=-Wall
ERR=-Werror
OPT=-O3
DBG=-g

INC=-I.

LINK=
LINK_SDL=-lSDL -lSDL_gfx
LINK_CURSES=-lncurses
LINK_CURSES+=-lpanel
LINK_CURSES+=-lmenu
LINK_CURSES+=-lform

all:	ncurses_color_test.o ncurses_panel_test.o ncurses_menu_test.o ncurses_menu2_test.o ncurses_form1_test.o ncurses_form2_test.o ncurses_form3_test.o ncurses_mouse_test.o
	$(LD) $(LINK) $(LINK_CURSES) -o ncurses_color_test   ncurses_color_test.o
	$(LD) $(LINK) $(LINK_CURSES) -o ncurses_panel_test   ncurses_panel_test.o
	$(LD) $(LINK) $(LINK_CURSES) -o ncurses_menu_test    ncurses_menu_test.o
	$(LD) $(LINK) $(LINK_CURSES) -o ncurses_menu2_test   ncurses_menu2_test.o
	$(LD) $(LINK) $(LINK_CURSES) -o ncurses_form1_test   ncurses_form1_test.o
	$(LD) $(LINK) $(LINK_CURSES) -o ncurses_form2_test   ncurses_form2_test.o
	$(LD) $(LINK) $(LINK_CURSES) -o ncurses_form3_test   ncurses_form3_test.o
	$(LD) $(LINK) $(LINK_CURSES) -o ncurses_mouse_test   ncurses_mouse_test.o

ncurses_color_test.o:	ncurses_color_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_color_test.c -o ncurses_color_test.o

ncurses_panel_test.o:	ncurses_panel_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_panel_test.c -o ncurses_panel_test.o

ncurses_menu_test.o:	ncurses_menu_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_menu_test.c -o ncurses_menu_test.o

ncurses_menu2_test.o:	ncurses_menu2_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_menu2_test.c -o ncurses_menu2_test.o

ncurses_form1_test.o:	ncurses_form1_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_form1_test.c -o ncurses_form1_test.o

ncurses_form2_test.o:   ncurses_form2_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_form1_test.c -o ncurses_form2_test.o

ncurses_form3_test.o:   ncurses_form3_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_form1_test.c -o ncurses_form3_test.o

ncurses_mouse_test.o:	ncurses_mouse_test.c
	$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c ncurses_mouse_test.c -o ncurses_mouse_test.o

clean:
	rm -f *.o
	rm -f ncurses_color_test
	rm -f ncurses_panel_test
	rm -f ncurses_menu_test
	rm -f ncurses_menu2_test
	rm -f ncurses_form1_test
	rm -f ncurses_form2_test
	rm -f ncurses_form3_test
	rm -f ncurses_mouse_test
