// What client can do:
// -> send message with type and priority

#include <stdio.h>
#include <sys/msg.h>
#include "../msg/types.h"
#include "../lib/utils.h"

msg_type mtype;
int smsgid, cmsgid;
key_t cid;
char uname[14], topic[14], cmsg[40];

void establish_connection();
void get_user_data();
void send_client_credentials();
void subscribe();
void send_msg();
int create_client_channel();

int main(int argc, char *argv[]) {
  get_user_data();
  establish_connection();
  cid = create_client_channel();
  send_client_credentials();
  subscribe();

  for(;;) {
    fgets(cmsg, sizeof(cmsg), stdin);
    send_msg();
  }

  if(msgctl(cmsgid, IPC_RMID, NULL) == -1)
    panic("cannot close the client's queue");

  return 0;
}

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

// based on the number of the clients received from the server, client generate id and opens queue with it

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

// sends to the server a username and id

void send_client_credentials() {
  mtype = CLIENT_ID;
  t_msgbuf mbuf = {mtype, {cid}};
  sprintf(mbuf.cmsg.text, "%s", uname);
  printf("sending client credentials: %d, %s\n", mbuf.cmsg.id, mbuf.cmsg.text);
  if(msgsnd(smsgid, &mbuf, sizeof(mbuf.cmsg), 0) == -1)
    panic("cannot send the client's credentials");
}

// sends to the server a topic user wants to join

void subscribe() {
  mtype = SUBSCRIBE_TOPIC;
  pingbuf mbuf = {mtype};
  printf("pinging the server\n");
  if(msgsnd(smsgid, &mbuf, 0, 0) == -1)
    panic("cannot ping the server");

  t_msgbuf tmbuf = {mtype, {cid}};
  sprintf(tmbuf.cmsg.text, "%s", topic);
  if(msgsnd(smsgid, &tmbuf, sizeof(tmbuf.cmsg), 0) == -1)
    panic("cannot send the subscription topic");
  printf("the subscription topic sent\n");
}

// takes input as user credentials

void get_user_data() {
  printf("Enter your name: ");
  scanf("%s", uname);
  printf("Enter a topic you want to subscribe: ");
  scanf("%s", topic);
}

void send_msg() {
  mtype = CLIENT_MSG;
  pingbuf pmbuf = {mtype};
  if(msgsnd(smsgid, &pmbuf, 0, 0) == -1)
    panic("cannot ping the server");

  t_msgbuf mbuf = {mtype, {cid}};
  sprintf(mbuf.cmsg.text, "%s", cmsg);
  if(msgsnd(smsgid, &mbuf, sizeof(mbuf.cmsg), 0) == -1)
    panic("cannot send the message");
}
