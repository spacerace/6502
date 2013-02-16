/* assumes WORD = 16bit, BYTE = 8bit!! */
typedef unsigned short WORD;
typedef unsigned char BYTE;
   
/* Address mask. Atari Asteroids/Deluxe use 0x7fff -
 * but use 0xffff for full 16 bit decode
 */
extern WORD addrmask;

/* pointer to the game memory map */
extern BYTE *gameImage;

/* must be called first to initialise all 6502 engines arrays */
extern void init6502(void);

/* sets all of the 6502 registers. The program counter is set from
 * locations $FFFC and $FFFD masked with the above addrmask
 */
extern void reset6502(void);

/* run the 6502 engine for specified number of clock cycles */
extern void exec6502(int timerTicks);

