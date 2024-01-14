// User list
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>
#include "msg/types.h"

struct{} clients[100];

msg_type mtype;

int clients_number = 0;

// The client is responsible for monitoring new connections and storing credentials of upcoming users in the database

void register_new_client() {

}

void send_to_clients() {

}

void receive_from_client() {

}

int main(int argc, char *argv[])
{
  // opening a message queue for clients "owned" by the server
  int msgid = msgget(0x123, 0600|IPC_CREAT);
  if(msgid == -1) {
    perror("connecting to message queue");
    exit(-1);
  }

  msg_buf mbuf;
  mtype = REGISTER_REQUEST;
  printf("waiting for a client\n");
  if(msgrcv(msgid, &mbuf, sizeof(mbuf), mtype, 0) == -1) {
    perror("receiving");
    exit(-1);
  }
  printf("new client arrived\n");

  clients_number++;
  printf("increasing number of clients\n");
  mtype = CLIENTS_NUMBER;
  msg_buf cmbuf;
  sprintf(cmbuf.text, "%d", clients_number);
  cmbuf.type = mtype;
  printf("sending number of clients: %s\n", cmbuf.text);
  if(msgsnd(msgid, &cmbuf, strlen(cmbuf.text) + 1, 0) == -1) {
    perror("sending number of clients");
    exit(-1);
  }

  // msgrcv(mid, &m, sizeof(m), 5, 0);
  // printf("%s\n", m.text);
  msgctl(msgid, IPC_RMID, NULL);
  return 0;
}
