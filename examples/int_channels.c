/* local libs */
#include "../src/channels.h"

/* external libs */
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#define handle_error_en(en, msg)				\
  do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg)				\
  do { perror(msg); exit(EXIT_FAILURE); } while (0)

// from here on, int channels are available, we can
/*
  - Create a channel
 */
CHANNEL(int);

typedef struct server_args
{
  int_channel* c;
  pthread_t* pid;
} server_args ;

void* server(void* argv) {
  int exit = 0;
  server_args* args = (server_args*)argv;
  int_channel* c = args->c;
  pthread_t* pid = args->pid;
  do {
    int* next = int_rcv(c);
    if(!next || *next == 0)
      exit = 1;
    else
      printf("[Server]: Recived integer %d\n", *next);
  } while(!exit);
  pthread_exit(NULL);
}

typedef struct client_args
{
  int_channel* c;
  pthread_t* pid;
} client_args ;

void* client(void* argv) {
  client_args* args = (client_args*)argv;
  int_channel* c = args->c;
  pthread_t* pid = args->pid;
  int sequence[10] = {1,2,3,4,5,6,7,8,9,0};
  for(int i=0; i<10; i++){
    printf("[Client] Sending integer %d\n", sequence[i]);
    int_snd(c, sequence[i]);
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int_channel* c = int_new_channel();

  /* Run server */
  pthread_t* server_pid = (pthread_t*)malloc(sizeof(pthread_t));
  server_args* sargs = (server_args*)malloc(sizeof(server_args));
  sargs->pid = server_pid;
  sargs->c = c;
  pthread_create(server_pid, NULL, server, sargs);


  /* Run client */
  pthread_t* client_pid = (pthread_t*)malloc(sizeof(pthread_t));
  client_args* cargs = (client_args*)malloc(sizeof(client_args));
  cargs->pid = client_pid;
  cargs->c = c;
  pthread_create(client_pid, NULL, client, cargs);

  pthread_join(*client_pid, NULL);
  printf("Client Terminated\n");
  pthread_join(*server_pid, NULL);
  printf("Server Terminated\n");

  return 0;
}

