/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include <malloc.h>
#include <mythread.h>
#include <mythread_queue.h>
#include <string.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>

/* Return the user exposed handle to the current thread */
mythread_t mythread_self()
{
	pid_t tid = (pid_t) syscall(SYS_gettid);
	mythread_t tcb;
	tcb.tid = tid;
	return tcb;
}
