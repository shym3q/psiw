#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "handlers.h"
#include "../lib/utils.h"

struct Client *c;

void client_exit(int);

void client_init() {
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

  if(c->is_async) {
    if(pid == 0) {
      for(;;) {
        TextMsgBuf tmbuf;  
        if(msgrcv(c->client_msqid, &tmbuf, sizeof(tmbuf), CLIENT_MSG, 0) == -1)
          panic("cannot receive the message");
        printf("%s: %s\n", tmbuf.cmsg.name, tmbuf.cmsg.text);
      }
    } else {
      for(;;) {
        fgets(c->msg, MSG_MAX_LENGTH, stdin);
        send_msg(c);
      }
    }
  } else {
    for(;;) {
      scanf("%s", c->msg);
      if(strcmp(":s", c->msg) == 0) {
        TextMsgBuf tmbuf;
        msgrcv(c->client_msqid, &tmbuf, sizeof(tmbuf), CLIENT_MSG, 0);
        printf("%s: %s\n", tmbuf.cmsg.name, tmbuf.cmsg.text);
      } else {
        send_msg(c);
      }
    }
  }
}

void client_exit(int signum) {
  msgctl(c->client_msqid, IPC_RMID, NULL);
  free(c);
  putchar('\n');
  exit(0);
}
