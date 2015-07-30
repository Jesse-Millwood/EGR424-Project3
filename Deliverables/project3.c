#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/lm3s6965.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "rit128x96x4.h"
#include "scheduler.h"

//#define PIN_CONTEXT LED_TI

void main(void)
{
  unsigned i;
  periphs_init();
  iprintf("Num of threads: %d\n", NUM_THREADS);
  // Start running coroutines
  currThread = -1;
  // Initialize 
  initialize_threads();
  // Initialize the global thread lock
  lock_init(&OLED_lock);
  lock_init(&UART0_lock);
  lock_init(&UART1_lock);
  // cause exception to move program to scheduler
  yield();
  // If scheduler() returns, all coroutines are inactive and we return
  // from main() hence exit() should be called implicitly (according to
  // ANSI C). However, TI's startup_gcc.c code (ResetISR) does not
  // call exit() so we do it manually.
  exit(0);
}
/* Function:    lock_init
 * Description: initialize lock structures for the peripherals
 */ 
void lock_init(lock_t* lock)
{
    lock->lock_state = 1; // initialize lock as released
    lock->lock_count = 0; // initialize no lock
    lock->lock_owner = -1; // not an owner

}
/* Function:    lock_acquire
 * Description: Attempt to aquire a lock, if it isn't acquired
 *              it can be acquired, otherwise it is free to be
 *              locked
 */ 
unsigned lock_acquire(lock_t* lock)
{
    if(lock->lock_state == 0 && lock->lock_owner == currThread)
    {
        // If already locked, increment lock count
        lock->lock_count ++;
    }

        asm volatile("MOV 	r1, #0\n"
                     "LDREX 	r2, [r0]\n"	// R2: Lock value
                     "CMP 	r2, r1\n"	// Is it 0? y:locked
                     "ITT 	NE\n"		// 
                     "STREXNE 	r2, r1, [r0]\n" // if not locked try to claim it R2: 0 if success, 1 if failure
                     "CMPNE 	r2, #1\n"	// check success
                     "BEQ 	1f\n");		// branch if lock was already 0
    // If this code executes, it did not branch
        lock->lock_count++ ;  // increment lock count
        lock->lock_state = 0; // locked
        lock->lock_owner = currThread; // owner
        asm volatile("MOV 	R0, #1\n"	// Indicate success, return 1
                     "BX 	LR\n"		// Branch out of function
                     "1:\n"			// label to branch to if we did not get the lock
                     "CLREX\n"			// clear exclusive access
                     "MOV 	R0, #0\n"	// indicate failure
                     "BX 	LR\n");
}
/* Function:    lock_release
 * Description: release the lock that was acquired,
 *              take into account how many times the peripheral has
 *              been locked
 */ 
void lock_release(lock_t* lock)
{
    lock->lock_count --;  // decrement lock count

    if(lock->lock_count == 0)
    {
        // really unlocked, release the lock
        lock->lock_state = 1;
        lock->lock_owner = -1;
    }
}
/* Function:    yield
 * Description: This function is called from within user thread
 *              context. It executes a jump back to the scheduler.
 *              When the scheduler returns here, it acts like a
 *              standard function return back to the caller of yield().
 */ 
void yield(void)
{
    // call scheduler as an exception
    asm volatile ("svc #100");
}
/* Function:    threadStarter
 * Description: This is the starting point for all threads.
 *              It runs in user thread context using the
 *              thread-specific stack. The address of this function
 *              is saved by createThread() in the LR field of the
 *              jump buffer so that the first time the scheduler()
 *              does a longjmp() to the thread, we start here.
 */ 
void threadStarter(void)
{
  // Call the entry point for this thread. The next line returns
  // only when the thread exits.
  (*(threadTable[currThread]))();

  // Do thread-specific cleanup tasks. Currently, this just means marking
  // the thread as inactive. Do NOT free the stack here because we're
  // still using it! Remember, this function runs in user thread context.
  threads[currThread].active = 0;

  // This yield returns to the scheduler and never returns back since
  // the scheduler identifies the thread as inactive.
  yield();
}
/* Function:    scheduler_handler
 * Description: This is a handler that is called either by
 *              an 'svc' call or the 'systick' timer.
 */ 
void scheduler_handler(void)
{
    // toggle pin for measuring context switch time
    //PIN_CONTEXT ^= 1; 
    // Save current thread state if not the first time through
    if(currThread != -1)
    {
        save_registers(threads[currThread].savedregs);
    }
    // Loop through the active threads
    do
    {
        if (++currThread >= NUM_THREADS)
        {
            currThread = 0;
        }
    } while (threads[currThread].active != 1);
    // toggle pin for measuring context switch time
    //PIN_CONTEXT ^= 1;     
    // Restore the thread state for the thread about to be executed
    // This jumps to the next thread
    restore_registers(threads[currThread].savedregs);
    
}
/* Function:    systick_init
 * Description: Initialize the systick timer
 */ 
void systick_init(void)
{

    // Fire every 1 second, with 8MHz clock
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_RELOAD_R = 0x1F40;
    NVIC_ST_CURRENT_R = 0;
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC |
        NVIC_ST_CTRL_INTEN |
        NVIC_ST_CTRL_ENABLE;
}
/* Function:    periphs_init
 * Description: Initialize the following:
 *               * system clock
 *               * RIT OLED
 *               * UART0
 *               * LED0 Pins
 *               * call systick initialize function
 *               * IntMasterEnable
 */ 
void periphs_init(void)
{
    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Initialize the OLED display and write status.
    RIT128x96x4Init(1000000);
    RIT128x96x4StringDraw("Scheduler Demo",       20,  0, 15);

    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Set GPIO A0 and A1 as UART pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
    
    // Initialize Pins for LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
    GPIO_PORTF_DIR_R = 0x01;
    GPIO_PORTF_DEN_R = 0x01;
    
    systick_init();
    IntMasterEnable();
}
/* Function:    save_registers
 * Description: move the process stack pointer to register 1,
 *              store register 1 and r4-r12 at the address
 *              in the address at r0. 
 */ 
int save_registers(unsigned* buffer)
{
    // Takes Place of setjmp
    // Saves the current environment
    asm volatile ("mrs r1,psp\n"
                  "stm r0, {r1, r4-r12}");
    // return 0 when setting up the buffer
    return 0;
    
}
/* Function:    restore_registers
 * Description: Takes place of longjmp. Restores
 *              the environment previously saved by
 *              save_registers(), branch back with
 *              the process stack, returning to thread mode
 *
 */ 
void restore_registers(unsigned* buffer)
{
    asm volatile ("ldr r1, [r0] \n"
                  "add r0, r0, #4\n"
                  "msr psp, r1\n"
                  "ldm r0, {r4-r12}\n"
                  "movw lr, 0xfffd\n"
                  "movt lr, 0xffff\n"
                  "bx lr");
}
/* Function:    initialize_threads
 * Description: loop through the threads and make active.
 *              allocate memmory for the thread's stack,
 *              make sure the pointer to the allocated memmory
 *              is at the top of the memmory block
 */ 
void initialize_threads(void)
{
    
    unsigned i;

    // Create all the threads and allocate a stack for each one
    for (i = 0; i < NUM_THREADS; i++) 
    {
        //Mark thread as runnable
        threads[i].active = 1;
        //Allocate stack
        threads[i].stack = (char *)malloc(STACK_SIZE) + STACK_SIZE;
        if (threads[i].stack == 0)
        {
            //out of memory
            exit(1);
        }
        //Create each thread
        createThread(threads[i].savedregs, &(threads[i].stack));
  }
}

/*
 * Compile with:
 * ${CC} -o lockdemo.elf -I${STELLARISWARE} -L${STELLARISWARE}/driverlib/gcc 
 *     -Tlinkscript.x -Wl,-Map,lockdemo.map -Wl,--entry,ResetISR 
 *     lockdemo.c create.S threads.c startup_gcc.c syscalls.c rit128x96x4.c 
 *     -ldriver
 */

