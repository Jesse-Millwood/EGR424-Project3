#include <stdio.h>
#include "scheduler.h"

void thread1_UART(void){
	//loop
	while(1)
	{
		if(lock_acquire(&UART0_lock))
		{
			//print
		}
		yield();
	}
}

void thread2_LED(void){
	//loop
	while(1)
	{
		if(lock_acquire(&LED_lock))
		{
			//toggle LED every second
		}
	}
	//(systick timer times out ever 1ms, so proves pre-emption works)
}

void thread3_OLED(void)
{
	//loop
	while(1)
	{
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
		if(lock_acquire(&UART1_lock))
		{
			//whatever
		}
	}
}
