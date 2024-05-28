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

CHANNEL(int);
// from here on, int channels are available, we can
/*
  - Create a channel -> int_new_channel()
  - Destroy (annihilate) channels -> int_del_channel(c)
  - Send over a channel (non-blocking) -> int_snd(c, i)
  - Recive over a channel (blocking) -> int_rcv
*/

typedef struct server_args {
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
    else {
      printf("[Server]: Recived integer %d\n", *next);
      // I'm responsible for the data I recived, I have to free it!
      free(next);
    }
  } while(!exit);
  pthread_exit(NULL);
}

typedef struct client_args {
  int_channel* c;
  pthread_t* pid;
} client_args ;

void* client(void* argv) {
  client_args* args = (client_args*)argv;
  int_channel* c = args->c;
  pthread_t* pid = args->pid;
  for(int i=0; i<10; i++){
    int* to_send = (int*) malloc(sizeof(int));
    *to_send = (i + 1) % 10;
    printf("[Client] Sending integer %d\n", *to_send);
    int_snd(c, to_send);
    // I'm not responsible anymore for the portion of memory I've
    // sent! the reciver is!
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  /* create channel */
  int_channel* c = int_new_channel();

  /* run server */
  pthread_t* server_pid = (pthread_t*)malloc(sizeof(pthread_t));
  server_args* sargs = (server_args*)malloc(sizeof(server_args));
  sargs->pid = server_pid;
  sargs->c = c;
  pthread_create(server_pid, NULL, server, sargs);

  /* run client */
  pthread_t* client_pid = (pthread_t*)malloc(sizeof(pthread_t));
  client_args* cargs = (client_args*)malloc(sizeof(client_args));
  cargs->pid = client_pid;
  cargs->c = c;
  pthread_create(client_pid, NULL, client, cargs);

  /* delete client */
  pthread_join(*client_pid, NULL);
  printf("Client Terminated\n");
  free(cargs);
  free(client_pid);

  /* delete server */
  pthread_join(*server_pid, NULL);
  printf("Server Terminated\n");
  free(sargs);
  free(server_pid);

  /* delete channel */
  int_del_channel(c);

  exit(EXIT_SUCCESS);
}

