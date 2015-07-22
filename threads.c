#include <stdio.h>
#include "scheduler.h"

void thread1_UART(void){
    //loop
    while(1)
    {
        iprintf("Thread: %d\n",currThread);
        if(lock_acquire(&UART0_lock))
        {
            lock_acquire(&UART0_lock);
            // Simulate code that is occasionally interrupted
            iprintf("THIS IS T");
            yield(); // context switch "interrupt"
            iprintf("HREAD NU");
            yield(); // context switch "interrupt"
            iprintf("MBER 1\r\n");
            lock_release(&UART0_lock);
            lock_release(&UART0_lock);
        }
        yield();
	}
}

void thread2_LED(void){
    //loop
    while(1)
    {
        iprintf("Thread: %d\n",currThread);
        if(lock_acquire(&LED_lock))
        {
            //toggle LED every second
        }
        yield();
    }
    //(systick timer times out ever 1ms, so proves pre-emption works)
}

void thread3_OLED(void)
{
    //loop
    while(1)
    {
        iprintf("Thread: %d\n",currThread);
        if(lock_acquire(&OLED_lock))
        {
            //display on OLED
        }
        yield();
    }
}

//for implementing locks, EC
void thread4_UART(void)
{	
    while(1)
    {
        iprintf("Thread: %d\n",currThread);
        if(lock_acquire(&UART0_lock))
        {
            iprintf("this is t");
            yield(); // context switch "interrupt"
            iprintf("hread number 2\r\n");
            lock_release(&UART0_lock);
        }
        yield();
    }
}
