/* this tables are used by the disassembler */

#include "6502.h"
#include "tables.h"

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
