#ifndef __CPU_6502_H__
#define __CPU_6502_H__

#include <inttypes.h>

#define A a_reg
#define X x_reg
#define Y y_reg
#define P flag_reg
#define S s_reg
#define PC pc_reg

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

#endif

