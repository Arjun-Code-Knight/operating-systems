/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
/*
 *
 * My Scheduler handling scheduling based on priority and pre-emption count
 *
 */

#include "mythread.h"
#include "futex.h"
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <syscall.h>
#include <string.h>
#include "myqueue.h"
#include "mysched.h"
#include <math.h>

#define NOT_SIGNAL_RUNQ_SIGUSR1 4
#define DEFAULT_ATTR_VALUE 1
#define BLOCKED 8
#define SIGNAL_RUNQ_SIGUSR1 2

struct sigaction sig_act;
struct sigaction oldsig_act;
sigset_t sigmask;
sigset_t oldsigmask;
static int timer_value_initialized = 0;

int power(int value){
  int i;	
  int pow = 1;
  for(i=0; i<value; i++){
    pow *= 2;
  }
  return pow;
}

int checkReadyQ() {
	if (*mythread_readyq() != NULL)
		return 1;
	else
		return 0;
}

void signal_handler(int signal) {
	
	mythread_t self = mythread_self();
	self->attr.attr -= 1;
	if(self->attr.attr > 0){
		/* Timeslice not yet expired, signal other threads and exit */
		self->reschedule = 0;
		mythread_t self_tid = mythread_self();
		mythread_queue_t runq_head = *mythread_runq();
		mythread_queue_t head = runq_head;
		do {
			mythread_t current_qitem_tid = ((mythread_t) runq_head->item);
			if (current_qitem_tid->reschedule == 1 && current_qitem_tid != self_tid
					&& !(current_qitem_tid->state & BLOCKED)) {
				kill(current_qitem_tid->tid, SIGUSR1);//send signal SIGUSR1
			}
			runq_head = runq_head->next;
		} while (runq_head != head);
		return;
	}
		
	if (signal == SIGALRM)//SIGALRM is recieved,so need to send signals to all threads
		self->state |= SIGNAL_RUNQ_SIGUSR1;
	else
		self->state |= NOT_SIGNAL_RUNQ_SIGUSR1;// try and pre-empt urself as its not a SIGALRM

	self->reschedule = 1;//setting it to 1,reset if we get scheduled or priority of self is high
	mythread_queue_t* run_q = mythread_runq();

	if (mythread_tryenter_kernel() == TRUE) {//am inside the kernel, so can pre-empt myself
		mythread_queue_t readq = mythread_readyq();		
		void* item = readq->item;			
		
		/*signal all threads,so as to pre-empt myself incase,I dont get blocked and other threads for this time splice ALRM*/
		internal_scheduler();
		if (checkReadyQ() && !(self->state & BLOCKED) && mythread_inq(run_q, self)) {
			/* Set new timeslice and increment preemptions */
			mythread_t head_q_tid = ((mythread_t) item);
			self->reschedule = 0;
			self->preemptions +=1;
			if(self->preemptions < 10){
				self->attr.attr = power(self->preemptions);
				/* Check for lower priority item in the readyq */
				if (get_priority(head_q_tid) <= get_priority(self)) {
					mythread_unblock(mythread_readyq(),BLOCKED);
					mythread_block(mythread_readyq(), BLOCKED);
				} else {
					mythread_leave_kernel_nonpreemptive();
				}
			}
		} else {
			mythread_leave_kernel_nonpreemptive();
		}
	}
	
	/*If not inside the kernel,signal other run-q threads which mite have to be rescheduled,as it could not pre-empt
	OR had no threads in ready Q*/
	mythread_t self_tid = mythread_self();
	mythread_queue_t runq_head = *mythread_runq();
	mythread_queue_t head = runq_head;
	do {
		mythread_t current_qitem_tid = ((mythread_t) runq_head->item);
		if (current_qitem_tid->reschedule == 1 && current_qitem_tid != self_tid
				&& !(current_qitem_tid->state & BLOCKED)) {
			kill(current_qitem_tid->tid, SIGUSR1);//send signal SIGUSR1
		}
		runq_head = runq_head->next;
	} while (runq_head != head);
}

/* Called once by each Thread - init_sched */
void mythread_init_sched() {
	/* Initializing attr for 1 timeslice */
	mythread_t tcb = mythread_self();
	mythread_attr_init(&tcb->attr);
	memset(&sig_act, '\0', sizeof(sig_act));
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGALRM);
	sigaddset(&sigmask, SIGUSR1);
	sigprocmask(SIG_UNBLOCK, &sigmask, &oldsigmask);
	sig_act.sa_handler = signal_handler;
	sig_act.sa_flags = SA_RESTART;

	//Add SIGALRM-to sigaction//
	if (sigaction(SIGALRM, &sig_act, &oldsig_act) == -1) {
		printf("Registering SIGALRM handler ERROR\n");
		exit(-1);
	}

	//Add SIGUSR1-to sigaction//
	if (sigaction(SIGUSR1, &sig_act, &oldsig_act) == -1) {
		printf("Registering SIGUSR1 handler ERROR\n");
		exit(-1);
	}

	struct timeval timer_time;
	struct itimerval timer;

	if (timer_value_initialized == 0) {
		timer_value_initialized = 1;
		timer_time.tv_sec = 0;
		timer_time.tv_usec = 1000; // Timer to 1ms timeslice
		timer.it_interval = timer_time;
		timer.it_value = timer_time;
		setitimer(ITIMER_REAL, &timer, NULL);//schedules based on real time and not based on clock tics
	}
}


/**
 *
 * Leave kernel and call runq to release all threads which needs to be re-scheduled
 *
 */
void mythread_leave_kernel() {
	mythread_leave_kernel_nonpreemptive();//call non-premeptive leave kernel
	/* Send SIGUSR1 to all threads in the runq */
	mythread_t self_tid = mythread_self();
	mythread_queue_t runq_head = *mythread_runq();
	mythread_queue_t head = runq_head;
	do {
		mythread_t current_qitem_tid = ((mythread_t) runq_head->item);
		if (current_qitem_tid->reschedule == 1 && current_qitem_tid != self_tid
				&& !(current_qitem_tid->state & BLOCKED)) {
			kill(current_qitem_tid->tid, SIGUSR1);//send signal SIGUSR1
		}
		runq_head = runq_head->next;
	} while (runq_head != head);
}


/*
 * Destroy the attributes of the thread
 */
int mythread_attr_destroy(mythread_attr_t *attribute) {
	if (attribute == NULL) {
		return -1;
	}
	attribute->attr = DEFAULT_ATTR_VALUE;
	return 0;
}

/*
 * Initialize the attributes of the thread
 */
int mythread_attr_init(mythread_attr_t *attribute) {
	if (attribute == NULL) {
		return -1;
	}
	attribute->attr = DEFAULT_ATTR_VALUE;
	return 0;
}


void mythread_exit_sched() {
	
	if (sigaction(SIGUSR1, &oldsig_act, &sig_act) == -1) {
		printf("Error in removing the SIGUSR1 handler\n");
		exit(-1);
	}

	/* Restore old SIGALRM handler */
	if (sigaction(SIGALRM, &oldsig_act, &sig_act) == -1) {
		printf("Error in removing the SIGALRM handler\n");
		exit(-1);
	}

	/*Unblock the signal mask with old mask*/
	sigprocmask(SIG_SETMASK, &oldsigmask, &sigmask);
}

/*
 * Called when the thread exits the handler, since we need to check which thread needs to be re-scheduled for every time splice.
 * This is called for every SIGALRM received.
 * */
void internal_scheduler()
{
	mythread_queue_t ptr, head;
	mythread_t self = mythread_self();
	if (self->state & SIGNAL_RUNQ_SIGUSR1) {//SIGALRM but couldnt pre-empt myself,so signal all threads
		head = *mythread_runq();
		ptr = head;
		do {
			if (ptr == NULL) {
				break;
			}
			if (self != ptr->item) {
				kill(((mythread_t) ptr->item)->tid, SIGUSR1);
			}
			ptr = ptr->next;
		} while (ptr != head);
	}

	/* Reset all signal states */
	self->state &= ~SIGNAL_RUNQ_SIGUSR1;//gets reset
	self->state &= ~NOT_SIGNAL_RUNQ_SIGUSR1;//gets reset
}
