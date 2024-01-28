#ifndef SERVER_HANDLERS_H
#define SERVER_HANDLERS_H

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "object.h"
#include "bus.h"
#include "../lib/utils.h"

int send_clients_number(struct Server*);
void await_client_credentials(struct Server*, struct Prot*);
void await_client_topic(struct Server*, struct Prot*);
void await_client_msg(struct Server*, struct Database*);

// awaits a ping from any client then serves it

void handle_request(struct Server *s, struct Database *db) {
  PingBuf pbuf;

  // awaits for upcoming request, if the waiting takes too long the server terminates the connection
  if(msgrcv(s->msqid, &pbuf, sizeof(pbuf), GENERIC, 0) == -1)
    panic("cannot handle upcoming requests");

  // the child's protocol
  struct Prot prot;

  signal(SIGALRM, handle_timeout);
  alarm(1);
  switch(pbuf.type) {
    case REGISTER_REQUEST:
      prot.client_number = send_clients_number(s);
      await_client_credentials(s, &prot);
      prot.type = REGISTER_REQUEST;
      break;
    case SUBSCRIBE_TOPIC:
      await_client_topic(s, &prot);
      prot.type = SUBSCRIBE_TOPIC;
      break;
    case CLIENT_MSG:
      await_client_msg(s, db);
      prot.type = CLIENT_MSG;
      break;
    default:
      panic("invalid (so far) request received");
  }
  alarm(0);
  InternalMsgBuf imsgbuf = {INTERNAL, prot};
  msgsnd(s->msqid, &imsgbuf, sizeof(imsgbuf.imsg), 0);
  exit(0);
}

// sends the number of the clients so the recipent can generate a unique identifier

int send_clients_number(struct Server *s) {
  DecimalMsgBuf mbuf = {CLIENTS_NUMBER, ++s->client_number};
  printf("sending the number of clients: %d\n", mbuf.i);
  if(msgsnd(s->msqid, &mbuf, sizeof(int), 0) == -1)
    panic("cannot send the number of clients");
  return s->client_number;
}

void await_client_credentials(struct Server *s, struct Prot *imsg) {
  TextMsgBuf mbuf;
  printf("waiting for a client's credentials\n");
  if(msgrcv(s->msqid, &mbuf, sizeof(mbuf), CLIENT_ID, 0) == -1)
    panic("cannot receive upcoming client's credentials");
  printf("received the client's credentials: %d, %s\n", mbuf.cmsg.client_id, mbuf.cmsg.text);
  sprintf(imsg->name, "%s", mbuf.cmsg.text);
  imsg->client_id = mbuf.cmsg.client_id;
}

void await_client_topic(struct Server *s, struct Prot *imsg) {
  TextMsgBuf mbuf;
  printf("waiting for a client's subscription topic\n");
  if(msgrcv(s->msqid, &mbuf, sizeof(mbuf), SUBSCRIBE_TOPIC, 0) == -1)
    panic("cannot receive upcoming client's subscription topic\n");
  printf("received the client's (%i) subscription topic: %s\n", mbuf.cmsg.client_id, mbuf.cmsg.text);
  sprintf(imsg->topic, "%s", mbuf.cmsg.text);
  imsg->client_id = mbuf.cmsg.client_id;
  imsg->subscription_type = mbuf.cmsg.subscription_type;
  imsg->msg_left = mbuf.cmsg.msg_number_requested;
}

void await_client_msg(struct Server *s, struct Database *db) {
  TextMsgBuf mbuf;
  if(msgrcv(s->msqid, &mbuf, sizeof(mbuf), CLIENT_MSG, 0) == -1)
    panic("cannot receive the message");
  printf("redirecting a new message\n");
  distribute_msg(&mbuf, db);
}

#endif // !SERVER_HANDLERS_H
