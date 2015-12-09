#include <unistd.h>
#include <mythread.h>
#include <futex.h>

/* The job of the wrapper is to suspend the thread until its woken up.
   On waking up call the start_func (User function) of the thread */

int wrapper_function(void *thread_tcb)
{
	mythread_t *new_tcb =  (mythread_t *) thread_tcb;

	/* Suspend the thread */
	futex_down(&new_tcb->futex);

	/* Call the user-defined function. Woken up */
	new_tcb->start_func(new_tcb->arg);

	/* If the execution reaches here, it means mythread_exit() is not called in start_func. 
	   Explicitly calling mythread_exit()*/
	mythread_exit(new_tcb->returnValue);
	
	return 0;
}
