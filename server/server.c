#include "object.h"
#include "handlers.h"

// The server is responsible for monitoring new connections and storing credentials of upcoming users in the database

struct server *s;

void server_exit(int);

int main(int argc, char *argv[]) {
  // opening a message queue for clients "owned" by the server
  s = new_server();
  if(s->msgid == -1)
    panic("cannot connect to the server queue");
  
  signal(SIGINT, server_exit);
  // the main loop of the server
  for(;;) {
    if (fork() == 0) handle_request(s);
    else wait(NULL);
  }
  // msgctl(msgid, IPC_RMID, NULL);
  return 0;
}

void server_exit(int signum) {
  msgctl(s->msgid, IPC_RMID, NULL);
  free(s);
  exit(0);
}
