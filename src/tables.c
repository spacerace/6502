/* this tables are used by the disassembler */

#include "6502.h"

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
