**Overview**  
	This is my simple 6502 debugger and emulator. It takes plain binary images  
	for machine code.  
  
	Some things it supports yet:  
	* multiple CPUs  
	* some memory mapped IO  
		* Random Number Generator  
		* sebseb7's USB-SID-Module (broken ATM)  
		* Random Number Generator (broken)  
		* Keyboard Input (broken)  
  
**Benchmark**  
If you try it, PLEASE do a bench and mail the result to <nils.stec@gmail.com>.  
Please include information about your system. CPU, RAM and Kernel should be  
enough. If you run it on a "special" system (not PC, Server, ...) please   
write a bit more about your system. I will add these results to the following  
list.  
  
**How to build the emu and rum code**  
	$ make  
  
	$ ./6502 --rom bin_img/image_test1.65b 0x0000 --dbg  
		This loads image_test1.65b to address 0 and starts the debugger  
		see "debugger commands"  
  
	$ ./6502 --rom bin_img/image_test1.65b 0x0000 --bench 100000000  
		This will run the image for the given cycles and measure the   
		time the emu used. After that the imaginary speed of the 6502  
		cpu will be calculated.  
  
**Benchmark results**  
  
	* On a Intel Atom 1,6Ghz, 1024MB RAM i get about 90mhz...110mhz  
	  with the cpu core i use right now.   
	* Pentium 90/128MB, 2.6 => 5Mhz  
	* Pentium 4 3.2Ghz, 3.0 => 67Mhz  
	* Core2Duo 2.2Ghz, 8GB, 3.7 => 205Mhz  
	* i7 @ 1.2Ghz, 8GB, 3.2 => 430Mhz  
	* Samsung Galaxy S2, Android => 24Mhz  
  
  
**CLI arguments**  
	TBD  
	TBD  
	TBD  
  
**How to use it**  
  
	Debugger Commands:  
	d - dump mem  
	r - reset  
	i - irq  
	N - nmi  
	X - quit  
	v - show vectors  
	n - do a step  
	s - cpu status  
	S - set memory  
	p - set pc  
   
**The mysterious windows-tool "6502"**  
	IS NOT MINE. This is a example, what i want to have at the end.  
  
**Notes**  
	For this (still very simple) EMU we could use 2 different "CPU-cores".  
  
	At the moment we use a modified version of a 6502 cpu emulator, originally   
	written by <unknown>. It was used in Neil Bradley's Asteroids EMU and Steve  
	Green's MAC 6502 emu (google that, dont ask me!).  

	The first emu I used was a whole rewrite of this emu, but now it isn't working  
	anymore. I got a speed-increase by about 80%, depending on the machine code   
	that is executed, but atm it lacks some opcodes and others will throw   
	segfaults. It's working for the simple demo images i've made, but not more.  
	It is on the way, i think about 10% of the errors are fixed by now.   
  
	Before i started rewriting the core it ran a C64 rom-set and the REALLY COOL   
	demos from m6502 fine. Unfortunately the homepage of m6502 is down right now,  
	so we can't look at :(  

**changelog**  
		16th feb 2012 - please see file CHANGELOG from now on, i won't  
				update this changelog anymore.  
		25th jan 2012 - added memory mapped IO (random)  
				fixed some bugs  
				added opcode, nmi and irq hooks  
				  fixed some speed issues  

**LINKS**  
	PLEASE SEE http://www.6502asm.com/ - THE **VERY BEST** 6502 EMULATOR AROUND
