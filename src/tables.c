/* this tables are used by the disassembler */

#include <string.h>

#include "6502.h"
#include "tables.h"
#include "log.h"

#define adrmode		addressing_modes
#define implied6502	AM_IMP
//#define			AM_A
#define immediate6502	AM_IMM
#define	zp6502		AM_ZP
#define	zpx6502		AM_ZPX
#define	zpy6502		AM_ZPY
#define	relative6502	AM_REL
#define	abs6502		AM_ABS
#define	absx6502	AM_ABSX
#define	absy6502	AM_ABSY
#define	indirect6502	AM_IND
#define	indx6502	AM_INDEXED_IND
#define	indy6502	AM_IND_INDEXED

static int addressing_modes[256];

static void disasm_set_addressing_modes_6502_default();

void init_disasm() {
	disasm_set_addressing_modes_6502_default();
	return;
}

int get_adrmode(uint8_t op) {
	return adrmode[op];
}

static void disasm_set_addressing_modes_6502_default() {
	adrmode[0x00]=implied6502;
	adrmode[0x01]=indx6502;
	adrmode[0x02]=implied6502;
	adrmode[0x03]=implied6502;
	adrmode[0x04]=zp6502;
	adrmode[0x05]=zp6502;
	adrmode[0x06]=zp6502;
	adrmode[0x07]=implied6502;
	adrmode[0x08]=implied6502;
	adrmode[0x09]=immediate6502;
	adrmode[0x0a]=implied6502;
	adrmode[0x0b]=implied6502;
	adrmode[0x0c]=abs6502;
	adrmode[0x0d]=abs6502;
	adrmode[0x0e]=abs6502;
	adrmode[0x0f]=implied6502;
	adrmode[0x10]=relative6502;
	adrmode[0x11]=indy6502;
	//adrmode[0x12]=indzp6502;
	adrmode[0x13]=implied6502;
	adrmode[0x14]=zp6502;
	adrmode[0x15]=zpx6502;
	adrmode[0x16]=zpx6502;
	adrmode[0x17]=implied6502;
	adrmode[0x18]=implied6502;
	adrmode[0x19]=absy6502;
	adrmode[0x1a]=implied6502;
	adrmode[0x1b]=implied6502;
	adrmode[0x1c]=abs6502;
	adrmode[0x1d]=absx6502;
	adrmode[0x1e]=absx6502;
	adrmode[0x1f]=implied6502;
	adrmode[0x20]=abs6502;
	adrmode[0x21]=indx6502;
	adrmode[0x22]=implied6502;
	adrmode[0x23]=implied6502;
	adrmode[0x24]=zp6502;
	adrmode[0x25]=zp6502;
	adrmode[0x26]=zp6502;
	adrmode[0x27]=implied6502;
	adrmode[0x28]=implied6502;
	adrmode[0x29]=immediate6502;
	adrmode[0x2a]=implied6502;
	adrmode[0x2b]=implied6502;
	adrmode[0x2c]=abs6502;
	adrmode[0x2d]=abs6502;
	adrmode[0x2e]=abs6502;
	adrmode[0x2f]=implied6502;
	adrmode[0x30]=relative6502;
	adrmode[0x31]=indy6502;
	//adrmode[0x32]=indzp6502;
	adrmode[0x33]=implied6502;
	adrmode[0x34]=zpx6502;
	adrmode[0x35]=zpx6502;
	adrmode[0x36]=zpx6502;
	adrmode[0x37]=implied6502;
	adrmode[0x38]=implied6502;
	adrmode[0x39]=absy6502;
	adrmode[0x3a]=implied6502;
	adrmode[0x3b]=implied6502;
	adrmode[0x3c]=absx6502;
	adrmode[0x3d]=absx6502;
	adrmode[0x3e]=absx6502;
	adrmode[0x3f]=implied6502;
	adrmode[0x40]=implied6502;
	adrmode[0x41]=indx6502;
	adrmode[0x42]=implied6502;
	adrmode[0x43]=implied6502;
	adrmode[0x44]=implied6502;
	adrmode[0x45]=zp6502;
	adrmode[0x46]=zp6502;
	adrmode[0x47]=implied6502;
	adrmode[0x48]=implied6502;
	adrmode[0x49]=immediate6502;
	adrmode[0x4a]=implied6502;
	adrmode[0x4b]=implied6502;
	adrmode[0x4c]=abs6502;
	adrmode[0x4d]=abs6502;
	adrmode[0x4e]=abs6502;
	adrmode[0x4f]=implied6502;
	adrmode[0x50]=relative6502;
	adrmode[0x51]=indy6502;
	//adrmode[0x52]=indzp6502;
	adrmode[0x53]=implied6502;
	adrmode[0x54]=implied6502;
	adrmode[0x55]=zpx6502;
	adrmode[0x56]=zpx6502;
	adrmode[0x57]=implied6502;
	adrmode[0x58]=implied6502;
	adrmode[0x59]=absy6502;
	adrmode[0x5a]=implied6502;
	adrmode[0x5b]=implied6502;
	adrmode[0x5c]=implied6502;
	adrmode[0x5d]=absx6502;
	adrmode[0x5e]=absx6502;
	adrmode[0x5f]=implied6502;
	adrmode[0x60]=implied6502;
	adrmode[0x61]=indx6502;
	adrmode[0x62]=implied6502;
	adrmode[0x63]=implied6502;
	adrmode[0x64]=zp6502;
	adrmode[0x65]=zp6502;
	adrmode[0x66]=zp6502;
	adrmode[0x67]=implied6502;
	adrmode[0x68]=implied6502;
	adrmode[0x69]=immediate6502;
	adrmode[0x6a]=implied6502;
	adrmode[0x6b]=implied6502;
	adrmode[0x6c]=indirect6502;
	adrmode[0x6d]=abs6502;
	adrmode[0x6e]=abs6502;
	adrmode[0x6f]=implied6502;
	adrmode[0x70]=relative6502;
	adrmode[0x71]=indy6502;
	//adrmode[0x72]=indzp6502;
	adrmode[0x73]=implied6502;
	adrmode[0x74]=zpx6502;
	adrmode[0x75]=zpx6502;
	adrmode[0x76]=zpx6502;
	adrmode[0x77]=implied6502;
	adrmode[0x78]=implied6502;
	adrmode[0x79]=absy6502;
	adrmode[0x7a]=implied6502;
	adrmode[0x7b]=implied6502;
	//adrmode[0x7c]=indabsx6502;
	adrmode[0x7d]=absx6502;
	adrmode[0x7e]=absx6502;
	adrmode[0x7f]=implied6502;
	adrmode[0x80]=relative6502;
	adrmode[0x81]=indx6502;
	adrmode[0x82]=implied6502;
	adrmode[0x83]=implied6502;
	adrmode[0x84]=zp6502;
	adrmode[0x85]=zp6502;
	adrmode[0x86]=zp6502;
	adrmode[0x87]=implied6502;
	adrmode[0x88]=implied6502;
	adrmode[0x89]=immediate6502;
	adrmode[0x8a]=implied6502;
	adrmode[0x8b]=implied6502;
	adrmode[0x8c]=abs6502;
	adrmode[0x8d]=abs6502;
	adrmode[0x8e]=abs6502;
	adrmode[0x8f]=implied6502;
	adrmode[0x90]=relative6502;
	adrmode[0x91]=indy6502;
	//adrmode[0x92]=indzp6502;
	adrmode[0x93]=implied6502;
	adrmode[0x94]=zpx6502;
	adrmode[0x95]=zpx6502;
	adrmode[0x96]=zpy6502;
	adrmode[0x97]=implied6502;
	adrmode[0x98]=implied6502;
	adrmode[0x99]=absy6502;
	adrmode[0x9a]=implied6502;
	adrmode[0x9b]=implied6502;
	adrmode[0x9c]=abs6502;
	adrmode[0x9d]=absx6502;
	adrmode[0x9e]=absx6502;
	adrmode[0x9f]=implied6502;
	adrmode[0xa0]=immediate6502;
	adrmode[0xa1]=indx6502;
	adrmode[0xa2]=immediate6502;
	adrmode[0xa3]=implied6502;
	adrmode[0xa4]=zp6502;
	adrmode[0xa5]=zp6502;
	adrmode[0xa6]=zp6502;
	adrmode[0xa7]=implied6502;
	adrmode[0xa8]=implied6502;
	adrmode[0xa9]=immediate6502;
	adrmode[0xaa]=implied6502;
	adrmode[0xab]=implied6502;
	adrmode[0xac]=abs6502;
	adrmode[0xad]=abs6502;
	adrmode[0xae]=abs6502;
	adrmode[0xaf]=implied6502;
	adrmode[0xb0]=relative6502;
	adrmode[0xb1]=indy6502;
	//adrmode[0xb2]=indzp6502;
	adrmode[0xb3]=implied6502;
	adrmode[0xb4]=zpx6502;
	adrmode[0xb5]=zpx6502;
	adrmode[0xb6]=zpy6502;
	adrmode[0xb7]=implied6502;
	adrmode[0xb8]=implied6502;
	adrmode[0xb9]=absy6502;
	adrmode[0xba]=implied6502;
	adrmode[0xbb]=implied6502;
	adrmode[0xbc]=absx6502;
	adrmode[0xbd]=absx6502;
	adrmode[0xbe]=absy6502;
	adrmode[0xbf]=implied6502;
	adrmode[0xc0]=immediate6502;
	adrmode[0xc1]=indx6502;
	adrmode[0xc2]=implied6502;
	adrmode[0xc3]=implied6502;
	adrmode[0xc4]=zp6502;
	adrmode[0xc5]=zp6502;
	adrmode[0xc6]=zp6502;
	adrmode[0xc7]=implied6502;
	adrmode[0xc8]=implied6502;
	adrmode[0xc9]=immediate6502;
	adrmode[0xca]=implied6502;
	adrmode[0xcb]=implied6502;
	adrmode[0xcc]=abs6502;
	adrmode[0xcd]=abs6502;
	adrmode[0xce]=abs6502;
	adrmode[0xcf]=implied6502;
	adrmode[0xd0]=relative6502;
	adrmode[0xd1]=indy6502;
	//adrmode[0xd2]=indzp6502;
	adrmode[0xd3]=implied6502;
	adrmode[0xd4]=implied6502;
	adrmode[0xd5]=zpx6502;
	adrmode[0xd6]=zpx6502;
	adrmode[0xd7]=implied6502;
	adrmode[0xd8]=implied6502;
	adrmode[0xd9]=absy6502;
	adrmode[0xda]=implied6502;
	adrmode[0xdb]=implied6502;
	adrmode[0xdc]=implied6502;
	adrmode[0xdd]=absx6502;
	adrmode[0xde]=absx6502;
	adrmode[0xdf]=implied6502;
	adrmode[0xe0]=immediate6502;
	adrmode[0xe1]=indx6502;
	adrmode[0xe2]=implied6502;
	adrmode[0xe3]=implied6502;
	adrmode[0xe4]=zp6502;
	adrmode[0xe5]=zp6502;
	adrmode[0xe6]=zp6502;
	adrmode[0xe7]=implied6502;
	adrmode[0xe8]=implied6502;
	adrmode[0xe9]=immediate6502;
	adrmode[0xea]=implied6502;
	adrmode[0xeb]=implied6502;
	adrmode[0xec]=abs6502;
	adrmode[0xed]=abs6502;
	adrmode[0xee]=abs6502;
	adrmode[0xef]=implied6502;
	adrmode[0xf0]=relative6502;
	adrmode[0xf1]=indy6502;
	//adrmode[0xf2]=indzp6502;
	adrmode[0xf3]=implied6502;
	adrmode[0xf4]=implied6502;
	adrmode[0xf5]=zpx6502;
	adrmode[0xf6]=zpx6502;
	adrmode[0xf7]=implied6502;
	adrmode[0xf8]=implied6502;
	adrmode[0xf9]=absy6502;
	adrmode[0xfa]=implied6502;
	adrmode[0xfb]=implied6502;
	adrmode[0xfc]=implied6502;
	adrmode[0xfd]=absx6502;
	adrmode[0xfe]=absx6502;
	adrmode[0xff]=implied6502;

	int n;
	uint32_t size_mnemonics = 0;
	uint32_t size_adrmode = 0;
	uint32_t size_opcode_len = 0;
	uint32_t size_descriptions = 0;

	for(n = 0; n < 256; n++) {
		size_mnemonics += strlen(mnemonics[n]);
	}
	size_mnemonics += sizeof(mnemonics);

	for(n = 0; n < INSTR_REFS; n++) {
		size_descriptions += strlen(descriptions[n]);
	}
	size_descriptions += sizeof(descriptions);

	size_adrmode = 256*4;

	size_opcode_len = 256*4;

	_logf("disassembler: memory usage for all tables: %dbytes", size_adrmode+size_mnemonics+size_opcode_len+size_descriptions);
	_logf("disassembler:    addressing modes:        %dbytes", size_adrmode);
	_logf("disassembler:    mnemonics:               %dbytes", size_mnemonics);
	_logf("disassembler:    opcode length:           %dbytes", size_opcode_len);
	_logf("disassembler:    instruction reference:   %dbytes", size_descriptions);
}


char *mnemonics[256] = { 	"BRK","ORA", "", "", "", "ORA", "ASL", "", "PHP", "ORA", "ASL", "", "", "ORA", "ASL", "",
		     		"BPL", "ORA", "", "", "", "ORA", "ASL", "", "CLC", "ORA", "", "", "", "ORA", "ASL", "",
		     		"JSR", "AND", "", "", "BIT", "AND", "ROL", "", "PLP", "AND", "ROL", "", "BIT", "AND", "ROL", "",
		     		"BMI", "AND", "", "", "", "AND", "ROL", "", "SEC", "AND", "", "", "", "AND", "ROL", "RLA",
		     		"RTI", "EOR", "", "", "", "EOR", "LSR", "", "PHA", "EOR", "LSR", "", "JMP", "EOR", "LSR", "",
		     		"BVC", "EOR", "", "", "", "EOR", "LSR", "", "CLI", "EOR", "", "" , "", "EOR", "LSR", "",
		     		"RTS", "ADC", "", "", "", "ADC", "ROR", "", "PLA", "ADC", "ROR", "", "JMP", "ADC", "ROR", "",
		     		"BVS", "ADC", "", "", "", "ADC", "ROR", "", "SEI", "ADC", "", "", "", "ADC", "ROR", "",
		     		"", "STA", "", "", "STY", "STA", "STX", "", "DEY", "", "TXA", "", "STY", "STA", "STX", "",
		     		"BCC", "STA", "", "", "STY", "STA", "STX", "", "TYA", "STA", "TXS", "", "", "STA", "", "",
				"LDY", "LDA", "LDX", "", "LDY", "LDA", "LDX", "", "TAY", "LDA", "TAX", "", "LDY", "LDA", "LDX", "",
				"BCS", "LDA", "", "", "LDY", "LDA", "LDX", "", "CLV", "LDA", "TSX", "", "LDY", "LDA", "LDX", "",
				"CPY", "CMP", "", "", "CPY", "CMP", "DEC", "", "INY", "CMP", "DEX", "", "CPY", "CMP", "DEC", "",
				"BNE", "CMP", "", "", "", "CMP", "DEC", "", "CLD", "CMP", "", "", "", "CMP", "DEC", "",
				"CPX", "SBC", "", "", "CPX", "SBC", "INC", "", "INX", "SBC", "NOP", "", "CPX", "SBC", "INC", "",
				"BEQ", "SBC", "", "", "", "SBC", "INC", "", "SED", "SBC", "", "", "", "SBC", "INC", "" };

#define DESC_ORA	0
#define DESC_ADC	1
#define DESC_AND	2
#define DESC_ASL	3
#define DESC_BIT	4
#define DESC_BRANCH	5
#define DESC_BRK	6
#define DESC_CMP	7
#define DESC_CPX	8
#define DESC_CPY	9
#define DESC_DEC	10
#define DESC_EOR	11
#define DESC_FLAG	12
#define DESC_INC	13
#define DESC_JMP	14
#define DESC_JSR	15
#define DESC_LDA	16
#define DESC_LDX	17
#define DESC_LDY	18
#define DESC_LSR	19
#define DESC_NOP	20
#define DESC_ORA	21
#define DESC_REGISTER	22
#define DESC_ROL	23
#define DESC_ROR	24
#define DESC_RTI	25
#define DESC_RTS	26
#define DESC_SBC	27
#define DESC_STA	28
#define DESC_STACK	29
#define DESC_STX	30
#define DESC_STY	31

char *desc_lookup[32] = { "ORA:00,9,5,15,0d,1d,19,01,11",	// format: OPCODE:position_in_desc_table,op,co,de,s
			"ADC:01,69,65,75,6d,7d,79,61,71",
			"AND:02,29,25,35,2d,3d,39,21,31",
			"ASL:03,0A,06,16,0e,1e",
			"BIT:04,

char *descriptions[INSTR_REFS] = { 
			  "[ORA] - bitwise [OR] with [A]ccumulator\nflags [SZ]\n\nMODE|||SYNTAX|||HEX|LEN|TIMING\nImmediate||[ORA #$44]||$09|2|2\nZero Page||[ORA $44]|||$05|2|3\nZero Page,X||[ORA $44,X]||$15|2|4\nAbsolute||[ORA $4400]||$0D|3|4\nAbsolute,X||[ORA $4400,X]||$1D|3|4+\nAbsolute,Y||[ORA $4400,Y]||$19|3|4+\nIndirect,X||[ORA ($44,X)]||$01|2|6\nIndirect,Y||[ORA ($44),Y]||$11|2|5+\n\n+ add 1 cycle if page boundary crossed.\n",
			  "[ADC] - [AD]d with [C]arry\nflags [SVZC]\n\nMODE|||SYNTAX|||HEX|LEN|TIMING\nImmediate||[ADC #$44]||$69|2|2\nZero Page||[ADC $44]|||$65|2|3\nZero Page,X||[ADC $44,X]||$75|2|4\nAbsolute||[ADC $4400]||$6D|3|4\nAbsolute,X||[ADC $4400,X]||$7D|3|4+\nAbsolute,Y||[ADC $4400,Y]||$79|3|4+\nIndirect,X||[ADC ($44,X)]||$61|2|6\nIndirect,Y||[ADC ($44),Y]||$71|2|5+\n\n+ add 1 cycle if page boundary crossed.\nADC works in BCD and binary mode. carry-bit is ALWAYS\nshifted out.\n",
			  "[AND] - bitwise [AND] with accumulator\nflags [SZ]\n\nMODE|||SYNTAX|||HEX|LEN|TIMIMG\nImmediate||[AND #$44]||$29|2|2\nZero Page||[AND $44]|||$25|2|3\nZero Page,X||[AND $44,X]||$35|2|4\nAbsolute||[AND $4400]||$2D|3|4\nAbsolute,X||[AND $4400,X]||$3D|3|4+\nAbsolute,Y||[AND $4400,Y]||$39|3|4+\nIndirect,X||[AND ($44,X)]||$21|2|6\nIndirect,Y||[AND ($44),Y]||$31|2|5+\n\n+ add 1 cycle if page boundary crossed.\n",
			  "[ASL] - [A]rithmetic [S]hift [L]eft\nflags [SZC]\n\nMODE|||SYNTAX|||HEX|LEN|TIMING\nAccumulator||[ASL A]|||$0A|1|2\nZero Page||[ASL $44]|||$06|2|5\nZero Page,X||[ASL $44,X]||$16|2|6\nAbsolute||[ASL $4400]||$0E|3|6\nAbsolute,X||[ASL $4400,X]||$1E|3|7\n\n[ASL] shifts all 1 bit left, bit7 into carry.\n",
			  "[BIT] - test [BIT]s\nflags [SNVZ]\n\nMODE|||SYNTAX|||HEX|LEN|TIMING\nZero Page||[BIT $44]|||$24|2|3\nAbsolute||[BIT $4400]||$2C|3|4\n\nBIT sets Z flag after ANDing value with accumulator.\nS and V flags are set to match bits 7 and 6 of value\nat tested address.\n",
			  "[Branch Instructions]\nflags [none]\n\nMNEMONIC|||||||HEX\n[BPL] - branch on plus||||$10\n[BMI] - branch on minus||||$30\n[BVC] - branch on overflow clear||$50\n[BVS] - branch on overflow set||$70\n[BCC] - branch on carry clear|||$90\n[BCS] - branch on carry set|||$B0\n[BNE] - branch on not equal|||$D0\n[BEQ] - branch on equal||||$F0\n\nAll branches are [implied] mode and have a length of two\nbytes. Syntax is '[Bxx displacement/label]'.\n",
			  "[BRK] - [BR]ea[K]\nflags [B]\naddressing mode [implied]\n\n{BRK} causes a NMI and increments PC by one. Therefore a\n{RTI} will jump to the address of the {BRK} +2 so that {BRK}\nmay be used to replace instruction with 2b in size.\n\nThis is very useful for debug-routines.\n",
			  "[CMP] - [C]o[MP]are accumulator\nflags [SZC]\nMODE|||SYNTAX|||HEX|LEN|TIMING\nImmediate||[CMP #$44]||$C9|2|2\nZero Page||[CMP $44]|||$C5|2|3\nZero Page,X||[CMP $44,X]||$D5|2|4\nAbsolute||[CMP $4400]||$CD|3|4\nAbsolute,X||[CMP $4400,X]||$DD|3|4+\nAbsolute,Y||[CMP $4400,Y]||$D9|3|4+\nIndirect,X||[CMP ($44,X)]||$C1|2|6\nIndirect,Y||[CMP ($44),Y]||$D1|2|5+\n\n+ add 1 cycle if page boundary crossed.\n[CMP] sets flags if a substraction had been carried out.\nIf value in A is = or > value, carry is set. Equal (Z)\nand sign (S) flags will be set according to A.\n",
			  "[CPX] - [C]om[P]are [X] register\nflags [SZC]\n\nMODE|||SYNTAX|||HEX|LEN|TIMING\nImmediate||[CPX #$44]||$E0|2|2\nZero Page||[CPX $44]|||$E4|2|3\nAbsolute||[CPX $4400]||$EC|3|4\n\nOperation and flag results are identical to equivalent\nmode accumulator CMP ops.\n",
			  "[CPY] - [C]om[P]are [Y] register\nflags [SZC]\n\nMODE|||SYNTAX|||HEX|LEN|TIMING\nImmediate||[CPY #$44]||$C0|2|2\nZero Page||[CPY $44]|||$C4|2|3\nAbsolute||[CPY $4400]||$CC|3|4\n\nOperation and flag results are identical to equivalent\nmode accumulator CMP ops.\n",
			  "[DEC] - [DEC]rement memory\nflags [SZ]\n\nMODE|||SYNTAX|||HEX|LEN|TIMING\nZero Page||[DEC $44]|||$C6|2|5\nZero Page,X||[DEC $44,X]||$D6|2|6\nAbsolute||[DEC $4400]||$CE|3|6\nAbsolute,X||[DEC $4400,X]||$DE|3|7\n",
			  "[EOR] - bitwise [E]xclusive [OR]\nflags [SZ]\n\nMODE|||SYNTAX|||HEX|LEN|TIMING\nImmediate||[EOR #$44]||$49|2|2\nZero Page||[EOR $44]|||$45|2|3\nZero Page,X||[EOR $44,X]||$55|2|4\nAbsolute||[EOR $4400]||$4D|3|4\nAbsolute,X||[EOR $4400,X]||$5D|3|4+\nAbsolute,Y||[EOR $4400,Y]||$59|3|4+\nIndirect,X||[EOR ($44,X)]||$41|2|6\nIndirect,Y||[EOR ($44),Y]||$51|2|5+\n\n+ add 1 cycle if page boundary crossed.\n",
			  "[Flag (Processor Status)] Instructions\n\nAll are [implied], size is [2b] and require [2 cycles].\n\nMNEMONIC|||HEX||MNEMONIC|||HEX\n[CLC] (CLr Carry)||$18||[SEC] (SEt Carry)||$38\n[CLI] (CLear Int)||$58||[SED] (SEt Dec)||$F8\n[SEI] (SEt Int)||$78||[CLD] (CLr Dec)||$D8\n[CLV] (CLr oVflow)|$B8\n\nI-flag disables (SEI) or enables (CLI) IRQs.\nDEC-flag controls how the CPU substracts and adds.\nReset State of DEC is undefined, not changed by ints.\nOVF-flag, after ADC or SBC, will be set according\nto bit7 of result.\n",
"[INC] - [INC]rement memory\n\
flags [SZ]\n\n\
MODE|||SYNTAX|||HEX|LEN|TIMING\n\
Zero Page||[INC $44]|||$E6|2|5\n\
Zero Page,X||[INC $44,X]||$F6|2|6\n\
Absolute||[INC $4400]||$EE|3|6|\n\
Absolute,X||[INC $4400,X]||$FE|3|7\n",
 
"[JMP] ([J]u[MP])\n\
flags [none]\n\n\
MODE|||SYNTAX|||HEX|LEN|TIMING\n\
Absolute||[JMP $5597]||$4C|3|3\n\
Indirect||[JMP ($5597)]||$6C|3|5\n\n\
JMP transfers program execution to the following\n\
address (absolute) or to the location contained in the\n\
following address (indirect). Note that there is no\n\
carry associated with the indirect jump so: [AN IND JMP\n\
MUST NEVER USE A VECTOR BEGINNING ON THE LAST BYTE OF\n\
A PAGE!]\n",

"[JSR] - [J]ump to [S]ub[R]outine\n\
flags [none]\n\n\
MODE|||SYNTAX||HEX|LEN|TIM\n\
Absolute||[JSR $5597]|$20|3|6\n\n\
JSR pushes the address-1 of the next operation on the\n\
stack before setting PC to subroutine. Subroutines are\n\
normally terminated by a {RTS} op code.\n",

"[LDA] - [L]oa[D] [A]ccumulator\n\
flags [SZ]\n\n\
MODE          SYNTAX        HEX LEN TIM\n\
Immediate     [LDA #$44]      $A9  2   2\n\
Zero Page     [LDA $44]       $A5  2   3\n\
Zero Page,X   [LDA $44,X]     $B5  2   4\n\
Absolute      [LDA $4400]     $AD  3   4\n\
Absolute,X    [LDA $4400,X]   $BD  3   4+\n\
Absolute,Y    [LDA $4400,Y]   $B9  3   4+\n\
Indirect,X    [LDA ($44,X)]   $A1  2   6\n\
Indirect,Y    [LDA ($44),Y]   $B1  2   5+\n\n\
+ add 1 cycle if page boundary crossed.",

"[LDX] - [L]oa[D] [X] register\n\
flags [SZ]\n\n\
MODE           SYNTAX       HEX LEN TIM\n\
Immediate     [LDX #$44]      $A2  2   2\n\
Zero Page     [LDX $44]       $A6  2   3\n\
Zero Page,Y   [LDX $44,Y]     $B6  2   4\n\
Absolute      [LDX $4400]     $AE  3   4\n\
Absolute,Y    [LDX $4400,Y]   $BE  3   4+\n\n\
+ add 1 cycle if page boundary crossed.\n",

"[LDY] - [L]oa[D] [Y] register\n\
flags [SZ]\n\n\
MODE           SYNTAX       HEX LEN TIM\n\
Immediate     [LDY #$44]      $A0  2   2\n\
Zero Page     [LDY $44]       $A4  2   3\n\
Zero Page,X   [LDY $44,X]     $B4  2   4\n\
Absolute      [LDY $4400]     $AC  3   4\n\
Absolute,X    [LDY $4400,X]   $BC  3   4+\n\n\
+ add 1 cycle if page boundary crossed.",
 
"[LSR] - [L]ogical [S]hift [R]ight\n\
flags [SZC]\n\n\
MODE           SYNTAX       HEX LEN TIM\n\
Accumulator   [LSR A]         $4A  1   2\n\
Zero Page     [LSR $44]       $46  2   5\n\
Zero Page,X   [LSR $44,X]     $56  2   6\n\
Absolute      [LSR $4400]     $4E  3   6\n\
Absolute,X    [LSR $4400,X]   $5E  3   7\n\n\
LSR shifts all bits right one position.\n\
0 is shifted into bit 7 and the original\n\
bit 0 is shifted into the Carry.\n",

"[NOP] - [N]o [OP]eration\n\
flags [none]\n\n\
MODE           SYNTAX       HEX LEN TIM\n\
Implied       [NOP]           $EA  1   2\n\n\
NOP is used to reserve space for future\n\
modifications or effectively REM out existing code.\n",

"ORA bitwise OR with Accumulator\n\
flags [none]\n\n\
MODE           SYNTAX       HEX LEN TIM\n\
Immediate     [ORA #$44]      $09  2   2\n\
Zero Page     [ORA $44]       $05  2   3\n\
Zero Page,X   [ORA $44,X]     $15  2   4\n\
Absolute      [ORA $4400]     $0D  3   4\n\
Absolute,X    [ORA $4400,X]   $1D  3   4+\n\
Absolute,Y    [ORA $4400,Y]   $19  3   4+\n\
Indirect,X    [ORA ($44,X)]   $01  2   6\n\
Indirect,Y    [ORA ($44),Y]   $11  2   5+\n\n\
+ add 1 cycle if page boundary crossed.\n",
	               
"Register Instructions\n\
flags [SZ]\n\n\
These instructions are implied mode, have a length\n\
of one byte and require two machine cycles.\n\
MNEMONIC                 HEX\n\
[TAX] (Transfer A to X)    $AA\n\
[TXA] (Transfer X to A)    $8A\n\
[DEX] (DEcrement X)        $CA\n\
[INX] (INcrement X)        $E8\n\
[TAY] (Transfer A to Y)    $A8\n\
[TYA] (Transfer Y to A)    $98\n\
[DEY] (DEcrement Y)        $88\n\
[INY] (INcrement Y)        $C8\n",

"ROL ROtate Left\n\
flags [SZC]\n\n\
MODE           SYNTAX       HEX LEN TIM\n\
Accumulator   ROL A         $2A  1   2\n\
Zero Page     ROL $44       $26  2   5\n\
Zero Page,X   ROL $44,X     $36  2   6\n\
Absolute      ROL $4400     $2E  3   6\n\
Absolute,X    ROL $4400,X   $3E  3   7\n\n\
ROL shifts all bits left one position. The Carry\n\
is shifted into bit 0 and the original bit 7 is\n\
shifted into the Carry.\n",

"ROR ROtate Right\n\
Affects Flags: S Z C\n\n\
MODE           SYNTAX       HEX LEN TIM\n\
Accumulator   ROR A         $6A  1   2\n\
Zero Page     ROR $44       $66  2   5\n\
Zero Page,X   ROR $44,X     $76  2   6\n\
Absolute      ROR $4400     $6E  3   6\n\
Absolute,X    ROR $4400,X   $7E  3   7\n\
ROR shifts all bits right one position. The Carry\n\
is shifted into bit 7 and the original bit 0 is\n\
shifted into the Carry.\n",
	 

"RTI ReTurn from Interrupt\n\
Affects Flags: all\n\n\
MODE           SYNTAX       HEX LEN TIM\n\
Implied       RTI           $40  1   6\n\n\
RTI retrieves the Processor Status Word (flags)\n\
and the Program Counter from the stack in that order\n\
(interrupts push the PC first and then the PSW).\n\
Note that unlike RTS, the return address on the stack\n\
is the actual address rather than the address-1.\n",

"RTS ReTurn from Subroutine\n\
Affects Flags: none\n\n\
MODE           SYNTAX       HEX LEN TIM\n\
Implied       RTS           $60  1   6\n\n\
RTS pulls the top two bytes off the stack\n\
(low byte first) and transfers program control to\n\
that address+1. It is used, as expected, to exit a\n\
subroutine invoked via JSR which pushed the\n\
address-1.\n",

"[SBC] - [S]u[B]tract with [C]arry\n\
flags [SVZC]\n\n\
MODE          SYNTAX        HEX LEN TIM\n\
Immediate     [SBC #$44]      $E9  2   2\n\
Zero Page     [SBC $44]       $E5  2   3\n\
Zero Page,X   [SBC $44,X]     $F5  2   4\n\
Absolute      [SBC $4400]     $ED  3   4\n\
Absolute,X    [SBC $4400,X]   $FD  3   4+\n\
Absolute,Y    [SBC $4400,Y]   $F9  3   4+\n\
Indirect,X    [SBC ($44,X)]   $E1  2   6\n\
Indirect,Y    [SBC ($44),Y]   $F1  2   5+\n\n\
+ add 1 cycle if page boundary crossed.\n\
SBC results are dependant on the setting of the\n\
decimal flag. In decimal mode, subtraction is\n\
carried out on the assumption that the values\n\
involved are packed BCD (Binary Coded Decimal).\n\
There is no way to subtract without the carry which\n\
works as an inverse borrow. i.e, to subtract you\n\
set the carry before the operation. If the carry is\n\
cleared by the operation, it indicates a borrow occurred.\n",

"STA STore Accumulator\n\
Affects Flags: none\n\
MODE           SYNTAX       HEX LEN TIM\n\
Zero Page     STA $44       $85  2   3\n\
Zero Page,X   STA $44,X     $95  2   4\n\
Absolute      STA $4400     $8D  3   4\n\
Absolute,X    STA $4400,X   $9D  3   5\n\
Absolute,Y    STA $4400,Y   $99  3   5\n\
Indirect,X    STA ($44,X)   $81  2   6\n\
Indirect,Y    STA ($44),Y   $91  2   6\n",
	                         

"Stack Instructions\n\n\
instructions are implied mode, have a length of one\n\
byte and require machine cycles as indicated.\n\
The PuLl operations are known as POP on most\n\
other microprocessors. With the 6502, the stack is\n\
always on page one ($100-$1FF) and works top down.\n\
MNEMONIC                        HEX TIM\n\
TXS (Transfer X to Stack ptr)   $9A  2\n\
TSX (Transfer Stack ptr to X)   $BA  2\n\
PHA (PusH Accumulator)          $48  3\n\
PLA (PuLl Accumulator)          $68  4\n\
PHP (PusH Processor status)     $08  3\n\
PLP (PuLl Processor status)     $28  4\n",
	             

"STX STore X register\n\
Affects Flags: none\n\n\
MODE           SYNTAX       HEX LEN TIM\n\
Zero Page     STX $44       $86  2   3\n\
Zero Page,Y   STX $44,Y     $96  2   4\n\
Absolute      STX $4400     $8E  3   4\n",
	             

"STY STore Y register\n\
Affects Flags: none\n\n\
MODE           SYNTAX       HEX LEN TIM\n\
Zero Page     STY $44       $84  2   3\n\
Zero Page,X   STY $44,X     $94  2   4\n\
Absolute      STY $4400     $8C  3   4\n"

};		

int opcode_len[256] = {
	1, 	// 0x00 BRK
	2, 	// 0x01 PRA IND,X
	0,	// 0x02 
	0,	// 0x03
	0,	// 0x04
	2,	// 0x05 ORA ZP
	2,	// 0x06 ASL ZP
	0,	// 0x07
	1,	// 0x08 PHP
	2,	// 0x09 ORA IMM
	1,	// 0x0A ASL A
	0,	// 0x0B
	0,	// 0x0C
	3,	// 0x0D ORA ABS
	3,	// 0x0E ASL ABS
	0,	// 0x0F
	1,	// 0x10 BPL
	2,	// 0x11 ORA IND,Y
	0,	// 0x12
	0,	// 0x13
	0,	// 0x14
	2,	// 0x15 ORA ZP,X
	2,	// 0x16 ASL ZP,X
	0,	// 0x17
	1,	// 0x18 CLC
	3,	// 0x19 ORA ABS,Y
	0,
	0,
	0,
	3,	// 0x1d ora abs,x
	3,	// 0x1e asl abs,x
	0,
	3,	// 0x20 JSR ABS
	2,	// 0x21 AND IND,X
	0,	// 0x22
	0,	// 0x23
	2,	// 0x24 BIT ZP
	2,	// 0x25 AND ZP
	2,	// 0x26 ROL ZP
	0,	// 0x27
	1,	// 0x28 PLP
	2,	// 0x29 AND IMM
	1,	// 0x2a rol a
	0,	// 0x2b
	3,	// 0x2c bit abs
	3,	// 0x2d and abs
	3,	// 0x2e rol abs
	0,
	1,	// 0x30 BMI
	2,	// 0x31 AND IND,Y
	0,	// 0x32
	0,	// 0x33
	0,	// x034
	2,	// 0x35 AND ZP,X
	2,	// 0x36 ROL ZP,X
	0,	// 0x37
	1,	// 0x38 SEC
	3,	// 0x39 AND ABS,Y
	0,	// 0x3A
	0,	// 0x3B
	0,	// 0x3C
	3,	// 0x3D AND ABS,X
	3, 	// 0x3E ROL ABS,X
	0,	// 0x3F
	1,	// 0x40 RTI
	2,	// 0x41 EOR IND,X
	0,	// 0x42
	0,	// 0x43
	0,	// 0x44
	2,	// 0x45 EOR ZP
	2,	// 0x46 LSR ZP
	0,	// 0x47
	1,	// 0x48 PHA
	2,	// 0x49 EOR IMM
	1,	// 0x4A LSR A
	0,	// 0x4B
	3,	// 0x4C JMP ABS
	3,	// 0x4D EOR ABS
	3,	// 0x4E LSR ABS
	0,	// 0x4F
	1,	// 0x50 BVC
	2,	// 0x51 EOR IND,Y
	0,	// 0x52
	0,	// 0x53
	0,	// 0x54
	2,	// 0x55 EOR ZP,X
	2,	// 0x56 LSR ZP,X
	0,	// 0x57
	1,	// 0x58 CLI
	3,	// 0x59 EOR ABS,Y
	0,	// 0x5A
	0,	// 0x5B
	0,	// 0x5C
	3,	// 0x5D EOR ABS,X
	3,	// 0x5E LSR ABS,X
	0,	// 0x5F
	1,	// 0x60 RTS
	2,	// 0x61 ADC IND,X
	0,	// 0x62
	0,	// 0x63
	0,	// 0x64
	2,	// 0x65 ADC ZP
	2,	// 0x66 ROR ZP
	0,	// 0x67
	1,	// 0x68 PLA
	2,	// 0x69 ADC IMM
	1,	// 0x6A ROR A
	0,	// 0x6B
	3,	// 0x6C JMP (IND)
	3,	// 0x6D JMP ABS
	3,	// 0x6E ROR ABS
	0,	// 0x6F
	1,	// 0x70 BVS
	2,	// 0x71 ADC IND,Y
	0,	// 0x72
	0,	// 0x73
	0,	// 0x74
	2,	// 0x75
	2,	// 0x76 ROR ZP,X
	0,	// 0x77
	1,	// 0x78 SEI
	3,	// 0x79 ADC ABS,Y
	0,	// 0x7A
	0,	// 0x7B
	0,	// 0x7C
	3,	// 0x7D ADC ABS,X
	3, 	// 0x7E ROR ABS,X
	0,	// 0x7F
	0,	// 0x80
	2,	// 0x81	STA IND,X
	0,	// 0x82
	0,	// 0x83
	2,	// 0x84 STY ZP
	2,	// 0x85 STA ZP
	2,	// 0x86 STX ZP
	0,	// 0x87
	1,	// 0x88 DEY
	0,	// 0x89
	1,	// 0x8A TXA
	0,	// 0x8B
	3,	// 0x8C STY ABS
	3,	// 0x8D STA ABS
	3,	// 0x8E STX ABS
	0,	// 0x8f
	1,	// 0x90 bcc
	2,	// 0x91 sta ind,y
	0,
	0,	
	2,	// 0x94 sty zp,x
	2,	// 0x95 sta zp,x
	2,	// 0x96 stx zp,y
	0,
	1,	// 0x98 tya
	3,	// 0x99 sta abs,y
	1,	// 0x9a txs
	0,
	0,
	3,	// 9d sta abs,x
	0,
	0,
	2,	// a0 ldy imm
	2,	// a1 lda ind,x
	2,	// a2 ldx imm
	0,
	2,	// a4 ldy zp
	2,	// a5 lda zp
	2,	// a6 ldx zp
	0,
	1,	// a8 tay
	2,	// a9 lda imm
	1,	// aa tax
	3,	// ab rol abs
	1,	// ac ror a
	3,	// ad lda abs
	3,	// ae ldx abs
	0,
	1,	// b0 bcs
	2,	// b1 lda ind,y
	0,
	0,
	2,	// b4 ldy zp,x
	2,	// b5 lda zp,x
	2,	// b6 ldx zp,y
	0,
	1,	// b8 clv
	3,	// b9 abs,y
	2,	// ba txs
	0,
	3,	// bc ldy abs,x
	3,	// bd lda abs,x
	3,	// be ldx 
	0,
	2,	// c0 cpy imm
	2,	// c1 cmp ind,x
	0,
	0,
	2,	// c4 cpy zp
	2,	// c5 cmp zp
	2,	// c6 dec zp
	0,
	1,	// c8 iny
	2,	// c9 imm
	1,	// ca dex
	0,
	3,	// cc cpy abs
	3,	// cd cmp abs
	3,	// ce dec abs
	0,
	1,	// d0 bne
	2,	// d1 cmp ind,y
	0,
	0,
	0,
	2,	// d5 cmp zp,x
	2,	// d6 dec zp,x
	0,
	1,	// d8 cld
	3,	// d9 cmp abs,x
	0,	
	0,
	0,
	3,	// dd cmp abs,x
	3,	// de dec abs,x
	0,
	2,	// e0 cpx imm
	2,	// e1 sbc ind,x
	0,
	0,
	2,	// e4 cpx zp
	2,	// e5 sbc zp
	2,	// e6 inc zp
	0,
	1,	// e8 inx
	2,	// e9 sbc imm
	1,	// ea nop
	0,
	3,	// ec cpx abs
	3,	// ed sbc abs
	3,	// ee inc abs
	0,
	1,	// f0 beq
	2,	// f1 sbc ind,y
	0,
	0,
	0,
	2,	// f5 sbc zp,x
	2,	// f6 inc zp,x
	0,
	1,	// f8 sed
	3,	// f9 sbc abs,y
	0,
	0,
	0,
	3,	// fd sbc abs,x
	3,	// fe inc abs,x
	0
};
