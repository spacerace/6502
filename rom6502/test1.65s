	.ORG $0000
	JMP start
start:	LDA #$55
	INX
	INY
	JMP start

	.ORG $00a0
	DEX
	DEY
	rti
	
	.ORG $00B0	; IRQ HANDLER
	.DW $0000


	.ORG $FFFA ; NMI points to $0000, same as reset	
	.DW $0000 
	.ORG $FFFC ; RESET 
	.DW $0000
	.ORG $FFFE ; IRQ 
	.DW $00A0
	
	
