target system: 			sbc-default
target system version:		0.01
target system date:		2013/06/20
target system author:		nils stec
target system author email:	nils.stec@gmail.com

---------------------------------------------------


this target system emulates a SINGLE BOARD COMPUTER with 1 CPU, terminal I/O,
and a random rumber generator.

target accepts following options:   
	--target=sbc-default
	--sbc-rom0-size=_____  (hex, in bytes)
	--sbc-rom0-addr=_____
	...
	--sbc-rom7-size=_____
	--sbc-rom7-size=_____
	... (same again for ram)

	--sbc-load-rom=file,0xoffset (can be given as often as you like)

	--sbc-terminal-i=0xffff	<addr>
	--sbc-terminal-o=0xffff <addr>
	--sbc-terminal-ctrl=0xffff <addr> (only active if cli option given)
	--sbc-terminal-ctrl-data=0xffff <data>
	--sbc-terminal-net=only_accept_from:port
	--sbc-terminal-redirect=/dev/tty
	--sbc-terminal-file-o=file


