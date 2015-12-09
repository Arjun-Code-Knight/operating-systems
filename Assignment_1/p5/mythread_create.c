/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#define _GNU_SOURCE 
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sched.h>
#include <mythread_queue.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <mythread.h>
#include <futex.h>

/* Default stack size for the thread. Used when attr is passed as null to mythread_create */
#ifndef STACKSIZE_DEFAULT
#define STACKSIZE_DEFAULT 16384
#endif

/* Idle thread tcb and function */
mythread_t idle_tcb;
void *idle_thread(void *);

/* Wrapper function (mythread_wrapper.c) is called for each thread after clone.*/
int wrapper_function(void *);

/* Yield Futex used by the dispatcher to take care of race condition so that other thread 
   does not yeild before one thread has finished yeilding */
extern struct futex yield_futex;

/* The global queue which is used to hold TCBs of all spawned threads*/
mythread_queue_global* mythread_queue_globalVar;

/* mythread_create function creates a new thread using clone system call.
   It passes wrapper function to the clone system which invokes the user function.
   attr can also be passed, if not passed, then default attr will be used. 
 */
int mythread_create(mythread_t* new_thread,
		    mythread_attr_t * attr,
		    void *(*start_func) (void *), void *arg)
{
	/* Global thread queue would be empty on first mythread_create call. 
	   This would add the main thread in the queue and also an idle thread, 
 	   and then proceed with the new thread creation */

	if (mythread_queue_globalVar == NULL) {
		mythread_queue_globalVar = malloc(sizeof(struct mythread_queue));
		mythread_t *main_t_tcb = (mythread_t *) malloc(sizeof(mythread_t));

		/* Initializing TCB values for main thread */

		main_t_tcb->start_func = NULL; /* There is no explicit function to call for the main thread */
		main_t_tcb->state = READY;
		main_t_tcb->returnValue = NULL;
		main_t_tcb->arg = NULL;
		main_t_tcb->joinq.headNode = NULL;
		main_t_tcb->joinq.tailNode = NULL;
		main_t_tcb->tid = (pid_t) syscall(SYS_gettid);

		/* Initialize main thread futex */
		futex_init(&main_t_tcb->futex, 0);

		/* Adding main thread to the queue */
		queue_add_element(mythread_queue_globalVar, main_t_tcb);

		/* Initialise the yeild futex */
		futex_init(&yield_futex, 1);

		/* Recursive call to mythread_create for the idle thread creation*/
		mythread_create(&idle_tcb, NULL, idle_thread, NULL);
	}

	mythread_t *new_mythread_t = (mythread_t *) malloc(sizeof(mythread_t));
	pid_t tid;

	/* Clone flags to be passed, taken from pthread source code - CLONE_PTRACE is removed */
	int clone_flags = (CLONE_VM | CLONE_FS  
			   | CLONE_FILES | CLONE_SIGHAND 
			   | CLONE_PARENT_SETTID | CLONE_THREAD
			   | CLONE_CHILD_CLEARTID | CLONE_SYSVSEM);

	/* Check if attr is passed, if not, initialize with default attr */
	if (attr == NULL || (attr->stackbase == NULL && attr->stacksize != 0)){
		mythread_attr_t attr_new;
		mythread_attr_init(&attr_new);
		attr = &attr_new;
	}

	/* Initialize new thread's TCB */
	new_mythread_t->arg = arg;
	new_mythread_t->state = READY;
	new_mythread_t->start_func = start_func;
	new_mythread_t->joinq.headNode = NULL;
	new_mythread_t->joinq.tailNode = NULL;
	new_mythread_t->returnValue = NULL;

	/* Initialize the tcb's futex to zero. */
	futex_init(&new_mythread_t->futex, 0);

	/* Clone system call. This will invoke the wrapper_function. */
	tid = clone(wrapper_function, (char *)attr->stackbase, clone_flags,new_mythread_t);

	new_mythread_t->tid = tid;
	/* Returning the reference of the created TCB */
	*new_thread = *new_mythread_t;

	/* Adding the thread to the global TCB queue */
	queue_add_element(mythread_queue_globalVar, new_mythread_t);
	return 0;
}
