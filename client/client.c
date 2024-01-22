// What client can do:
// -> send message with type and priority

#include <unistd.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include "../lib/msg.h"
#include "../lib/utils.h"

int smsgid, cmsgid, chid;
key_t cid;
char uname[14], topic[14], cmsg[20];

void establish_connection();
void get_user_data();
void send_client_credentials();
void subscribe();
void send_msg();
void client_exit(int);
int create_client_queue();

int main(int argc, char *argv[]) {
  get_user_data();
  establish_connection();
  cid = create_client_queue();
  signal(SIGINT, client_exit);
  send_client_credentials();
  subscribe();

  pid_t pid = fork();
  if (pid == -1)
    panic("cannot launch the client");
  if(pid == 0) {
    for(;;) {
      printf("waiting for the messages\n");
      TextMsgBuf tmbuf;  
      if(msgrcv(cmsgid, &tmbuf, sizeof(tmbuf), CLIENT_MSG, 0) == -1)
        panic("cannot receive the message");
      printf("%d: %s\n", tmbuf.cmsg.id, tmbuf.cmsg.text);
    }
  } else {
    for(;;) {
      printf("waiting for the input\n");
      fgets(cmsg, sizeof(cmsg), stdin);
      send_msg();
    }
  }

  return 0;
}

// The client tries to establish a connection with the server.

void establish_connection() {
  // asking the server for a number of the clients
  smsgid = msgget(0x123, 0600|IPC_CREAT);
  if(smsgid == -1) 
    panic("cannot connect to the server's queue");
  
  PingBuf mbuf = {REGISTER_REQUEST};
  printf("pinging the server\n");
  if(msgsnd(smsgid, &mbuf, 0, 0) == -1)
    panic("cannot send the connection request");
}

// based on the number of the clients received from the server, client generate id and opens queue with it

int create_client_queue() {
  // fedback from the server
  DecMsgBuf mbuf;
  msgrcv(smsgid, &mbuf, sizeof(mbuf), CLIENTS_NUMBER, 0);
  printf("received a number of clients: %i\n", mbuf.i);
  chid = mbuf.i;

  // opening an own channel for upcoming messages with unique id
  key_t ch = ftok("/tmp", mbuf.i);
  cmsgid = msgget(ch, 0600 | IPC_CREAT);
  printf("created own channel: %i\n", ch);

  return ch;
}

// sends to the server a username and id

void send_client_credentials() {
  TextMsgBuf mbuf = {CLIENT_ID, {cid}};
  sprintf(mbuf.cmsg.text, "%s", uname);
  printf("sending client credentials: %d, %s\n", mbuf.cmsg.id, mbuf.cmsg.text);
  if(msgsnd(smsgid, &mbuf, sizeof(mbuf.cmsg), 0) == -1)
    panic("cannot send the client's credentials");
}

// sends to the server a topic user wants to join

void subscribe() {
  PingBuf pbuf = {SUBSCRIBE_TOPIC};
  printf("pinging the server\n");
  if(msgsnd(smsgid, &pbuf, 0, 0) == -1)
    panic("cannot ping the server");

  TextMsgBuf tmbuf = {SUBSCRIBE_TOPIC, {cid}};
  sprintf(tmbuf.cmsg.text, "%s", topic);
  if(msgsnd(smsgid, &tmbuf, sizeof(tmbuf.cmsg), 0) == -1)
    panic("cannot send the subscription topic");
  printf("the subscription topic sent\n");

  DecMsgBuf cmbuf;
  msgrcv(smsgid, &cmbuf, sizeof(cmbuf), CHANNEL_ID, 0);
  chid = cmbuf.i;
  printf("setting up the channel id: %d\n", chid);
}

// takes input as user credentials

void get_user_data() {
  printf("Enter your name:\n");
  scanf("%s", uname);
  printf("Enter a topic you want to subscribe:\n");
  scanf("%s", topic);
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

void send_msg() {
  printf("attemting to send the message\n");
  PingBuf pmbuf = {CLIENT_MSG};
  if(msgsnd(smsgid, &pmbuf, 0, 0) == -1)
    panic("cannot ping the server");

  TextMsgBuf mbuf = {CLIENT_MSG, {cid, chid}};
  sprintf(mbuf.cmsg.text, "%s", cmsg);
  if(msgsnd(smsgid, &mbuf, sizeof(mbuf.cmsg), 0) == -1)
    panic("cannot send the message");
}

void client_exit(int signum) {
  msgctl(cmsgid, IPC_RMID, NULL);
  exit(0);
}
