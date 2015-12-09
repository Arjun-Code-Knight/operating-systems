/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include <stdio.h>
#include "mycond.h"
#include "mybarrier.h"
#include "mythread.h"

#define NTHREADS 8	

mythread_mutex_t mutex;
mythread_cond_t condition;
mythread_cond_t condition;
mythread_barrier_t barrier;
volatile int count=0;

struct futex printFutex;

/* For printing to console */
void printToConsole(char *buffer){
  futex_down(&printFutex);
  write(1, buffer, strlen(buffer));
  futex_up(&printFutex);
}

/* Each thread will increment the count (global) protected by mutex */
void* thread_func(void *arg)
{
	char buffer[1024];
	/*Lock Mutex*/
	int temp = 0;
	mythread_mutex_lock(&mutex);
	while(temp<1000){
		temp++;
	}
	/*Wait to increment until main thread signals*/
	mythread_cond_wait(&condition,&mutex);
	count++;
	mythread_enter_kernel();
	sprintf(buffer, "Incremented Count to : %d\n", count);
	printToConsole(buffer);
	mythread_leave_kernel();
	mythread_mutex_unlock(&mutex);
	/* Threads will wait on the barrier. Main thread will also wait */
	mythread_barrier_wait(&barrier);
	mythread_exit(NULL);	
}

int main(int argc, char **argv)
{	
	int i;
	char buffer[1024];
	futex_init(&printFutex, 1);
	mythread_t mythread1[NTHREADS];
	mythread_t signalingThread[NTHREADS/2];
	mythread_setconcurrency(4);
	mythread_mutex_init(&mutex,NULL);
	mythread_cond_init(&condition,NULL);
	mythread_barrier_init(&barrier,NULL,NTHREADS+1);
	
	/* Create Threads */
	for(i=0;i<NTHREADS;i++){
		sprintf(buffer, "Created thread : %d\n", i+1);
		printToConsole(buffer);
		mythread_create(&mythread1[i],NULL,&thread_func, NULL);
	}
	
	/*Signal threads waiting on cond var*/
	while(count<NTHREADS){
		/* Special case for testing broadcast*/
		if(count == NTHREADS/2){
			mythread_cond_broadcast(&condition);
		}else{
			mythread_cond_signal(&condition);
		}
	}
	
	/* Waiting on barrier. Last thread, or this main thread will unblock the barrier depending on the execution sequence */
	mythread_barrier_wait(&barrier);
	sprintf(buffer, "Out of barrier, main thread exiting..\n");
	printToConsole(buffer);
	/* Destroy mutex, barrier and cond*/
	mythread_cond_destroy(&condition);
        mythread_mutex_destroy(&mutex);
	mythread_barrier_destroy(&barrier);
	sprintf(buffer, "Finished Execution\n");
	printToConsole(buffer);
}
