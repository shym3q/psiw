#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include "handlers.h"
#include "../lib/utils.h"

struct Client *c;

void client_exit(int);

int main(int argc, char *argv[]) {
  c = new_client();
  get_user_data(c);
  establish_connection(c);
  c->client_key = create_client_queue(c);
  signal(SIGINT, client_exit);
  send_client_credentials(c);
  subscribe(c);

  pid_t pid = fork();
  if (pid == -1)
    panic("cannot launch the client");

  if(pid == 0) {
    for(;;) {
      TextMsgBuf tmbuf;  
      if(msgrcv(c->client_msqid, &tmbuf, sizeof(tmbuf), CLIENT_MSG, 0) == -1)
        panic("cannot receive the message");
      printf("%d: %s\n", tmbuf.cmsg.id, tmbuf.cmsg.text);
    }
  } else {
    for(;;) {
      fgets(c->msg, MSG_MAX_LENGTH, stdin);
      send_msg(c);
    }
  }

  return 0;
}

void client_exit(int signum) {
  msgctl(c->client_msqid, IPC_RMID, NULL);
  exit(0);
}
