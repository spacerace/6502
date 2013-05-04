#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <inttypes.h>

extern uint8_t ram[0x10000];
extern void mem_clear();
int32_t load_rom_image(char *filename, uint16_t offset);


#endif // __MEMORY_H__
