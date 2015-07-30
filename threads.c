
#include <stdio.h>
#include "scheduler.h"
#include "inc/lm3s6965.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "rit128x96x4.h"

#define LED_TI (HWREG(0x40025000 + 0x004))

void thread1_UART(void)
{
  unsigned count;
  volatile long i;

  while(1)
  {
    if(lock_acquire(&UART0_lock))
    {
      for (count = 0; count < 3; count++)
      {
          for(i = 0; i < 100000; i++); // delay
          iprintf("In UART thread %d -- pass %d\r\n", currThread, count);
        }
      lock_release(&UART0_lock);
      yield();
    }
  }
}

void thread2_LED(void)
{
  while(1)
  {
    volatile unsigned long i;
    for(i = 0; i < 100000; i++);
    LED_TI ^= 1;  //toggle the LED
  }
}

void thread3_OLED(void)
{
  while(1)
  {
    volatile long i;
    for(i = 0; i < 100000; i++); // delay
    RIT128x96x4StringDraw("Multi        ", 30, 8, 15);
    for(i = 0; i < 100000; i++); // delay
    RIT128x96x4StringDraw("Threaded      ", 30, 8, 15);
    for(i = 0; i < 100000; i++); // delay
    RIT128x96x4StringDraw("Kernel      ", 30, 8, 15);
    
    yield();
  }
}



void thread4_UART(void)
{
  unsigned count;
  volatile long i;
  
  while(1)
  {
    if(lock_acquire(&UART0_lock))
    {
      for (count = 0; count < 5; count++)
      {
          for(i = 0; i < 100000; i++); // delay
        iprintf("In UART thread %d -- pass %d\r\n", currThread, count);
      }
      lock_release(&UART0_lock);
      yield();
    }
  }
}


