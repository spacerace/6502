#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

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

	if(0) get_system_shell(NULL);

	getch();
	
	deinit_ncui();
	return 0;
}

static int get_system_shell(char *cmd_str) {
	int sh_ret = -1;

	def_prog_mode();	// save tty modes
	endwin();		// temporarily leave ncurses mode
	
	if(cmd_str == NULL) {
		_logf("ncui-new: get_system_shell() running '%s'", SH_COMMAND);
		sh_ret = system(SH_COMMAND);
	} else {
		_logf("ncui-new: get_system_shell() running '%s'", cmd_str);
		sh_ret = system(cmd_str);
	}

	/* TODO */
	/* add a "wait for keypress" func here */

	_logf("ncui-new: get_system_shell() got '%d' from shell", sh_ret);

	reset_prog_mode();	// go back to ncurses mode
	refresh();

	return sh_ret;
}

static int window_create_main() {
	ui.win_main = newwin(ui.screen_y, ui.screen_x, 0, 0);	// lines, cols, beginy, beginx
	bkgd(COLOR_PAIR(1));
	wbkgd(ui.win_main, COLOR_PAIR(1));

	wattrset(ui.win_main, A_BOLD|COLOR_PAIR(4));	
	mvwprintw(ui.win_main, 0, 0, "File Debugger Settings Windows Help");
	refresh();
	wrefresh(ui.win_main);

	return 0;
}

static int window_update_main() {
	wrefresh(ui.win_main);
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

	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_BLACK, COLOR_RED);
	init_pair(4, COLOR_YELLOW, COLOR_RED);

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


