#ifndef CLIENT_OBJECT_H
#define CLIENT_OBJECT_H

#include <stdlib.h>
#include "../lib/msg.h"

struct Client {
  int server_msqid, client_msqid, channel_id;
  key_t client_key;
  char name[NAME_MAX_LENGTH], topic[TOPIC_MAX_LENGTH], msg[MSG_MAX_LENGTH];
};

struct Client *new_client() {
  struct Client *c = (struct Client*)malloc(sizeof(struct Client));
  return c;
}

#endif // !CLIENT_OBJECT_H
