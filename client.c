// What client can do:
// -> send message with type and priority
//
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg/types.h"
#include "lib/utils.h"

msg_type mtype;
int smsgid, cmsgid;
key_t cid;


// The client tries to establish a connection with the server.

void establish_connection() {
  // asking the server for clients number
  mtype = REGISTER_REQUEST;
  smsgid = msgget(0x123, 0600|IPC_CREAT);
  if(smsgid == -1) 
    panic("cannot connect to the server's queue");
  
  msg_buf mbuf = {mtype};
  sprintf(mbuf.text, "%s", "connection request");
  printf("sending message to the server: %s\n", mbuf.text);
  if(msgsnd(smsgid, &mbuf, strlen(mbuf.text) + 1, 0) == -1)
    panic("cannot send the connection request");
}

int create_client_channel() {
  // fedback from the server
  mtype = CLIENTS_NUMBER;
  msg_buf mbuf;
  msgrcv(smsgid, &mbuf, sizeof(mbuf), mtype, 0);
  int clients_number;
  sscanf(mbuf.text, "%d", &clients_number);
  printf("received a number of clients: %i\n", clients_number);

  // opening an own channel for upcoming messages with unique id
  key_t ch = ftok("channel-key", clients_number);
  cmsgid = msgget(ch, 0600 | IPC_CREAT);
  printf("created own channel: %i\n", ch);

  return ch;
}

void send_client_id() {
  mtype = CLIENT_ID;
  msg_buf mbuf = {mtype};
  sprintf(mbuf.text, "%d", cid);
  printf("%ld: sending client id: %s\n", mbuf.type, mbuf.text);
  if(msgsnd(smsgid, &mbuf, strlen(mbuf.text) + 1, 0) == -1)
    panic("cannot send the client's id");
}

void send_to_server() {
  // TODO
}

// -> receive message in synchronious or asynchronious way
// synchronious: on demand
// asynchronious: automatically

void receive_from_server() {
  // TODO
}

void subscribe(){
  // TODO
}

int main(int argc, char *argv[])
{
  establish_connection();
  cid = create_client_channel();
  send_client_id();
  if(msgctl(cmsgid, IPC_RMID, NULL) == -1)
    panic("cannot close the client's queue");

  return 0;
}
