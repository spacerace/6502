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
extern void clear_memory();
#define WIN_SIZE_MIN_X	80
#define WIN_SIZE_MIN_Y	25

#define WIN_SIZE_X	80
#define WIN_SIZE_Y	25

static int init_curses();
static void deinit_curses();
static int cursor_color = COLOR_PAIR(7);
static int cursor_attributes;
void update_cpu_panel();

static int32_t instr_n;

#define WIN_INSTRUCTION	0
#define WIN_CPU		1
#define WIN_MEM		2
#define WIN_DISASM	3
#define RESET		9999

WINDOW *win_instruction;
WINDOW *win_cpu;
WINDOW *win_mem;
WINDOW *win_disasm;

PANEL *pan_cpu;
PANEL *pan_mem;
PANEL *pan_disasm;
PANEL *pan_instruction;

static uint16_t rnd_base = 0xfe;
static int active_window = WIN_INSTRUCTION;

static int mem_cursor_x = 0;
static int mem_cursor_y = 0;
static uint16_t memstart = 0;
static int mem_editmode = 0;

static uint16_t disasmstart = 0;

void update_memory_panel(uint16_t start_address);
void update_disasm_panel();
void update_instruction_panel();
void highlight_window();
void dehighlight_windows();
void handle_input(int c);

uint16_t get_memwin_cursor_addr() {
	return memstart + (mem_cursor_y*16) + mem_cursor_x;
}

void create_panel_instruction() {
	wclear(win_instruction);
	win_instruction = newwin(18, 58, 28, 32);
	box(win_instruction, ACS_VLINE, ACS_HLINE);
	pan_instruction = new_panel(win_instruction);
	wbkgd(win_instruction, COLOR_PAIR(1));
	return;
}

void create_panel_cpu() {
	wclear(win_cpu);
	win_cpu = newwin(18, 31, 28, 1);
	box(win_cpu, ACS_VLINE, ACS_HLINE);
	pan_cpu = new_panel(win_cpu);
	wbkgd(win_cpu, COLOR_PAIR(1));
	return;
}

void create_panel_mem() {
	wclear(win_mem);
	win_mem = newwin(21, 58, 1, 32);
	box(win_mem, ACS_VLINE, ACS_HLINE);
	pan_mem = new_panel(win_mem);
	wbkgd(win_mem, COLOR_PAIR(1));
	return;
}

void create_panel_disasm() {
	wclear(win_disasm);
	win_disasm = newwin(27, 31, 1, 1);
	box(win_disasm, ACS_VLINE, ACS_HLINE);
	pan_disasm = new_panel(win_disasm);
	wbkgd(win_mem, COLOR_PAIR(1));
	return;
}

int ncurses_ui() {
	atexit(deinit_curses);

	init_curses();
	init_disasm();
	init_mmio();
	rng8_init(rnd_base, 0);
	init6502();
	reset6502();



	int c;

	for(;;) {
		create_panel_disasm();
		create_panel_cpu();
		create_panel_mem();
		create_panel_instruction();
		dehighlight_windows();
		highlight_window();
		update_cpu_panel();
		update_memory_panel(memstart);
		update_disasm_panel(disasmstart);	
		update_instruction_panel();
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
				active_window = WIN_INSTRUCTION;
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
void handle_input_instruction(int c);

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
			case WIN_INSTRUCTION:
				handle_input_instruction(c);
				break;
		}
	}
	return;
}

void handle_input_instruction(int c) {
	switch(c) {
		case KEY_LEFT:
			instr_n--;
			break;
		case KEY_RIGHT:
			instr_n++;
			break;
	}
	if(instr_n < 0) instr_n = 0;
	if(instr_n >= INSTR_REFS) instr_n = INSTR_REFS-1;

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
		case WIN_INSTRUCTION:
			wbkgd(win_instruction, COLOR_PAIR(4));
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
	wbkgd(win_instruction, COLOR_PAIR(1));
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



void update_instruction_panel() {
	wattrset(win_instruction, COLOR_PAIR(3));
	wattron(win_instruction, A_BOLD);
	mvwprintw(win_instruction, 0, 10, "instruction reference");
	mvwprintw(win_instruction, 0, 33, "page %d/%d", instr_n+1, INSTR_REFS);
	wattroff(win_instruction, A_BOLD);



        if(active_window == WIN_INSTRUCTION) {                                                                                                                                                           
	        wattrset(win_instruction, COLOR_PAIR(3));                                                                                                                                                
	        wattron(win_instruction, A_BOLD);                                                                                                                                                        
        } else {                                                                                                                                                                                 
	        wattrset(win_instruction, COLOR_PAIR(4));                                                                                                                                                
	        wattron(win_instruction, A_BOLD);                                                                                                                                                        
        }                                                                                                                                                                                        
        mvwaddstr(win_instruction, 0, 54, " F8 ");


	wattrset(win_instruction, COLOR_PAIR(2));


	int desc_n = instr_n;

	int x = 0, y = 0;                                                                                                                                                                               
	int x_off = 2, y_off = 1;                                                                                                                                                               
	int count;                                                                                                                                                                               

	for(count = 0; count < strlen(descriptions[desc_n]); count++) {                                                                                                                               
		switch(descriptions[desc_n][count]) {
			case '\n':
				y++;
				x = 0;
				break;
			case '[':
				wattrset(win_instruction, A_BOLD|COLOR_PAIR(2));
				break;
			case ']':
				wattrset(win_instruction, 0|COLOR_PAIR(2));
				break;
			case '{':
				wattrset(win_instruction, A_BOLD|COLOR_PAIR(6));
				break;
			case '}':
				wattrset(win_instruction, 0|COLOR_PAIR(2));
				break;
			case '|':
				do {
					mvwprintw(win_instruction, y_off+y, x_off+x, " ");
					x++;
				} while((x%4) != 0);				
				break;
			default:
				mvwprintw(win_instruction, y_off+y, x_off+x, "%c", descriptions[desc_n][count]);
				x++;
				break;
		}
	}

	update_panels();
	doupdate();

	return;
}


int disasm(uint16_t addr, char *str, int *operands) {
	*operands = 0;
	int adrmode;

	uint16_t disaddr = addr;
	char disasm_str[50];
	if(opcode_len == 0) return -1;
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
			return -1;
			break;
	}
	sprintf(str, "%s", disasm_str);
	return 0;
}

void update_disasm_panel() {
	wattrset(win_disasm, COLOR_PAIR(3));
	wattron(win_disasm, A_BOLD);
	mvwaddstr(win_disasm, 0, 11, "disassembly");
	wattroff(win_disasm, A_BOLD);

	wattrset(win_disasm, COLOR_PAIR(2));

        uint16_t cur_addr = get_memwin_cursor_addr();

	char disasm_str[50];
	int operands;
	int ret;

	wattrset(win_disasm, cursor_attributes|cursor_color);
	mvwprintw(win_disasm, 2, 3, ">");
	wattrset(win_disasm, A_BLINK|A_BOLD|COLOR_PAIR(4));
	mvwprintw(win_disasm, 3, 3, ">");
	
	disasm_str[0] = '\0';
	ret = disasm(cur_addr, disasm_str, &operands);
	if(ret == 0) {
		wattrset(win_disasm, cursor_attributes|cursor_color);
		mvwprintw(win_disasm, 2, 5, "%s", disasm_str);
	} 
	if(strlen(disasm_str) < 1) {
		wattrset(win_disasm, COLOR_PAIR(5)|A_BOLD|A_BLINK);
		mvwprintw(win_disasm, 2, 6, "- illegal opcode $%02x -", ram[get_memwin_cursor_addr()]);
	}


	disasm_str[0] = '\0';
	ret = disasm(get_pc_cpu(0), disasm_str, &operands);
	if(ret == 0) {
		wattrset(win_disasm, COLOR_PAIR(4)|A_BOLD);
		mvwprintw(win_disasm, 3, 5, "%s", disasm_str);
	}
	if(strlen(disasm_str) < 1) {
		wattrset(win_disasm, COLOR_PAIR(5)|A_BOLD|A_BLINK);
		mvwprintw(win_disasm, 3, 5, "- illegal opcode $%02x -", ram[get_memwin_cursor_addr()]);
	}

	update_panels();
	doupdate();

	return;
}

void update_memory_panel(uint16_t start_address) {
	int cpu = get_cpu();
	static int operands = 0;
	uint16_t addr;
	int base;
	int linecounter;
	int bytecounter;
	int attributes = 0;
	cursor_attributes = 0;
	cursor_color = COLOR_PAIR(2);
	int text_color = COLOR_PAIR(2);

	wattrset(win_mem, A_BOLD|COLOR_PAIR(3));                                                                                                                                                        
	mvwaddstr(win_mem, 0, 21, "memory editor");

        if(active_window == WIN_MEM) wattrset(win_mem, A_BOLD|COLOR_PAIR(3));                                                                                                                                                
        else wattrset(win_mem, A_BOLD|COLOR_PAIR(4));                                                                                                                                                
	mvwaddstr(win_mem, 0, 54, " F6 ");

       	wattrset(win_mem, A_BOLD|COLOR_PAIR(2));
        mvwprintw(win_mem, 2, 2,      "$%04x  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F", get_memwin_cursor_addr());

        wattrset(win_mem, 0);
        mvwaddstr(win_mem, 3, 2,      "------------------------------------------------------");                                                                                                


	for(linecounter = 0; linecounter < 16; linecounter++) {                                                                                                                                                       
		base = start_address+(linecounter*16);
		
		attributes = A_BOLD|COLOR_PAIR(2);
		wattrset(win_mem, attributes);
		mvwprintw(win_mem, 4+linecounter, 2, "%04x", base);

		attributes = 0;
		wattrset(win_mem, attributes);
		mvwprintw(win_mem, 4+linecounter, 6, ":");
		
  		for(bytecounter = 0; bytecounter < 16; bytecounter++) {
  			addr = base+bytecounter;
  			cursor_attributes = 0;
			attributes = 0;
			text_color = COLOR_PAIR(2);

			if(get_pc_cpu(cpu) == addr) {						// PC
				attributes   = A_BOLD;
				operands     = opcode_len[ram[addr]];
			}
			if(operands) {								// if value is the operand of the actual opcode we highlight it
				text_color = COLOR_PAIR(1);
				operands--;
			}
			
			if(addr == get_memwin_cursor_addr()) {					// if cursor is over value
				text_color = cursor_color;
			}

			if(get_memwin_cursor_addr() == get_pc_cpu(0)) {				// if cursor and pc overlapping
				cursor_attributes = A_BOLD;
				cursor_color = COLOR_PAIR(4);
			} else {
				cursor_color = COLOR_PAIR(4);
				attributes = cursor_attributes;
			}
			
			wattrset(win_mem, attributes|text_color);
			mvwprintw(win_mem, 4+linecounter, 9+(bytecounter*3), "%02x", ram[addr]);
		        
		        wattrset(win_mem, cursor_attributes|cursor_color);                                                                                                                                                         
			mvwprintw(win_mem, 2, 2,      "$%04x", get_memwin_cursor_addr());

			if(addr == get_pc_cpu(0)) {
				wattrset(win_mem, A_BOLD|COLOR_PAIR(4));
				mvwprintw(win_mem, 4+linecounter, 9+(bytecounter*3), "%02x", ram[addr]);
			}
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
	init_pair(5, COLOR_RED, COLOR_BLACK);
	init_pair(6, COLOR_BLUE, COLOR_BLACK);
	init_pair(7, COLOR_MAGENTA, COLOR_BLACK);

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
	del_panel(pan_cpu);
	del_panel(pan_mem);
	del_panel(pan_disasm);
	del_panel(pan_instruction);
	delwin(win_cpu);
	delwin(win_mem);
	delwin(win_disasm);
	delwin(win_instruction);
	endwin();
	_logf("curses: deinit_curses() done");
	return;
}

