#ifndef __DISASM_TABLES_H__
#define __DISASM_TABLES_H__

#define AM_IMP		1	// implicit 		1byte		CLC		; clear carry flag
#define AM_A		2	// on acc 		1byte		RLA		; rotate left accumulator
#define AM_IMM		3	// immediate 		2byte		LDA #$10	; load $10 into acc
#define AM_ZP		4	// zero page 		2byte		LDA $00		; load acc from $0000
#define AM_ZPX		5	// zero page,x 		2byte		STY $10,X
#define AM_ZPY		6	// zero page,y 		2byte		LDX ($10),Y
#define AM_REL		7	// relative 		2byte		BEQ label
#define AM_ABS		8	// absolute 		3byte		JMP $1234
#define AM_ABSX		9	// absoulte,x 		3byte		STA $3000,X
#define AM_ABSY		10	// absolute,y 		3byte		AND ($4000),Y
#define	AM_IND		11	// indirect 		3byte		JMP ($FFFC)
#define AM_INDEXED_IND	12	// indexed indirect 	3byte	 	LDA ($40,X)
#define AM_IND_INDEXED	13	// indirect indexed 	3byte		LDA ($40),Y

#define INSTR_REFS	32

extern void init_disasm();
extern int get_adrmode(uint8_t op);
extern char *descriptions[INSTR_REFS];
#endif
