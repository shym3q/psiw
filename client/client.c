// What client can do:
// -> send message with type and priority

#include <unistd.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include "../lib/msg.h"
#include "../lib/utils.h"

int server_msqid, client_msqid, channel_id;
key_t client_id;
char name[NAME_MAX_LENGTH], topic[TOPIC_MAX_LENGTH], msg[MSG_MAX_LENGTH];

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
  client_id = create_client_queue();
  signal(SIGINT, client_exit);
  send_client_credentials();
  subscribe();

  pid_t pid = fork();
  if (pid == -1)
    panic("cannot launch the client");

  if(pid == 0) {
    for(;;) {
      TextMsgBuf tmbuf;  
      if(msgrcv(client_msqid, &tmbuf, sizeof(tmbuf), CLIENT_MSG, 0) == -1)
        panic("cannot receive the message");
      printf("%d: %s\n", tmbuf.cmsg.id, tmbuf.cmsg.text);
    }
  } else {
    for(;;) {
      fgets(msg, sizeof(msg), stdin);
      send_msg();
    }
  }

  return 0;
}

// The client tries to establish a connection with the server.

void establish_connection() {
  // asking the server for a number of the clients
  server_msqid = msgget(0x123, 0600|IPC_CREAT);
  if(server_msqid == -1) 
    panic("cannot connect to the server's queue");
  
  PingBuf mbuf = {REGISTER_REQUEST};
  if(msgsnd(server_msqid, &mbuf, 0, 0) == -1)
    panic("cannot send the connection request");
}

// based on the number of the clients received from the server, client generate id and opens queue with it

int create_client_queue() {
  // fedback from the server
  DecMsgBuf mbuf;
  msgrcv(server_msqid, &mbuf, sizeof(mbuf), CLIENTS_NUMBER, 0);
  channel_id = mbuf.i;

  // opening an own channel for upcoming messages with unique id
  key_t ch = ftok("/tmp", mbuf.i);
  client_msqid = msgget(ch, 0600 | IPC_CREAT);

  return ch;
}

// sends to the server a username and id

void send_client_credentials() {
  TextMsgBuf mbuf = {CLIENT_ID, {client_id}};
  sprintf(mbuf.cmsg.text, "%s", name);
  if(msgsnd(server_msqid, &mbuf, sizeof(mbuf.cmsg), 0) == -1)
    panic("cannot send the client's credentials");
}

// sends to the server a topic user wants to join

void subscribe() {
  PingBuf pbuf = {SUBSCRIBE_TOPIC};
  if(msgsnd(server_msqid, &pbuf, 0, 0) == -1)
    panic("cannot ping the server");

  TextMsgBuf tmbuf = {SUBSCRIBE_TOPIC, {client_id}};
  sprintf(tmbuf.cmsg.text, "%s", topic);
  if(msgsnd(server_msqid, &tmbuf, sizeof(tmbuf.cmsg), 0) == -1)
    panic("cannot send the subscription topic");

  DecMsgBuf cmbuf;
  msgrcv(server_msqid, &cmbuf, sizeof(cmbuf), CHANNEL_ID, 0);
  channel_id = cmbuf.i;
}

// takes input as user credentials

void get_user_data() {
  printf("Enter your name:");
  scanf("%s", name);
  printf("Enter a topic you want to subscribe:");
  scanf("%s", topic);

  // there is currently a strange behaviour of the "fgets" function that it reads an empty string after the above code execution
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

void send_msg() {
  PingBuf pmbuf = {CLIENT_MSG};
  if(msgsnd(server_msqid, &pmbuf, 0, 0) == -1)
    panic("cannot ping the server");

  TextMsgBuf mbuf = {CLIENT_MSG, {client_id, channel_id}};
  sprintf(mbuf.cmsg.text, "%s", msg);
  if(msgsnd(server_msqid, &mbuf, sizeof(mbuf.cmsg), 0) == -1)
    panic("cannot send the message");
}

void client_exit(int signum) {
  msgctl(client_msqid, IPC_RMID, NULL);
  exit(0);
}
