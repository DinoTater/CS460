/******************** timer.c file *************************************/
#include "timer.h"

int tinth();

int enable_irq(u16 irq_nr)
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));

}

int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */

  printf("timer init\n");

  hours = minutes = seconds = tick = 0;

  //set_vector(8, tinth);

  out_byte(TIMER_MODE, SQUARE_WAVE);	// set timer to run continuously
  out_byte(TIMER0, TIMER_COUNT);	// timer count low byte
  out_byte(TIMER0, TIMER_COUNT >> 8);	// timer count high byte 
  enable_irq(TIMER_IRQ);
}

/*===================================================================*
 *		    timer interrupt handler                           		     *
 *===================================================================*/
int thandler()
{
  u16 base = 0xB800;
  int w, i, update = 0;
  int orig_col = column;
  int orig_row = row;
  column = 70;
  row = 24;
  color = 0x0A;

  if (!(++tick%60))
  {
    seconds++;
  }

  if (tick%60 == 0)
  {
    if (!(seconds%60))
    {
      minutes++;
      seconds = 0;
    }
    if (!(minutes%60) && !(seconds%60))
    {
      hours++;
      minutes = 0;
    }
  }

  putc(hours/10 + '0');
  putc(hours%10 + '0');
  putc(':');
  putc(minutes/10 + '0');
  putc(minutes%10 + '0');
  putc(':');
  putc(seconds/10 + '0');
  putc(seconds%10 + '0');
  

  column = orig_col;
  row = orig_row;

  //running->inkmode > 1 ? putc('K') : putc('U');
  //printf("mode\n");

  out_byte(0x20, 0x20);                // tell 8259 PIC EOI
  
}
