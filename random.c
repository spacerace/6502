#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include "mmio.h"

static uint16_t rng_base;
static int rng_enabled;
static int rng_urand;

extern uint8_t ram[0x10000];

int rng8_init(uint16_t addr, int urand) {
        rng_enabled = 1;
        rng_base = addr;
        rng_urand = urand;

        switch(urand) {
                case 0:         // "normal random"
                        srand(time(NULL));
                        break;
                case 1:         // urandom
                        srand(time(NULL));
                        break;
                default:
                        rng_enabled = 0;
                        break;
        }

        return rng_enabled;
}

void rng8_deinit() {
        rng_enabled = 0;
        return;
}

void rng8_getrnd() {
        if(rng_enabled) {
                switch(rng_urand) {
                        case 0:         // "normal" random
                                ram[rng_base] = (uint8_t)(rand()&0xff);
                                break;
                        case 1:         // urandom
                                ram[rng_base] = (uint8_t)(rand()&0xff);
                                break;
                }
        }
        return;
}



