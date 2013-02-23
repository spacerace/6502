#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <panel.h>
#include "nc_ui.h"
#include "c64_colors.h"

#define WIN_SIZE_MIN_X	80
#define WIN_SIZE_MIN_Y	25

#define WIN_SIZE_X	80
#define WIN_SIZE_Y	25

static int init_curses();
static void deinit_curses();

WINDOW *win_main;
WINDOW *win_cpu;
WINDOW *win_mem;
WINDOW *win_disasm;
WINDOW *win_log;
PANEL *pan_main;
PANEL *pan_cpu;
PANEL *pan_mem;
PANEL *pan_disasm;
PANEL *pan_log;

int ncurses_ui() {
	atexit(deinit_curses);

	printf("starting ncurses user interface...\n");
	if(init_curses()) {
		deinit_curses();
		printf(" error while init ncurses...\n");
		exit(-1);
	}

	int winx, winy;

	getmaxyx(stdscr, winy, winx);

	if((winx < WIN_SIZE_MIN_X)||(winy < WIN_SIZE_MIN_Y)) {
		printw("Your console is too small, can't go on.\n");
		printw("You'll need 80x25, now it is %d*%d\n\n", winx, winy);
		printw("Press a key to quit.\n");
		refresh();
		getch();
		deinit_curses();
		exit(-1);
	}

        win_main = newwin(40, 100, 0, 0);   // 40 lines, 100 cols, start y, start x
        win_cpu = newwin(5, 40, 34, 2);
	win_disasm = newwin(30, 30, 3, 2);
	win_mem = newwin(22, 59, 3, 33);
	win_log = newwin(5, 50, 34, 44);
	box(win_log, ACS_VLINE, ACS_HLINE);
        box(win_main, ACS_VLINE, ACS_HLINE);
        box(win_cpu, ACS_VLINE, ACS_HLINE);
	box(win_mem, ACS_VLINE, ACS_HLINE);
	box(win_disasm, ACS_VLINE, ACS_HLINE);

	pan_log = new_panel(win_log);
        pan_main = new_panel(win_main);
        pan_cpu = new_panel(win_cpu);
	pan_mem = new_panel(win_mem);
	pan_disasm = new_panel(win_disasm);

        bkgd(COLOR_PAIR(1));

        wbkgd(win_main, COLOR_PAIR(2));
        wbkgd(win_cpu, COLOR_PAIR(3));
	wbkgd(win_mem, COLOR_PAIR(3));
	wbkgd(win_disasm, COLOR_PAIR(3));
	wbkgd(win_log, COLOR_PAIR(3));

        mvwaddstr(win_main, 0, 3, " 6502 debugger (ncurses) ");


	mvwaddstr(win_cpu, 0, 3, " CPU- and system-status ");
	

	mvwaddstr(win_cpu, 1, 1, " A=$00  X=$00 Y=$00  SP=$ff  PC=$0000");
	mvwaddstr(win_cpu, 2, 1, " cycles: 0           time: 0uS");
	mvwaddstr(win_cpu, 3, 1, " cpu-freq: 1022khz");
	

	mvwaddstr(win_mem, 0, 3, " memory editor ");
	mvwaddstr(win_mem, 2, 2,      "       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
	mvwaddstr(win_mem, 3, 2,      "------------------------------------------------------");
	int base;
	for(base = 0; base < 16; base++)
	mvwaddstr(win_mem, 4+base, 2, "0000:  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");	

	mvwaddstr(win_disasm, 0, 3, " disassembly ");

	mvwaddstr(win_log, 0, 3, " debugger log ");

        update_panels();
        doupdate();

	refresh();
	getch();

	deinit_curses();

	return 0;
}

static int init_curses() {
	initscr();
	clear();
	curs_set(0);		// turn off cursor
	cbreak();
	raw();			// line buffering disabled
	keypad(stdscr, TRUE);	// we want F1, F2, ...
	noecho();		// don't show chars typed into keyboard
	
	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_RED);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);

	return 0;
}

static void deinit_curses() {
	del_panel(pan_main);
	del_panel(pan_cpu);
	del_panel(pan_mem);
	del_panel(pan_disasm);
	del_panel(pan_log);
	delwin(win_main);
	delwin(win_cpu);
	delwin(win_mem);
	delwin(win_disasm);
	delwin(win_log);
	endwin();
	return;
}

