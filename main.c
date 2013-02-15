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
#include "6502.h"

extern __6502_system_t cpu[N_CPUS];
extern uint32_t active_cpu;

void run_image(uint32_t steps);
void debugger();
void print_help();

int32_t load_rom_image(char *filename, uint16_t offset);

uint16_t get_addr(char *str); 	// read 16bit hex from keyboard (ex: 3f00)
uint8_t get_hex8(char *str);
void term_block();		// keyboard settings, wait for return
void term_nonblock();		// keyboard settings, dont wait for return

int main(int argc, char **argv) {
	uint32_t i;
	uint16_t addr;

	if(argc <= 1) {
		printf("can't do anything without arguments...\n");
		exit(-1);
	}

	init_mmio();

	for(i = 1; i < argc; i++) {
		if(!(strcmp("-h", argv[i]))) {
			print_help();
			exit(0);
		}
		if(!(strcmp("rom", argv[i]))) {
			addr = atoi(argv[i+2]);
			load_rom_image(argv[i+1],addr);
			i+=3;	
		}
		if(!(strcmp("bench", argv[i]))) {
			run_image(atoi(argv[i+1]));
		}
		if(!(strcmp("dbg", argv[i]))) {
			debugger();
		}
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

	printf("file %s, size $%04x bytes\n", filename, (uint32_t)stbuf.st_size);

	if((stbuf.st_size + offset) > 0x10000) 
		printf("warning! image exceeds memory, loading anyway, will be cut at 0xffff!\n");

	if(f == NULL) {
		printf("WTF? can't open file...\n");
		exit(-2);
	}

	addr = offset;
	while(addr <= 0xffff) {
		data = getc(f);
		ram[addr] = data;
		addr++; 		
	}
	
	fclose(f);

	return 0;
} 

void print_help() {
	printf("6502 emulator (c) 2010-2013 NS\n");
	printf(" rom <file> <offset>	load a plain binary image to <offset>\n");
	printf(" bench 		        do a benchmark\n");
	printf(" dbg			start debugger\n");
	return;
}

void debugger() {
	int run = 1;
	int8_t kbuf = 0;
	uint16_t addr = 0;
	uint8_t val;

	printf("debugger\n");
	init6502();
	reset6502();

	term_nonblock();
	
	uint32_t steps = 0;

	kbuf = 's';

	cpu[active_cpu].reg.pc = 0;

	while(run) {
		opcode = ram[PC];
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
				
				int x, y;
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
			case 'r':
				printf("reset\n");
				reset6502();
				opcode = ram[PC];
				goto cpustatus;
				break;
			case 'i':
				printf("irq\n");
				irq6502();
#ifdef MMIO_USE_OPCODEHOOK
				mmio_irqhook();
#endif
				opcode = ram[PC];
				goto cpustatus;
				break;
			case 'h':
				printf(" d - dump mem\n");
				printf(" r - reset\n");
				printf(" i - int\n");
				printf(" h - help\n");
				printf(" N - nmi\n");
				printf(" X - quit\n");
				printf(" v - show vectors\n");
				printf(" n - do a step\n");
				printf(" s - cpu status\n S - set memory\n p - set pc\n");
				break;
			case 'p':
				PC = get_addr("set pc to");
				opcode = ram[PC];
				goto cpustatus;
				break;
			case 'N':
				printf("nmi\n");
				opcode = ram[PC];
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
				PC++;
				instruction[opcode]();
				clockticks6502 += ticks[opcode];
				steps++;
#ifdef MMIO_USE_OPCODEHOOK
				mmio_opcodehook();
#endif
				opcode = ram[PC];
			case 's':
cpustatus:
				printf(" step %04d PC=$%04x dis:'%s", steps+1, PC, mnemonics[opcode]);
				switch(opcode_len[opcode]) {
					case 1:
						printf("        ");
						break;
					case 2:
						printf(" $%02x    ", ram[PC+1]);
						break;
					case 3:
						printf(" $%02x $%02x", ram[PC+1], ram[PC+2]);
						break;
				}
				printf("' opcode=$%02x(%db) A=$%02x X=$%02x Y=$%02x SP=$%02x S=$%02x cycles=%d cpu-cycles=%d\n", opcode, opcode_len[opcode], A, X, Y, S, P, ticks[opcode], clockticks6502);
				break;
		}
		read(0, &kbuf, 1);
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
	scanf("%x", &addr);

	term_nonblock();

	return (uint16_t)addr;
}

uint8_t get_hex8(char *str) {
	unsigned int val;

	term_block();
	printf("%s: ", str);
	scanf("%x", &val);

	term_nonblock();

	return (uint16_t)val;
}


void run_image(uint32_t steps) {
        uint32_t i;
	clock_t t1, t2;
        PC=0;

        init6502();
        printf("init 6502 ok\n");
        reset6502();
        printf("reset 6502 ok\n");

	printf("running %d steps...\n", steps);

        t1 = clock();
	for(i = 0; i < steps; i++) {
                opcode = ram[PC++];
                instruction[opcode]();
                clockticks6502 += ticks[opcode];
#ifdef MMIO_USE_OPCODEHOOK
	mmio_opcodehook();		
#endif
        }
	t2 = clock();

	float host_time = (float)(t2-t1)/CLOCKS_PER_SEC;
	float emu_speed_clk = (host_time*1000000)/clockticks6502;  
        float emu_speed_hz = 1000000.0/emu_speed_clk/1000000;

	printf(" 6502 ticks total: %d\n", clockticks6502);
	printf(" host time used:    %f\n", host_time);
	printf(" emu speed mhz:    %f\n", emu_speed_hz);
}

