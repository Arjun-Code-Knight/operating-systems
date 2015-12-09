/* 	Author info
 *	athimma Arjun Thimmareddy
 * 
 */
 
#include "myqueue.h"
#include "mythread.h"
#include <malloc.h>
#include <stdio.h>

int mythread_inq(mythread_queue_t *headp, void *item)
{
  mythread_queue_t old, q, head;

  head = *headp;

#ifndef TRASH
  if (head == NULL)
    return FALSE;
#endif

  q = head;
  do {
    if (q->item == item)
      return TRUE; /* found */
    q = q->next;
  } while (q != head);

  return FALSE; /* not found */
}

int mythread_lenq(mythread_queue_t *headp)
{
  mythread_queue_t old, q, head;
  int ret = 0;

  head = *headp;

#ifndef TRASH
  if (head == NULL)
    return ret;
#endif

  q = head;
  do {
    q = q->next;
    ret++;
  } while (q != head);

  return ret;
}

void mythread_q_init(mythread_queue_t *headp, void *item)
{
  mythread_queue_t new, head;
  new = (mythread_queue_t) malloc(sizeof(struct mythread_queue));
  if (new == NULL) {
    printf("malloc error\n");
    exit(1);
  }
  new->item = item;

  new->prev = new;
  new->next = new;

  *headp = new;
 // print(headp, 0);
}

void print(mythread_queue_t* head, int flag)
{
    if(head == mythread_runq())
	printf("Runq : %d ",flag);
    else if(head == mythread_readyq())
	printf("Readyq : %d ",flag);
    else
	return;
    mythread_queue_t temp = *head;
    do
    {
        printf("%d->%d ",((mythread_t)temp->item)->preemptions, ((mythread_t)temp->item)->tid);
        temp = temp->next;
    } while(temp!= *head);
    printf("\n");
}

/*Enq by priority*/
void mythread_enq_tail(mythread_queue_t *headp, void *item)
{
  mythread_queue_t new, head;
  if (*headp == NULL) {
    mythread_q_init(headp, item);
    return;
  }

  head = *headp;
  mythread_t head_t = (mythread_t)head->item;
  int prio = head_t->preemptions;
  mythread_t item_t = (mythread_t)item;
  
  new = (mythread_queue_t) malloc(sizeof(struct mythread_queue));
  if (new == NULL) {
    printf("malloc error\n");
    exit(1);
  }
  int flag;
  while(prio <= item_t->preemptions){
    head = head->next;
    head_t = (mythread_t)head->item;
    prio = head_t->preemptions;
    flag = 1;
    if(head == *headp)
	break;
  }
  new->item = item;
  new->prev = head->prev;
  new->next = head;
  head->prev->next = new;
  head->prev = new;
  /*new head*/
  if(head == *headp && !flag)
	*headp = new;
//  print(headp, 1);
}


void mythread_enq_head(mythread_queue_t *headp, void *item)
{
  mythread_queue_t new, head;

  if (*headp == NULL) {
    mythread_q_init(headp, item);
    return;
  }

  head = *headp;

  mythread_enq_tail(&head->next, item);

  *headp = head->prev;
}

void mythread_deq(mythread_queue_t *headp, void *item)
{
  mythread_queue_t old, q, head;
 
  head = *headp;

  q = head;
  do {
    if (q->item == item) {
      old = q;
      old->next->prev = q->prev;
      old->prev->next = q->next;
      if (old == head) {
	if (old->next == old)
	  *headp = (mythread_queue_t) NULL;
	else
	  *headp = old->next;
      }
      free(old);
      //if(headp != NULL)
	//      print(headp);
      return; /* found */
    }
    q = q->next;
  } while (q != head);

  /* not found */
}

int get_priority(mythread_t tid) {
	return tid->preemptions;
}
