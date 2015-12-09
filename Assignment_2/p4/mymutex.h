/* Author info:
	athimma Arjun Thimmareddy
 * 
 */

typedef struct mymutex_attr {
         int type;
} mythread_mutexattr_t;

typedef struct mymutex {
	int lock;
	volatile int no_of_blocked_threads;
	mythread_mutexattr_t *attr;
	mythread_queue_t *blocked_thread_q; 
	volatile mythread_t owner_thread; 
}mythread_mutex_t;

int mythread_mutex_init(mythread_mutex_t *mutex, const mythread_mutexattr_t *attr);

int mythread_mutex_lock(mythread_mutex_t *mutex);

int mythread_mutex_unlock(mythread_mutex_t *mutex);

int mythread_mutex_destroy(mythread_mutex_t *mutex);
