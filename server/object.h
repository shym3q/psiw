#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <sys/msg.h>
#include "../msg/types.h"

struct server {
  int msgid, cn, tn; 
};

struct server *new_server() {
  struct server *s = (struct server*)malloc(sizeof(struct server));
  // opening a message queue for clients "owned" by the server
  s->msgid = msgget(0x123, 0600|IPC_CREAT);
  s->cn = 0;
  s->tn = 0;
  return s;
}

#endif // !OBJECT_H
