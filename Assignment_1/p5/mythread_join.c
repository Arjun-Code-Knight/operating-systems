#include <mythread.h>
#include <mythread_queue.h>
#include <sys/syscall.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* The global queue which is used to hold TCBs of all spawned */
mythread_queue_global* mythread_queue_globalVar;

/*
 * mythread_join - suspend calling thread if target_thread has not finished,
 * enqueue on the join Q of the target thread, then dispatch ready thread;
 * once target_thread finishes, it activates the calling thread / marks it
 * as ready (If the current thread is not in any other thread's join Q)
 */
int mythread_join(mythread_t target_thread, void **status)
{
	
	mythread_t *thread_current, *thread_target_ptr;
	thread_current = queue_search_elementbyId(mythread_queue_globalVar, (pid_t) syscall(SYS_gettid));
	/* Search for target thread TCB in the queue */
	thread_target_ptr = queue_search_elementbyId(mythread_queue_globalVar, target_thread.tid);
	
	/* No need to do anything if target thread is terminated */
	if (thread_target_ptr->state == TERMINATED)
	{
		if (status != NULL) 
			*status = thread_target_ptr->returnValue;
		return 0;
	}

	/* Change status of current thread to Blocked */
	thread_current->state = BLOCKED;

	/* Add current thread TCB in join Q of target thread */
	queue_add_element(&(thread_target_ptr->joinq),thread_current);

	/* Yield the current thread */
	mythread_yield();

	/* Target thread terminated, control back to current thread. Set the status to return value */
	if (status != NULL) 
		*status = thread_target_ptr->returnValue;
	return 0;
}
