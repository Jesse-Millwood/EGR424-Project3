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

  
	@Store the actual process stack pointer (char*) in r2 from the char ** in r1
	ldr     r2, [r1]

	@Initialize xPSR (first on stack) with 0x01000000 (Thumb bit set)
	movw  	r3, 0x0000
	movt 	r3, 0x0100
	stmdb 	r2!, {r3}	@Store xPSR at the highest memory location on the stack

	@Store threadStarter as addressToReturnTo (second on stack)
	ldr  	r3, =threadStarter
	stmdb   r2!, {r3}	@Store addressToReturnTo (threadStarter) on second highest memory on stack

	@Initialize r0-r3, r12, and r14
	mov  	r3, #0
	mov  	r12, #0
	stmdb   r2!, {r3}	 		@Simulate storing r14 (LR) as 0
	stmdb 	r2!, {r3, r12}		@Store r3, r12 (IP) on stack
	stmdb   r2!, {r3} 			@Simulate storing r2 on the stack by using the value 0
	stmdb   r2!, {r3, r12}		@Simulate storing r0 (0) and r1 (0) on stack by storing zeros

	@Save the pointer to the stack pointer
	str r2, [r1]

	@Store r4-r12 and PSP on array
	stm 	r0, {r2, r4-r12}

	bx		lr
.L0:
    .word   threadStarter
