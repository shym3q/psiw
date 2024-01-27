#ifndef CLIENT_HANDLERS_H
#define CLIENT_HANDLERS_H

#include <sys/msg.h>
#include "object.h"
#include "../lib/utils.h"

// The client tries to establish a connection with the server.

void establish_connection(struct Client *c) {
  // asking the server for a number of the clients
  c->server_msqid = msgget(0x123, 0600|IPC_CREAT);
  if(c->server_msqid == -1) 
    panic("cannot connect to the server's queue");
  
  PingBuf mbuf = {REGISTER_REQUEST};
  if(msgsnd(c->server_msqid, &mbuf, 0, 0) == -1)
    panic("cannot send the connection request");
}

// based on the number of the clients received from the server, client generate id and opens queue with it

int create_client_queue(struct Client *c) {
  // fedback from the server
  DecimalMsgBuf mbuf;
  msgrcv(c->server_msqid, &mbuf, sizeof(mbuf), CLIENTS_NUMBER, 0);
  c->channel_id = mbuf.i;

  // opening an own channel for upcoming messages with unique id
  key_t ch = ftok("/tmp", mbuf.i);
  c->client_msqid = msgget(ch, 0600 | IPC_CREAT);

  return ch;
}

// sends to the server a username and id

void send_client_credentials(struct Client *c) {
  TextMsgBuf mbuf = {CLIENT_ID, {c->client_key}};
  sprintf(mbuf.cmsg.text, "%s", c->name);
  if(msgsnd(c->server_msqid, &mbuf, sizeof(mbuf.cmsg), 0) == -1)
    panic("cannot send the client's credentials");
}

// sends to the server a topic user wants to join

void subscribe(struct Client *c) {
  PingBuf pbuf = {SUBSCRIBE_TOPIC};
  if(msgsnd(c->server_msqid, &pbuf, 0, 0) == -1)
    panic("cannot ping the server");

  TextMsgBuf tmbuf = {SUBSCRIBE_TOPIC, {c->client_key}};
  sprintf(tmbuf.cmsg.text, "%s", c->topic);
  if(msgsnd(c->server_msqid, &tmbuf, sizeof(tmbuf.cmsg), 0) == -1)
    panic("cannot send the subscription topic");

  DecimalMsgBuf cmbuf;
  msgrcv(c->server_msqid, &cmbuf, sizeof(cmbuf), CHANNEL_ID, 0);
  c->channel_id = cmbuf.i;
}

void send_msg(struct Client *c) {
  PingBuf pmbuf = {CLIENT_MSG};
  if(msgsnd(c->server_msqid, &pmbuf, 0, 0) == -1)
    panic("cannot ping the server");

  TextMsgBuf mbuf = {CLIENT_MSG, {c->client_key, c->channel_id}};
  sprintf(mbuf.cmsg.text, "%s", c->msg);
  sprintf(mbuf.cmsg.name, "%s", c->name);
  if(msgsnd(c->server_msqid, &mbuf, sizeof(mbuf.cmsg), 0) == -1)
    panic("cannot send the message");
}

// takes input as user credentials

void get_user_data(struct Client *c) {
  printf("Enter your name: ");
  scanf("%s", c->name);
  printf("Enter a topic you want to subscribe: ");
  scanf("%s", c->topic);

  // there is currently a strange behaviour of the "fgets" function that it reads an empty string after the above code execution
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF);
}

#endif // !CLIENT_HANDLERS_H
