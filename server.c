#include <string.h>
#include <stdio.h>
#include <sys/msg.h>
#include <unistd.h>
#include "msg/types.h"
#include "lib/utils.h"

// The client is responsible for monitoring new connections and storing credentials of upcoming users in the database

struct{} clients[100];

msg_type mtype;
int msgid, cn = 0;

void handle_request();
void send_clients_number();
void await_client_id();

int main(int argc, char *argv[])
{
  // opening a message queue for clients "owned" by the server
  msgid = msgget(0x123, 0600|IPC_CREAT);
  if(msgid == -1)
    panic("cannot connect to the server queue");

  // the main loop
  for(;;) {
    handle_request();
  }
  msgctl(msgid, IPC_RMID, NULL);
  return 0;
}

void handle_request() {
  mtype = UNKNOWN;
  msg_buf mbuf;

  if(msgrcv(msgid, &mbuf, sizeof(mbuf), mtype, 0) == -1)
    panic("cannot handle upcoming requests");

  switch(mbuf.type) {
  case REGISTER_REQUEST:
    send_clients_number();
    await_client_id();
    break;
  default:
    panic("invalid (so far) request received");
  }
}

void send_clients_number() {
  mtype = CLIENTS_NUMBER;
  msg_buf cmbuf = {mtype};
  cn++;
  sprintf(cmbuf.text, "%d", cn);
  printf("sending the number of clients: %s\n", cmbuf.text);
  if(msgsnd(msgid, &cmbuf, strlen(cmbuf.text) + 1, 0) == -1)
    panic("cannot send the number of clients");
}

void await_client_id() {
  mtype = CLIENT_ID;
  msg_buf mbuf;
  printf("waiting for a client's id\n");
  if(msgrcv(msgid, &mbuf, sizeof(mbuf), mtype, 0) == -1)
    panic("cannot receive upcoming client's id");

  printf("received the client's id: %s\n", mbuf.text);
}
