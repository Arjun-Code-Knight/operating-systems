/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "mythread.h"
#include "mysched.h"

#define NTHREADS 10

void* thread_function(void* arg) {
	int argument = *(int*) arg;
	int i = 0;
	while (i<10) {
		int count = 0;
		while(count < 10000000){
			count++;
		}
		mythread_enter_kernel();
		printf("%d<-counter | Thread %d->%d\n", i,argument, mythread_self()->tid);
		mythread_leave_kernel();
		i++;
	}
}

int main(int argc, char* argv[]) {
	int i = 0;
	mythread_t threads[NTHREADS];
	int count[NTHREADS];

	mythread_setconcurrency(1);
	
	for(i = 0; i < NTHREADS; i++) {
		count[i] = i;
		mythread_create(&threads[i], NULL, thread_function, &count[i]);
	}
	for (i = 0; i < NTHREADS; i++) {
		mythread_join(threads[i], NULL);
	}
	printf("Main thread exiting\n");
        return 0;
}

