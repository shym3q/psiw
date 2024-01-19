#ifndef HANDLERS_H
#define HANDLERS_H

#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "object.h"
#include "../lib/utils.h"
#include "../msg/types.h"

void send_clients_number(struct server*);
void await_client_credentials(struct server*);
void await_client_topic(struct server*);
void await_client_msg(struct server*);

// awaits a ping from any client then serves it

void handle_request(struct server *s) {
  s->mtype = GENERIC;
  pingbuf pbuf;

  // awaits for upcoming request, if the waiting takes too long the server terminates the connection
  if(msgrcv(s->msgid, &pbuf, sizeof(pbuf), s->mtype, 0) == -1)
    panic("cannot handle upcoming requests");

  signal(SIGALRM, handle_timeout);
  alarm(1);
  switch(pbuf.type) {
    case REGISTER_REQUEST:
      send_clients_number(s);
      await_client_credentials(s);
      break;
    case SUBSCRIBE_TOPIC:
      await_client_topic(s);
      break;
    case CLIENT_MSG:
      await_client_msg(s);
      break;
    default:
      panic("invalid (so far) request received");
  }
  exit(0);
}

// sends the number of the clients so the recipent can generate a unique identifier

void send_clients_number(struct server *s) {
  s->mtype = CLIENTS_NUMBER;
  dec_msgbuf cmbuf = {s->mtype, ++(s->cn)};
  printf("sending the number of clients: %d\n", cmbuf.i);
  if(msgsnd(s->msgid, &cmbuf, sizeof(int), 0) == -1)
    panic("cannot send the number of clients");
}

void await_client_credentials(struct server *s) {
  s->mtype = CLIENT_ID;
  t_msgbuf mbuf;
  printf("waiting for a client's credentials\n");
  if(msgrcv(s->msgid, &mbuf, sizeof(mbuf), s->mtype, 0) == -1)
    panic("cannot receive upcoming client's credentials");
  printf("received the client's credentials: %d, %s\n", mbuf.cmsg.id, mbuf.cmsg.text);
}

void await_client_topic(struct server *s) {
  s->mtype = SUBSCRIBE_TOPIC;
  t_msgbuf ctmbuf;
  printf("waiting for a client's subscription topic\n");
  if(msgrcv(s->msgid, &ctmbuf, sizeof(ctmbuf), s->mtype, 0) == -1)
    panic("cannot receive upcoming client's subscription topic\n");

  printf("received the client's (%i) subscription topic: %s\n", ctmbuf.cmsg.id, ctmbuf.cmsg.text);
}

void await_client_msg(struct server *s) {
  s->mtype = CLIENT_MSG;
  t_msgbuf mbuf;
  if(msgrcv(s->msgid, &mbuf, sizeof(mbuf), s->mtype, 0) == -1)
    panic("cannot receive the message");
  printf("received the message: %s\n", mbuf.cmsg.text);
}

#endif // !HANDLERS_H
