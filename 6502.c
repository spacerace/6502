/* 6502 emulator
 *
 * (c) 2010, 2012-2013 Nils Stec <nils.stec@gmail.com>
 * (c) ???? Neil Bradley, others
 *
 * This 6502 CPU Emu aims to be perfect - unlike other populars.
 * First attempt was only a slightly modified version of the 
 * famous asteroids simulator of Neil Bradley. At this point of
 * development this cpu-emu is a whole rewrite of this cpu core
 * and it's heavily modified. Because of that even Neil Bradley 
 * does not know where this emu came from - anywhere on the internet
 * - i'll keep him and "others" in the author field.
 *
 * I can't say wheter he's still maintaining his emu or follows
 * my work. So please don't annoy him with questions rather than me.
 *
 */


#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <string.h>
#include "6502.h"

__6502_system_t cpu[N_CPUS];
uint32_t active_cpu = 0;
uint8_t ram[0x10000];

/* helper vars */
uint8_t value;
uint16_t help;
uint16_t savepc;
int32_t sum;
int32_t saveflags;


void set_cpu(uint32_t n) {
	active_cpu = n;
	return;
}

uint32_t get_cpu() {
	return active_cpu;
}

uint32_t get_cpus() {
	return N_CPUS;
}

/* can be used for simple opcode tracing */
#define PRE_OP	0
#define PAST_OP	1
static uint64_t opcode_counter[2][256];

void pre_opcode_hook() {
	opcode_counter[PRE_OP][cpu[active_cpu].opcode]++;
	return;
}

void past_opcode_hook() {
	opcode_counter[PAST_OP][cpu[active_cpu].opcode]++;
	return;
}
/* hooks end */

/* addressing mode ABS,X
 * take an absolute address and add X to it 
 */
void absx6502() {
      	savepc = get_pc();
      	cpu[active_cpu].reg.pc += 2;

	if(cpu[active_cpu].inst.opcode_ticks[cpu[active_cpu].opcode] == 4) {				// we want correct cycle-handling, so
              	if((savepc>>8) != ((savepc+cpu[active_cpu].reg.x)>>8)) {	// add 1 if we cross a page boundary
                      cpu[active_cpu].ticks++;
		}
	}
      	savepc += cpu[active_cpu].reg.x;
	return;
}

/* addressing mode ABS,Y */
void absy6502() {
      	savepc = get_pc();
      	cpu[active_cpu].reg.pc += 2;

      	if(cpu[active_cpu].inst.opcode_ticks[cpu[active_cpu].opcode]==4)				// page boundary cross
              if ((savepc>>8) != ((savepc+cpu[active_cpu].reg.y)>>8))
                      cpu[active_cpu].ticks++;
      	savepc += cpu[active_cpu].reg.y;
}

/* ZP */
void zp6502() {
      	savepc=ram[cpu[active_cpu].reg.pc++];
}

/* ZP,X */
void zpx6502() {
      	savepc=ram[cpu[active_cpu].reg.pc++]+cpu[active_cpu].reg.x;
      	savepc &= 0x00ff;	// maybe faster when setting savepc to 0 first?
}

/* ZP,Y */
void zpy6502() {
      	savepc=ram[cpu[active_cpu].reg.pc++]+cpu[active_cpu].reg.y;
      	savepc &= 0x00ff;
}

/* (ZP,X) */
void indx6502() {
      	value = ram[cpu[active_cpu].reg.pc++]+cpu[active_cpu].reg.x;
      	savepc = ram[value] + (ram[value+1] << 8);
}

/* (ZP),Y */
void indy6502() {
      	value = ram[cpu[active_cpu].reg.pc++];
      	savepc = ram[value] + (ram[value+1] << 8);
      	if (cpu[active_cpu].inst.opcode_ticks[cpu[active_cpu].opcode]==5)
              if ((savepc>>8) != ((savepc+cpu[active_cpu].reg.y)>>8))
                      cpu[active_cpu].ticks++;
      	savepc += cpu[active_cpu].reg.y;
}

/* (ABS,X) */
void indabsx6502() {
      	help = ram[cpu[active_cpu].reg.pc] + (ram[cpu[active_cpu].reg.pc + 1] << 8) + cpu[active_cpu].reg.x;
      	savepc = ram[help] + (ram[help + 1] << 8);
}

/* (ZP) */
void indzp6502() {
      	value = ram[cpu[active_cpu].reg.pc++];
      	savepc = ram[value] + (ram[value + 1] << 8);
}

/* Instructions */
void adc6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	value = ram[savepc];

      	saveflags = (cpu[active_cpu].reg.flags & FLAG_CARRY);			// get old carry

      	sum = ((char) cpu[active_cpu].reg.a) + ((char) value) + saveflags;	// do the ADC calculation with respect to old carry

	/* check wheter overflown or not */
      	if((sum>0x7f) || (sum<-0x80)) 
		cpu[active_cpu].reg.flags |= FLAG_OVF; else cpu[active_cpu].reg.flags &= ~FLAG_OVF;
      	
	/* if overflown our result would change, calculate again */
	sum = cpu[active_cpu].reg.a + value + saveflags;

      	/* check wheter we have to set carry */
	if(sum>0xff) 
		cpu[active_cpu].reg.flags |= FLAG_CARRY; else cpu[active_cpu].reg.flags &= ~FLAG_CARRY;


	/* place result into accumulator */
      	cpu[active_cpu].reg.a = sum;

	/* are we working in decimal mode? */
      	if(cpu[active_cpu].reg.flags & FLAG_DEC) {
		cpu[active_cpu].reg.flags &= ~FLAG_CARRY;
              	if((cpu[active_cpu].reg.a & 0x0f)>0x09) cpu[active_cpu].reg.a += 0x06;
              	if((cpu[active_cpu].reg.a & 0xf0)>0x90) {
                      cpu[active_cpu].reg.a += 0x60;
                      cpu[active_cpu].reg.flags |= FLAG_CARRY;
              	}
      	} else {
              cpu[active_cpu].ticks++;
      	}

	/* result zero ? */
      	if(cpu[active_cpu].reg.a) 	cpu[active_cpu].reg.sp &= ~FLAG_ZERO; 
	else 				cpu[active_cpu].reg.sp |= FLAG_ZERO;

      	/* result negative ? */
	if(cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= FLAG_NEG; 
	else 				 cpu[active_cpu].reg.sp &= ~FLAG_NEG;
}

void and6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	value = ram[savepc];

      	cpu[active_cpu].reg.a &= value;		// here we do the AND-operation on our accumulator

	// do some tests on result for correct flag handling

	/* is result zero ? */
	if(cpu[active_cpu].reg.a) cpu[active_cpu].reg.flags &= ~FLAG_ZERO;
	else 			  cpu[active_cpu].reg.flags |= FLAG_ZERO;

	/* overflown ? */
     	if(cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.flags |= FLAG_NEG; 
	else 				 cpu[active_cpu].reg.flags &= ~FLAG_NEG;
}

/* arithmetic shift left */
void asl6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	value = ram[savepc];

      	cpu[active_cpu].reg.flags = (cpu[active_cpu].reg.flags & 0xfe) | ((value >> 7) & 0x01);
      	
	value = value << 1;
      	ram[savepc] = value;

	if(value) cpu[active_cpu].reg.flags &= ~FLAG_ZERO; 
	else      cpu[active_cpu].reg.flags |= FLAG_ZERO;

	if(value & 0x80) cpu[active_cpu].reg.flags |= FLAG_NEG; 
	else 		 cpu[active_cpu].reg.flags &= ~FLAG_NEG;
}

void asla6502() {
      	cpu[active_cpu].reg.sp= (cpu[active_cpu].reg.flags & 0xfe) | ((cpu[active_cpu].reg.a >> 7) & 0x01);

      	cpu[active_cpu].reg.a = cpu[active_cpu].reg.a << 1;

      	if(cpu[active_cpu].reg.a) cpu[active_cpu].reg.flags &= ~FLAG_ZERO; 
	else 			  cpu[active_cpu].reg.flags |= FLAG_ZERO;

      	if(cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.flags |= FLAG_NEG; 
	else 				 cpu[active_cpu].reg.flags &= ~FLAG_NEG;
}


/* branch on carry clear */
void bcc6502() {
      	if(IS_FLAG_CARRY_CLEAR) {
              cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
              cpu[active_cpu].reg.pc += savepc;
              cpu[active_cpu].ticks++;
      	}
      	else value = ram[cpu[active_cpu].reg.pc++];
	return;
}

/* branch on carry set */
void bcs6502() {
      	if (IS_FLAG_CARRY_SET) {
              cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
              cpu[active_cpu].reg.pc += savepc;
              cpu[active_cpu].ticks++;
      	} else value=ram[cpu[active_cpu].reg.pc++];
	return;
}

/* branch on equal */
void beq6502() {
      	if(IS_FLAG_ZERO_SET) {
              cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
              cpu[active_cpu].reg.pc += savepc;
              cpu[active_cpu].ticks++;
      	} else value=ram[cpu[active_cpu].reg.pc++];
	return;
}

/* TODO check */
/* check bit, set flags corresponding to result */
void bit6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	value=ram[savepc];

      	/* non-destrucive logically And between value and the accumulator
      	 * and set zero flag */
      	if(value & cpu[active_cpu].reg.a) cpu[active_cpu].reg.flags &= ~FLAG_ZERO; 
	else 				  cpu[active_cpu].reg.flags |= FLAG_ZERO;

	/* TODO CHECKECHKECHKKADSJLKAJSDLKAJSDL */
      	/* set negative and overflow flags from value */
      	cpu[active_cpu].reg.flags = (cpu[active_cpu].reg.flags & 0x3f) | (value & 0xc0);

	return;
}

/* branch on minus */
void bmi6502() {
      	if(IS_FLAG_NEG_SET) {
		cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
              	cpu[active_cpu].reg.pc += savepc;
              	cpu[active_cpu].ticks++;
      	} else value = ram[cpu[active_cpu].reg.pc++];

	return;
}

/* branch on not equal */
void bne6502() {
      	if(IS_FLAG_ZERO_CLEAR) {
        	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
              	cpu[active_cpu].reg.pc += savepc;
              	cpu[active_cpu].ticks++;
      	}
      	else value = ram[cpu[active_cpu].reg.pc++];

	return;
}

/* branch on plus */
void bpl6502() {
      	if(IS_FLAG_NEG_CLEAR) {
              	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
              	cpu[active_cpu].reg.pc += savepc;
             	cpu[active_cpu].ticks++;
      	}
      	else value = ram[cpu[active_cpu].reg.pc++];
}


/*
 * TODO:
 * I found some information like this:
 *  A characteristic of the BRK and COP instructions is
 *  that the processor treats either as a two byte
 *  instruction: the cpu[active_cpu].opcode itself and the following
 *  byte, which is referred to as the "signature."
 *  Upon execution of BRK or COP, the processor will
 *  add two to the program counter prior to pushing
 *  it to the stack.
 *  Hence when RTI (ReTurn from Interrupt) is executed,
 *  the interrupted program will continue at the address
 *  immediately following the signature.
 */
void brk6502() {
      	cpu[active_cpu].reg.pc++;
	ram[0x0100+cpu[active_cpu].reg.sp--] = (uint8_t)(cpu[active_cpu].reg.pc>>8);		// save pc to stack
	ram[0x0100+cpu[active_cpu].reg.sp--] = (uint8_t)(cpu[active_cpu].reg.pc & 0xff);	//   -""-
	ram[0x0100+cpu[active_cpu].reg.sp--] = cpu[active_cpu].reg.flags;			// save flags to stack
	cpu[active_cpu].reg.flags |= 0x14;				// set flags
      	cpu[active_cpu].reg.pc = ram[0xfffe] + (ram[0xffff] << 8);	// set pc to vector @0xfffe
}

/* branch on overflow clear */
void bvc6502() {
      	if(IS_FLAG_OVF_CLEAR) {	// check V bit
              	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
              	cpu[active_cpu].reg.pc += savepc;
              	cpu[active_cpu].ticks++;
      	}
      	else value = ram[cpu[active_cpu].reg.pc++];

	return;
}

/* branch on overflow set */
void bvs6502() {
      	if(IS_FLAG_OVF_SET) {
              	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
              	cpu[active_cpu].reg.pc += savepc;
              	cpu[active_cpu].ticks++;
      	}
      	else value = ram[cpu[active_cpu].reg.pc++];

	return;
}


/* clear carry */
void clc6502() {
      	cpu[active_cpu].reg.flags &= ~FLAG_CARRY;
}

/* clear decimal */
void cld6502() {
      	cpu[active_cpu].reg.flags &= ~FLAG_DEC;
}

/* clear interrupt */
void cli6502() {
      	cpu[active_cpu].reg.flags &= ~FLAG_INT;
}

/* clear overflow */
void clv6502() {
      	cpu[active_cpu].reg.flags &= ~FLAG_OVF;
}

/* compare */
void cmp6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	value = ram[savepc];
      	if (cpu[active_cpu].reg.a+0x100-value>0xff) cpu[active_cpu].reg.sp |= 0x01; else cpu[active_cpu].reg.sp &= 0xfe;
      	value=cpu[active_cpu].reg.a+0x100-value;
      	if (value) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (value & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void cpx6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	value = ram[savepc];
      	if (cpu[active_cpu].reg.x+0x100-value>0xff) cpu[active_cpu].reg.sp |= 0x01; else cpu[active_cpu].reg.sp &= 0xfe;
      	value=cpu[active_cpu].reg.x+0x100-value;
      	if (value) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (value & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void cpy6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	value = ram[savepc];
      	if (cpu[active_cpu].reg.y+0x100-value>0xff) cpu[active_cpu].reg.sp |= 0x01; else cpu[active_cpu].reg.sp &= 0xfe;
      	value=cpu[active_cpu].reg.y+0x100-value;
      	if (value) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (value & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void dec6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	ram[savepc]--;
      	value = ram[savepc];
      	if (value) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (value & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void dex6502() {
      	cpu[active_cpu].reg.x--;
      	if (cpu[active_cpu].reg.x) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.x & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void dey6502() {
      	cpu[active_cpu].reg.y--;
      	if (cpu[active_cpu].reg.y) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.y & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void eor6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	cpu[active_cpu].reg.a ^= ram[savepc];
      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void inc6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	ram[savepc]++;
      	value = ram[savepc];
      	if (value) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (value & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void inx6502() {
      	cpu[active_cpu].reg.x++;
      	if (cpu[active_cpu].reg.x) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.x & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void iny6502() {
      	cpu[active_cpu].reg.y++;
      	if (cpu[active_cpu].reg.y) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.y & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

/* jump */
void jmp6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	cpu[active_cpu].reg.pc=savepc;
}

void jsr6502() {
      	cpu[active_cpu].reg.pc++;
	ram[0x0100+cpu[active_cpu].reg.sp--] = (uint8_t)(cpu[active_cpu].reg.pc >> 8);
	ram[0x0100+cpu[active_cpu].reg.sp--] = (uint8_t)(cpu[active_cpu].reg.pc & 0xff);
      	cpu[active_cpu].reg.pc--;
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	cpu[active_cpu].reg.pc=savepc;
}

void lda6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	cpu[active_cpu].reg.a=ram[savepc];
      	// set the zero flag
      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	// set the negative flag
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void ldx6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	cpu[active_cpu].reg.x=ram[savepc];
      	if (cpu[active_cpu].reg.x) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.x & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void ldy6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	cpu[active_cpu].reg.y=ram[savepc];
      	if (cpu[active_cpu].reg.y) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.y & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void lsr6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	value=ram[savepc];

      	/* set carry flag if shifting right causes a bit to be lost */
      	cpu[active_cpu].reg.sp = (cpu[active_cpu].reg.sp & 0xfe) | (value & 0x01);

      	value = value >>1;
	ram[savepc] = value;

      	/* set zero flag if value is zero */
      	if (value != 0) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;

      	/* set negative flag if bit 8 set??? can this happen on an LSR? */
      	if ((value & 0x80) == 0x80)
        	cpu[active_cpu].reg.sp |= 0x80;
      	else
        	cpu[active_cpu].reg.sp &= 0x7f;
}

void lsra6502() {
	cpu[active_cpu].reg.sp= (cpu[active_cpu].reg.sp & 0xfe) | (cpu[active_cpu].reg.a & 0x01);
     	cpu[active_cpu].reg.a = cpu[active_cpu].reg.a >>1;
      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}


/* THE WORLDS GREATEST INSTRUCTION!!!!! IT GETS GREATER AND GREATER WITH EVERY CPU FAMILY USING IT 
 *
 *  ..... do nothing */
void nop6502() {

}

void ora6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	cpu[active_cpu].reg.a |= ram[savepc];
      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void pha6502() {
      	ram[0x100+cpu[active_cpu].reg.sp--] = cpu[active_cpu].reg.a;
}

/* TODO CHECK */
void php6502() {
      	ram[0x100+cpu[active_cpu].reg.sp--] = cpu[active_cpu].reg.flags;
}

void pla6502() {
      	cpu[active_cpu].reg.a=ram[++cpu[active_cpu].reg.sp+0x100];

      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}
/* TODO CHECK */
void plp6502() {
      	cpu[active_cpu].reg.flags=ram[++cpu[active_cpu].reg.sp+0x100] | 0x20;
}

void rol6502() {
      	saveflags=(cpu[active_cpu].reg.sp & 0x01);
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	value = ram[savepc];
      	cpu[active_cpu].reg.sp= (cpu[active_cpu].reg.sp & 0xfe) | ((value >>7) & 0x01);
      	value = value << 1;
      	value |= saveflags;

	ram[savepc] = value;

	if (value) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (value & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void rola6502() {
      	saveflags=(cpu[active_cpu].reg.sp & 0x01);
      	cpu[active_cpu].reg.sp = (cpu[active_cpu].reg.sp & 0xfe) | ((cpu[active_cpu].reg.a >>7) & 0x01);
      	cpu[active_cpu].reg.a = cpu[active_cpu].reg.a << 1;
      	cpu[active_cpu].reg.a |= saveflags;
      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void ror6502() {
      	saveflags=(cpu[active_cpu].reg.sp & 0x01);
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	value=ram[savepc];
      	cpu[active_cpu].reg.sp= (cpu[active_cpu].reg.sp & 0xfe) | (value & 0x01);
      	value = value >>1;
      	if (saveflags) value |= 0x80;

	ram[savepc] = value;

	if (value) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (value & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void rora6502() {
      	saveflags=(cpu[active_cpu].reg.sp & 0x01);
      	cpu[active_cpu].reg.sp = (cpu[active_cpu].reg.sp & 0xfe) | (cpu[active_cpu].reg.a & 0x01);
      	cpu[active_cpu].reg.a = cpu[active_cpu].reg.a >>1;
      	if (saveflags) cpu[active_cpu].reg.a |= 0x80;
      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}
/* TODO CHECK */
void rti6502() {
	cpu[active_cpu].reg.flags=ram[++cpu[active_cpu].reg.sp+0x100] | 0x20;
      	cpu[active_cpu].reg.pc=ram[++cpu[active_cpu].reg.sp+0x100];
      	cpu[active_cpu].reg.pc |= (ram[++cpu[active_cpu].reg.sp+0x100] << 8);
}
/* TODO CHECK */
void rts6502() {
      	cpu[active_cpu].reg.pc=ram[++cpu[active_cpu].reg.sp+0x100];
      	cpu[active_cpu].reg.pc |= (ram[++cpu[active_cpu].reg.sp+0x100] << 8);
      	cpu[active_cpu].reg.pc++;
}

void sbc6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	value = ram[savepc] ^ 0xff;
      	saveflags=(cpu[active_cpu].reg.sp & 0x01);
      	sum= ((char) cpu[active_cpu].reg.a) + ((char) value) + (saveflags << 4);
      	if ((sum>0x7f) || (sum<-0x80)) cpu[active_cpu].reg.sp |= 0x40; else cpu[active_cpu].reg.sp &= 0xbf;
      	sum= cpu[active_cpu].reg.a + value + saveflags;
      	if (sum>0xff) cpu[active_cpu].reg.sp |= 0x01; else cpu[active_cpu].reg.sp &= 0xfe;
      	cpu[active_cpu].reg.a=sum;
      	if (cpu[active_cpu].reg.sp & 0x08) {
              cpu[active_cpu].reg.a -= 0x66;
              cpu[active_cpu].reg.sp &= 0xfe;
              if ((cpu[active_cpu].reg.a & 0x0f)>0x09)
                      cpu[active_cpu].reg.a += 0x06;
              if ((cpu[active_cpu].reg.a & 0xf0)>0x90) {
                      cpu[active_cpu].reg.a += 0x60;
                      cpu[active_cpu].reg.sp |= 0x01;
              }
      	} else {
              cpu[active_cpu].ticks++;
      	}
      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

/* set carry */
void sec6502() {
      	cpu[active_cpu].reg.flags |= FLAG_CARRY;
}

/* set dec */
void sed6502() {
      	cpu[active_cpu].reg.flags |= FLAG_DEC;
}

/* set int */
void sei6502() {
      	cpu[active_cpu].reg.flags |= FLAG_INT;
}

/* store acc */
void sta6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
	ram[savepc] = cpu[active_cpu].reg.a;
}

/* store X */
void stx6502() {
	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	ram[savepc] = cpu[active_cpu].reg.x;
}

/* store Y */
void sty6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	ram[savepc] = cpu[active_cpu].reg.y;
}

/* transfer A to X */
void tax6502() {
      	cpu[active_cpu].reg.x=cpu[active_cpu].reg.a;
      	if (cpu[active_cpu].reg.x) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.x & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

/* transfer A to Y */
void tay6502() {
      	cpu[active_cpu].reg.y=cpu[active_cpu].reg.a;
      	if (cpu[active_cpu].reg.y) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.y & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

/* TODO CHECK */
/* transfer SP to X */
void tsx6502() {
      	cpu[active_cpu].reg.x = cpu[active_cpu].reg.sp;
      	if (cpu[active_cpu].reg.x) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.x & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void txa6502() {
      	cpu[active_cpu].reg.a=cpu[active_cpu].reg.x;
      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void txs6502() {
      	cpu[active_cpu].reg.sp=cpu[active_cpu].reg.x;
}

void tya6502() {
      	cpu[active_cpu].reg.a=cpu[active_cpu].reg.y;
      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void bra6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	cpu[active_cpu].reg.pc += savepc;
      	cpu[active_cpu].ticks++;
}

void dea6502() {
      	cpu[active_cpu].reg.a--;
      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void ina6502() {
      	cpu[active_cpu].reg.a++;
      	if (cpu[active_cpu].reg.a) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.a & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void phx6502() {
      	ram[0x100+cpu[active_cpu].reg.sp--] = cpu[active_cpu].reg.x;
}

void plx6502() {
      	cpu[active_cpu].reg.x=ram[++cpu[active_cpu].reg.sp+0x100];
      	if (cpu[active_cpu].reg.x) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.x & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void phy6502() {
	ram[0x100+cpu[active_cpu].reg.sp--] = cpu[active_cpu].reg.y;
}

void ply6502() {
      	cpu[active_cpu].reg.y=ram[++cpu[active_cpu].reg.sp+0x100];

      	if (cpu[active_cpu].reg.y) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
      	if (cpu[active_cpu].reg.y & 0x80) cpu[active_cpu].reg.sp |= 0x80; else cpu[active_cpu].reg.sp &= 0x7f;
}

void stz6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
	ram[savepc] = 0;
}

void tsb6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	ram[savepc] |= cpu[active_cpu].reg.a;
      	if (ram[savepc]) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
}

void trb6502() {
      	cpu[active_cpu].inst.adrmode[cpu[active_cpu].opcode]();
      	ram[savepc] = ram[savepc] & (cpu[active_cpu].reg.a ^ 0xff);
      	if (ram[savepc]) cpu[active_cpu].reg.sp &= 0xfd; else cpu[active_cpu].reg.sp |= 0x02;
}

/* Reset CPU
 *  on a reset the default 6502 state will be set:
 *  registers A, X, Y = 0
 *  stack pointer = 0xff
 *  flags to default = 0x20
 */
void reset6502() {
	int i;

	for(i = 0; i < 256; i++) {
		opcode_counter[PRE_OP][i] = 0;
		opcode_counter[PAST_OP][i] = 0;
	}

       	cpu[active_cpu].reg.a = \
        cpu[active_cpu].reg.x = \
        cpu[active_cpu].reg.y = \
        cpu[active_cpu].reg.flags = 0;	// these are set now 0
       	cpu[active_cpu].reg.flags |= 0x20;
       	cpu[active_cpu].reg.sp = INITIAL_STACK;
	//cpu[active_cpu].reg.pc = ram[0xfffc];
        //cpu[active_cpu].reg.pc |= ram[0xfffd] << 8;
	cpu[active_cpu].reg.pc = get_reset_vector();
	cpu[active_cpu].ticks = 0;		// clock ticks since last reset / power on
	cpu[active_cpu].ticks_total = 0;
}

/* TODO TODO TODO
 * i dont have in mind WHAT exactly will be saved to stack, check the
 * nmi6502 and irq6502 functions for correctnes */

/* Non maskerable interrupt */
void nmi6502() {
        ram[0x0100+cpu[active_cpu].reg.sp--] = (uint8_t)(cpu[active_cpu].reg.pc>>8);
        ram[0x0100+cpu[active_cpu].reg.sp--] = (uint8_t)(cpu[active_cpu].reg.pc & 0xff);
        ram[0x0100+cpu[active_cpu].reg.sp--] = cpu[active_cpu].reg.flags;	// TODO check
      	cpu[active_cpu].reg.flags |= 0x04;
	//cpu[active_cpu].reg.pc = ram[0xfffa];
      	//cpu[active_cpu].reg.pc |= ram[0xfffb] << 8;
	cpu[active_cpu].reg.pc = get_nmi_vector();
}

/* Maskerable Interrupt */
void irq6502() {
	ram[0x0100+cpu[active_cpu].reg.sp--] = (uint8_t)(cpu[active_cpu].reg.pc>>8);
        ram[0x0100+cpu[active_cpu].reg.sp--] = (uint8_t)(cpu[active_cpu].reg.pc & 0xff);
        ram[0x0100+cpu[active_cpu].reg.sp--] = cpu[active_cpu].reg.flags;	// *TODO* check
   	cpu[active_cpu].reg.flags |= 0x04;
   	//cpu[active_cpu].reg.pc =  ram[0xfffe];
   	//cpu[active_cpu].reg.pc |= ram[0xffff] << 8;
	cpu[active_cpu].reg.pc = get_irq_vector();
}

/* Adressing modes */
/* Implied */
void implied6502() {
}

/* #Immediate */
void immediate6502() {
      	savepc=cpu[active_cpu].reg.pc++;
}

/* ABS */
void abs6502() {
      	savepc = ram[cpu[active_cpu].reg.pc] + (ram[cpu[active_cpu].reg.pc + 1] << 8);
      	cpu[active_cpu].reg.pc++;
      	cpu[active_cpu].reg.pc++;
}

/* Branch */
void relative6502() {
      	savepc = ram[cpu[active_cpu].reg.pc++];
      	if (savepc & 0x80) savepc -= 0x100;
      	if ((savepc>>8) != (cpu[active_cpu].reg.pc>>8))
              cpu[active_cpu].ticks++;
}

/* (ABS) */
void indirect6502() {
      	help = ram[cpu[active_cpu].reg.pc] + (ram[cpu[active_cpu].reg.pc + 1] << 8);
      	savepc = ram[help] + (ram[help + 1] << 8);
      	cpu[active_cpu].reg.pc++;
      	cpu[active_cpu].reg.pc++;
}


void init6502() {
	printf("cpu created, size %dbytes\n", sizeof(cpu));

	active_cpu = 0;

	/* default instruction set */
      	cpu[active_cpu].inst.opcode_ticks[0x00]=7; cpu[active_cpu].inst.instruction[0x00]=brk6502; cpu[active_cpu].inst.adrmode[0x00]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x01]=6; cpu[active_cpu].inst.instruction[0x01]=ora6502; cpu[active_cpu].inst.adrmode[0x01]=indx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x02]=2; cpu[active_cpu].inst.instruction[0x02]=nop6502; cpu[active_cpu].inst.adrmode[0x02]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x03]=2; cpu[active_cpu].inst.instruction[0x03]=nop6502; cpu[active_cpu].inst.adrmode[0x03]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x04]=3; cpu[active_cpu].inst.instruction[0x04]=tsb6502; cpu[active_cpu].inst.adrmode[0x04]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x05]=3; cpu[active_cpu].inst.instruction[0x05]=ora6502; cpu[active_cpu].inst.adrmode[0x05]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x06]=5; cpu[active_cpu].inst.instruction[0x06]=asl6502; cpu[active_cpu].inst.adrmode[0x06]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x07]=2; cpu[active_cpu].inst.instruction[0x07]=nop6502; cpu[active_cpu].inst.adrmode[0x07]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x08]=3; cpu[active_cpu].inst.instruction[0x08]=php6502; cpu[active_cpu].inst.adrmode[0x08]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x09]=3; cpu[active_cpu].inst.instruction[0x09]=ora6502; cpu[active_cpu].inst.adrmode[0x09]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0x0a]=2; cpu[active_cpu].inst.instruction[0x0a]=asla6502; cpu[active_cpu].inst.adrmode[0x0a]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x0b]=2; cpu[active_cpu].inst.instruction[0x0b]=nop6502; cpu[active_cpu].inst.adrmode[0x0b]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x0c]=4; cpu[active_cpu].inst.instruction[0x0c]=tsb6502; cpu[active_cpu].inst.adrmode[0x0c]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x0d]=4; cpu[active_cpu].inst.instruction[0x0d]=ora6502; cpu[active_cpu].inst.adrmode[0x0d]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x0e]=6; cpu[active_cpu].inst.instruction[0x0e]=asl6502; cpu[active_cpu].inst.adrmode[0x0e]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x0f]=2; cpu[active_cpu].inst.instruction[0x0f]=nop6502; cpu[active_cpu].inst.adrmode[0x0f]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x10]=2; cpu[active_cpu].inst.instruction[0x10]=bpl6502; cpu[active_cpu].inst.adrmode[0x10]=relative6502;
      	cpu[active_cpu].inst.opcode_ticks[0x11]=5; cpu[active_cpu].inst.instruction[0x11]=ora6502; cpu[active_cpu].inst.adrmode[0x11]=indy6502;
      	cpu[active_cpu].inst.opcode_ticks[0x12]=3; cpu[active_cpu].inst.instruction[0x12]=ora6502; cpu[active_cpu].inst.adrmode[0x12]=indzp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x13]=2; cpu[active_cpu].inst.instruction[0x13]=nop6502; cpu[active_cpu].inst.adrmode[0x13]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x14]=3; cpu[active_cpu].inst.instruction[0x14]=trb6502; cpu[active_cpu].inst.adrmode[0x14]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x15]=4; cpu[active_cpu].inst.instruction[0x15]=ora6502; cpu[active_cpu].inst.adrmode[0x15]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x16]=6; cpu[active_cpu].inst.instruction[0x16]=asl6502; cpu[active_cpu].inst.adrmode[0x16]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x17]=2; cpu[active_cpu].inst.instruction[0x17]=nop6502; cpu[active_cpu].inst.adrmode[0x17]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x18]=2; cpu[active_cpu].inst.instruction[0x18]=clc6502; cpu[active_cpu].inst.adrmode[0x18]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x19]=4; cpu[active_cpu].inst.instruction[0x19]=ora6502; cpu[active_cpu].inst.adrmode[0x19]=absy6502;
      	cpu[active_cpu].inst.opcode_ticks[0x1a]=2; cpu[active_cpu].inst.instruction[0x1a]=ina6502; cpu[active_cpu].inst.adrmode[0x1a]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x1b]=2; cpu[active_cpu].inst.instruction[0x1b]=nop6502; cpu[active_cpu].inst.adrmode[0x1b]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x1c]=4; cpu[active_cpu].inst.instruction[0x1c]=trb6502; cpu[active_cpu].inst.adrmode[0x1c]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x1d]=4; cpu[active_cpu].inst.instruction[0x1d]=ora6502; cpu[active_cpu].inst.adrmode[0x1d]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x1e]=7; cpu[active_cpu].inst.instruction[0x1e]=asl6502; cpu[active_cpu].inst.adrmode[0x1e]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x1f]=2; cpu[active_cpu].inst.instruction[0x1f]=nop6502; cpu[active_cpu].inst.adrmode[0x1f]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x20]=6; cpu[active_cpu].inst.instruction[0x20]=jsr6502; cpu[active_cpu].inst.adrmode[0x20]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x21]=6; cpu[active_cpu].inst.instruction[0x21]=and6502; cpu[active_cpu].inst.adrmode[0x21]=indx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x22]=2; cpu[active_cpu].inst.instruction[0x22]=nop6502; cpu[active_cpu].inst.adrmode[0x22]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x23]=2; cpu[active_cpu].inst.instruction[0x23]=nop6502; cpu[active_cpu].inst.adrmode[0x23]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x24]=3; cpu[active_cpu].inst.instruction[0x24]=bit6502; cpu[active_cpu].inst.adrmode[0x24]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x25]=3; cpu[active_cpu].inst.instruction[0x25]=and6502; cpu[active_cpu].inst.adrmode[0x25]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x26]=5; cpu[active_cpu].inst.instruction[0x26]=rol6502; cpu[active_cpu].inst.adrmode[0x26]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x27]=2; cpu[active_cpu].inst.instruction[0x27]=nop6502; cpu[active_cpu].inst.adrmode[0x27]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x28]=4; cpu[active_cpu].inst.instruction[0x28]=plp6502; cpu[active_cpu].inst.adrmode[0x28]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x29]=3; cpu[active_cpu].inst.instruction[0x29]=and6502; cpu[active_cpu].inst.adrmode[0x29]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0x2a]=2; cpu[active_cpu].inst.instruction[0x2a]=rola6502; cpu[active_cpu].inst.adrmode[0x2a]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x2b]=2; cpu[active_cpu].inst.instruction[0x2b]=nop6502; cpu[active_cpu].inst.adrmode[0x2b]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x2c]=4; cpu[active_cpu].inst.instruction[0x2c]=bit6502; cpu[active_cpu].inst.adrmode[0x2c]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x2d]=4; cpu[active_cpu].inst.instruction[0x2d]=and6502; cpu[active_cpu].inst.adrmode[0x2d]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x2e]=6; cpu[active_cpu].inst.instruction[0x2e]=rol6502; cpu[active_cpu].inst.adrmode[0x2e]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x2f]=2; cpu[active_cpu].inst.instruction[0x2f]=nop6502; cpu[active_cpu].inst.adrmode[0x2f]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x30]=2; cpu[active_cpu].inst.instruction[0x30]=bmi6502; cpu[active_cpu].inst.adrmode[0x30]=relative6502;
      	cpu[active_cpu].inst.opcode_ticks[0x31]=5; cpu[active_cpu].inst.instruction[0x31]=and6502; cpu[active_cpu].inst.adrmode[0x31]=indy6502;
      	cpu[active_cpu].inst.opcode_ticks[0x32]=3; cpu[active_cpu].inst.instruction[0x32]=and6502; cpu[active_cpu].inst.adrmode[0x32]=indzp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x33]=2; cpu[active_cpu].inst.instruction[0x33]=nop6502; cpu[active_cpu].inst.adrmode[0x33]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x34]=4; cpu[active_cpu].inst.instruction[0x34]=bit6502; cpu[active_cpu].inst.adrmode[0x34]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x35]=4; cpu[active_cpu].inst.instruction[0x35]=and6502; cpu[active_cpu].inst.adrmode[0x35]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x36]=6; cpu[active_cpu].inst.instruction[0x36]=rol6502; cpu[active_cpu].inst.adrmode[0x36]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x37]=2; cpu[active_cpu].inst.instruction[0x37]=nop6502; cpu[active_cpu].inst.adrmode[0x37]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x38]=2; cpu[active_cpu].inst.instruction[0x38]=sec6502; cpu[active_cpu].inst.adrmode[0x38]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x39]=4; cpu[active_cpu].inst.instruction[0x39]=and6502; cpu[active_cpu].inst.adrmode[0x39]=absy6502;
      	cpu[active_cpu].inst.opcode_ticks[0x3a]=2; cpu[active_cpu].inst.instruction[0x3a]=dea6502; cpu[active_cpu].inst.adrmode[0x3a]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x3b]=2; cpu[active_cpu].inst.instruction[0x3b]=nop6502; cpu[active_cpu].inst.adrmode[0x3b]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x3c]=4; cpu[active_cpu].inst.instruction[0x3c]=bit6502; cpu[active_cpu].inst.adrmode[0x3c]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x3d]=4; cpu[active_cpu].inst.instruction[0x3d]=and6502; cpu[active_cpu].inst.adrmode[0x3d]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x3e]=7; cpu[active_cpu].inst.instruction[0x3e]=rol6502; cpu[active_cpu].inst.adrmode[0x3e]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x3f]=2; cpu[active_cpu].inst.instruction[0x3f]=nop6502; cpu[active_cpu].inst.adrmode[0x3f]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x40]=6; cpu[active_cpu].inst.instruction[0x40]=rti6502; cpu[active_cpu].inst.adrmode[0x40]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x41]=6; cpu[active_cpu].inst.instruction[0x41]=eor6502; cpu[active_cpu].inst.adrmode[0x41]=indx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x42]=2; cpu[active_cpu].inst.instruction[0x42]=nop6502; cpu[active_cpu].inst.adrmode[0x42]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x43]=2; cpu[active_cpu].inst.instruction[0x43]=nop6502; cpu[active_cpu].inst.adrmode[0x43]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x44]=2; cpu[active_cpu].inst.instruction[0x44]=nop6502; cpu[active_cpu].inst.adrmode[0x44]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x45]=3; cpu[active_cpu].inst.instruction[0x45]=eor6502; cpu[active_cpu].inst.adrmode[0x45]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x46]=5; cpu[active_cpu].inst.instruction[0x46]=lsr6502; cpu[active_cpu].inst.adrmode[0x46]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x47]=2; cpu[active_cpu].inst.instruction[0x47]=nop6502; cpu[active_cpu].inst.adrmode[0x47]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x48]=3; cpu[active_cpu].inst.instruction[0x48]=pha6502; cpu[active_cpu].inst.adrmode[0x48]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x49]=3; cpu[active_cpu].inst.instruction[0x49]=eor6502; cpu[active_cpu].inst.adrmode[0x49]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0x4a]=2; cpu[active_cpu].inst.instruction[0x4a]=lsra6502; cpu[active_cpu].inst.adrmode[0x4a]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x4b]=2; cpu[active_cpu].inst.instruction[0x4b]=nop6502; cpu[active_cpu].inst.adrmode[0x4b]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x4c]=3; cpu[active_cpu].inst.instruction[0x4c]=jmp6502; cpu[active_cpu].inst.adrmode[0x4c]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x4d]=4; cpu[active_cpu].inst.instruction[0x4d]=eor6502; cpu[active_cpu].inst.adrmode[0x4d]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x4e]=6; cpu[active_cpu].inst.instruction[0x4e]=lsr6502; cpu[active_cpu].inst.adrmode[0x4e]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x4f]=2; cpu[active_cpu].inst.instruction[0x4f]=nop6502; cpu[active_cpu].inst.adrmode[0x4f]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x50]=2; cpu[active_cpu].inst.instruction[0x50]=bvc6502; cpu[active_cpu].inst.adrmode[0x50]=relative6502;
      	cpu[active_cpu].inst.opcode_ticks[0x51]=5; cpu[active_cpu].inst.instruction[0x51]=eor6502; cpu[active_cpu].inst.adrmode[0x51]=indy6502;
      	cpu[active_cpu].inst.opcode_ticks[0x52]=3; cpu[active_cpu].inst.instruction[0x52]=eor6502; cpu[active_cpu].inst.adrmode[0x52]=indzp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x53]=2; cpu[active_cpu].inst.instruction[0x53]=nop6502; cpu[active_cpu].inst.adrmode[0x53]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x54]=2; cpu[active_cpu].inst.instruction[0x54]=nop6502; cpu[active_cpu].inst.adrmode[0x54]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x55]=4; cpu[active_cpu].inst.instruction[0x55]=eor6502; cpu[active_cpu].inst.adrmode[0x55]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x56]=6; cpu[active_cpu].inst.instruction[0x56]=lsr6502; cpu[active_cpu].inst.adrmode[0x56]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x57]=2; cpu[active_cpu].inst.instruction[0x57]=nop6502; cpu[active_cpu].inst.adrmode[0x57]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x58]=2; cpu[active_cpu].inst.instruction[0x58]=cli6502; cpu[active_cpu].inst.adrmode[0x58]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x59]=4; cpu[active_cpu].inst.instruction[0x59]=eor6502; cpu[active_cpu].inst.adrmode[0x59]=absy6502;
      	cpu[active_cpu].inst.opcode_ticks[0x5a]=3; cpu[active_cpu].inst.instruction[0x5a]=phy6502; cpu[active_cpu].inst.adrmode[0x5a]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x5b]=2; cpu[active_cpu].inst.instruction[0x5b]=nop6502; cpu[active_cpu].inst.adrmode[0x5b]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x5c]=2; cpu[active_cpu].inst.instruction[0x5c]=nop6502; cpu[active_cpu].inst.adrmode[0x5c]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x5d]=4; cpu[active_cpu].inst.instruction[0x5d]=eor6502; cpu[active_cpu].inst.adrmode[0x5d]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x5e]=7; cpu[active_cpu].inst.instruction[0x5e]=lsr6502; cpu[active_cpu].inst.adrmode[0x5e]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x5f]=2; cpu[active_cpu].inst.instruction[0x5f]=nop6502; cpu[active_cpu].inst.adrmode[0x5f]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x60]=6; cpu[active_cpu].inst.instruction[0x60]=rts6502; cpu[active_cpu].inst.adrmode[0x60]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x61]=6; cpu[active_cpu].inst.instruction[0x61]=adc6502; cpu[active_cpu].inst.adrmode[0x61]=indx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x62]=2; cpu[active_cpu].inst.instruction[0x62]=nop6502; cpu[active_cpu].inst.adrmode[0x62]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x63]=2; cpu[active_cpu].inst.instruction[0x63]=nop6502; cpu[active_cpu].inst.adrmode[0x63]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x64]=3; cpu[active_cpu].inst.instruction[0x64]=stz6502; cpu[active_cpu].inst.adrmode[0x64]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x65]=3; cpu[active_cpu].inst.instruction[0x65]=adc6502; cpu[active_cpu].inst.adrmode[0x65]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x66]=5; cpu[active_cpu].inst.instruction[0x66]=ror6502; cpu[active_cpu].inst.adrmode[0x66]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x67]=2; cpu[active_cpu].inst.instruction[0x67]=nop6502; cpu[active_cpu].inst.adrmode[0x67]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x68]=4; cpu[active_cpu].inst.instruction[0x68]=pla6502; cpu[active_cpu].inst.adrmode[0x68]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x69]=3; cpu[active_cpu].inst.instruction[0x69]=adc6502; cpu[active_cpu].inst.adrmode[0x69]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0x6a]=2; cpu[active_cpu].inst.instruction[0x6a]=rora6502; cpu[active_cpu].inst.adrmode[0x6a]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x6b]=2; cpu[active_cpu].inst.instruction[0x6b]=nop6502; cpu[active_cpu].inst.adrmode[0x6b]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x6c]=5; cpu[active_cpu].inst.instruction[0x6c]=jmp6502; cpu[active_cpu].inst.adrmode[0x6c]=indirect6502;
      	cpu[active_cpu].inst.opcode_ticks[0x6d]=4; cpu[active_cpu].inst.instruction[0x6d]=adc6502; cpu[active_cpu].inst.adrmode[0x6d]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x6e]=6; cpu[active_cpu].inst.instruction[0x6e]=ror6502; cpu[active_cpu].inst.adrmode[0x6e]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x6f]=2; cpu[active_cpu].inst.instruction[0x6f]=nop6502; cpu[active_cpu].inst.adrmode[0x6f]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x70]=2; cpu[active_cpu].inst.instruction[0x70]=bvs6502; cpu[active_cpu].inst.adrmode[0x70]=relative6502;
      	cpu[active_cpu].inst.opcode_ticks[0x71]=5; cpu[active_cpu].inst.instruction[0x71]=adc6502; cpu[active_cpu].inst.adrmode[0x71]=indy6502;
      	cpu[active_cpu].inst.opcode_ticks[0x72]=3; cpu[active_cpu].inst.instruction[0x72]=adc6502; cpu[active_cpu].inst.adrmode[0x72]=indzp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x73]=2; cpu[active_cpu].inst.instruction[0x73]=nop6502; cpu[active_cpu].inst.adrmode[0x73]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x74]=4; cpu[active_cpu].inst.instruction[0x74]=stz6502; cpu[active_cpu].inst.adrmode[0x74]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x75]=4; cpu[active_cpu].inst.instruction[0x75]=adc6502; cpu[active_cpu].inst.adrmode[0x75]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x76]=6; cpu[active_cpu].inst.instruction[0x76]=ror6502; cpu[active_cpu].inst.adrmode[0x76]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x77]=2; cpu[active_cpu].inst.instruction[0x77]=nop6502; cpu[active_cpu].inst.adrmode[0x77]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x78]=2; cpu[active_cpu].inst.instruction[0x78]=sei6502; cpu[active_cpu].inst.adrmode[0x78]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x79]=4; cpu[active_cpu].inst.instruction[0x79]=adc6502; cpu[active_cpu].inst.adrmode[0x79]=absy6502;
      	cpu[active_cpu].inst.opcode_ticks[0x7a]=4; cpu[active_cpu].inst.instruction[0x7a]=ply6502; cpu[active_cpu].inst.adrmode[0x7a]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x7b]=2; cpu[active_cpu].inst.instruction[0x7b]=nop6502; cpu[active_cpu].inst.adrmode[0x7b]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x7c]=6; cpu[active_cpu].inst.instruction[0x7c]=jmp6502; cpu[active_cpu].inst.adrmode[0x7c]=indabsx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x7d]=4; cpu[active_cpu].inst.instruction[0x7d]=adc6502; cpu[active_cpu].inst.adrmode[0x7d]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x7e]=7; cpu[active_cpu].inst.instruction[0x7e]=ror6502; cpu[active_cpu].inst.adrmode[0x7e]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x7f]=2; cpu[active_cpu].inst.instruction[0x7f]=nop6502; cpu[active_cpu].inst.adrmode[0x7f]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x80]=2; cpu[active_cpu].inst.instruction[0x80]=bra6502; cpu[active_cpu].inst.adrmode[0x80]=relative6502;
      	cpu[active_cpu].inst.opcode_ticks[0x81]=6; cpu[active_cpu].inst.instruction[0x81]=sta6502; cpu[active_cpu].inst.adrmode[0x81]=indx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x82]=2; cpu[active_cpu].inst.instruction[0x82]=nop6502; cpu[active_cpu].inst.adrmode[0x82]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x83]=2; cpu[active_cpu].inst.instruction[0x83]=nop6502; cpu[active_cpu].inst.adrmode[0x83]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x84]=2; cpu[active_cpu].inst.instruction[0x84]=sty6502; cpu[active_cpu].inst.adrmode[0x84]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x85]=2; cpu[active_cpu].inst.instruction[0x85]=sta6502; cpu[active_cpu].inst.adrmode[0x85]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x86]=2; cpu[active_cpu].inst.instruction[0x86]=stx6502; cpu[active_cpu].inst.adrmode[0x86]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x87]=2; cpu[active_cpu].inst.instruction[0x87]=nop6502; cpu[active_cpu].inst.adrmode[0x87]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x88]=2; cpu[active_cpu].inst.instruction[0x88]=dey6502; cpu[active_cpu].inst.adrmode[0x88]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x89]=2; cpu[active_cpu].inst.instruction[0x89]=bit6502; cpu[active_cpu].inst.adrmode[0x89]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0x8a]=2; cpu[active_cpu].inst.instruction[0x8a]=txa6502; cpu[active_cpu].inst.adrmode[0x8a]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x8b]=2; cpu[active_cpu].inst.instruction[0x8b]=nop6502; cpu[active_cpu].inst.adrmode[0x8b]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x8c]=4; cpu[active_cpu].inst.instruction[0x8c]=sty6502; cpu[active_cpu].inst.adrmode[0x8c]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x8d]=4; cpu[active_cpu].inst.instruction[0x8d]=sta6502; cpu[active_cpu].inst.adrmode[0x8d]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x8e]=4; cpu[active_cpu].inst.instruction[0x8e]=stx6502; cpu[active_cpu].inst.adrmode[0x8e]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x8f]=2; cpu[active_cpu].inst.instruction[0x8f]=nop6502; cpu[active_cpu].inst.adrmode[0x8f]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x90]=2; cpu[active_cpu].inst.instruction[0x90]=bcc6502; cpu[active_cpu].inst.adrmode[0x90]=relative6502;
      	cpu[active_cpu].inst.opcode_ticks[0x91]=6; cpu[active_cpu].inst.instruction[0x91]=sta6502; cpu[active_cpu].inst.adrmode[0x91]=indy6502;
      	cpu[active_cpu].inst.opcode_ticks[0x92]=3; cpu[active_cpu].inst.instruction[0x92]=sta6502; cpu[active_cpu].inst.adrmode[0x92]=indzp6502;
      	cpu[active_cpu].inst.opcode_ticks[0x93]=2; cpu[active_cpu].inst.instruction[0x93]=nop6502; cpu[active_cpu].inst.adrmode[0x93]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x94]=4; cpu[active_cpu].inst.instruction[0x94]=sty6502; cpu[active_cpu].inst.adrmode[0x94]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x95]=4; cpu[active_cpu].inst.instruction[0x95]=sta6502; cpu[active_cpu].inst.adrmode[0x95]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x96]=4; cpu[active_cpu].inst.instruction[0x96]=stx6502; cpu[active_cpu].inst.adrmode[0x96]=zpy6502;
      	cpu[active_cpu].inst.opcode_ticks[0x97]=2; cpu[active_cpu].inst.instruction[0x97]=nop6502; cpu[active_cpu].inst.adrmode[0x97]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x98]=2; cpu[active_cpu].inst.instruction[0x98]=tya6502; cpu[active_cpu].inst.adrmode[0x98]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x99]=5; cpu[active_cpu].inst.instruction[0x99]=sta6502; cpu[active_cpu].inst.adrmode[0x99]=absy6502;
      	cpu[active_cpu].inst.opcode_ticks[0x9a]=2; cpu[active_cpu].inst.instruction[0x9a]=txs6502; cpu[active_cpu].inst.adrmode[0x9a]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x9b]=2; cpu[active_cpu].inst.instruction[0x9b]=nop6502; cpu[active_cpu].inst.adrmode[0x9b]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0x9c]=4; cpu[active_cpu].inst.instruction[0x9c]=stz6502; cpu[active_cpu].inst.adrmode[0x9c]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0x9d]=5; cpu[active_cpu].inst.instruction[0x9d]=sta6502; cpu[active_cpu].inst.adrmode[0x9d]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x9e]=5; cpu[active_cpu].inst.instruction[0x9e]=stz6502; cpu[active_cpu].inst.adrmode[0x9e]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0x9f]=2; cpu[active_cpu].inst.instruction[0x9f]=nop6502; cpu[active_cpu].inst.adrmode[0x9f]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xa0]=3; cpu[active_cpu].inst.instruction[0xa0]=ldy6502; cpu[active_cpu].inst.adrmode[0xa0]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0xa1]=6; cpu[active_cpu].inst.instruction[0xa1]=lda6502; cpu[active_cpu].inst.adrmode[0xa1]=indx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xa2]=3; cpu[active_cpu].inst.instruction[0xa2]=ldx6502; cpu[active_cpu].inst.adrmode[0xa2]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0xa3]=2; cpu[active_cpu].inst.instruction[0xa3]=nop6502; cpu[active_cpu].inst.adrmode[0xa3]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xa4]=3; cpu[active_cpu].inst.instruction[0xa4]=ldy6502; cpu[active_cpu].inst.adrmode[0xa4]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xa5]=3; cpu[active_cpu].inst.instruction[0xa5]=lda6502; cpu[active_cpu].inst.adrmode[0xa5]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xa6]=3; cpu[active_cpu].inst.instruction[0xa6]=ldx6502; cpu[active_cpu].inst.adrmode[0xa6]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xa7]=2; cpu[active_cpu].inst.instruction[0xa7]=nop6502; cpu[active_cpu].inst.adrmode[0xa7]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xa8]=2; cpu[active_cpu].inst.instruction[0xa8]=tay6502; cpu[active_cpu].inst.adrmode[0xa8]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xa9]=3; cpu[active_cpu].inst.instruction[0xa9]=lda6502; cpu[active_cpu].inst.adrmode[0xa9]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0xaa]=2; cpu[active_cpu].inst.instruction[0xaa]=tax6502; cpu[active_cpu].inst.adrmode[0xaa]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xab]=2; cpu[active_cpu].inst.instruction[0xab]=nop6502; cpu[active_cpu].inst.adrmode[0xab]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xac]=4; cpu[active_cpu].inst.instruction[0xac]=ldy6502; cpu[active_cpu].inst.adrmode[0xac]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0xad]=4; cpu[active_cpu].inst.instruction[0xad]=lda6502; cpu[active_cpu].inst.adrmode[0xad]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0xae]=4; cpu[active_cpu].inst.instruction[0xae]=ldx6502; cpu[active_cpu].inst.adrmode[0xae]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0xaf]=2; cpu[active_cpu].inst.instruction[0xaf]=nop6502; cpu[active_cpu].inst.adrmode[0xaf]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xb0]=2; cpu[active_cpu].inst.instruction[0xb0]=bcs6502; cpu[active_cpu].inst.adrmode[0xb0]=relative6502;
      	cpu[active_cpu].inst.opcode_ticks[0xb1]=5; cpu[active_cpu].inst.instruction[0xb1]=lda6502; cpu[active_cpu].inst.adrmode[0xb1]=indy6502;
      	cpu[active_cpu].inst.opcode_ticks[0xb2]=3; cpu[active_cpu].inst.instruction[0xb2]=lda6502; cpu[active_cpu].inst.adrmode[0xb2]=indzp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xb3]=2; cpu[active_cpu].inst.instruction[0xb3]=nop6502; cpu[active_cpu].inst.adrmode[0xb3]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xb4]=4; cpu[active_cpu].inst.instruction[0xb4]=ldy6502; cpu[active_cpu].inst.adrmode[0xb4]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xb5]=4; cpu[active_cpu].inst.instruction[0xb5]=lda6502; cpu[active_cpu].inst.adrmode[0xb5]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xb6]=4; cpu[active_cpu].inst.instruction[0xb6]=ldx6502; cpu[active_cpu].inst.adrmode[0xb6]=zpy6502;
      	cpu[active_cpu].inst.opcode_ticks[0xb7]=2; cpu[active_cpu].inst.instruction[0xb7]=nop6502; cpu[active_cpu].inst.adrmode[0xb7]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xb8]=2; cpu[active_cpu].inst.instruction[0xb8]=clv6502; cpu[active_cpu].inst.adrmode[0xb8]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xb9]=4; cpu[active_cpu].inst.instruction[0xb9]=lda6502; cpu[active_cpu].inst.adrmode[0xb9]=absy6502;
      	cpu[active_cpu].inst.opcode_ticks[0xba]=2; cpu[active_cpu].inst.instruction[0xba]=tsx6502; cpu[active_cpu].inst.adrmode[0xba]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xbb]=2; cpu[active_cpu].inst.instruction[0xbb]=nop6502; cpu[active_cpu].inst.adrmode[0xbb]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xbc]=4; cpu[active_cpu].inst.instruction[0xbc]=ldy6502; cpu[active_cpu].inst.adrmode[0xbc]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xbd]=4; cpu[active_cpu].inst.instruction[0xbd]=lda6502; cpu[active_cpu].inst.adrmode[0xbd]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xbe]=4; cpu[active_cpu].inst.instruction[0xbe]=ldx6502; cpu[active_cpu].inst.adrmode[0xbe]=absy6502;
      	cpu[active_cpu].inst.opcode_ticks[0xbf]=2; cpu[active_cpu].inst.instruction[0xbf]=nop6502; cpu[active_cpu].inst.adrmode[0xbf]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xc0]=3; cpu[active_cpu].inst.instruction[0xc0]=cpy6502; cpu[active_cpu].inst.adrmode[0xc0]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0xc1]=6; cpu[active_cpu].inst.instruction[0xc1]=cmp6502; cpu[active_cpu].inst.adrmode[0xc1]=indx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xc2]=2; cpu[active_cpu].inst.instruction[0xc2]=nop6502; cpu[active_cpu].inst.adrmode[0xc2]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xc3]=2; cpu[active_cpu].inst.instruction[0xc3]=nop6502; cpu[active_cpu].inst.adrmode[0xc3]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xc4]=3; cpu[active_cpu].inst.instruction[0xc4]=cpy6502; cpu[active_cpu].inst.adrmode[0xc4]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xc5]=3; cpu[active_cpu].inst.instruction[0xc5]=cmp6502; cpu[active_cpu].inst.adrmode[0xc5]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xc6]=5; cpu[active_cpu].inst.instruction[0xc6]=dec6502; cpu[active_cpu].inst.adrmode[0xc6]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xc7]=2; cpu[active_cpu].inst.instruction[0xc7]=nop6502; cpu[active_cpu].inst.adrmode[0xc7]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xc8]=2; cpu[active_cpu].inst.instruction[0xc8]=iny6502; cpu[active_cpu].inst.adrmode[0xc8]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xc9]=3; cpu[active_cpu].inst.instruction[0xc9]=cmp6502; cpu[active_cpu].inst.adrmode[0xc9]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0xca]=2; cpu[active_cpu].inst.instruction[0xca]=dex6502; cpu[active_cpu].inst.adrmode[0xca]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xcb]=2; cpu[active_cpu].inst.instruction[0xcb]=nop6502; cpu[active_cpu].inst.adrmode[0xcb]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xcc]=4; cpu[active_cpu].inst.instruction[0xcc]=cpy6502; cpu[active_cpu].inst.adrmode[0xcc]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0xcd]=4; cpu[active_cpu].inst.instruction[0xcd]=cmp6502; cpu[active_cpu].inst.adrmode[0xcd]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0xce]=6; cpu[active_cpu].inst.instruction[0xce]=dec6502; cpu[active_cpu].inst.adrmode[0xce]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0xcf]=2; cpu[active_cpu].inst.instruction[0xcf]=nop6502; cpu[active_cpu].inst.adrmode[0xcf]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xd0]=2; cpu[active_cpu].inst.instruction[0xd0]=bne6502; cpu[active_cpu].inst.adrmode[0xd0]=relative6502;
      	cpu[active_cpu].inst.opcode_ticks[0xd1]=5; cpu[active_cpu].inst.instruction[0xd1]=cmp6502; cpu[active_cpu].inst.adrmode[0xd1]=indy6502;
      	cpu[active_cpu].inst.opcode_ticks[0xd2]=3; cpu[active_cpu].inst.instruction[0xd2]=cmp6502; cpu[active_cpu].inst.adrmode[0xd2]=indzp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xd3]=2; cpu[active_cpu].inst.instruction[0xd3]=nop6502; cpu[active_cpu].inst.adrmode[0xd3]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xd4]=2; cpu[active_cpu].inst.instruction[0xd4]=nop6502; cpu[active_cpu].inst.adrmode[0xd4]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xd5]=4; cpu[active_cpu].inst.instruction[0xd5]=cmp6502; cpu[active_cpu].inst.adrmode[0xd5]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xd6]=6; cpu[active_cpu].inst.instruction[0xd6]=dec6502; cpu[active_cpu].inst.adrmode[0xd6]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xd7]=2; cpu[active_cpu].inst.instruction[0xd7]=nop6502; cpu[active_cpu].inst.adrmode[0xd7]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xd8]=2; cpu[active_cpu].inst.instruction[0xd8]=cld6502; cpu[active_cpu].inst.adrmode[0xd8]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xd9]=4; cpu[active_cpu].inst.instruction[0xd9]=cmp6502; cpu[active_cpu].inst.adrmode[0xd9]=absy6502;
      	cpu[active_cpu].inst.opcode_ticks[0xda]=3; cpu[active_cpu].inst.instruction[0xda]=phx6502; cpu[active_cpu].inst.adrmode[0xda]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xdb]=2; cpu[active_cpu].inst.instruction[0xdb]=nop6502; cpu[active_cpu].inst.adrmode[0xdb]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xdc]=2; cpu[active_cpu].inst.instruction[0xdc]=nop6502; cpu[active_cpu].inst.adrmode[0xdc]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xdd]=4; cpu[active_cpu].inst.instruction[0xdd]=cmp6502; cpu[active_cpu].inst.adrmode[0xdd]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xde]=7; cpu[active_cpu].inst.instruction[0xde]=dec6502; cpu[active_cpu].inst.adrmode[0xde]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xdf]=2; cpu[active_cpu].inst.instruction[0xdf]=nop6502; cpu[active_cpu].inst.adrmode[0xdf]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xe0]=3; cpu[active_cpu].inst.instruction[0xe0]=cpx6502; cpu[active_cpu].inst.adrmode[0xe0]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0xe1]=6; cpu[active_cpu].inst.instruction[0xe1]=sbc6502; cpu[active_cpu].inst.adrmode[0xe1]=indx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xe2]=2; cpu[active_cpu].inst.instruction[0xe2]=nop6502; cpu[active_cpu].inst.adrmode[0xe2]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xe3]=2; cpu[active_cpu].inst.instruction[0xe3]=nop6502; cpu[active_cpu].inst.adrmode[0xe3]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xe4]=3; cpu[active_cpu].inst.instruction[0xe4]=cpx6502; cpu[active_cpu].inst.adrmode[0xe4]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xe5]=3; cpu[active_cpu].inst.instruction[0xe5]=sbc6502; cpu[active_cpu].inst.adrmode[0xe5]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xe6]=5; cpu[active_cpu].inst.instruction[0xe6]=inc6502; cpu[active_cpu].inst.adrmode[0xe6]=zp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xe7]=2; cpu[active_cpu].inst.instruction[0xe7]=nop6502; cpu[active_cpu].inst.adrmode[0xe7]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xe8]=2; cpu[active_cpu].inst.instruction[0xe8]=inx6502; cpu[active_cpu].inst.adrmode[0xe8]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xe9]=3; cpu[active_cpu].inst.instruction[0xe9]=sbc6502; cpu[active_cpu].inst.adrmode[0xe9]=immediate6502;
      	cpu[active_cpu].inst.opcode_ticks[0xea]=2; cpu[active_cpu].inst.instruction[0xea]=nop6502; cpu[active_cpu].inst.adrmode[0xea]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xeb]=2; cpu[active_cpu].inst.instruction[0xeb]=nop6502; cpu[active_cpu].inst.adrmode[0xeb]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xec]=4; cpu[active_cpu].inst.instruction[0xec]=cpx6502; cpu[active_cpu].inst.adrmode[0xec]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0xed]=4; cpu[active_cpu].inst.instruction[0xed]=sbc6502; cpu[active_cpu].inst.adrmode[0xed]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0xee]=6; cpu[active_cpu].inst.instruction[0xee]=inc6502; cpu[active_cpu].inst.adrmode[0xee]=abs6502;
      	cpu[active_cpu].inst.opcode_ticks[0xef]=2; cpu[active_cpu].inst.instruction[0xef]=nop6502; cpu[active_cpu].inst.adrmode[0xef]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xf0]=2; cpu[active_cpu].inst.instruction[0xf0]=beq6502; cpu[active_cpu].inst.adrmode[0xf0]=relative6502;
      	cpu[active_cpu].inst.opcode_ticks[0xf1]=5; cpu[active_cpu].inst.instruction[0xf1]=sbc6502; cpu[active_cpu].inst.adrmode[0xf1]=indy6502;
      	cpu[active_cpu].inst.opcode_ticks[0xf2]=3; cpu[active_cpu].inst.instruction[0xf2]=sbc6502; cpu[active_cpu].inst.adrmode[0xf2]=indzp6502;
      	cpu[active_cpu].inst.opcode_ticks[0xf3]=2; cpu[active_cpu].inst.instruction[0xf3]=nop6502; cpu[active_cpu].inst.adrmode[0xf3]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xf4]=2; cpu[active_cpu].inst.instruction[0xf4]=nop6502; cpu[active_cpu].inst.adrmode[0xf4]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xf5]=4; cpu[active_cpu].inst.instruction[0xf5]=sbc6502; cpu[active_cpu].inst.adrmode[0xf5]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xf6]=6; cpu[active_cpu].inst.instruction[0xf6]=inc6502; cpu[active_cpu].inst.adrmode[0xf6]=zpx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xf7]=2; cpu[active_cpu].inst.instruction[0xf7]=nop6502; cpu[active_cpu].inst.adrmode[0xf7]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xf8]=2; cpu[active_cpu].inst.instruction[0xf8]=sed6502; cpu[active_cpu].inst.adrmode[0xf8]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xf9]=4; cpu[active_cpu].inst.instruction[0xf9]=sbc6502; cpu[active_cpu].inst.adrmode[0xf9]=absy6502;
      	cpu[active_cpu].inst.opcode_ticks[0xfa]=4; cpu[active_cpu].inst.instruction[0xfa]=plx6502; cpu[active_cpu].inst.adrmode[0xfa]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xfb]=2; cpu[active_cpu].inst.instruction[0xfb]=nop6502; cpu[active_cpu].inst.adrmode[0xfb]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xfc]=2; cpu[active_cpu].inst.instruction[0xfc]=nop6502; cpu[active_cpu].inst.adrmode[0xfc]=implied6502;
      	cpu[active_cpu].inst.opcode_ticks[0xfd]=4; cpu[active_cpu].inst.instruction[0xfd]=sbc6502; cpu[active_cpu].inst.adrmode[0xfd]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xfe]=7; cpu[active_cpu].inst.instruction[0xfe]=inc6502; cpu[active_cpu].inst.adrmode[0xfe]=absx6502;
      	cpu[active_cpu].inst.opcode_ticks[0xff]=2; cpu[active_cpu].inst.instruction[0xff]=nop6502; cpu[active_cpu].inst.adrmode[0xff]=implied6502;
	/* ^ default instruction set ^ */


	cpu[active_cpu].frequency_khz = 1000;

}



