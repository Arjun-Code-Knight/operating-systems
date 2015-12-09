#include <mythread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Default stack size for the thread */
#ifndef STACKSIZE_DEFAULT
#define STACKSIZE_DEFAULT 16384
#endif

int mythread_attr_init(mythread_attr_t *attr)
{
	void *stackaddr;
	/* Initialize attr memory */	
	memset(attr, '\0', sizeof(mythread_attr_t));
	/* Assigning memory with default stack size */
	posix_memalign(&stackaddr, 8, STACKSIZE_DEFAULT);
	stackaddr = stackaddr + STACKSIZE_DEFAULT;
	/* Setting stack with default attributes */
	mythread_attr_setstack(attr, stackaddr, STACKSIZE_DEFAULT);
	return 0;
}

int mythread_attr_destroy(mythread_attr_t *attr)
{	
	/* Free attr memory */
	free(attr);
	return 0;
}
