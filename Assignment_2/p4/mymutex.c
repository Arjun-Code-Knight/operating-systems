/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include "mythread.h"
#include "mymutex.h"

int mythread_mutex_init(mythread_mutex_t *mutex, const mythread_mutexattr_t *attr)
{
	/*Initialize the mutex structure*/
	mutex =(mythread_mutex_t*)malloc(sizeof(mythread_mutex_t));
	mutex->lock=0;
	mutex->no_of_blocked_threads=0;
	mutex->attr=attr;
	mutex->owner_thread=NULL;
	mutex->blocked_thread_q=NULL;
	return 0;
}

int extended_TTSLock(int *lock)
{
	int max_number_of_tries=50;
	int number_of_retries=0;
	int old_Value;

	while(number_of_retries<max_number_of_tries){ // Try 50 times as the late back off scenario
		while(*lock==1);//wait until the lock becomes available, as other thread has to release the lock
		old_Value=compare_and_swap(lock,1,0);//newvalue=1,oldvalue=0, on success return oldvalue=0
		if(old_Value==0){
			return old_Value;//At success scenario
		}
		else{
			number_of_retries++;//increase the retry count
		}

	}
	return 1;//retun failure
}

int mythread_mutex_lock(mythread_mutex_t *mutex)
{
	int resultLockStatus = 1;
	if(mutex->lock==-1)//the lock value be chnaged to -1, when destroy was called
	{
		return -1;//return invalid scenario
	}
	while(resultLockStatus != 0){
		if(mutex->owner_thread == NULL)
		{
			resultLockStatus = extended_TTSLock(&mutex->lock);//try acquiring the TTS lock
			if(resultLockStatus == 0)//lock acquired by the thread,success scenario
			{
				mythread_enter_kernel();
				mutex->owner_thread = mythread_self();//get the current thread running
				mythread_leave_kernel();
				continue;
			}

		continue;
		}
		if(mutex->owner_thread != NULL && (mutex->no_of_blocked_threads <= 0))
		{
			resultLockStatus = extended_TTSLock(&mutex->lock);//Late back off scenario,as there is only one thread running
			while(resultLockStatus == 1)//lock is not acquired,try acquiring the lock again
			{
				mythread_enter_kernel();
				mutex->no_of_blocked_threads=mutex->no_of_blocked_threads+1;
				mythread_block(&mutex->blocked_thread_q,0);//since lock is not acquired, blocking the thread with status 0
				mythread_leave_kernel();
				resultLockStatus = extended_TTSLock(&mutex->lock);
				if(resultLockStatus == 0){
					mythread_enter_kernel();
					mutex->owner_thread = mythread_self();//get the current thread running
					mythread_leave_kernel();
				}
			}
			mutex->owner_thread = mythread_self();//get the current thread running
			continue;
		}

		if(mutex->owner_thread != NULL && mutex->no_of_blocked_threads > 0)//early back off scenario
		{
			mythread_enter_kernel();
			mutex->no_of_blocked_threads=mutex->no_of_blocked_threads+1;
			mythread_block(&(mutex->blocked_thread_q),0);//since lock is not acquired, blocking the thread with status 0
			mythread_leave_kernel();
			resultLockStatus = extended_TTSLock(&mutex->lock);
			if(resultLockStatus == 0){
				mythread_enter_kernel();
				mutex->owner_thread = mythread_self();//get the current thread running
				mythread_leave_kernel();
				continue;
			}
		continue;
		}
	}

	return resultLockStatus;
}

int mythread_mutex_unlock(mythread_mutex_t *mutex)
{
	int resultStatus;

	if(mutex->lock==-1){
		return 1;//mutex is destroyed
	}

	resultStatus=compare_and_swap(&mutex->lock,0,1);//lock,newValue=0,oldValue=1

	while(resultStatus==0){//failed to set the lock back to 0
		resultStatus=compare_and_swap(&mutex->lock,0,1);
	}

	mythread_enter_kernel();
	mutex->owner_thread=NULL;
	if(mutex->no_of_blocked_threads > 0){
		mutex->no_of_blocked_threads=mutex->no_of_blocked_threads-1;
		mythread_unblock(&mutex->blocked_thread_q,1);
	}
	mythread_leave_kernel();
	return 0;//return success
}

int mythread_mutex_destroy(mythread_mutex_t *mutex)
{
	int resultStatus;
	mythread_enter_kernel();
	int currentLockVal = mutex->lock;
	resultStatus=compare_and_swap(&(mutex->lock),-1,mutex->lock);//lock,newValue=-1,oldValue=current_lock_value
	while(resultStatus != currentLockVal){
		resultStatus=compare_and_swap(&(mutex->lock),-1,mutex->lock);//lock,newValue=-1,oldValue=0
	}
	mutex->owner_thread=NULL;
	mutex->attr = NULL;
	mutex->no_of_blocked_threads = 0;
	mutex->blocked_thread_q = NULL;
	mythread_leave_kernel();
	return 0;
}

