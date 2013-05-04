#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "memory.h"
#include "log.h"


uint8_t ram[0x10000]; 

void mem_clear() {
	int i;
	for(i = 0; i < 0x10000; i++) {
		ram[i] = 0;
	}
	_logf("memory: cleared");
	return;
}


int32_t load_rom_image(char *filename, uint16_t offset) {
	uint32_t addr;
	uint8_t data;
	FILE *f = fopen(filename, "r");
	struct stat stbuf;

	stat(filename, &stbuf);

	_logf("load_rom_image(): file %s, size $%04x bytes", filename, (uint32_t)stbuf.st_size);

	if((stbuf.st_size + offset) > 0x10000) 
		_logf("load_rom_image(): warning! image exceeds memory, loading anyway, will be cut at 0xffff!");

	if(f == NULL) {
		_logf("load_rom_image(): WTF? can't open file...");
		exit(-2);
	}

	addr = offset;
	while(addr <= 0xffff) {
		data = getc(f);
		ram[addr] = data;
		addr++;	
	}

	fclose(f);
	_logf("load_rom_image(): rom image loaded");

	return 0;
}




