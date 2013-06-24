BIN=6502
CC=gcc
CFLAGS=-march=native -pipe
WARN=-Wall -pedantic -std=c99
OPT=-O0
DBG=-g

INC=-I./src/include/
LINK_SDL=
LINK_CURSES=-lncurses -lform -lpanel -lmenu -ltinfo

TARGET_ASM=xa
TARGET_ASM_OPTS=

all:	help

help:
	@echo "6502 - emulator and debugger - please read also README - (c) 2011"
	@echo "what do you want to build? you can give make all of these args"
	@echo "at once or only compile a part of this package."
	@echo
	@echo " # make {6502,examples,tests,vimcc,clean,mrproper,linecount}"
	@echo "	   6502		build the emulator"
	@echo "	   6502static	build a statically linked binary, output will have a \"-static\"-suffix"
	@echo "	   examples	build some machine code examples"
	@echo "	   tests	build some test codes, only used while development"
	@echo "	   vim_cc	build ctags database from all source files for vim syntax highlighting"
	@echo "     	   linecount	count lines of source code"
	@echo
	@echo "On ubuntu quantal i and a few other had some linking issues. The linker proves he could"
	@echo "not find the ncurses libs. A quick fix for that was written fast, to use it don't build"
	@echo "with \"make 6502\", use \"make 6502_dirty_quantal\"."
	@echo 
	@echo "This build-system lets you configure your build according to your system-/compiler-conf."
	@echo " These are the configuration variables:"
	@echo "	BIN	- name of the output binary file"
	@echo " 	CC	- your c compiler shell command (gcc devfault)"
	@echo "	CFLAGS  - compiler flags, default=\"$(CFLAGS)\""
	@echo "	DBG	- Debugging options, set to -g for compilation for GDB"
	@echo "	"	
	@echo " Example:"
	@echo "   \$$ make 6502 examples CC=arm-linux-gcc DBG=-g"
	@echo ""
	@echo "If you simply want to compile use \"make 6502\" and enjoy :)"
	@echo "" 
	
vim_cc:
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q src/include/ src/

tests:
	@echo ">>> building test programs..."

examples:	ex_starfield2d.o ex_random.o ex_breakout.o 

ex_starfield2d.o:	target-src/starfield2d.asm
	$(TARGET_ASM) target-src/starfield2d.asm $(TARGET_ASM_OPTS) -o bin/examples-bin/starfield2d.bin

ex_random.o:	target-src/random.asm
	$(TARGET_ASM) target-src/random.asm $(TARGET_ASM_OPTS) -o bin/examples-bin/random.bin

ex_amiga.o:	target-src/amiga.asm
	$(TARGET_ASM) target-src/amiga.asm $(TARGET_ASM_OPTS) -o bin/examples-bin/amiga.bin

ex_breakout.o:	target-src/breakout.asm
	$(TARGET_ASM) target-src/breakout.asm $(TARGET_ASM_OPTS) -o bin/examples-bin/breakout.bin

ex_byterun.o:	target-src/byterun.asm
	$(TARGET_ASM) target-src/byterun.asm $(TARGET_ASM_OPTS) -o bin/examples-bin/byterun.bin

6502:	vim_cc 6502.o tables.o main.o mmio.o random.o ncui.o ncio.o log.o ncui_new.o memory.o systems.o config.o glcd.o 7seg.o hexkey.o hd44780.o c16-keyboard.o c128-keyboard.o c64-keyboard.o keyboard.o pc-gameport.o ps2.o video.o sid.o beep.o 
	@echo "  [LINK] 6502"
	@$(CC) $(LINK_CURSES) $(LINK_SDL) -o bin/current/$(BIN) 6502.o memory.o tables.o main.o mmio.o random.o ncui.o ncio.o log.o ncui_new.o systems.o config.o glcd.o 7seg.o hexkey.o hd44780.o c16-keyboard.o c128-keyboard.o c64-keyboard.o keyboard.o pc-gameport.o ps2.o video.o sid.o beep.o
	cp bin/current/$(BIN) bin/$(BIN)

6502static:	vim_cc 6502.o tables.o main.o mmio.o random.o ncui.o ncio.o log.o ncui_new.o memory.o
	@echo "  [LINK] 6502-static"
	@$(CC) $(LINK_CURSES) $(LINK_SDL) -static -o bin/current/$(BIN)-static 6502.o memory.o tables.o main.o mmio.o random.o ncui.o ncio.o log.o ncui_new.o
	ln -s bin/current/$(BIN)-static bin/$(BIN)-static

6502_dirty_quantal:	vim_cc 6502.o tables.o main.o mmio.o random.o ncui.o ncio.o log.o ncui_new.o memory.o
	@#dirty hack for ubuntu quantal, even with symlinks to /usr/link we have linking problems...
	@$(LD) $(LINK) -o bin/current/$(BIN)-ubnhack 6502.o memory.o tables.o main.o mmio.o random.o ncui.o ncio.o log.o /usr/lib/x86_64-linux-gnu/libncurses.so /usr/lib/x86_64-linux-gnu/libpanel.so
	ln -s bin/current/$(BIN)-ubnhack bin/$(BIN)-ubnhack

6502_dirty_quantal-static:	vim_cc 6502.o tables.o main.o mmio.o random.o ncui.o ncio.o log.o ncui_new.o memory.o
	@$(LD) $(LINK) -static -o bin/current/$(BIN)-ubnhack-static 6502.o memory.o tables.o main.o mmio.o random.o ncui.o ncio.o log.o /usr/lib/x86_64-linux-gnu/libncurses.so /usr/lib/x86_64-linux-gnu/libpanel.so
	ln -s bin/current/$(BIN)-ubnhack-static bin/$(BIN)-ubnhack-static

systems.o:	src/systems/systems.c
	@echo "  [cc] systems/systems.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/systems/systems.c -o systems.o

config.o:	src/config.c
	@echo "  [cc] config.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/config.c -o config.o

glcd.o:		src/mmio/glcd.c
	@echo "  [cc] mmio/glcd.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/glcd.c -o glcd.o

7seg.o:		src/mmio/7seg.c
	@echo "  [cc] mmio/7seg.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/7seg.c -o 7seg.o

keyboard.o:	src/mmio/keyboard.c
	@echo "  [cc] mmio/keyboard.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/keyboard.c -o keyboard.o

pc-gameport.o:	src/mmio/pc-gameport.c
	@echo "  [cc] mmio/pc-gameport.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/pc-gameport.c -o pc-gameport.o

ps2.o:		src/mmio/ps2.c
	@echo "  [cc] mmio/ps2.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/ps2.c -o ps2.o

video.o:	src/mmio/video.c
	@echo "  [cc] mmio/video.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/video.c -o video.o

sid.o:		src/mmio/sid.c
	@echo "  [cc] mmio/sid.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/sid.c -o sid.o

beep.o:		src/mmio/beep.c
	@echo "  [cc] mmio/beep.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/beep.c -o beep.o

hexkey.o:	src/mmio/hexkey.c
	@echo "  [cc] mmio/hexkey.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/hexkey.c -o hexkey.o

hd44780.o:	src/mmio/hd44780.c
	@echo "  [cc] mmio/hd44780.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/hd44780.c -o hd44780.o

c16-keyboard.o:	src/mmio/c16-keyboard.c
	@echo "  [cc] mmio/c16-keyboard.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/c16-keyboard.c -o c16-keyboard.o

c128-keyboard.o:	src/mmio/c128-keyboard.c
	@echo "  [cc] mmio/c128-keyboard.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/c128-keyboard.c -o c128-keyboard.o

c64-keyboard.o:	src/mmio/c64-keyboard.c
	@echo "  [cc] mmio/c64-keyboard.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio/c64-keyboard.c -o c64-keyboard.o

memory.o:	src/memory.c
	@echo "  [cc] memory.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/memory.c -o memory.o

ncui_new.o:	src/ncui_new.c
	@echo "  [cc] ncui_new.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/ncui_new.c -o ncui_new.o

log.o:	src/log.c
	@echo "  [CC] log.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/log.c -o log.o

random.o: src/random.c
	@echo "  [CC] random.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/random.c -o random.o

main.o: src/main.c
	@echo "  [CC] main.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/main.c -o main.o

tables.o: src/tables.c
	@echo "  [CC] tables.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/tables.c -o tables.o

6502.o: src/6502.c
	@echo "  [CC] 6502.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/6502.c -o 6502.o

mmio.o: src/mmio.c
	@echo "  [CC] mmio.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/mmio.c -o mmio.o

ncui.o: src/ncui.c
	@echo "  [CC] ncui.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/ncui.c -o ncui.o

ncio.o: src/ncio.c
	@echo "  [CC] ncio.c"
	@$(CC) $(CFLAGS) $(WARN) $(ERR) $(OPT) $(DBG) $(INC) -c src/ncio.c -o ncio.o

linecount:
	cat Makefile README.md src/*.c src/include/*.h src/tests/*.c src/tests/Makefile | wc -l
	cat rom6502/src/*.asm | wc -l

clean:
	rm -rf .*~ *.o *~ tags bin/current/* bin/examples-bin/*.bin bin/test-progs/* bin/$(BIN) bin/$(BIN)-static bin/$(BIN)-ubnhack bin/$(BIN)-ubnhack-static

mrproper:
	rm -rf .*~ *~ src/.*~ src/*~ src/include/*~ src/include/.*~ src/tests/*~ log-6502.txt
