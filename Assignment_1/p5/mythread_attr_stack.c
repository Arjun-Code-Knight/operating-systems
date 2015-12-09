/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include <mythread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mythread_attr_setstack(mythread_attr_t *attr, void *stackaddr,
			   size_t stacksize)
{
	attr->stacksize = (int) stacksize;
	attr->stackbase = stackaddr;
	return 0;
}

int mythread_attr_getstack(const mythread_attr_t *attr,
			   void **stackaddr,
			   size_t *stacksize)
{
	*stacksize = (size_t) attr->stacksize;
	*stackaddr = attr->stackbase;
	return 0;
}

int mythread_attr_setstacksize(mythread_attr_t *attr, size_t stacksize)
{
	attr->stacksize = (int) stacksize;
	return 0;
}

int mythread_attr_getstacksize(const mythread_attr_t *attr,
			   size_t *stacksize)
{
	*stacksize = (size_t) attr->stacksize;
	return 0;
}

