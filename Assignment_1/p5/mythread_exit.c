#include <mythread.h>
#include <malloc.h>
#include <mythread_queue.h>
#include <sys/syscall.h>
#include <stdbool.h>
#include <mythread_yield.c>

/* The global queue which is used to hold TCBs of all spawned threads*/
mythread_queue_global* mythread_queue_globalVar;

/* mythread_exit function. 
   Used to check the Join Q of the thread for whether to mark joining threads as Ready.
   Then exit the system. */

void mythread_exit(void *retval)
{
	/* self call for current thread TCB*/
	mythread_t *self_ptr = queue_search_elementbyId(mythread_queue_globalVar, mythread_self().tid);

	/* Change state to TERMINATED and set the return value */
	self_ptr->state = TERMINATED;
	self_ptr->returnValue = retval;

	/* Check the join Q to set waiting threads' state to ready. 
	   This will happen only if the waiting thread is not in the join Q of any other thread. 
	   For this we are traversing the global TCB queue and checking in the join Q of each TCB  */
	mythread_queue_t joinq = self_ptr->joinq;
	if(joinq.headNode != NULL)
	{
		struct mythread_queue_node* tempNode  = joinq.headNode;
		do
		{
			/* flag to hold whether to change the status of the thread to ready or not*/
			bool flipFlag = TRUE;
			struct mythread_queue_node* tempGlobHead = mythread_queue_globalVar->headNode;
			do
			{
				
				if(tempGlobHead->currentThread->tid != self_ptr->tid && tempGlobHead->currentThread->joinq.headNode != NULL && tempGlobHead->currentThread->state != TERMINATED)
				{
					struct mythread_queue_node* innerQueueNode = tempGlobHead->currentThread->joinq.headNode;
					do
					{
						/* If the thread is found in other thread's join queue, update the flag */
						if(tempNode->currentThread->tid == innerQueueNode->currentThread->tid)
							flipFlag = FALSE;
						innerQueueNode = innerQueueNode->nextNode;
					}while(innerQueueNode != tempGlobHead->currentThread->joinq.headNode);
				}

				tempGlobHead = tempGlobHead->nextNode;

			}while(tempGlobHead != mythread_queue_globalVar->headNode);

			/* Thread not found in any other thread's join queue */
			if(flipFlag == 	TRUE)
			{
				tempNode->currentThread->state = READY;
			}
			tempNode = tempNode->nextNode;
		}while(tempNode != joinq.headNode);

	}

	/* Call the dispacher to execute the next READY thread*/
	__dispatch_thread(self_ptr);

	/* Exit system call */
	syscall(SYS_exit, 0);

}
