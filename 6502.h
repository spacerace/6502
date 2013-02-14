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

#define INITIAL_STACK	0xff

// macros with commonly used routines
#define get_reset_vector()	ram[0xfffc] | (ram[0xfffd]<<8)
#define get_nmi_vector()	ram[0xfffa] | (ram[0xfffb]<<8)
#define get_irq_vector()	ram[0xfffe] | (ram[0xffff]<<8)
#define get_pc() 	        ram[cpu[active_cpu].reg.pc]+(ram[cpu[active_cpu].reg.pc+1]<<8)    // get addr from pc into uint16_t x


/* the whole cpu and parts of the system are covered by this datatype.
 * this allows to create virtual systems with more than one cpu. a kind
 * of more or less SMP could be possible. i think of a RTOS running on a
 * fast ARM-chip.
 * we also hold the whole instruction set in this datatype, so we can
 * change opcodes on one and the other cpu remains unaffected. this can
 * be quite cool if we would emulate a 2-CPU-system with a 65c02 and a xxxxxx
 * with a little changed instruction set.
 *
 * however, this is only the datatype for handling a cpu. the emu that
 * uses this untested black magic still has to be written.
 *
 * these things are implemented yet or will be soon:
 *
 *   + cpu (registers, irq/nmi/reset callback functions)
 *   + memory image
 *   + memory mapped I/O samples: simple rng, timer (not working right now)
 *   - frequency setting
 *
 *
 * of course, this needs a large amount of memory. at the moment each cpu
 * consumes about 68k of ram.
 *
 */


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


/* holding the whole instruction-set-information in a "datatype"
 * allows us to easily switch beetween instruction sets for each
 * of our CPUs. for example an original 6502 and a 65c02. */
typedef struct {
	uint16_t opcode_ticks[256];	// table of clockticks of instruction set
	void (*adrmode[256])();		// table of function ptrs to the addressing-
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



extern uint8_t ram[0x10000];
extern void init6502(void);
extern void reset6502(void);
extern char *mnemonics[256];
extern void irq6502();
extern void nmi6502();
extern int opcode_len[256];

#endif

