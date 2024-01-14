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
  smsgid = msgget(0x123, 0600|IPC_CREAT);
  if(smsgid == -1) 
    panic("cannot connect to the server queue");
  
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
  printf("getting info about clients number\n");
  msgrcv(smsgid, &mbuf, sizeof(mbuf), mtype, 0);
  printf("%s\n", mbuf.text);
  int clients_number;
  sscanf(mbuf.text, "%d", &clients_number);
  printf("received a number of clients: %i\n", clients_number);

  // opening an own channel for upcoming messages with unique id
  key_t ch = ftok("channel-key", clients_number); //TODO: to actually open that queue
  cmsgid = msgget(ch, 0600 | IPC_CREAT);
  printf("created own channel: ");
  printf("%i\n", ch);

  // announcing the landing on the server 
  // sprintf(mbuf.text, "%d", ch);
  // mbuf.type = 5;
  //TODO: including in landing message for the server the id of self-created channel (which will be treated as a client id)
  // msgsnd(mid, &m, strlen(m.text) + 1, 0);
  return ch;
}

void send_client_id() {
  mtype = CLIENT_ID;
  msg_buf mbuf = {mtype};
  sprintf(mbuf.text, "%d", cid);
  if(msgsnd(smsgid, &mbuf, strlen(mbuf.text) + 1, 0) == -1)
    panic("cannot send the client id");
}

void subscribe(){

}

int main(int argc, char *argv[])
{
  establish_connection();
  cid = create_client_channel();
  if(msgctl(cmsgid, IPC_RMID, NULL) == -1)
    panic("cannot close the client's queue");

  return 0;
}
