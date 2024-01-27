// The server is responsible for monitoring new connections and storing credentials of upcoming users in the database

#include "object.h"
#include "bus.h"
#include "handlers.h"
#include <stdio.h>

struct Server *s;
struct Database *db;

void server_init();
void server_exit(int);
void update_records(pid_t, struct Server*);

void server_init() {
  s = new_server();
  db = new_db();
  if(s->msqid == -1)
    panic("cannot connect the server queue");
  
  signal(SIGINT, server_exit);
  pid_t pid;
  // the main loop of the server
  for(;;) {
    pid = fork();
    if(pid == -1)
      panic("couldn't launch the crucial server process");
    if(pid == 0)
      handle_request(s, db);
    else
      update_records(pid, s);
  }
}

void server_exit(int signum) {
  msgctl(s->msqid, IPC_RMID, NULL);
  free(s);
  free(db);
  exit(0);
}

void update_records(pid_t pid, struct Server *s) {
  // waits for the child process to finish serving a client
  int wstat;
  waitpid(pid, &wstat, 0);
  if(WIFEXITED(wstat)) {
    // check the child's protocol
    InternalMsgBuf imbuf;
    msgrcv(s->msqid, &imbuf, sizeof(imbuf), INTERNAL, 0);
    switch(imbuf.imsg.type) {
      case REGISTER_REQUEST:
        s->client_number = imbuf.imsg.client_number;
        break;
      case SUBSCRIBE_TOPIC:
        int ch = get_channel_id(db, imbuf.imsg.topic);
        DecimalMsgBuf cmsgbuf = {CHANNEL_ID, ch};
        msgsnd(s->msqid, &cmsgbuf, sizeof(int), 0);
        channel_connect(db, ch, imbuf.imsg.client_id);
        break;
    }
  }
}
