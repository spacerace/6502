#include <stdio.h>
#include <stdint.h>
#include <ncurses.h>
#include <stdlib.h>
#include <panel.h>
#include <string.h>

#include "log.h"
#include "6502.h"
#include "nc_ui.h"
#include "c64_colors.h"
#include "mmio.h"
#include "random.h"
#include "tables.h"

extern __6502_system_t cpu[N_CPUS];

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
static int active_window = WIN_MEM;

static int mem_cursor_x = 0;
static int mem_cursor_y = 0;
static uint16_t memstart = 0;
static int mem_editmode = 0;

static uint16_t disasmstart = 0;

void update_memory_panel(uint16_t start_address);
void update_disasm_panel();
void update_log_panel();
void highlight_window();
void dehighlight_windows();
void handle_input(int c);

uint16_t get_memwin_cursor_addr() {
	return memstart + (mem_cursor_y*16) + mem_cursor_x;
}

int ncurses_ui() {
	atexit(deinit_curses);

	init_curses();
	init_disasm();
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

	int c;

	for(;;) {
		dehighlight_windows();
		highlight_window();
		update_cpu_panel();
		update_memory_panel(memstart);
		update_disasm_panel(disasmstart);	
		update_log_panel();
		refresh();
		c = getch();
		switch(c) {
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
			default:
				handle_input(c);
		}
	}

	deinit_curses();

	return 0;
}
void handle_input_cpu(int c);
void handle_input_mem(int c);

void handle_input(int c) {
	if(c == KEY_F(2)) {
		active_window++;
		if(active_window > 3) active_window = 0;
	} else {
		switch(active_window) {
			case WIN_CPU:
				handle_input_cpu(c);
				break;
			case WIN_MEM:
				handle_input_mem(c);
				break;
		}
	}
	return;
}

void handle_input_cpu(int c) {
	switch(c) {
		case ' ':
			rng8_getrnd();
                        cpu[get_cpu()].reg.pc++;                                                                                                                                         
                        cpu[get_cpu()].inst.instruction[cpu[get_cpu()].opcode]();                                                                                                        
                        cpu[get_cpu()].ticks += cpu[get_cpu()].inst.opcode_ticks[cpu[get_cpu()].opcode];                                                                                 
                        cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
                	break;
                case 'r':
                	reset6502();
                	cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
                	break;
                case 'n':
                	nmi6502();
                	cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
                	break;
                case 'i':
                	irq6502();
                	cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
                	break;
	}
	return;
}

void handle_input_mem(int c) {
	static int nibble = 0;
	static char newtmp[3] = "\0";

	switch(c) {
		case KEY_UP:
			mem_cursor_y--;
			if(mem_cursor_y < 0) mem_cursor_y = 0;
			break;
		case KEY_DOWN:
			mem_cursor_y++;
			if(mem_cursor_y > 15) mem_cursor_y = 15;
			break;
		case KEY_LEFT:
			mem_cursor_x--;
			if(mem_cursor_x < 0) mem_cursor_x = 0;
			break;
		case KEY_RIGHT:
			mem_cursor_x++;
			if(mem_cursor_x > 15) mem_cursor_x = 15;
			break;
		case ' ':
			rng8_getrnd();
			cpu[get_cpu()].reg.pc++;                                                                                                                                                 
			cpu[get_cpu()].inst.instruction[cpu[get_cpu()].opcode]();                                                                                                                
			cpu[get_cpu()].ticks += cpu[get_cpu()].inst.opcode_ticks[cpu[get_cpu()].opcode];                                                                                         
			cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
			break;
		case 'r':
			reset6502();
			cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
			break;
		case 'n':
			nmi6502();
			cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
			break;
		case 'i':
			irq6502();
			cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
			break;
		case KEY_PPAGE:
			memstart -= 256;
			break;
		case KEY_NPAGE:
			memstart += 256;
			break;
	}

	int value;

	if( ((c >= '0')&&(c <= '9')) || ((c >= 'a')&&(c <= 'f'))) {
		if(nibble == 0) {
				mem_editmode = 1;
				newtmp[0] = c;	
				nibble = 1;
		} else if(nibble == 1) {
				newtmp[1] = c;
				newtmp[2] = '\0';
				sscanf(newtmp, "%x", &value);
				ram[get_memwin_cursor_addr()] = (uint8_t)value;
				newtmp[0] = '\0';
				newtmp[1] = '\0';
				nibble = 0;
				mem_editmode = 0;
		}

	}
	return;
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

void reverse_str(char *str);

void reverse_str(char *str) {
	int i;
	char cpstr[strlen(str)+1];
        for(i=0; i < strlen(str); i++) {
        	cpstr[i] = str[strlen(str)-i-1];
	}
	cpstr[i] = '\0';
	strcpy(str, cpstr);
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

	char lines[3][75];
	strcpy(lines[0], "                                                      ");
	strcpy(lines[1], "                                                      ");
	strcpy(lines[2], "                                                      ");

	

	mvwprintw(win_log, 1, 2, "%s", lines[0]);
	mvwprintw(win_log, 2, 2, "%s", lines[1]);
	mvwprintw(win_log, 3, 2, "%s", lines[2]);
	update_panels();
	doupdate();

	return;
}


void disasm(uint16_t addr, char *str, int *operands) {
	*operands = 0;
	int adrmode;

	uint16_t disaddr = addr;
	char disasm_str[50];

	adrmode = get_adrmode(ram[disaddr]);
	switch(adrmode) {
		case AM_IMP:		// 1byte instructions
		case AM_A:
			sprintf(disasm_str, "%s", mnemonics[ram[disaddr]]);
			*operands = 0;
			break;
		case AM_IMM:		// 2byte instructions
			sprintf(disasm_str, "%s #$%02x", mnemonics[ram[disaddr]], ram[disaddr+1]);
			*operands = 1;
			break;
		case AM_ZP:
			sprintf(disasm_str, "%s $%02x", mnemonics[ram[disaddr]], ram[disaddr+1]);
			*operands = 1;
			break;
		case AM_ZPX:
			sprintf(disasm_str, "%s $%02x,X", mnemonics[ram[disaddr]], ram[disaddr+1]);
			*operands = 1;
			break;
		case AM_ZPY:
			sprintf(disasm_str, "%s ($%02x),Y", mnemonics[ram[disaddr]], ram[disaddr+1]);
			*operands = 1;
			break;
		case AM_REL:
			sprintf(disasm_str, "%s $%02x", mnemonics[ram[disaddr]], ram[disaddr+1]);
			*operands = 1;
			break;		// 3byte instruction
		case AM_ABS:
			sprintf(disasm_str, "%s $%02x%02x", mnemonics[ram[disaddr]], ram[disaddr+2], ram[disaddr+1]);
			*operands = 2;
			break;
		case AM_ABSX:
			sprintf(disasm_str, "%s $%02x%02x,X", mnemonics[ram[disaddr]], ram[disaddr+2], ram[disaddr+1]);
			*operands = 2;
			break;
		case AM_ABSY:
			sprintf(disasm_str, "%s ($%02x%02x),Y", mnemonics[ram[disaddr]], ram[disaddr+2], ram[disaddr+1]);
			*operands = 2;
			break;
		case AM_IND:
			sprintf(disasm_str, "%s ($%02x%02x)", mnemonics[ram[disaddr]], ram[disaddr+2], ram[disaddr+1]);
			*operands = 2;
			break;
		case AM_INDEXED_IND:
			sprintf(disasm_str, "%s ($%02x%02x,X)", mnemonics[ram[disaddr]], ram[disaddr+2], ram[disaddr+1]);
			*operands = 2;
			break;
		case AM_IND_INDEXED:
			sprintf(disasm_str, "%s ($%02x%02x),Y", mnemonics[ram[disaddr]], ram[disaddr+2], ram[disaddr+1]);
			*operands = 2;
			break;
		default:
			sprintf(disasm_str, " ");
			break;
	}
	sprintf(str, "%s", disasm_str);
}


void update_disasm_panel() {
	wattrset(win_disasm, COLOR_PAIR(3));
	wattron(win_disasm, A_BOLD);
	mvwaddstr(win_disasm, 0, 2, "disassembly");
	wattroff(win_disasm, A_BOLD);

	wattrset(win_disasm, COLOR_PAIR(2));

        uint16_t cur_addr = get_memwin_cursor_addr();

	char disasm_str[50];
	int operands;

	disasm(cur_addr, disasm_str, &operands);

	mvwprintw(win_disasm, 7, 2, "                     ");
	mvwprintw(win_disasm, 8, 2, "                     ");
	if(strlen(disasm_str)>2) {
		mvwprintw(win_disasm, 7, 2, "CURSOR: %s", disasm_str);
	}


	disasm(get_pc_cpu(0), disasm_str, &operands);

	if(strlen(disasm_str)>2) {
		mvwprintw(win_disasm, 8, 2, "PC: %s", disasm_str);
	}

	update_panels();
	doupdate();

	return;
}

void update_memory_panel(uint16_t start_address) {
	int cpu = get_cpu();
	static int operands = 0;

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
        mvwprintw(win_mem, 2, 2,      "$%04x  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F", get_memwin_cursor_addr());
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
  			
  			if(get_pc_cpu(cpu) == addr) {			// we display ram which PC points to
  				wattron(win_mem, A_BOLD);		// make the value bold
  				operands = opcode_len[ram[addr]];	// look if we have operands to highlight
  			}
  			
  			if((mem_cursor_x == bytecounter) && (mem_cursor_y == linecounter)) wattron(win_mem, A_BLINK);	// if our cursor is over we blink this value

  			if((addr == get_memwin_cursor_addr() && mem_editmode)) {	// when editing change color
				wattron(win_mem, COLOR_PAIR(4));
  			} else wattron(win_mem, COLOR_PAIR(2));

			if(operands) {					// if value is the operand of the actual opcode we highlight it
				wattron(win_mem, COLOR_PAIR(1));
				operands--;
  			} else wattron(win_mem, COLOR_PAIR(2));

			mvwprintw(win_mem, 4+linecounter, 9+(bytecounter*3), "%02x", ram[addr]);
			
			wattroff(win_mem, A_BOLD|A_BLINK);
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

