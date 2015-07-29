/*
 * Implement the thread creation task:
 *
 *   - initialize the jump buffer with appropriate values for
 *     R13 (stack) and R14 (first address to jump to)
 *   - all other registers are irrelevant upon thread creation
 *
 *   In the jump buffer, the R13 slot is set to the second parameter of this
 *   function (the top-of-stack address, passed in R1). The R14 slot is set to
 *   the address of the threadStarter() function.
 *
 *   The C prototype for this function call is:
 *       createThread(threads[i].state, threads[i].stack)
 *   thus:
 *       R0 <-- state (a setjmp()-style jump buffer)
 *       R1 <-- stack (address of top-of-stack)
 */
    .syntax unified
    .text
    .align 2
    .thumb
    .thumb_func
    .type createThread,function
    .global createThread
createThread:  

  /* Save registers in the jump buffer. Their values are
     not important when the thread is first created. This line is the same as
     the first two lines of setjmp(), except we don't save SP and LR since we 
     want to set these to our own values. Really, the only point of this 
     instruction is to advance R0 to the right location in the jump buffer for 
     pointing to SP (without having to do any math :-) */

     stmea  r0!, { r4-r11 }	// Non-automatically saved registers
									
	/*
	 *	Now we need to store r0-r3, r12, r14, address to return to, and xPSR
	 *	default values to the thread's stack. These will be used automatically
	 *	by the thread if they are in the correct order.
	 */
	
	// Store the necessary default values into general-purpose registers
	mov  r7, #0
	mov  r8, #0
	ldr  r9, .L0
	mov  r10, 0x1000000
	mov  r11, #0
	
	// Decrement the stack pointer then store for each register
	stmdb r1!, { r7-r11 }
	
	// Store the necessary default values into general-purpose registers
	mov  r8, #0
	mov  r9, #0
	mov  r10, #0
	mov  r11, #0
	
	// Decrement the stack pointer then store for each register
	stmdb  r1!, { r8-r11 }
	
	/*
	 *	Store the stack pointer in the thread array after storing values to
	 *	the stack, so that the stack pointer is in the correct memory location
	 *	when the thread starts.
	 */
    
  
  /* Now we save SP and LR in that order. SP is the R1 parameter, and we have
   * to get the address of threadStarter() into a higher register (so they are
   * placed in the jump buffer in the right order). */

    ldr     R2, .L0
    stmea   R0!, { R1, R2 }   @ Store "SP" and "LR" for the new thread





        
    bx      lr
    
.L0:
    .word   threadStarter
