#include <stdio.h>
#include <stdint.h>
#include <ncurses.h>
#include <stdlib.h>
#include <panel.h>

#include "log.h"
#include "6502.h"
#include "nc_ui.h"
#include "c64_colors.h"

#define WIN_SIZE_MIN_X	80
#define WIN_SIZE_MIN_Y	25

#define WIN_SIZE_X	80
#define WIN_SIZE_Y	25

static int init_curses();
static void deinit_curses();

void update_cpu_panel();

#define WIN_LOG		0
#define WIN_CPU		1
#define WIN_MEM		2
#define WIN_DISASM	3
#define RESET		9999

WINDOW *win_log;
WINDOW *win_cpu;
WINDOW *win_mem;
WINDOW *win_disasm;

PANEL *pan_cpu;
PANEL *pan_mem;
PANEL *pan_disasm;
PANEL *pan_log;

static uint16_t rnd_base = 0xfe;
static int active_window = WIN_CPU;

void update_memory_panel(uint16_t start_address);
void update_disasm_panel(uint16_t start_address);
void update_log_panel();
void highlight_window();
void dehighlight_windows();

int ncurses_ui() {
	atexit(deinit_curses);

	init_curses();

	init_mmio();
	rng8_init(rnd_base, 0);
	init6502();
	reset6502();

        win_log = newwin(5, 58, 34, 32);   // 40 lines, 100 cols, start y, start x
        win_cpu = newwin(9, 31, 30, 1);
	win_disasm = newwin(29, 31, 1, 1);
	win_mem = newwin(21, 58, 1, 32);
        box(win_log, ACS_VLINE, ACS_HLINE);
        box(win_cpu, ACS_VLINE, ACS_HLINE);
	box(win_mem, ACS_VLINE, ACS_HLINE);
	box(win_disasm, ACS_VLINE, ACS_HLINE);

        pan_log = new_panel(win_log);
        pan_cpu = new_panel(win_cpu);
	pan_mem = new_panel(win_mem);
	pan_disasm = new_panel(win_disasm);

        wbkgd(win_log, COLOR_PAIR(1));
        wbkgd(win_cpu, COLOR_PAIR(1));
	wbkgd(win_mem, COLOR_PAIR(1));
	wbkgd(win_disasm, COLOR_PAIR(1));

	for(;;) {
		dehighlight_windows();
		highlight_window();
		update_cpu_panel();
		update_memory_panel(0);
		update_disasm_panel(0);	
		update_log_panel();
		refresh();
		switch(getch()) {
			case KEY_F(10):
				exit(0);
				break;
			case KEY_F(5):
				active_window = WIN_CPU;
				break;
			case KEY_F(6):
				active_window = WIN_MEM;
				break;
			case KEY_F(8):
				active_window = WIN_LOG;
				break;
		}


	}

	deinit_curses();

	return 0;
}

void highlight_window() {
	switch(active_window) {
		case WIN_CPU:
			wbkgd(win_cpu, COLOR_PAIR(4));
			break;
		case WIN_MEM:
			wbkgd(win_mem, COLOR_PAIR(4));
			break;
		case WIN_LOG:
			wbkgd(win_log, COLOR_PAIR(4));
			break;
	}
	update_panels();
	doupdate();
	return;
}

void dehighlight_windows() {
	wbkgd(win_disasm, COLOR_PAIR(1));
	wbkgd(win_mem, COLOR_PAIR(1));
	wbkgd(win_cpu, COLOR_PAIR(1));
	wbkgd(win_log, COLOR_PAIR(1));
	update_panels();
	doupdate();
	return;
}

void update_log_panel() {


	wattrset(win_log, COLOR_PAIR(3));
	wattron(win_log, A_BOLD);
	mvwprintw(win_log, 0, 2, "emu log");
	wattroff(win_log, A_BOLD);

        if(active_window == WIN_LOG) {                                                                                                                                                           
	        wattrset(win_log, COLOR_PAIR(3));                                                                                                                                                
	        wattron(win_log, A_BOLD);                                                                                                                                                        
        } else {                                                                                                                                                                                 
	        wattrset(win_log, COLOR_PAIR(4));                                                                                                                                                
	        wattron(win_log, A_BOLD);                                                                                                                                                        
        }                                                                                                                                                                                        
        mvwaddstr(win_log, 0, 54, " F8 ");


	wattrset(win_log, COLOR_PAIR(2));

	mvwprintw(win_log, 1, 2, "hahahah");
	update_panels();
	doupdate();

	return;
}

void update_disasm_panel(uint16_t start_address) {
	wattrset(win_disasm, COLOR_PAIR(3));
	wattron(win_disasm, A_BOLD);
	mvwaddstr(win_disasm, 0, 2, "disassembly");
	wattroff(win_disasm, A_BOLD);

	wattrset(win_disasm, COLOR_PAIR(2));
	update_panels();
	doupdate();

	return;
}

void update_memory_panel(uint16_t start_address) {
	int cpu = get_cpu();

	wattrset(win_mem, COLOR_PAIR(3));                                                                                                                                                        
        wattron(win_mem, A_BOLD);
	mvwaddstr(win_mem, 0, 2, "memory editor");

        if(active_window == WIN_MEM) {                                                                                                                                                           
        	wattrset(win_mem, COLOR_PAIR(3));                                                                                                                                                
	        wattron(win_mem, A_BOLD);                                                                                                                                                        
        } else {                                                                                                                                                                                 
	        wattrset(win_mem, COLOR_PAIR(4));                                                                                                                                                
	        wattron(win_mem, A_BOLD);                                                                                                                                                        
        }                                                                                                                                                                                        
	mvwaddstr(win_mem, 0, 54, " F6 ");


       	wattrset(win_mem, COLOR_PAIR(2));
	wattron(win_mem, A_BOLD);        
        mvwaddstr(win_mem, 2, 2,      "       00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");                                                                                                 
        wattroff(win_mem, A_BOLD);
        
        mvwaddstr(win_mem, 3, 2,      "------------------------------------------------------");                                                                                                

        int base, linecounter, bytecounter;                                                                                                                                                                                
	uint16_t addr;

	for(linecounter = 0; linecounter < 16; linecounter++) {                                                                                                                                                       
		base = start_address+(linecounter*16);
	
		wattron(win_mem, A_BOLD);
		mvwprintw(win_mem, 4+linecounter, 2, "%04x", base);
		wattroff(win_mem, A_BOLD);
		mvwprintw(win_mem, 4+linecounter, 6, ":");
		
  		for(bytecounter = 0; bytecounter < 16; bytecounter++) {
  			addr = base+bytecounter;
  			if(get_pc_cpu(cpu) == addr) {
				wattron(win_mem, A_BOLD);
  			}
			mvwprintw(win_mem, 4+linecounter, 9+(bytecounter*3), "%02x", ram[addr]);
			wattroff(win_mem, A_BOLD);
  		}
  	}
  	update_panels();
  	doupdate();
  	
  	return;
}

void update_cpu_panel() {
        uint32_t cpu = get_cpu();

        wattrset(win_cpu, COLOR_PAIR(3));
        wattron(win_cpu, A_BOLD);
        mvwaddstr(win_cpu, 0, 2, "cpu- and system-status");

        if(active_window == WIN_CPU) {
	        wattrset(win_cpu, COLOR_PAIR(3));
	        wattron(win_cpu, A_BOLD);
	} else {
		wattrset(win_cpu, COLOR_PAIR(4));
		wattron(win_cpu, A_BOLD);
	}

	mvwaddstr(win_cpu, 0, 27, " F5 ");

	wattrset(win_cpu, COLOR_PAIR(2));
	mvwprintw(win_cpu, 2, 1, " A=$%02x X=$%02x Y=$%02x SP=$%02x", get_a(cpu), get_x(cpu), get_y(cpu), get_sp(cpu));
        mvwprintw(win_cpu, 3, 1, " PC=$%04x F:$%02x NV-BDIZC", get_pc_cpu(cpu), get_flags(cpu));
	mvwprintw(win_cpu, 4, 1, " cycles: %d", get_cycles(cpu));
        mvwprintw(win_cpu, 5, 1, " CPU frequency: %dkhz", get_frequency(cpu));
        mvwprintw(win_cpu, 6, 1, " CPUs: %d, CPU in view: %d", N_CPUS, cpu); 
        mvwaddstr(win_cpu, 7, 1, " TIMER GPIO UART SID LOG GFX");
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
	
	init_pair(1, COLOR_CYAN, COLOR_BLACK);		// borders
	init_pair(2, COLOR_WHITE, COLOR_BLACK);			// normal text
	init_pair(3, COLOR_GREEN, COLOR_BLACK);			// window titles
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);			// borders highlighted

	_logf("curses: init_curses() done");

        int winx, winy;                                                                                                                                                                          
        getmaxyx(stdscr, winy, winx);                                                                                                                                                            
        if((winx < WIN_SIZE_MIN_X)||(winy < WIN_SIZE_MIN_Y)) {                                                                                                                                   
                 printw("Your console is too small, can't go on.\n");                                                                                                                             
                 printw("You'll need 80x25, now it is %d*%d\n\n", winx, winy);                                                                                                                    
                 printw("Press a key to quit.\n");                                                                                                                                                
                 _logf("curses: your console is too small!");                                                                                                                                     
                 refresh();                                                                                                                                                                       
                 getch();                                                                                                                                                                         
                 deinit_curses();                                                                                                                                                                 
                 exit(-1);                                                                                                                                                                        
        }


	return 0;
}

static void deinit_curses() {
	//del_panel(pan_main);
	del_panel(pan_cpu);
	del_panel(pan_mem);
	del_panel(pan_disasm);
	//del_panel(pan_log);
	//delwin(win_main);
	delwin(win_cpu);
	delwin(win_mem);
	delwin(win_disasm);
	//delwin(win_log);
	endwin();
	_logf("curses: deinit_curses() done");
	return;
}

