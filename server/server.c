#include <stdio.h>
#include <sys/msg.h>
#include "../msg/types.h"
#include "../lib/utils.h"

// The client is responsible for monitoring new connections and storing credentials of upcoming users in the database

msg_type mtype;
int msgid, cn = 0;

void handle_request();
void send_clients_number();
void await_client_credentials();
void await_client_topic();
void await_client_msg();

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

// awaits a ping from any client then serves it

void handle_request() {
  mtype = GENERIC;
  pingbuf pbuf;

  if(msgrcv(msgid, &pbuf, sizeof(pbuf), mtype, 0) == -1)
    panic("cannot handle upcoming requests");

  switch(pbuf.type) {
  case REGISTER_REQUEST:
    send_clients_number();
    await_client_credentials();
    break;
  case SUBSCRIBE_TOPIC:
    await_client_topic();
    break;
  case CLIENT_MSG:
    await_client_msg();
    break;
  default:
    panic("invalid (so far) request received");
  }
}

// sends the number of the clients so the recipent can generate a unique identifier

void send_clients_number() {
  mtype = CLIENTS_NUMBER;
  dec_msgbuf cmbuf = {mtype, ++cn};
  printf("sending the number of clients: %d\n", cmbuf.i);
  if(msgsnd(msgid, &cmbuf, sizeof(int), 0) == -1)
    panic("cannot send the number of clients");
}

void await_client_credentials() {
  mtype = CLIENT_ID;
  t_msgbuf mbuf;
  printf("waiting for a client's credentials\n");
  if(msgrcv(msgid, &mbuf, sizeof(mbuf), mtype, 0) == -1)
    panic("cannot receive upcoming client's credentials");
  printf("received the client's credentials: %d, %s\n", mbuf.cmsg.id, mbuf.cmsg.text);
}

void await_client_topic() {
  mtype = SUBSCRIBE_TOPIC;
  t_msgbuf ctmbuf;
  printf("waiting for a client's subscription topic\n");
  if(msgrcv(msgid, &ctmbuf, sizeof(ctmbuf), mtype, 0) == -1)
    panic("cannot receive upcoming client's subscription topic\n");

  printf("received the client's (%i) subscription topic: %s\n", ctmbuf.cmsg.id, ctmbuf.cmsg.text);
}

void await_client_msg() {
  t_msgbuf mbuf;
  if(msgrcv(msgid, &mbuf, sizeof(mbuf), mtype, 0) == -1)
    panic("cannot receive the message");
  printf("received the message: %s\n", mbuf.cmsg.text);
}
