#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main() {
  int mid = msgget(0x123, 0600 | IPC_CREAT);
  struct msgbuf
  {
    long type;
    char text[1024];
  } my_msg;
  strcpy(my_msg.text, "Text");
  my_msg.type =5;
  msgsnd(mid, &my_msg, strlen(my_msg.text)+1, 0);
  int received = msgrcv(mid, &my_msg, 1024, 5, 0);
  printf("%i\n", received);
  msgctl(mid, IPC_RMID, NULL);   
  return 0;
}

