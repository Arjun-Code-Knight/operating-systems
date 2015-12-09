/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include "mythread.h"

typedef struct mythread_barrierattr_t {
 	int dummy;
} mythread_barrierattr_t;

typedef struct mythread_barrier_t {
        mythread_barrierattr_t *attr; 
        int no_of_threads; 
	int no_of_blocked_threads;                 
        mythread_queue_t *blocked_threads_q;                       
}mythread_barrier_t;


int mythread_barrier_init(mythread_barrier_t *barrier,const mythread_barrierattr_t *attr,unsigned int count);

int mythread_barrier_wait(mythread_barrier_t *barrier);

int mythread_barrier_destroy(mythread_barrier_t *barrier);	
