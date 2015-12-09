/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include "mythread.h"
#include "mymutex.h"

typedef struct mythread_condattr_t {
 	int dummy;
} mythread_condattr_t;

typedef struct mythread_cond_var {
        mythread_condattr_t *attr;
        volatile int no_of_blocked_threads;                 
        mythread_queue_t *blocked_threads_q;          
}mythread_cond_t;

int mythread_cond_init(mythread_cond_t *cond, const mythread_condattr_t *attr);

int mythread_cond_wait(mythread_cond_t *cond, mythread_mutex_t *mutex);

int mythread_cond_signal(mythread_cond_t *cond);

int mythread_cond_broadcast(mythread_cond_t *cond);

int mythread_cond_destroy(mythread_cond_t *cond); 
