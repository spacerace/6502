#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include "6502.h"

#ifdef MMIO_USE_THREADS
static pthread_t mmio_thread;
static void *mmio_thread_func(void *ptr);
#ifdef MMIO_TIMER
static uint8_t treg0;
static uint8_t treg1;
static uint8_t treg2;
static uint8_t treg3;
#endif
#endif
#ifdef MMIO_RANDOM
#define RNDTAB_SIZE 1024
#endif
static int mmio_run;
#ifdef MMIO_RANDOM
static void mmio_random();
static uint8_t rnd_tab[RNDTAB_SIZE];
static int rnd_count;
#endif
#ifdef MMIO_SID
static int ser_port;
#endif
int init_mmio() {
#ifdef MMIO_USE_THREADS
#ifdef MMIO_TIMER
	treg0 = 255;
	treg1 = 255;
	treg2 = 255;
	treg3 = 255;

	ram[MMIO_TIMER_BASE+1] = treg0;
	ram[MMIO_TIMER_BASE+2] = treg1;
	ram[MMIO_TIMER_BASE+3] = treg2;
	ram[MMIO_TIMER_BASE+4] = treg3;
#endif

#ifdef MMIO_SID
	ser_port = open("/dev/ttyUSB0", O_RDWR);
	if(ser_port == -1) {
		printf("no serial port!\n");
	}

	struct termios ser_config;
	tcgetattr(ser_port, &ser_config);

	cfsetispeed(&ser_config, B115200);
	cfsetospeed(&ser_config, B115200);

	tcsetattr(ser_port, TCSANOW, &ser_config);
#endif // MMIO_SID
#endif

	mmio_run = 1;
#ifdef MMIO_RANDOM
	int i;
	srand(time(NULL));

	for(i = 0; i < RNDTAB_SIZE; i++) {
		rnd_tab[i] = (uint8_t)rand();
	}
	rnd_count = 0;
#endif

#ifdef MMIO_USE_THREADS
	pthread_create(&mmio_thread, NULL, mmio_thread_func, NULL);
#endif
	return 0;
}

int stop_mmio() {
	mmio_run = 0;
#ifdef MMIO_USE_THREADS
#ifdef MMIO_SID
	close(ser_port);
#endif // MMIO_SID
	printf("waiting for unfinished jobs...\n");
	pthread_join(mmio_thread, NULL);
#endif // MMIO_USE_PTHREADS
	return 0;
}

#ifdef MMIO_USE_OPCODEHOOK
__inline__ void mmio_opcodehook() {
	if(mmio_run) {
		//printf("mmio: opcode hook\n");
#ifdef MMIO_RANDOM
		mmio_random();
#endif
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

#ifdef MMIO_RANDOM
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
#endif

#ifdef MMIO_USE_THREADS
static void *mmio_thread_func(void *ptr) {
#ifdef MMIO_SID
	uint8_t *sid_ptr;
#endif
	while(mmio_run) {
		usleep(1000);
#ifdef MMIO_TIMER
		if(ram[MMIO_TIMER_CFG] & 0x01)		// timer0 on
			ram[MMIO_TIMER_REG0]++;

		if(ram[MMIO_TIMER_CFG] & 0x02)		// timer1 on
			ram[MMIO_TIMER_REG1]++;

		if(ram[MMIO_TIMER_CFG] & 0x04)		// timer2 on
			ram[MMIO_TIMER_REG2]++;

		if(ram[MMIO_TIMER_CFG] & 0x08)		// timer3 on
			ram[MMIO_TIMER_REG3]++;

		if((ram[MMIO_TIMER_CFG] & 0x10)||(ram[MMIO_TIMER_CFG] & 0x20)||(ram[MMIO_TIMER_CFG] & 0x40)||(ram[MMIO_TIMER_CFG] & 0x80)) {
			if(TREG0 == 0) irq6502();
			if(TREG1 == 0) irq6502();
			if(TREG2 == 0) irq6502();
			if(TREG3 == 0) irq6502();
		}
#endif
#ifdef MMIO_SID
		if(ram[MMIO_SID_ENABLE]) {
			sid_ptr = ram + MMIO_SID_BASE;
			write(ser_port, sid_ptr, 26);
		//	printf(".");
			write(ser_port, (const void *)0xf7, 1);
		}
#endif
	}
	return NULL;
}
#endif

#ifdef MMIO_GPIO

static uint8_t port_a;
static uint8_t port_b;
static uint8_t port_c;
static uint8_t port_d;

void mmio_gpio() {
	port_a = ram[GPIO_BASE];
	port_b = ram[GPIO_BASE+1];
	port_c = ram[GPIO_BASE+2];
	port_d = ram[GPIO_BASE+3];
}

#endif

static uint16_t rng_base;
static int rng_enabled;
static int rng_urand;

int rng8_init(uint16_t addr, int urand) {
	rng_enabled = 1;
	rng_base = addr;
	rng_urand = urand;

	switch(urand) {
		case 0:		// "normal random"
			srand(time(NULL));
			break;
		case 1:		// urandom
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
			case 0:		// "normal" random
				ram[rng_base] = (uint8_t)(rand()&0xff);
				break;
			case 1:		// urandom
				ram[rng_base] = (uint8_t)(rand()&0xff);
				break;
		}
	}
	return;
}



