#include <stdio.h>
#include <stdlib.h>

/* Queue with its operations, call QUEUE(type) to build the queue
   operations and structures for that type. */

#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE(TYPE)							\
  /* Node type */							\
  typedef struct TYPE##_node {						\
    TYPE* data;								\
    struct TYPE##_node *next;						\
  } TYPE##_node;							\
									\
  /* Recursively delete nodes of a linked list, but DO NOT delete the	\
     data it points to, it was created outside of the library and as	\
     such I'm not responsible for it !*/				\
  void TYPE##_del_node(TYPE##_node* n) {				\
    if(n) {								\
      TYPE##_del_node(n->next);						\
      free(n);								\
    }									\
  }									\
									\
  /* Queue type */							\
  typedef struct TYPE##_queue {						\
    TYPE##_node *front;							\
    TYPE##_node *rear;							\
  } TYPE##_queue;							\
									\
  void TYPE##_del_queue(TYPE##_queue* q) {				\
    TYPE##_del_node(q->front); /* Delete the queue */			\
    free(q); /* Delete the struct */					\
  }									\
									\
  /* INIT queue */							\
  void TYPE##_queue_init(TYPE##_queue *q) {				\
    q->front = q->rear = NULL;						\
  }									\
									\
  TYPE##_queue* TYPE##_new_queue() {					\
    TYPE##_queue* q = (TYPE##_queue*) malloc (sizeof(TYPE##_queue));	\
    TYPE##_queue_init(q);						\
    return q;								\
  }									\
									\
  /* enqueue operation FIFO mode */					\
  /* 0 if ok, 1 -> error */						\
  int TYPE##_push(TYPE##_queue *q, TYPE* value) {			\
    TYPE##_node *new_node = (TYPE##_node *)malloc(sizeof(TYPE##_node));	\
    if (!new_node) {							\
      fprintf(stderr, "Memory allocation error\n");			\
      return 1;								\
    }									\
    new_node->data = value;						\
    new_node->next = NULL;						\
    if (q->rear == NULL) {						\
      q->front = q->rear = new_node;					\
    } else {								\
      q->rear->next = new_node;						\
      q->rear = new_node;						\
    }									\
    return 0;								\
  }									\
									\
  /* Dequeue, also FIFO mode */						\
  TYPE* TYPE##_pop(TYPE##_queue *q) {					\
    if (!(q->front)) {							\
      /* No error message, just return null, callee responsability */	\
      return NULL;							\
    }									\
    TYPE##_node *temp = q->front;					\
    TYPE* value = temp->data;						\
    q->front = q->front->next;						\
    									\
    if (q->front == NULL) {						\
      q->rear = NULL;							\
    }									\
    return value;							\
  }									\

#endif /* QUEUE_H */
