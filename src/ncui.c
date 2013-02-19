#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

#include "nc_ui.h"

static int init_curses();
static void deinit_curses();

int ncurses_ui() {
	printf("starting ncurses user interface...\n");
	if(init_curses()) {
		printf(" error while init ncurses...\n");
		exit(-1);
	}

	printw("6502 debugger - the ncurses-ui\n");

	getch();

	deinit_curses();

	return 0;
}

static int init_curses() {
	initscr();
	raw();			// line buffering disabled
	keypad(stdscr, TRUE);	// we want F1, F2, ...
	noecho();		// don't show chars typed into keyboard
	return 0;
}

static void deinit_curses() {
	endwin();
	return;
}

