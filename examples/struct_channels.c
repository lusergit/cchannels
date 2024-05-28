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

typedef struct my_string {
  char* str;
  size_t size;
} my_string;

#define MAX_STR_SIZE 256
/* Yes, this does indeed copy way more memory then I need to / should
   copy... */
my_string* new_from_str(char* str) {
  my_string* new_str = (my_string*) malloc(sizeof(my_string));
  new_str->str = (char*) malloc(sizeof(char) * MAX_STR_SIZE);
  strlcpy(new_str->str, str, MAX_STR_SIZE);
  new_str->size = MAX_STR_SIZE;
  return new_str;
}

int cmp(my_string* left, my_string* right) {
  int res = 0;
  if(left->size == right->size){
    size_t size = left->size;	/* same as right->size by latter
				   comparison */
    int same = 1;
    for(int i = 0; i < size && same; i++) {
      same = left->str + i == right->str + i;
    }
    res = same;
  }
  return res;
}

CHANNEL(my_string);

typedef struct server_args {
  my_string_channel* c;
  pthread_t* pid;
} server_args ;

void* server(void* argv) {
  int exit = 0;
  server_args* args = (server_args*)argv;
  my_string_channel* c = args->c;
  pthread_t* pid = args->pid;
  /* Recive one string and print it */
  my_string* next = my_string_rcv(c);
  printf("[Server]: Recived string %s\n", next->str);
  // I'm responsible for the data I recived, I have to free it!
  if(next->str) free(next->str);
  free(next);
  pthread_exit(EXIT_SUCCESS);
}

typedef struct client_args {
  my_string_channel* c;
  pthread_t* pid;
} client_args ;

void* client(void* argv) {
  client_args* args = (client_args*)argv;
  my_string_channel* c = args->c;
  pthread_t* pid = args->pid;
  my_string* to_send = new_from_str("This is a test string!");
  printf("[Client]: Sending struct\n");
  my_string_snd(c, to_send);
  // I'm not responsible anymore for the portion of memory I've
  // sent! the reciver is!
  pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
  /* create channel */
  my_string_channel* c = my_string_new_channel();

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
  my_string_del_channel(c);

  exit(EXIT_SUCCESS);
}

