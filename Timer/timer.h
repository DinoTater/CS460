#include "type.h"

#define LATCH_COUNT     0x00	   /* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	   /* ccaammmb, a = access, m = mode, b = BCD */

#define TIMER_FREQ      1193182L	   /* clock frequency for timer in PC and AT */
#define TIMER_COUNT     TIMER_FREQ/60 /* initial value for counter*/

#define TIMER0          0x40
#define TIMER_MODE      0x43
#define TIMER_IRQ       0
#define INT_CNTL        0x20
#define INT_MASK        0x21

//u16 tick;
int hours, minutes, seconds, tick;
