#ifndef __CPU_6502_H__
#define __CPU_6502_H__

#include <inttypes.h>
#include <stdio.h>
#include <pthread.h>

#include "mmio.h"

#define N_CPUS	32


#define FLAG_NEG	0x80
#define FLAG_OVF	0x40
#define FLAG_NONAME	0x20	// always set to 1
#define FLAG_BRK	0x10
#define FLAG_DEC	0x08
#define FLAG_INT	0x04
#define FLAG_ZERO	0x02
#define FLAG_CARRY	0x01

// macros with commonly used routines
#define get_reset_vector()	ram[0xfffc] | (ram[0xfffd]<<8)
#define get_nmi_vector()	ram[0xfffa] | (ram[0xfffb]<<8)
#define get_irq_vector()	ram[0xfffe] | (ram[0xffff]<<8)
#define get_pc() 	        ram[cpu[n].reg.pc]+(ram[cpu[n].reg.pc+1]<<8)    // get addr from pc into uint16_t x


// in the emulator we use a datatype to cover the whole system. here it's defined
typedef struct {
	uint8_t a;
	uint8_t x;
	uint8_t y;
	uint8_t flags;	// cpu flags register
	uint8_t sp;	// stack pointer
	uint16_t pc;
} __6502_system_reg_t;

typedef struct {
	uint8_t image[0x10000];
} __6502_system_mem_t;

typedef struct {
	uint32_t flags;
/*	(void *) pre_irq_hook();
	(void *) past_irq_hook();
	(void *) pre_reset_hook();
	(void *) past_reset_hook();
	(void *) pre_nmi_hook();
	(void *) past_nmi_hook();
*/
} __6502_system_irq_t;

typedef struct {
	uint16_t opcode_ticks[256];	// table of clockticks of instruction set
	void (*adrmode[256])();	// table of function ptrs to the addressing-
				// mode "helpers"
	void (*instruction[256])();	// table of function ptrs to the 
				// opcode-handlers	
} __6502_system_instrset_t;


typedef struct {
	__6502_system_reg_t reg;
	__6502_system_mem_t mem;
	__6502_system_irq_t irq;
	__6502_system_instrset_t inst;
	uint32_t frequency_khz;	
	uint32_t ticks;		// clock ticks since last reset / power on
	uint64_t ticks_total;	// clock ticks total
	uint8_t opcode;		// holds current opcode
} __6502_system_t;



// extern uint16_t addrmask;
extern uint8_t ram[0x10000];
extern void init6502(void);
extern void reset6502(void);
extern char *mnemonics[256];
// extern uint16_t pc_reg;
// extern uint8_t a_reg, x_reg, y_reg, flag_reg, s_reg;
// extern uint8_t opcode;
// extern void (*instruction[256])();
// extern int ticks[256];
// extern uint32_t clockticks6502;
extern void irq6502();
extern void nmi6502();
extern int opcode_len[256];

#endif

