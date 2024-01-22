#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <sys/msg.h>
#include "../lib/msg.h"

struct Server {
  int msqid, cn, tn; 
};

struct Server *new_server() {
  struct Server *s = (struct Server*)malloc(sizeof(struct Server));
  // opening a message queue for clients "owned" by the server
  s->msqid = msgget(0x123, 0600|IPC_CREAT);
  s->cn = 0;
  s->tn = 0;
  return s;
}

#endif // !OBJECT_H
