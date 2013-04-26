/* this test code is taken from "http://de.wikibooks.org/wiki/Ncurses:_Panels" and got a bit modified */

#include <panel.h>
#include <stdlib.h>

WINDOW *win1, *win2;
PANEL  *pan1, *pan2;

void quit(void) {
	del_panel(pan1);
	del_panel(pan2);
	delwin(win1);
	delwin(win2);
	endwin();
}

int main(void) {
	int flag=0;

	initscr();
	atexit(quit);
	clear();
	noecho();
	curs_set(0);
	cbreak();
	keypad(stdscr, 1);

	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLUE);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_BLACK, COLOR_YELLOW);

	win1 = newwin(10, 25, 5, 10);
	win2 = newwin(10, 25, 10, 15);
	box(win1, ACS_VLINE, ACS_HLINE);
	box(win2, ACS_VLINE, ACS_HLINE);
	pan1 = new_panel(win1);
	pan2 = new_panel(win2);
  
	bkgd(COLOR_PAIR(1));
	wbkgd(win1, COLOR_PAIR(2));
	wbkgd(win2, COLOR_PAIR(3));

	mvaddstr(2,4, "F9 beendet das Programm");
	mvwaddstr(win1, 2, 3, "Druecke eine Taste");
	mvwaddstr(win2, 7, 3, "Druecke eine Taste");
	
	update_panels();
	doupdate();

	while(getch() != KEY_F(9)) {
		if (flag==0) {
			top_panel(pan1);
			flag = 1; 
		} else {
			top_panel(pan2);
			flag = 0;
		}

		update_panels();
		doupdate();
	} 

	return (0);  
}

