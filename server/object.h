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

struct ClientNode {
  int client_id;
  struct ClientNode *next;
};

struct TopicNode {
  int topic_id;
  char topic_name[MSG_MAX_LENGTH];
  struct ClientNode *clients;
  struct TopicNode *next;
};

struct Database {
  int topic_count;
  struct TopicNode *topics;
};

struct Database *new_db() {
  struct Database *db = (struct Database*)malloc(sizeof(struct Database));
  db->topic_count = 0;
  return db;
}

#endif // !OBJECT_H
