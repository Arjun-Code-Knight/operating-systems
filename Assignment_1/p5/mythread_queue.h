/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
/*
* mythread_queue.h : Header for queue Function
*/

#include <mythread.h>

extern void queue_initialization(struct mythread_queue* queue,mythread_t* tcBlock);
extern mythread_t* queue_search_elementbyId(struct mythread_queue* queue, unsigned long );
extern struct mythread_queue_node* queue_search_element(struct mythread_queue* queue, unsigned long );
extern void queue_add_element(struct mythread_queue* queue,mythread_t* tcBlock);
extern void queue_del_element(struct mythread_queue* queue,mythread_t* tcBlock);
extern void printdebug(struct mythread_queue* queue);

