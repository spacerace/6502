#ifndef __CPU_6502_H__
#define __CPU_6502_H__

#include <inttypes.h>
#include <stdio.h>
#include <pthread.h>

//define MMIO_USE_THREADS
#define MMIO_USE_OPCODEHOOK
#define MMIO_RND_BASE	0x0300
#define MMIO_RND_ENABLE MMIO_RND_BASE
#define MMIO_RND_REG	MMIO_RND_BASE+1
#define A a_reg
#define X x_reg
#define Y y_reg
#define P flag_reg
#define S s_reg
#define PC pc_reg

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

