// User list
#include <stdio.h>
#include <sys/msg.h>
struct{} clients[100];

typedef struct msgbuf {
  long type;
  char text[1024];
} msg;

int register_new_client() {
  return msgget(0x123, 0600 | IPC_CREAT);
}

void send_to_clients() {

}

void receive_from_client() {

}

int main(int argc, char *argv[])
{
  int mid = register_new_client();
  msg m;
  msgrcv(mid, &m, sizeof(m), 5, 0);
  printf("%s\n", m.text);
  msgctl(mid, IPC_RMID, NULL);
  return 0;
}
