#include "object.h"
#include "pubsub/bus.h"
#include "handlers.h"
#include <stdio.h>

// The server is responsible for monitoring new connections and storing credentials of upcoming users in the database

struct server *s;
struct database *db;

void server_exit(int);
void update_records(pid_t, struct server*);

int main(int argc, char *argv[]) {
  // opening a message queue for clients "owned" by the server
  s = new_server();
  db = new_db();
  if(s->msgid == -1)
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
    else
      update_records(pid, s);
  }
  return 0;
}

void server_exit(int signum) {
  msgctl(s->msgid, IPC_RMID, NULL);
  free(s);
  free(db);
  exit(0);
}

void update_records(pid_t pid, struct server *s) {
  // waits for the child process to finish serving the clients
  int wstat;
  waitpid(pid, &wstat, 0);
  if(WIFEXITED(wstat)) {
    // check the child's protocol
    i_msgbuf imbuf;
    msgrcv(s->msgid, &imbuf, sizeof(imbuf), INTERNAL, 0);
    printf("received the protocol with type: %ld\n", imbuf.type);
    switch(imbuf.imsg.type) {
      case REGISTER_REQUEST:
        s->cn = imbuf.imsg.cn;
        printf("the protocol content: %s, %i\n", imbuf.imsg.name, imbuf.imsg.cid);
        break;
      case SUBSCRIBE_TOPIC:
        printf("the protocol content: %s\n", imbuf.imsg.topic);
        int chid = get_channel(db, imbuf.imsg.topic);
        channel_connect(db, chid, imbuf.imsg.cid);
        overview(db);
        printf("we've got %i clients", s->cn);
        break;
      case CLIENT_MSG:
      default:
        printf("an error occured\n");
        break;
    }
  }
}
