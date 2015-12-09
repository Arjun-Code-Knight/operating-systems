/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include <futex.h>
#include <mythread.h>
#include <mythread_queue.h>
#include <sys/syscall.h>

struct futex yield_futex;

/* The global queue which is used to hold TCBs of all spawned threads*/
mythread_queue_global* mythread_queue_globalVar;

/* Dispatcher to wake the next ready thread */
int __dispatch_thread(mythread_t * thread)
{
	struct mythread_queue_node* ptr = queue_search_element(mythread_queue_globalVar, thread->tid);
	/* Find the next READY thread */

	ptr = ptr->nextNode;
	while (ptr->currentThread->state != READY)
		ptr = ptr->nextNode;

	/* No other thread is READY. Dispatcher does nothing */
	if (ptr->currentThread == thread)
		return -1;
	else {
		/* Wake up the next ready thread */
		futex_up(&ptr->currentThread->futex);
		return 0;
	}
}

/* Uses dispatcher to yeild the next Ready thread */
int mythread_yield()
{
	mythread_t *self_tcb =  queue_search_elementbyId(mythread_queue_globalVar, mythread_self().tid);

	/* Using yield_futex so that no other thread yields when one thread is yielding */
	futex_down(&yield_futex);

	if (__dispatch_thread(self_tcb) == -1) {
		futex_up(&yield_futex);
		return 0;
	}
	/* Release the yield futex */
	futex_up(&yield_futex);

	/* Sleep till another process wakes us up */
	futex_down(&self_tcb->futex);

	return 0;
}
