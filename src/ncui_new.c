#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "6502.h"
#include "ncui_new.h"
#include "log.h"

static struct ncui_type {
	WINDOW *win_main;
	int screen_x;
	int screen_y;
} ui;

int ncui() {
	init_ncui();
	window_create_main();
	window_update_main();
	deinit_ncui();
	return 0;
}

static int window_create_main() {
	return 0;
}

static int window_update_main() {
	return 0;
}

static int init_ncui() {
	if(!initscr()) {
		_logf("ncui-new: can't init ncurses! exiting...");
		exit(-1);
	}

	clear();
	curs_set(0);
	cbreak();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	if(start_color()) {
		_logf("ncui-new: can't start ncurses color mode! exiting...");
		exit(-2);
	}

	_logf("ncui-new: ncurses init ok!");

	getmaxyx(stdscr, ui.screen_y, ui.screen_x);
	
	_logf("ncui-new: screen size is %d*%d chars", ui.screen_y, ui.screen_x);


	return 0;
}

static int deinit_ncui() {
	delwin(ui.win_main);

	endwin();

	_logf("ncui-new: exiting...");
	
	return 0;
}


