#ifndef BUS_H
#define BUS_H

#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include "object.h"
#include "../lib/msg.h"
#include "../lib/subscription.h"

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

void channel_connect(struct Database *db, int tid, int cid, int stype, int msgleft) {
  struct ClientNode *n = (struct ClientNode*)malloc(sizeof(struct ClientNode));
  n->client_id = cid;
  n->subscription_type = stype;
  n->messages_left = msgleft;
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

int distribute_msg(TextMsgBuf *m, struct Database *db, struct Server *s) {
  for(struct TopicNode *current_topic = db->topics; current_topic; current_topic = current_topic->next) {
    if(m->cmsg.channel_id == current_topic->topic_id) {
      int n = 0;
      for(struct ClientNode *current_client = current_topic->clients; current_client; current_client = current_client->next) {
        if(current_client->client_id != m->cmsg.client_id) {
          send_to_client(current_client->client_id, m);
          DecimalMsgBuf imbuf = {RECORD_ID, current_client->client_id};
          msgsnd(s->msqid, &imbuf, sizeof(int), 0);
          n++;
        }
      }
      return n;
    }
  }
  return 0;
}

void update_msg_left(int cid, struct Database *db) {
  for(struct TopicNode *current_topic = db->topics; current_topic; current_topic = current_topic->next) {
    struct ClientNode *head_client = current_topic->clients;
    if(head_client && head_client->client_id == cid && head_client->subscription_type == TEMPORARY && --head_client->messages_left == 0) {
      if(head_client->next) {
        *head_client = *head_client->next;
        return;
      }
      free(head_client);
      return;
    }
    for(struct ClientNode *current_client = head_client; current_client; current_client = current_client->next) {
      struct ClientNode *next_client = current_client->next;
      if(next_client && next_client->client_id == cid && next_client->subscription_type == TEMPORARY && --next_client->messages_left == 0) {
        if(!next_client->next) {
          free(next_client);
          current_client->next = NULL;
          return;
        } else {
          struct ClientNode *temp_client = next_client;
          current_client->next = next_client->next;
          free(temp_client);
          return;
        }
      }
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
