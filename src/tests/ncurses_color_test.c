#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

#define C64_COLORS_0_BLACK_R	0
#define C64_COLORS_0_BLACK_G	0
#define C64_COLORS_0_BLACK_B	0

#define C64_COLORS_1_WHITE_R	1000
#define C64_COLORS_1_WHITE_G	1000
#define C64_COLORS_1_WHITE_B	1000

#define C64_COLORS_2_RED_R    0
#define C64_COLORS_2_RED_G    0
#define C64_COLORS_2_RED_B    0

#define C64_COLORS_3_CYAN_R    0
#define C64_COLORS_3_CYAN_G    0
#define C64_COLORS_3_CYAN_B    0

#define C64_COLORS_4_PURPLE_R    0
#define C64_COLORS_4_PURPLE_G    0
#define C64_COLORS_4_PURPLE_B    0

#define C64_COLORS_5_GREEN_R    0
#define C64_COLORS_5_GREEN_G    0
#define C64_COLORS_5_GREEN_B    0

#define C64_COLORS_6_BLUE_R    0
#define C64_COLORS_6_BLUE_G    0
#define C64_COLORS_6_BLUE_B    0

#define C64_COLORS_7_YELLOW_R    0
#define C64_COLORS_7_YELLOW_G    0
#define C64_COLORS_7_YELLOW_B    0

#define C64_COLORS_8_ORANGE_R    0
#define C64_COLORS_8_ORANGE_G    0
#define C64_COLORS_8_ORANGE_B    0

#define C64_COLORS_9_BROWN_R    0
#define C64_COLORS_9_BROWN_G    0
#define C64_COLORS_9_BROWN_B    0

#define C64_COLORS_10_LRED_R    0
#define C64_COLORS_10_LRED_G    0
#define C64_COLORS_10_LRED_B    0

#define C64_COLORS_11_BLACK_R    0
#define C64_COLORS_11_BLACK_G    0
#define C64_COLORS_11_BLACK_B    0

#define C64_COLORS_12_BLACK_R    0
#define C64_COLORS_12_BLACK_G    0
#define C64_COLORS_12_BLACK_B    0

#define C64_COLORS_13_BLACK_R    0
#define C64_COLORS_13_BLACK_G    0
#define C64_COLORS_13_BLACK_B    0

#define C64_COLORS_14_BLACK_R    0
#define C64_COLORS_14_BLACK_G    0
#define C64_COLORS_14_BLACK_B    0

#define C64_COLORS_15_BLACK_R    0
#define C64_COLORS_15_BLACK_G    0
#define C64_COLORS_15_BLACK_B    0

static int init_curses();
static void deinit_curses();

int main() {
	if(init_curses()) {
		printf(" error while init ncurses...\n");
		exit(-1);
	}

	printw("6502 debugger tests - ncurses color test\n\n");

	if(has_colors() == FALSE) {
		deinit_curses();
		printf("your terminal does not support colors\n");
		exit(-1);
	}

	start_color();

	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);

	attron(COLOR_PAIR(1));
	printw(" COLOR_PAIR 1 : RED/BLACK\n");
	attroff(COLOR_PAIR(1));

        attron(COLOR_PAIR(2));
        printw(" COLOR_PAIR 2 : GREEN/BLACK\n");
        attroff(COLOR_PAIR(2));

        attron(COLOR_PAIR(3));
        printw(" COLOR_PAIR 3 : YELLOW/BLACK\n");
        attroff(COLOR_PAIR(3));

        attron(COLOR_PAIR(4));
        printw(" COLOR_PAIR 4 : BLUE/BLACK\n");
        attroff(COLOR_PAIR(4));

        attron(COLOR_PAIR(5));
        printw(" COLOR_PAIR 5 : MAGENTA/BLACK\n");
        attroff(COLOR_PAIR(5));

        attron(COLOR_PAIR(6));
        printw(" COLOR_PAIR 6 : CYAN/BLACK\n");
        attroff(COLOR_PAIR(6));

        attron(COLOR_PAIR(7));
        printw(" COLOR_PAIR 7 : WHITE/BLACK\n");
        attroff(COLOR_PAIR(7));


	printw("\n");

	if(can_change_color()) {
		printw("your terminal can change colors, so lets try to emulate the C64 color palette...\n");
	} else {
		printw("your terminal can't change colors, we can't try to emulate the C64 color palette...\n");
	}

	printw("\n\npress a key to continue...\n");
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

