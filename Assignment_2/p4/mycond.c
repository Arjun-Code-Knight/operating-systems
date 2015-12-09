/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include "mythread.h"
#include "mycond.h"

int mythread_cond_init(mythread_cond_t *cond, const mythread_condattr_t *attr)
{
	/* Initializing condition var with default values */
	cond = (mythread_cond_t *) malloc(sizeof(mythread_cond_t));
	cond->attr = attr;
	cond->no_of_blocked_threads = 0;
	cond->blocked_threads_q=NULL;
	return 0;
}

int mythread_cond_wait(mythread_cond_t *cond, mythread_mutex_t *mutex)
{
	mythread_t current_thread = mythread_self();
	/* Current thread should hold the mutex to wait on the condition */
	if(mutex->owner_thread == current_thread)
	{
		mythread_enter_kernel();
		/* Blocked phase 1, will add to the queue before suspending it, so that it can be signalled while releasing the lock 
		before being suspended */
		mythread_block_phase1(&(cond->blocked_threads_q), 1);
		mythread_leave_kernel();
		/*Unlocking Mutex*/
		mythread_mutex_unlock(mutex);
		mythread_enter_kernel();
		if(current_thread->state != 0){
			/*Thread not signalled, suspend execution*/
			cond->no_of_blocked_threads = cond->no_of_blocked_threads + 1;
			mythread_block_phase2();
		} else {
			mythread_leave_kernel();
		}
		/*Lock mutex again after resuming, and continue execution s*/
		mythread_mutex_lock(mutex);	
	}
	return 0;
}

int mythread_cond_signal(mythread_cond_t *cond)
{
	mythread_enter_kernel();
	/* Using unblock to wake up the first thread from the wait Q */
	if(cond->no_of_blocked_threads > 0){
		cond->no_of_blocked_threads -= 1;
		mythread_unblock(&(cond->blocked_threads_q), 1);
	}
	mythread_leave_kernel();
        return 0;
}

int mythread_cond_broadcast(mythread_cond_t *cond)
{
	/* Using unblock to wake up all threads in the wait Q */
	while(cond->no_of_blocked_threads > 0){
		mythread_enter_kernel();
 		cond->no_of_blocked_threads = cond->no_of_blocked_threads - 1;
 		mythread_unblock(&(cond->blocked_threads_q), 1);
		mythread_leave_kernel();
	}
	return 0;
}

int mythread_cond_destroy(mythread_cond_t *cond) 
{
	mythread_enter_kernel();
	if(cond->no_of_blocked_threads > 0){
		mythread_leave_kernel();
		return 1;
	}
	cond->attr = NULL;
	cond->no_of_blocked_threads = NULL;	
	mythread_leave_kernel();
	return 0;
}
