#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

/*
extern unsigned threadlock;
extern void lock_init(unsigned *lock);
extern unsigned lock_acquire(unsigned *lock);
extern void lock_release(unsigned *lock);
*/

// lock type
typedef struct LOCK_T
{
    unsigned lock_state;
    unsigned lock_count;
    int lock_owner;
}lock_t;

extern lock_t threadlock;
extern void lock_init(lock_t* lock);
extern unsigned lock_acquire(lock_t* lock);
extern void lock_release(lock_t* lock);

extern unsigned currThread;
extern void yield(void);

#endif // _SCHEDULER_H_
