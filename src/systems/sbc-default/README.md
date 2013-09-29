target system: 			sbc-default
target system version:		0.04
target system date:		2013/06/20,2013/09/29
target system author:		nils stec
target system author email:	nils.stec@gmail.com

---------------------------------------------------

this target system emulates a SINGLE BOARD COMPUTER with 1 CPU, terminal I/O,
and a random rumber generator.

target accepts following options:   
	--target=sbc-default
	
	--sbc-load-rom=file,0xOFF,0xLEN (can be given as often as you like)

	--sbc-terminal-base=0xADDR
	--sbc-terminal-net=LISTEN:PORT

	--sbc-7seg-enable
	--sbc-7seg-base=0xADDR
	--sbc-7seg-len=<1-16>

	--sbc-keypad-enable
	--sbc-keypad_base=0xADDR
	
	--sbc-port-io=PC_IOADDR,MAPPING

	--target-rng-base=0xADDR
	--target-rng-speed=<1-100>






