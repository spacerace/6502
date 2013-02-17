#ifndef __MMIO_H__
#define __MMIO_H__

// #define MMIO_RANDOM
#define MMIO_SID
// #define MMIO_TIMER

#define MMIO_USE_THREADS
#define MMIO_USE_OPCODEHOOK
#define MMIO_RND_BASE	0x0300
#define MMIO_RND_ENABLE MMIO_RND_BASE
#define MMIO_RND_REG	MMIO_RND_BASE+1
#define MMIO_TIMER_BASE	0x0310
#define MMIO_TIMER_CFG	MMIO_TIMER_BASE
#define MMIO_TIMER_REG0	MMIO_TIMER_BASE+1
#define MMIO_TIMER_REG1 MMIO_TIMER_BASE+2
#define MMIO_TIMER_REG2 MMIO_TIMER_BASE+3
#define MMIO_TIMER_REG3 MMIO_TIMER_BASE+4

#define TREG0	ram[MMIO_TIMER_REG0]
#define TREG1	ram[MMIO_TIMER_REG1]
#define TREG2	ram[MMIO_TIMER_REG2]
#define TREG3	ram[MMIO_TIMER_REG3]

#define MMIO_SID_BASE	0x0400
#define MMIO_SID_ENABLE 0x03FF

extern int init_mmio();
extern int stop_mmio();

extern void mmio_opcodehook();
extern void mmio_irqhook();
extern void mmio_nmihook();

extern int rng8_init(uint16_t addr, int urand);
extern void rng8_deinit();
extern void rng8_getrnd();

#endif // __MMIO_H__
