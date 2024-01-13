// User list
#include <stdio.h>
#include <sys/msg.h>
struct{} clients[100];

typedef struct msgbuf {
  long type;
  char text[1024];
} msg;

// The client is responsible for monitoring new connections and storing credentials of upcoming users in the database

void register_new_client() {

}

void send_to_clients() {

}

void receive_from_client() {

}

int main(int argc, char *argv[])
{
  int mid = msgget(0x123, 0600 | IPC_CREAT);
  msg m;
  msgrcv(mid, &m, sizeof(m), 5, 0);
  printf("%s\n", m.text);
  msgctl(mid, IPC_RMID, NULL);
  return 0;
}
