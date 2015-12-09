/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
Idea is to implement a synchronization primitives for Non preemptive user level thread library mythread.a with the following methods

mythread_mutex_init
	It will initialize the mutex structure that we defined in the mymutex.h file. It even keep track of number of blocked threads which is again a queue inside the structure.
it will take two parameters, one is pointer to the mutex struc itself, which will be malloced in the init method, next is the pointer to the attr struct.

mythread_mutex_lock
	This will lock the mutex based on the following condition:
if it is the first thread, which is calling the mutex for itself and if the owner thread is null, it will try to acquire lock 50 times, if it is a success it will set the current thread as owner thread.
if this condition fails, it will goto next condition, if the owner thread is filled and if the number of blocked threads is less than or equal to 0(late back off), it will again try to acquire the lock 50 times, if it is a success, it will increase the number of blocked threads in the mutex by 1, and it will add that thread to the queue by calling mythread_block, if it is a fail then we will check for one more condition where we will do the same check again where will check for number of threads is greater than 1, this if for early back off.
mythread_mutex_lock will internally set the value of lock value in mutex as 1	

mythread_mutex_unlock
	This will unlock the thread for other process to use, and will set the lock value of mutex as 0, and will also reduce the number of blocked threads by 1.
And it will also call mythread_unlock which will remove the entry from the blocked_threads_queue as well

mythread_mutex_destroy
	It will destroy the initialization we did in init.

mythread_cond_init
	It will initialize the structure that we created in mycond.h, it has a count to keep track of number of blocked threads and a queue which holds the number of blocked threads.

mythread_cond_wait
	It will do a conditional wait on the current thread which hold the mutex. It will call the mythread_block_phase1 function with the blocked threads queue. it will then unlock the mutex, and if the state of the current thread is not 0, then we have to call mythread_block_phase2

mythread_cond_signal
	It will reduce the number of blocked threads by 1, and it will call the mutex unlock which will take care of removal of entries fromm the queue as well

mythread_cond_broadcast
	It is similar to signal but it will wake up all the threads in the queue

mythread_cond_destroy
	It will destroy all the intialization we did in init

mythread_barrier_init
	It will initialize the structure that we used for barrier. it has again the number of blocked threads and blocked threads queue, it has one more parameter which is the number of threads that 
needs to be blocked in blocked thread queue, when we call wait

mythread_barrier_wait
	if thread wait count is reached , it will unblock all blocked threads on the barrier else block the thread

mythread_barrier_destroy
	It will destroy all the intialization we did in init
*********************************************

Building the file using make file

compiling,cleaning and creation of libraries can be done by running the statement 'make' in the terminal.
once it is compiled succesfully, make file will create executable file "mytest" for testing.
For testing we need to run the command ./mytest which will run the test program mytest.c and show the output in the console

*********************************************

Testing synchronization primitives

It is done by running the program mytest.c.
It has already predefined the number of threads which is going to get created in the test program.  It will run and test all the synchronization functionalities.

*********************************************

