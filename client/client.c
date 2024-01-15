// What client can do:
// -> send message with type and priority

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "../msg/types.h"
#include "../lib/utils.h"

msg_type mtype;
int smsgid, cmsgid;
key_t cid;

// The client tries to establish a connection with the server.

void establish_connection() {
  // asking the server for a number of the clients
  mtype = REGISTER_REQUEST;
  smsgid = msgget(0x123, 0600|IPC_CREAT);
  if(smsgid == -1) 
    panic("cannot connect to the server's queue");
  
  pingbuf mbuf = {mtype};
  printf("pinging the server\n");
  if(msgsnd(smsgid, &mbuf, 0, 0) == -1)
    panic("cannot send the connection request");
}

int create_client_channel() {
  // fedback from the server
  mtype = CLIENTS_NUMBER;
  dec_msgbuf mbuf;
  msgrcv(smsgid, &mbuf, sizeof(mbuf), mtype, 0);
  printf("received a number of clients: %i\n", mbuf.i);

  // opening an own channel for upcoming messages with unique id
  key_t ch = ftok("channel-key", mbuf.i);
  cmsgid = msgget(ch, 0600 | IPC_CREAT);
  printf("created own channel: %i\n", ch);

  return ch;
}

void send_client_id() {
  mtype = CLIENT_ID;
  dec_msgbuf mbuf = {mtype, cid};
  printf("sending client id: %d\n", mbuf.i);
  if(msgsnd(smsgid, &mbuf, sizeof(key_t), 0) == -1)
    panic("cannot send the client's id");
}

void subscribe() {
  mtype = SUBSCRIBE_TOPIC;
  pingbuf mbuf = {mtype};
  printf("pinging the server\n");
  if(msgsnd(smsgid, &mbuf, 0, 0) == -1)
    panic("cannot ping the server");

  t_msgbuf tmbuf = {mtype, {cid, "i love rust"}};
  if(msgsnd(smsgid, &tmbuf, sizeof(tmbuf.cmsg), 0) == -1)
    panic("cannot send the subscription topic");
  printf("the subscription topic send\n");
}

int main(int argc, char *argv[])
{
  establish_connection();
  cid = create_client_channel();
  send_client_id();
  subscribe();
  if(msgctl(cmsgid, IPC_RMID, NULL) == -1)
    panic("cannot close the client's queue");

  return 0;
}
