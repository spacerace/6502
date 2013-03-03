OUTFILE=6502
TARGET_OS=linux
CC=gcc
LD=gcc
CFLAGS=-march=native -pipe
# march should be set to your CPU
WARN=-Wall
ERR=-Werror
OPT=-O3
DBG=-g

INC=-I./src/include/

LINK=-lm
LINK+=-lpthread
LINK+=-lSDL
LINK+=-lSDL_gfx
LINK+=-lncurses
LINK+=-lform
LINK+=-lpanel
LINK+=-lmenu
LINK_PATH+=/usr/lib/x86_64-linux-gnu/

all:	prepare 6502 tests size

prepare:
	@echo ">>> building 6502 emulator, debugger, test cases and example programs..."

update_vim_code_completition:
     ctags -R --c++-kinds=+p --fields=+iaS --extra=+q src/include/ src/

tests:
	@echo ">>> building test programs..."
	@make -C src/tests/
	@cp src/tests/ncurses_color_test .
	@cp src/tests/ncurses_panel_test .
	@cp src/tests/ncurses_menu*_test .
	@cp src/tests/ncurses_form*_test .
	@cp src/tests/ncurses_mouse_test .

examples:
	@echo ">>> building 6502 examples..."

size:
	@size 6502 ncurses*test

6502:	6502.o tables.o main.o mmio.o random.o ncui.o ncio.o log.o
	@echo "  [LINK] 6502"
	#$(LD) -L$(LINK_PATH) $(LINK) -o $(OUTFILE) 6502.o tables.o main.o mmio.o random.o ncui.o ncio.o log.o
	# dirty hack for ubuntu quantal, even with symlinks to /usr/link we have linking problems...
	$(LD) $(LINK) -o $(OUTFILE) 6502.o tables.o main.o mmio.o random.o ncui.o ncio.o log.o /usr/lib/x86_64-linux-gnu/libncurses.so /usr/lib/x86_64-linux-gnu/libpanel.so
	@echo 

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

clean:
	rm -f log.o
	rm -f 6502.o
	rm -f main.o
	rm -f tables.o
	rm -f random.o
	rm -f mmio.o
	rm -f ncui.o
	rm -f ncio.o
	rm -f 6502
	rm -f ncurses_color_test
	rm -f ncurses_panel_test
	rm -f ncurses_menu*_test
	rm -f ncurses_form*_test
	rm -f ncurses_mouse_test
	make -C src/tests/ clean

mrproper:
	rm -f 6502.o
	rm -f log.o
	rm -f main.o
	rm -f tables.o
	rm -f random.o
	rm -f mmio.o
	rm -f ncui.o
	rm -f ncio.o
	rm -f 6502
	rm -f .*.c.swp
	rm -f .*.h.swp
	rm -f .Makefile.swp
	rm -f *.c~
	rm -f *.h~
	rm -f Makefile~
	rm -f src/.*.c.swp
	rm -f src/.*.h.swp
	rm -f src/*.c~
	rm -f src/*.h~
	make -C src/tests/ clean

linecount:
	 cat Makefile README.md src/*.c src/include/*.h src/tests/*.c src/tests/Makefile | wc -l

