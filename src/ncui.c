#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

#include "nc_ui.h"
#include "c64_colors.h"

#define WIN_SIZE_MIN_X	80
#define WIN_SIZE_MIN_Y	25

static int init_curses();
static void deinit_curses();

int ncurses_ui() {
	printf("starting ncurses user interface...\n");
	if(init_curses()) {
		printf(" error while init ncurses...\n");
		exit(-1);
	}

	printw("6502 debugger - the ncurses-ui\n");

	int winx, winy;

	getmaxyx(stdscr, winy, winx);

	if((winx < WIN_SIZE_MIN_X)||(winy < WIN_SIZE_MIN_Y)) {
		printw("Your console is too small, can't go on.\n");
		printw("You'll need 80x25, now it is %d*%d\n\n", winx, winy);
		printw("Press a key to quit.\n");
		getch();
		deinit_curses();
		exit(-1);
	}

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

