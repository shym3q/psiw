#include "object.h"
#include "pubsub/bus.h"
#include "handlers.h"

// The server is responsible for monitoring new connections and storing credentials of upcoming users in the database

struct server *s;
struct node *db;

void server_exit(int);

int main(int argc, char *argv[]) {
  // opening a message queue for clients "owned" by the server
  s = new_server();
  db = new_db();
  if(s->msgid == -1 || s->imsgid == -1)
    panic("cannot connect to the server queue");
  
  signal(SIGINT, server_exit);
  pid_t pid;
  // the main loop of the server
  for(;;) {
    pid = fork();
    if(pid == -1)
      panic("couldn't launch the server");
    if(pid == 0)
      handle_request(s);
    else {
      int wstat;
      waitpid(pid, &wstat, 0);
      if(WIFEXITED(wstat)) {
        // check the child's protocol
        i_msgbuf ibuf;
        msgrcv(s->imsgid, &ibuf, sizeof(ibuf), INTERNAL, 0);
        printf("received the protocol with status: %ld\n", ibuf.type);
      }
    }
  }
  return 0;
}

void server_exit(int signum) {
  msgctl(s->msgid, IPC_RMID, NULL);
  msgctl(s->imsgid, IPC_RMID, NULL);
  free(s);
  exit(0);
}
