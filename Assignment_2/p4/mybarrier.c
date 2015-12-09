/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include "mybarrier.h"
#include "mythread.h"

int mythread_barrier_init(mythread_barrier_t *barrier,const mythread_barrierattr_t *attr,unsigned int count)
{
	/* Initializing the barrier struct with default values */
	mythread_barrier_t *br =(mythread_barrier_t *)malloc(sizeof(mythread_barrier_t));
	br=barrier;
	br->no_of_threads=count;
	br->no_of_blocked_threads=0;
	br->attr = attr;
	br->blocked_threads_q=NULL;
	return 0;
}


int mythread_barrier_wait(mythread_barrier_t *barrier)
{
	mythread_enter_kernel();
	barrier->no_of_blocked_threads += 1;
	mythread_leave_kernel();
	/* Check if thread wait count is reached */
	mythread_enter_kernel();
	if (barrier->no_of_blocked_threads == barrier->no_of_threads){
		/* unblock all blocked threads on the barrier */
		while(barrier->no_of_blocked_threads > 1){
			barrier->no_of_blocked_threads -= 1;
			mythread_unblock(barrier->blocked_threads_q,0);
		}
		barrier->no_of_blocked_threads -= 1;
		mythread_leave_kernel();
	}
	else{
		/* Else block the thread */
		mythread_block((barrier->blocked_threads_q),1);
		mythread_leave_kernel();
	}	
	return 0;
}

int mythread_barrier_destroy(mythread_barrier_t *barrier)
{
	mythread_enter_kernel();
	barrier->no_of_threads=0;
	barrier->no_of_blocked_threads=0;
	barrier->attr = NULL;
	barrier->blocked_threads_q=NULL;
	mythread_leave_kernel();
	return 0;
}
