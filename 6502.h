#ifndef __CPU_6502_H__
#define __CPU_6502_H__

#include <inttypes.h>
#include <stdio.h>
#include <pthread.h>

// #define MMIO_RANDOM
#define MMIO_SID
// #define MMIO_TIMER

#define MMIO_USE_THREADS
#define MMIO_USE_OPCODEHOOK
#define MMIO_RND_BASE	0x0300
#define MMIO_RND_ENABLE MMIO_RND_BASE
#define MMIO_RND_REG	MMIO_RND_BASE+1
#define MMIO_TIMER_BASE	0x0310
#define MMIO_TIMER_CFG	MMIO_TIMER_BASE
#define MMIO_TIMER_REG0	MMIO_TIMER_BASE+1
#define MMIO_TIMER_REG1 MMIO_TIMER_BASE+2
#define MMIO_TIMER_REG2 MMIO_TIMER_BASE+3
#define MMIO_TIMER_REG3 MMIO_TIMER_BASE+4

#define TREG0	ram[MMIO_TIMER_REG0]
#define TREG1	ram[MMIO_TIMER_REG1]
#define TREG2	ram[MMIO_TIMER_REG2]
#define TREG3	ram[MMIO_TIMER_REG3]

#define MMIO_SID_BASE	0x0400
#define MMIO_SID_ENABLE 0x03FF

#define A a_reg
#define X x_reg
#define Y y_reg
#define P flag_reg
#define S s_reg
#define PC pc_reg


typedef struct {
	uint8_t a;
	uint8_t x;
	uint8_t y;
	uint8_t flags;
	uint8_t sp;
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
	__6502_system_reg_t reg;
	__6502_system_mem_t mem;
	__6502_system_irq_t irq;
	uint32_t frequency_khz;	
	uint32_t ticks;		// clock ticks since last reset / power on
	uint64_t ticks_total;	// clock ticks total
	int32_t sum;		// internal use
	int32_t saveflags;	// internal use
	uint16_t pc_save;	// internal use, saves state of pc
	uint16_t opcode_ticks[256];	// table of clockticks of instruction set
	void (*adrmode[256])();	// table of function ptrs to the addressing-
				// mode "helpers"
	void (*instruction[256])();	// table of function ptrs to the 
				// opcode-handlers
	uint8_t opcode;		// holds current opcode
	uint8_t value;		// internal use, holds result of opcode

} __6502_system_t;


extern int init_mmio();
extern int stop_mmio();

extern uint16_t addrmask;
extern uint8_t ram[0x10000];
extern void init6502(void);
extern void reset6502(void);
extern char *mnemonics[256];
extern uint16_t pc_reg;
extern uint8_t a_reg, x_reg, y_reg, flag_reg, s_reg;
extern uint8_t opcode;
extern void (*instruction[256])();
extern int ticks[256];
extern uint32_t clockticks6502;
extern void irq6502();
extern void nmi6502();
extern int opcode_len[256];

extern void mmio_opcodehook();
extern void mmio_irqhook();
extern void mmio_nmihook();
#endif

