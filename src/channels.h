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
  typedef struct TYPE##_channel {						\
    TYPE##_queue *messages;						\
    pthread_mutex_t *lock;						\
    sem_t *sem;								\
  } TYPE##_channel;							\
									\
  void del_channel(TYPE##_channel* c) {					\
    pthread_mutex_destroy(c->lock);					\
    sem_destroy(c->sem);						\
    TYPE##_queue_del(c->messages);					\
    free(c);								\
  }									\
									\
  void TYPE##_channel_init(TYPE##_channel *c) {				\
    c->messages = (TYPE##_queue *)malloc(sizeof(TYPE##_queue));		\
    c->lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));	\
    c->sem = (sem_t *)malloc(sizeof(sem_t));				\
    TYPE##_queue_init(c->messages);					\
    pthread_mutex_init(c->lock, NULL);					\
    sem_init(c->sem, 0, 0);						\
  }									\
									\
  TYPE##_channel* TYPE##_new_channel() {				\
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
    struct TYPE##_send_args* targs = args;				\
    /* write on queue */						\
    pthread_mutex_lock((targs->c)->lock);					\
    TYPE##_push((targs->c)->messages, targs->value);			\
    pthread_mutex_unlock((targs->c)->lock);				\
    /* signal semaphore that a new message is in queue */		\
    sem_post((targs->c)->sem);						\
    return NULL;							\
  }									\
  									\
  /* send a message on channel C. void since it guarantees that the	\
     message will eventually be written in the queue */			\
  void TYPE##_snd(TYPE##_channel* c, TYPE * v) {			\
    struct TYPE##_send_args* args =					\
      (struct TYPE##_send_args *)malloc(sizeof(struct TYPE##_send_args)); \
    args->c = c;							\
    args->value = v;							\
    pthread_t thread;							\
    /* Run primitive send in the same thread */				\
    pthread_create(&thread, NULL, TYPE##_psend, (void *)&args);		\
  }									\
  									\
  /* Recieve a message from channel C. Returns a pointer to the		\
     message */								\
  TYPE* TYPE##_rcv (TYPE##_channel* c) {				\
    TYPE *value = NULL;							\
    do {								\
      sem_wait(c->sem);							\
      pthread_mutex_lock(c->lock);					\
      value = TYPE##_pop(c->messages);					\
      pthread_mutex_unlock(c->lock);					\
    } while (!value);							\
    return value;							\
  }									\

#endif /* CHANNELS_H */
