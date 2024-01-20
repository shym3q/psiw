#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "object.h"
#include "../lib/utils.h"

int send_clients_number(struct server*);
void await_client_credentials(struct server*, struct prot*);
void await_client_topic(struct server*);
void await_client_msg(struct server*);
// void handle_timeout(int signum);

// awaits a ping from any client then serves it

void handle_request(struct server *s) {
  pingbuf pbuf;

  // awaits for upcoming request, if the waiting takes too long the server terminates the connection
  if(msgrcv(s->msgid, &pbuf, sizeof(pbuf), GENERIC, 0) == -1)
    panic("cannot handle upcoming requests");

  // the child's protocol
  struct prot imsg;

  signal(SIGALRM, handle_timeout);
  alarm(1);
  switch(pbuf.type) {
    case REGISTER_REQUEST:
      imsg.cn = send_clients_number(s);
      await_client_credentials(s, &imsg);
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
  i_msgbuf imsgbuf = {INTERNAL, imsg};
  msgsnd(s->imsgid, &imsgbuf, sizeof(imsgbuf), 0);
  exit(0);
}

// sends the number of the clients so the recipent can generate a unique identifier

int send_clients_number(struct server *s) {
  // TODO: update the number of the clients in the parent process also
  // maybe switch to a current time as a key seed instead
  dec_msgbuf cmbuf = {CLIENTS_NUMBER, ++s->cn};
  printf("sending the number of clients: %d\n", cmbuf.i);
  if(msgsnd(s->msgid, &cmbuf, sizeof(int), 0) == -1)
    panic("cannot send the number of clients");
  return s->cn;
}

void await_client_credentials(struct server *s, struct prot *imsg) {
  t_msgbuf mbuf;
  printf("waiting for a client's credentials\n");
  if(msgrcv(s->msgid, &mbuf, sizeof(mbuf), CLIENT_ID, 0) == -1)
    panic("cannot receive upcoming client's credentials");
  printf("received the client's credentials: %d, %s\n", mbuf.cmsg.id, mbuf.cmsg.text);
  imsg->name = mbuf.cmsg.text;
  imsg->cid = mbuf.cmsg.id;
}

void await_client_topic(struct server *s) {
  t_msgbuf ctmbuf;
  printf("waiting for a client's subscription topic\n");
  if(msgrcv(s->msgid, &ctmbuf, sizeof(ctmbuf), SUBSCRIBE_TOPIC, 0) == -1)
    panic("cannot receive upcoming client's subscription topic\n");

  printf("received the client's (%i) subscription topic: %s\n", ctmbuf.cmsg.id, ctmbuf.cmsg.text);
}

void await_client_msg(struct server *s) {
  t_msgbuf mbuf;
  if(msgrcv(s->msgid, &mbuf, sizeof(mbuf), CLIENT_MSG, 0) == -1)
    panic("cannot receive the message");
  printf("received the message: %s\n", mbuf.cmsg.text);
}

#endif // !HANDLERS_H
