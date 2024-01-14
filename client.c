// What client can do:
// -> send message with type and priority
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include "msg/types.h"

msg_type mtype;
int msgid;

void send_to_server() {

}

// -> receive message in synchronious or asynchronious way
// synchronious: on demand
// asynchronious: automatically

void receive_from_server() {

}

// The client tries to establish a connection with the server.

void establish_connection() {
  // asking the server for clients number
  mtype = REGISTER_REQUEST;
  msgid = msgget(0x123, 0600|IPC_CREAT);
  if(msgid == -1) {
    perror("getting queue");
    exit(-1);
  }
  msg_buf mbuf;
  sprintf(mbuf.text, "%s", "connecting");
  mbuf.type = mtype;
  printf("sending message to the server: %s\n", mbuf.text);
  int res = msgsnd(msgid, &mbuf, strlen(mbuf.text) + 1, 0);
  if(res == -1) {
    perror("sending");
    exit(-1);
  }
  printf("message sent to the server with result: %d\n", res);

}

void create_client_channel() {
  // fedback from the server
  mtype = CLIENTS_NUMBER;
  msg_buf mbuf;
  printf("getting info about clients number\n");
  msgrcv(msgid, &mbuf, sizeof(mbuf), mtype, 0);
  printf("%s\n", mbuf.text);
  int clients_number;
  sscanf(mbuf.text, "%d", &clients_number);
  printf("received a number of clients: %i\n", clients_number);

  // opening own channel for upcoming messages with unique id
  key_t ch = ftok("channel-key", clients_number); //TODO: to actually open that queue
  int own_queue = msgget(ch, 0600 | IPC_CREAT);
  printf("created own channel: ");
  printf("%i\n", ch);

  // announcing the landing on the server 
  // sprintf(mbuf.text, "%d", ch);
  // mbuf.type = 5;
  //TODO: including in landing message for the server the id of self-created channel (which will be treated as a client id)
  // msgsnd(mid, &m, strlen(m.text) + 1, 0);
}

void subscribe(){

}

int main(int argc, char *argv[])
{
  establish_connection();
  create_client_channel();
  return 0;
}
