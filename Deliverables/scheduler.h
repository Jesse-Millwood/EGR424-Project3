#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <setjmp.h>
#include "inc/lm3s6965.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
// thread_t is a pointer to function with no parameters and
// no return value...i.e., a user-space thread.
typedef void (*thread_t)(void);
extern void thread1(void);
extern void thread2(void);
extern void  thread1_UART(void);
extern void  thread2_LED(void);
extern void  thread3_OLED(void);
extern void  thread4_UART(void);
static thread_t threadTable[] = {
  thread1_UART,
  thread2_LED,
  thread3_OLED,
  thread4_UART
};
//-- Macros
#define LED_TI (HWREG(0x40025000 + 0x004))
#define STACK_SIZE 4096   // Amount of stack space for each thread
#define NUM_THREADS (sizeof(threadTable)/sizeof(threadTable[0]))
//-- Type Definitions
typedef struct LOCK_T
{
    unsigned lock_state;
    unsigned lock_count;
    int lock_owner;
}lock_t;
typedef struct
{
    int active;       // non-zero means thread is allowed to run
    char *stack;      // pointer to TOP of stack (highest memory location)
    unsigned savedregs[40]; 
} threadStruct_t;
//-- Global Variables
lock_t OLED_lock;
lock_t UART0_lock;
lock_t UART1_lock;
lock_t LED_lock;
static jmp_buf scheduler_buf;   // saves the state of the scheduler
static threadStruct_t threads[NUM_THREADS]; // the thread table
unsigned currThread;

//-- Function Prototypes
void lock_init(lock_t* lock);
unsigned lock_acquire(lock_t* lock);
void lock_release(lock_t* lock);
void yield(void);
void scheduler(void);
void systick_init(void);
void periphs_init(void);
void createThread(jmp_buf buf, char *stack);
void restore_registers(unsigned* buffer);
int save_registers(unsigned* buffer);
void initialize_threads(void);

#endif // _SCHEDULER_H_
