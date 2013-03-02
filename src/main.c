#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <termios.h>

#include "log.h"
#include "6502.h"
#include "main.h"
#include "nc_ui.h"

extern __6502_system_t cpu[N_CPUS];
//extern uint32_t active_cpu;

void run_image(uint32_t steps);
void debugger();
void print_help();

int32_t load_rom_image(char *filename, uint16_t offset);

uint16_t get_addr(char *str); 	// read 16bit hex from keyboard (ex: 3f00)
uint8_t get_hex8(char *str);
uint32_t get_dec32(char *str);
void term_block();		// keyboard settings, wait for return
void term_nonblock();		// keyboard settings, dont wait for return

uint16_t pc_init = 0;

#define ACTION_BENCH	1
#define ACTION_DEBUGGER	2
#define ACTION_NCURSES	3

void mem_clear() {
	int i;
	for(i = 0; i < 0x10000; i++) {
		ram[i] = 0;
	}
	_logf("memory: cleared");
	return;
}

int main(int argc, char **argv) {
	uint32_t i;
	uint16_t addr;

	int action = 0;
	uint32_t bench_steps = 0;

	_logf("--- started 6502 ---");

	mem_clear();


	if(argc <= 1) {
		_logf("starting ncurses ui");
		ncurses_ui();
		exit(0);
	}

	init_mmio();

	for(i = 1; i < argc; i++) {
		if(!(strcmp("-h", argv[i]))) {
			print_help();
			exit(0);
		}
		if(!(strcmp("--help", argv[i]))) {
			print_help();
			exit(0);
		}
		if(!(strcmp("--rom", argv[i]))) {
			addr = atoi(argv[i+2]);
			load_rom_image(argv[i+1],addr);
			i+=3;	
		}
		if(!(strcmp("--set-pc", argv[i]))) {
			pc_init = atoi(argv[i+1]);
			printf("initial pc=$%04x\n", pc_init);
			i+=2;
		}
		if(!(strcmp("--bench", argv[i]))) {
			//run_image(atoi(argv[i+1]));
			action = ACTION_BENCH;
			bench_steps = atoi(argv[i+1]);
		}
		if(!(strcmp("--dbg", argv[i]))) {
			//debugger();
			action = ACTION_DEBUGGER;
		}
		if(!(strcmp("--system-6502asm.com", argv[i]))) {

		}
		if(!(strcmp("--enable-random", argv[i]))) {
			uint16_t rng_base_addr = atoi(argv[i+1]);
			//rng_base_addr = 0xfe;
			printf("setting up 8bit RNG at address $%04x\n", rng_base_addr);
			i += 1;
			rng8_init(rng_base_addr, 0);
		}
		if(!(strcmp("--enable-keyboard", argv[i]))) {
			uint16_t kbd_base_addr = atoi(argv[i+1]);
			printf("setting up keyboard input at address $%04x\n", kbd_base_addr);
			i += 2;
		}
		if(!(strcmp("--cpus", argv[i]))) {
			uint32_t num_of_cpus = atoi(argv[i+1]);
			printf("setting up %d cpus\n", num_of_cpus);
			i += 2;
		}
		if(!(strcmp("--nc", argv[i]))||!(strcmp("--ncurses", argv[i]))) {
			action = ACTION_NCURSES;	
		}
	}

	switch(action) {
		case ACTION_BENCH:
			_logf("starting benchmark");
			run_image(bench_steps);
			break;
		case ACTION_DEBUGGER:
			_logf("starting debugger");
			printf("starting debugger\n");
			debugger();
			break;
		case ACTION_NCURSES:
			_logf("starting ncurses ui");
			ncurses_ui();
			break;
		default:
			print_help();
			exit(-1);
			break;
	}

	stop_mmio();

	return 0;
}


int32_t load_rom_image(char *filename, uint16_t offset) {
	uint32_t addr;
	uint8_t data;
	FILE *f = fopen(filename, "r");
	struct stat stbuf;

	stat(filename, &stbuf);

	_logf("load_rom_image(): file %s, size $%04x bytes", filename, (uint32_t)stbuf.st_size);

	if((stbuf.st_size + offset) > 0x10000) 
		_logf("load_rom_image(): warning! image exceeds memory, loading anyway, will be cut at 0xffff!");

	if(f == NULL) {
		_logf("load_rom_image(): WTF? can't open file...");
		exit(-2);
	}

	addr = offset;
	while(addr <= 0xffff) {
		data = getc(f);
		ram[addr] = data;
		addr++;	
	}

	fclose(f);
	_logf("load_rom_image(): rom image loaded");

	return 0;
}

void print_help() {
	printf("6502 emulator (c) 2010-2013 Nils Stec\n");
	printf(" --nc			start ncurses user interface (default when starting without arguments\n");
	printf(" --cpus <num>		how many CPUs to create/use\n");
	printf(" --rom <file> <offset>	load a plain binary image to <offset>\n");
	printf(" --bench	        do a benchmark\n");
	printf(" --dbg			start debugger\n");
	printf(" --set-pc <addr16>	set initial PC value\n");
	printf(" --system-6502asm.com	configure system to be compatible with 6502asm.com emu\n");
	printf(" --enable-random <addr16>	enable 8bit RNG at specific address\n");
	printf(" --enable-keyboard <addr16>	enable KBD input at specific address\n");
	printf(" --enable-video <mode>		enable Video output with specific mode: 0=c64 text, 1=6502asm.com\n");
	return;
}

void debugger() {
	int run = 1;
	int8_t kbuf = 0;
	uint16_t addr = 0;
	uint16_t rngaddr = 0;
	uint8_t val;

	printf("debugger\n");

	init6502();
	reset6502();

	term_nonblock();
	int retval;
	uint32_t steps = 0;
	uint32_t n_steps;
	uint32_t actcputmp;
	int x, y;
	kbuf = 's';

	cpu[get_cpu()].reg.pc = pc_init;

	while(run) {
		cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
		switch(kbuf) {
			case 'S':
				addr = get_addr("set memory\naddr");
				val = get_hex8("8bit hex");
				ram[addr] = val;
				goto cpustatus;
				break;
			case 'd':
				printf("memory dump\n");
				addr = get_addr("address");
				printf("       $.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .A .B .C .D .E .F\n");
				printf("       --- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --\n");
				for(y = 0; y < 16; y++) {
					printf(" $%04x: ", (unsigned int)addr);
					for(x = 0; x < 16; x++) {
						printf("%02x ", ram[addr]);
						addr++;
					}
					printf("\n");	
				}
				printf("\n");
				break;
			case 'D':
				printf("wide memory dump\n");
				addr = get_addr("address");
				printf("       $.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .A .B .C .D .E .F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F\n");
				for(y = 0; y < 32; y++) {
					printf(" $%04x: ", (unsigned int)addr);
					for(x = 0; x < 32; x++) {
						printf("%02x ", ram[addr]);
						addr++;
					}
					printf("\n");
				}
				printf("\n");
				break;
			case 'r':
				printf("reset\n");
				reset6502();
				cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
				goto cpustatus;
				break;
			case 'i':
				printf("irq\n");
				irq6502();
#ifdef MMIO_USE_OPCODEHOOK
				mmio_irqhook();
#endif
				cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
				goto cpustatus;
				break;
			case 'h':
				printf("avalaible commands\n\n");
				printf(" program execution:\n");
				printf("  n - do a step		p - set PC			G - couple of steps\n");
				printf("  r - reset		C - switch to another CPU\n");
				printf(" memory dumping/writing:\n");
				printf("  d - dump memory	D - dump memory (wide screen)	s - set memory\n");
				printf(" interrupts:\n");
				printf("  i - generate int	N - generate NMI");
				printf(" IO configuration:\n");
				printf("  R - set up RNG	V - set up video output		k - set up kbd\n");	 
				printf(" status:\n");
				printf("  v - show vectors	s - show CPU status		c - show sstem config\n");
				printf(" debugger control:\n");
				printf("  h - help		X - quit\n");
				goto cpustatus;
				break;
			case 'c':
				printf("system configuration:\n");
				printf(" * number of cpus: %d\n", get_cpus());
				break;
			case 'k':
				break;
			case 'R':
				rngaddr = get_addr("RNG base address, 0 for disable");
				if(rngaddr == 0) {
					rng8_deinit();
				} else {
					rng8_init(rngaddr, 0);
				}
				printf("RNG set to $%04x\n", rngaddr);
				break;
			case 'V':
				break;
			case 'C':
				actcputmp = get_cpu();
				char strtmp[50];
				sprintf(strtmp, "input number of CPU to use [%d]", actcputmp);
				set_cpu(get_dec32(strtmp));
				break;
			case 'p':
				cpu[get_cpu()].reg.pc = get_addr("set pc to");
				cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
				goto cpustatus;
				break;
			case 'N':
				printf("nmi\n");
				cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
				nmi6502();
#ifdef MMIO_USE_OPCODEHOOK
				mmio_nmihook();
#endif
				goto cpustatus;
				break;
			case 'X':
				printf("quit\n");
				run = 0;
				goto quit;
				break;
			case 'v':
				printf("vectors:\n");
				printf("NMI	$%02x%02x\n", ram[0xfffb], ram[0xfffa]);
				printf("RESET	$%02x%02x\n", ram[0xfffd], ram[0xfffc]);
				printf("IRQ/BRK $%02x%02X\n", ram[0xffff], ram[0xfffe]); 
				break;
			case 'n':
				rng8_getrnd();
				cpu[get_cpu()].reg.pc++;
				cpu[get_cpu()].inst.instruction[cpu[get_cpu()].opcode]();
				cpu[get_cpu()].ticks += cpu[get_cpu()].inst.opcode_ticks[cpu[get_cpu()].opcode];
				steps++;
#ifdef MMIO_USE_OPCODEHOOK
				mmio_opcodehook();
#endif
				cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
				goto cpustatus;
				break;
			case 'G':
				n_steps = get_dec32("how many steps to run? ");
				for(;n_steps > 0; n_steps--) {
					rng8_getrnd();
					cpu[get_cpu()].reg.pc++;
					cpu[get_cpu()].inst.instruction[cpu[get_cpu()].opcode]();
					cpu[get_cpu()].ticks += cpu[get_cpu()].inst.opcode_ticks[cpu[get_cpu()].opcode];
					steps++;
#ifdef MMIO_USE_OPCODEHOOK
					mmio_opcodehook();
#endif	// MMIO_USE_OPCODEHOOK
					cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc];
				}
				goto cpustatus;
				break;
			case 's':
cpustatus:
				printf(" step %04d PC=$%04x dis:'%s", steps+1, cpu[get_cpu()].reg.pc, mnemonics[cpu[get_cpu()].opcode]);
				switch(opcode_len[cpu[get_cpu()].opcode]) {
					case 1:
						printf("        ");
						break;
					case 2:
						printf(" $%02x    ", ram[cpu[get_cpu()].reg.pc+1]);
						break;
					case 3:
						printf(" $%02x $%02x", ram[cpu[get_cpu()].reg.pc+1], ram[cpu[get_cpu()].reg.pc+2]);
						break;
				}
				printf("' opcode=$%02x(%db) A=$%02x X=$%02x Y=$%02x SP=$%02x S=$%02x cycles=%d cpu-cycles=%d\n", cpu[get_cpu()].opcode, opcode_len[cpu[get_cpu()].opcode], cpu[get_cpu()].reg.a, cpu[get_cpu()].reg.x, cpu[get_cpu()].reg.x, cpu[get_cpu()].reg.sp, cpu[get_cpu()].reg.flags, cpu[get_cpu()].inst.opcode_ticks[cpu[get_cpu()].opcode], cpu[get_cpu()].ticks);
				break;
		}
		retval = read(0, &kbuf, 1);
		retval++;
	}
quit:
	term_block();
}

void term_nonblock() {
	struct termios term = {0};

        tcgetattr(0, &term);    // err handling

        term.c_lflag &= ~ICANON;
        term.c_lflag &= ~ECHO;
        term.c_cc[VMIN] = 1;
        term.c_cc[VTIME] = 0;

        tcsetattr(0, TCSANOW, &term);   // err handling
	
	return;

}

void term_block() {
	struct termios term = {0};
	
	tcgetattr(0, &term);

	term.c_lflag |= ICANON;
	term.c_lflag |= ECHO;

	tcsetattr(0, TCSANOW, &term);

	return;
}

uint16_t get_addr(char *str) {
	unsigned int addr;
	
	term_block();

	printf("%s: ", str);
	if(!scanf("%x", &addr)) printf("error\n");

	term_nonblock();

	return (uint16_t)addr;
}

uint32_t get_dec32(char *str) {
	uint32_t val;

	term_block();

	printf("%s: ", str);
	if(!scanf("%d", &val)) printf("error\n");

	term_nonblock();

	return val;
}

uint8_t get_hex8(char *str) {
	unsigned int val;

	term_block();
	printf("%s: ", str);
	if(!scanf("%x", &val)) printf("error\n");

	term_nonblock();

	return (uint16_t)val;
}


void run_image(uint32_t steps) {
        uint32_t i;
	clock_t t1, t2;
        cpu[get_cpu()].reg.pc=0;

        init6502();
        printf("init 6502 ok\n");
        reset6502();
        printf("reset 6502 ok\n");

	printf("switching to cpu 0\n");
	set_cpu(0);

	printf("running %d steps...\n", steps);

        t1 = clock();
	for(i = 0; i < steps; i++) {
                cpu[get_cpu()].opcode = ram[cpu[get_cpu()].reg.pc++];
                cpu[get_cpu()].inst.instruction[cpu[get_cpu()].opcode]();
                cpu[get_cpu()].ticks += cpu[get_cpu()].inst.opcode_ticks[cpu[get_cpu()].opcode];
#ifdef MMIO_USE_OPCODEHOOK
	mmio_opcodehook();		
#endif
        }
	t2 = clock();

	float host_time = (float)(t2-t1)/CLOCKS_PER_SEC;
	float emu_speed_clk = (host_time*1000000)/cpu[get_cpu()].ticks;  
        float emu_speed_hz = 1000000.0/emu_speed_clk/1000000;

	printf(" 6502 ticks total: %d\n", cpu[get_cpu()].ticks);
	printf(" host time used:    %f\n", host_time);
	printf(" emu speed mhz:    %f\n", emu_speed_hz);
}

