#ifndef HANDLERS_H
#define HANDLERS_H

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
  if(msgrcv(s->msgid, &pbuf, sizeof(pbuf), GENERIC, 0) == -1)
    panic("cannot handle upcoming requests");

  // the child's protocol
  struct Prot imsg;

  signal(SIGALRM, handle_timeout);
  alarm(1);
  switch(pbuf.type) {
    case REGISTER_REQUEST:
      imsg.cn = send_clients_number(s);
      await_client_credentials(s, &imsg);
      imsg.type = REGISTER_REQUEST;
      break;
    case SUBSCRIBE_TOPIC:
      await_client_topic(s, &imsg);
      imsg.type = SUBSCRIBE_TOPIC;
      break;
    case CLIENT_MSG:
      await_client_msg(s, db);
      imsg.type = CLIENT_MSG;
      break;
    default:
      panic("invalid (so far) request received");
  }
  alarm(0);
  InternalMsgBuf imsgbuf = {INTERNAL, imsg};
  msgsnd(s->msgid, &imsgbuf, sizeof(imsgbuf.imsg), 0);
  exit(0);
}

// sends the number of the clients so the recipent can generate a unique identifier

int send_clients_number(struct Server *s) {
  printf("increasing clients number from: %d\n", s->cn);
  DecMsgBuf cmbuf = {CLIENTS_NUMBER, ++s->cn};
  printf("sending the number of clients: %d\n", cmbuf.i);
  if(msgsnd(s->msgid, &cmbuf, sizeof(int), 0) == -1)
    panic("cannot send the number of clients");
  return s->cn;
}

void await_client_credentials(struct Server *s, struct Prot *imsg) {
  TextMsgBuf mbuf;
  printf("waiting for a client's credentials\n");
  if(msgrcv(s->msgid, &mbuf, sizeof(mbuf), CLIENT_ID, 0) == -1)
    panic("cannot receive upcoming client's credentials");
  printf("received the client's credentials: %d, %s\n", mbuf.cmsg.id, mbuf.cmsg.text);
  sprintf(imsg->name, "%s", mbuf.cmsg.text);
  imsg->cid = mbuf.cmsg.id;
}

void await_client_topic(struct Server *s, struct Prot *imsg) {
  TextMsgBuf ctmbuf;
  printf("waiting for a client's subscription topic\n");
  if(msgrcv(s->msgid, &ctmbuf, sizeof(ctmbuf), SUBSCRIBE_TOPIC, 0) == -1)
    panic("cannot receive upcoming client's subscription topic\n");
  printf("received the client's (%i) subscription topic: %s\n", ctmbuf.cmsg.id, ctmbuf.cmsg.text);
  sprintf(imsg->topic, "%s", ctmbuf.cmsg.text);
  imsg->cid = ctmbuf.cmsg.id;
}

// void send_client_channel(struct server)

void await_client_msg(struct Server *s, struct Database *db) {
  TextMsgBuf mbuf;
  if(msgrcv(s->msgid, &mbuf, sizeof(mbuf), CLIENT_MSG, 0) == -1)
    panic("cannot receive the message");
  printf("received the message: %s", mbuf.cmsg.text);
  distribute_msg(&mbuf.cmsg, db);
}

#endif // !HANDLERS_H
