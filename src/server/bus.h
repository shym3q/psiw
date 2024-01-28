#ifndef BUS_H
#define BUS_H

#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include "object.h"
#include "../lib/msg.h"

// searches the topic in the existing records, if it doesn't find it creates one

int get_channel_id(struct Database *db, char t[14]) {
  for(struct TopicNode *current = db->topics; current; current = current->next) {
    if(!strcmp(t, current->topic_name))
      return current->topic_id;
  }
  struct TopicNode *n = (struct TopicNode*)malloc(sizeof(struct TopicNode));
  n->next = db->topics;
  n->topic_id = db->topic_count;
  sprintf(n->topic_name, "%s", t);
  db->topics = n;
  return db->topic_count++;
}

// adds the client to the appropriate channel

void channel_connect(struct Database *db, int tid, int cid, int stype, int mleft) {
  struct ClientNode *n = (struct ClientNode*)malloc(sizeof(struct ClientNode));
  n->client_id = cid;
  n->subscription_type = stype;
  n->messages_left = mleft;
  for(struct TopicNode *current = db->topics; current; current = current->next) {
    if(current->topic_id == tid) {
      n->next = current->clients;
      current->clients = n;
      break;
    }
  }
}

void send_to_client(int cid, TextMsgBuf *m) {
  int cmsqid = msgget(cid, 0600|IPC_CREAT);
  msgsnd(cmsqid, m, sizeof(struct Msg), 0);
}

// redirects the captured message to the subscribers 

void distribute_msg(TextMsgBuf *m, struct Database *db) {
  for(struct TopicNode *current_topic = db->topics; current_topic; current_topic = current_topic->next) {
    if(m->cmsg.channel_id == current_topic->topic_id) {
      for(struct ClientNode *current_client = current_topic->clients; current_client; current_client = current_client->next) {
        if(current_client->client_id != m->cmsg.client_id)
          send_to_client(current_client->client_id, m);
      }
      return;
    }
  }
}

// for debugging purposes only

void overview(struct Database *db) {
  for (struct TopicNode *current_topic = db->topics; current_topic; current_topic = current_topic->next) {
    printf("at channel %s there are:\n", current_topic->topic_name);
    for (struct ClientNode *current_client = current_topic->clients; current_client; current_client = current_client->next) {
      printf("client: %i, %i, %i\n", current_client->client_id, current_client->subscription_type, current_client->messages_left);
    }
  }
}

#endif // BUS_H
