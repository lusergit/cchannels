#include "queue.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

/* Channels and their operations, essentially making queue.h thread
   safe */

#ifndef CHANNELS_H
#define CHANNELS_H

#define CHANNEL(TYPE)							\
  /* Init queues of that type */					\
  QUEUE(TYPE);								\
									\
  typedef struct TYPE##_channel {					\
    TYPE##_queue *messages;						\
    pthread_mutex_t *lock;						\
    sem_t *sem;								\
  } TYPE##_channel;							\
									\
  void TYPE##_del_channel(TYPE##_channel* c) {				\
    pthread_mutex_destroy(c->lock);					\
    sem_destroy(c->sem);						\
    TYPE##_del_queue(c->messages);					\
    free(c);								\
  }									\
									\
  void TYPE##_channel_init(TYPE##_channel *c) {				\
    c->messages = (TYPE##_queue *)malloc(sizeof(TYPE##_queue));		\
    c->lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));	\
    c->sem = (sem_t *)malloc(sizeof(sem_t));				\
    TYPE##_queue_init(c->messages);					\
    pthread_mutex_init(c->lock, NULL);					\
    sem_init(c->sem, 0, 1);						\
  }									\
  									\
  TYPE##_channel *TYPE##_new_channel(void) {				\
    TYPE##_channel* c = (TYPE##_channel*) malloc (sizeof(TYPE##_channel)); \
    TYPE##_channel_init(c);						\
    return c;								\
  }									\
									\
  typedef struct TYPE##_send_args {					\
    TYPE##_channel* c;							\
    TYPE *value;							\
  } TYPE##_send_args;							\
  									\
  void* TYPE##_psend(void *args) {					\
    TYPE##_send_args* targs = args;					\
    int res = 1;							\
    /* Try to write on queue, res = 1 means allocation error */		\
    while(res) {							\
      /* write on queue */						\
      pthread_mutex_lock((targs->c)->lock);				\
      res = TYPE##_push((targs->c)->messages, targs->value);		\
      pthread_mutex_unlock((targs->c)->lock);				\
    }									\
    /* signal semaphore that a new message is in queue */		\
    sem_post((targs->c)->sem);						\
    /* targs are temp and must be destroyed */				\
    free(targs);							\
    pthread_exit(0);							\
  }									\
  									\
  /* send a message on channel C. void since it guarantees that the	\
     message will eventually be written in the queue */			\
  void TYPE##_snd(TYPE##_channel* c, TYPE* v) {				\
    struct TYPE##_send_args* args =					\
      (struct TYPE##_send_args *)malloc(sizeof(struct TYPE##_send_args)); \
    args->value = v;							\
    args->c = c;							\
    pthread_t* ID = (pthread_t*)malloc(sizeof(pthread_t));		\
    /* Run primitive send in the same thread */				\
    pthread_create(ID, NULL, TYPE##_psend, args);			\
  }									\
  									\
  /* Recieve a message from channel C. Returns a pointer to the		\
     message */								\
  TYPE* TYPE##_rcv (TYPE##_channel* c) {				\
    TYPE *value = NULL;							\
    while(!value) {							\
      sem_wait(c->sem);							\
      pthread_mutex_lock(c->lock);					\
      value = TYPE##_pop(c->messages);					\
      pthread_mutex_unlock(c->lock);					\
    }									\
    return value;							\
  }									\

#endif /* CHANNELS_H */
