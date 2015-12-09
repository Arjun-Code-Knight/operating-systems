/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include <string.h>
#include <mythread.h>
#include <mythread_queue.h>
#include <stdlib.h>

/* Initialize queue for first time */
void queue_initialization(struct mythread_queue* queue,mythread_t* tcBlock){
	/* Create a queuenode to hold values */
	struct mythread_queue_node* newNode = malloc(sizeof(struct mythread_queue_node));
	newNode->currentThread = tcBlock;
	newNode->previousNode = newNode;
	newNode->nextNode = newNode;
	queue->headNode = newNode;
	queue->tailNode = newNode;
}


/* Search the element(Thread) from the queue */
mythread_t* queue_search_elementbyId(struct mythread_queue* queue,unsigned long threadId) {
	/* Create a queuenode to hold values */
	struct mythread_queue_node *tempNode;
	if (queue->headNode != NULL && queue->tailNode != NULL) {
		tempNode = queue->headNode;
		do {//traverse to the last node in queue
			if (tempNode->currentThread->tid == threadId){
				return tempNode->currentThread;
			}
			else
				tempNode = tempNode->nextNode;
		} while (tempNode != queue->headNode);

	}
	return NULL;
}


/* Search the element(node) from the queue */
struct mythread_queue_node* queue_search_element(struct mythread_queue* queue,unsigned long threadId) {
	/* Create a queuenode to hold values */
	struct mythread_queue_node *tempNode;
	if (queue->headNode != NULL && queue->tailNode != NULL) {
		tempNode = queue->headNode;
		do {    /* traverse to the last node in queue */
			if (tempNode->currentThread->tid == threadId)
				return tempNode;
			else
				tempNode = tempNode->nextNode;
		} while (tempNode != queue->headNode);

	}
	return NULL;
}

/* Add element(Thread) to the queue */
void queue_add_element(struct mythread_queue* queue,mythread_t* tcBlock){
	if (queue->headNode == NULL || queue->tailNode == NULL) {
		queue_initialization(queue,tcBlock);
		return;
	}
	struct mythread_queue_node* newNode = malloc(sizeof(struct mythread_queue_node));
	newNode->currentThread = tcBlock;
	newNode->previousNode = queue->tailNode;
	newNode->nextNode = queue->tailNode->nextNode;
	queue->tailNode->nextNode = newNode;
	queue->tailNode = newNode;
	queue->headNode->previousNode = queue->tailNode;
	return;
}

