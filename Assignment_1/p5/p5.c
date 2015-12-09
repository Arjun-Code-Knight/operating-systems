/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#define NTHREADS 10                                                         
#include <mythread.h>                                                            
#include <stdio.h>
#include <stdlib.h>                                                         
#include <unistd.h>
#include <string.h>   
#include <sys/syscall.h>                                                         

struct futex printFutex;

/* For printing to console */
void printToConsole(char *buffer){
  futex_down(&printFutex);
  write(1, buffer, strlen(buffer));
  futex_up(&printFutex);
}
                                                                  
void *create_thread(void *arg) { 
  char  buffer [1024];
  sprintf(buffer, "Running in Thread : %d \n", (int)mythread_self().tid);
  printToConsole(buffer);  
  sprintf(buffer, "Yielding Thread : %d \n", (int)mythread_self().tid);
  printToConsole(buffer);  
  mythread_yield(); 
  sprintf(buffer, "Exiting Thread : %d \n", (int)mythread_self().tid);
  printToConsole(buffer);  
  mythread_exit(NULL);
  return NULL;                                                                       
}                                                                               


                                                                         
int main(	) {      

  mythread_t th[NTHREADS];  
  mythread_attr_t attr;
  int i;   
  futex_init(&printFutex, 1);  
  char  buffer [1024];
  mythread_attr_init(&attr);
  size_t stacksize = 8192;
  void *stack_addr;
  posix_memalign(&stack_addr, 8, stacksize);
  sprintf(buffer,"Stack addr for first thread set to %ld \n",(long)stack_addr);
  printToConsole(buffer);
  sprintf(buffer,"Stack size for first thread set to %ld \n",(unsigned long)stacksize);
  printToConsole(buffer);  
  mythread_attr_setstack(&attr, stack_addr, stacksize);
  void *stack_addr2;
  size_t stacksize2;
  mythread_attr_getstack(&attr, &stack_addr2, &stacksize2);
  sprintf(buffer,"Stack addr for first thread (getstack) %ld\n",(unsigned long)stack_addr);
  printToConsole(buffer);  
  sprintf(buffer,"Stack size for first thread (getstack) %ld\n",(unsigned long)stacksize);                                     
  printToConsole(buffer);
  sprintf(buffer,"** Creating %d Threads** \n", NTHREADS);                                     
  printToConsole(buffer);
  for (i = 0; i<NTHREADS; i++) {
    /* Passing attr to first thread */
    if(i == 0)
      mythread_create(&th[i], &attr, create_thread, NULL);
  else
    /* attr passed as NULL to other threads */
    mythread_create(&th[i], NULL, create_thread, NULL);
  sprintf(buffer,"Main thread - Created thread : %ld\n", (unsigned long)th[i].tid); 
  printToConsole(buffer);
  }

  /* Once the create is done mythread_join will be called to join all the threads which are created */
  for (i = 0; i<NTHREADS; i++) {
  sprintf(buffer,"Main thread - Waiting for thread : %ld to join.\n", (unsigned long)th[i].tid);    
  printToConsole(buffer);
  mythread_join(th[i], NULL);
  sprintf(buffer,"Main thread - Joined with thread : %ld\n", (unsigned long)th[i].tid);
  printToConsole(buffer);
  fflush(stdout);
  }   

  /* Main thread exiting */
  sprintf(buffer,"Main thread -Exiting\n");
  printToConsole(buffer);
  mythread_exit(NULL);
  return 0;                       
}  
