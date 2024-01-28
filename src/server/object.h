#ifndef SERVER_OBJECT_H
#define SERVER_OBJECT_H

#include <stdlib.h>
#include <sys/msg.h>
#include "../lib/msg.h"

struct Server {
  int msqid, client_number, topic_number; 
};

struct Server *new_server() {
  struct Server *s = (struct Server*)malloc(sizeof(struct Server));
  // opening a message queue for clients "owned" by the server
  s->msqid = msgget(0x123, 0600|IPC_CREAT);
  s->client_number = 0;
  s->topic_number = 0;
  return s;
}

struct ClientNode {
  int client_id, subscription_type, messages_left;
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

void remove_db(struct Database *db) {
  for(struct TopicNode *current_topic = db->topics; current_topic;) {
    for(struct ClientNode *current_client = current_topic->clients; current_client;) {
      struct ClientNode *temp_client = current_client;
      current_client = current_client->next;
      free(temp_client);
    }
    struct TopicNode *temp_topic = current_topic;
    current_topic = current_topic->next;
    free(temp_topic);
  }
}

#endif // !SERVER_OBJECT_H
