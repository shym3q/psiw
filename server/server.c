#include "object.h"
#include "bus.h"
#include "handlers.h"
#include <stdio.h>

// The server is responsible for monitoring new connections and storing credentials of upcoming users in the database

struct Server *s;
struct Database *db;

void server_exit(int);
void update_records(pid_t, struct Server*);

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
      handle_request(s, db);
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

void update_records(pid_t pid, struct Server *s) {
  // waits for the child process to finish serving the clients
  int wstat;
  waitpid(pid, &wstat, 0);
  if(WIFEXITED(wstat)) {
    // check the child's protocol
    InternalMsgBuf imbuf;
    msgrcv(s->msgid, &imbuf, sizeof(imbuf), INTERNAL, 0);
    switch(imbuf.imsg.type) {
      case REGISTER_REQUEST:
        s->cn = imbuf.imsg.cn;
        break;
      case SUBSCRIBE_TOPIC:
        int ch = get_channel(db, imbuf.imsg.topic);
        DecMsgBuf cmsguf = {CHANNEL_ID, ch};
        msgsnd(s->msgid, &cmsguf, sizeof(int), 0);
        channel_connect(db, ch, imbuf.imsg.cid);
        break;
      default:
        printf("no protocol received\n");
    }
  }
}
