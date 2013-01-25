#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "6502.h"

#ifdef MMIO_USE_THREADS
static pthread_t mmio_thread;
static void *mmio_thread_func(void *ptr);
#endif

#define RNDTAB_SIZE 1024

static int mmio_run;
static void mmio_random();

static uint8_t rnd_tab[RNDTAB_SIZE];
static int rnd_count;

int init_mmio() {
	int i;

	mmio_run = 1;
	srand(time(NULL));

	for(i = 0; i < RNDTAB_SIZE; i++) {
		rnd_tab[i] = (uint8_t)rand();
	}
	rnd_count = 0;

#ifdef MMIO_USE_THREADS
	pthread_create(&mmio_thread, NULL, mmio_thread_func, NULL);
#endif
	return 0;
}

int stop_mmio() {
	mmio_run = 0;
#ifdef MMIO_USE_THREADS
	printf("waiting for unfinished jobs...\n");
	pthread_join(mmio_thread, NULL);
#endif
	return 0;
}

#ifdef MMIO_USE_OPCODEHOOK
__inline__ void mmio_opcodehook() {
	if(mmio_run) {
		//printf("mmio: opcode hook\n");
		mmio_random();
		;
	}
	return;
}

__inline__ void mmio_irqhook() {
	if(mmio_run) {
		//printf("mmio: irq hook\n");
		;
	}
	return;
}

__inline__ void mmio_nmihook() {
	if(mmio_run) {
		//printf("mmio: nmi hook\n");
		;
	}
	return;
}
#endif

static void mmio_random() {
	if(ram[MMIO_RND_ENABLE] & 0x01) ram[MMIO_RND_BASE+1] = rnd_tab[rnd_count];
	if(ram[MMIO_RND_ENABLE] & 0x02) A = rnd_tab[rnd_count++];	
	
	rnd_count++;
	
	if(rnd_count >= RNDTAB_SIZE) {
		rnd_count = (rand()%RNDTAB_SIZE)-1;
		ram[MMIO_RND_BASE+1] = (uint8_t)rand();
	}
	
	return;
}

#ifdef MMIO_USE_THREADS
static void *mmio_thread_func(void *ptr) {
	while(mmio_run) {
		usleep(500000);
	}
	return NULL;
}
#endif
