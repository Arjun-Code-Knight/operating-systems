#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <mythread.h>
#include <mythread_queue.h>

/* Idle thread function.
   Checks for the next READY thread in the queue and yeilds itself.
   If no READY thread found, then it would be the terminated.
 */
void *idle_thread(void *params)
{
	pid_t idleThreadId = (pid_t) syscall(SYS_gettid);
	/* Search for idle thread TCB in the queue*/
	struct mythread_queue_node* currentIdleThreadNode = queue_search_element(mythread_queue_globalVar,idleThreadId);
	
	mythread_t* tmpThread;
	struct mythread_queue_node* ptr = currentIdleThreadNode->nextNode;

	/* Check for other threads in the queue, which needs control*/
	while (1) {

		while (ptr->currentThread->state != READY){
			ptr = ptr->nextNode;
		}


		/* No other thread is READY. Exit the idle thread */
		if (ptr->currentThread->tid == currentIdleThreadNode->currentThread->tid){
			syscall(SYS_exit, 0);
		}

		ptr = currentIdleThreadNode->nextNode;
		tmpThread = ptr->currentThread;
		/* Loop through the queue to check any thread in Ready State to run */
		while (tmpThread->tid != idleThreadId) {
			if (tmpThread->state == READY) {
				break;
			}
			ptr = ptr->nextNode;
			tmpThread = ptr->currentThread;
		}
		/* Yield the idle thread to next READY thread*/
		mythread_yield();
	}
}
