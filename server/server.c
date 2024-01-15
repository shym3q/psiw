#include <string.h>
#include <stdio.h>
#include <sys/msg.h>
#include <unistd.h>
#include "../msg/types.h"
#include "../lib/utils.h"

// The client is responsible for monitoring new connections and storing credentials of upcoming users in the database

msg_type mtype;
int msgid, cn = 0;

void handle_request();
void send_clients_number();
void await_client_id();
void await_client_topic();

int main(int argc, char *argv[])
{
  // opening a message queue for clients "owned" by the server
  msgid = msgget(0x123, 0600|IPC_CREAT);
  if(msgid == -1)
    panic("cannot connect to the server queue");

  // the main loop of the server
  for(;;) {
    handle_request();
  }
  msgctl(msgid, IPC_RMID, NULL);
  return 0;
}

void handle_request() {
  mtype = GENERIC;
  pingbuf pbuf;

  if(msgrcv(msgid, &pbuf, sizeof(pbuf), mtype, 0) == -1)
    panic("cannot handle upcoming requests");

  switch(pbuf.type) {
  case REGISTER_REQUEST:
    send_clients_number();
    await_client_id();
    break;
  case SUBSCRIBE_TOPIC:
    await_client_topic();
    break;
  default:
    panic("invalid (so far) request received");
  }
}

void send_clients_number() {
  mtype = CLIENTS_NUMBER;
  dec_msgbuf cmbuf = {mtype, ++cn};
  printf("sending the number of clients: %d\n", cmbuf.i);
  if(msgsnd(msgid, &cmbuf, sizeof(int), 0) == -1)
    panic("cannot send the number of clients");
}

void await_client_id() {
  mtype = CLIENT_ID;
  dec_msgbuf mbuf;
  printf("waiting for a client's id\n");
  if(msgrcv(msgid, &mbuf, sizeof(mbuf), mtype, 0) == -1)
    panic("cannot receive upcoming client's id");

  printf("received the client's id: %d\n", mbuf.i);
}

void await_client_topic() {
  mtype = SUBSCRIBE_TOPIC;
  t_msgbuf ctmbuf;
  printf("waiting for a client's subscription topic\n");
  if(msgrcv(msgid, &ctmbuf, sizeof(ctmbuf), mtype, 0) == -1)
    panic("cannot receive upcoming client's subscription topic\n");

  printf("received the client's (%i) subscription topic: %s\n", ctmbuf.cmsg.id, ctmbuf.cmsg.text);
}
