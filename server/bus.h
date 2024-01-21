#ifndef BUS_H
#define BUS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../msg/types.h"

// a client node in the database
struct ClientNode {
  // client id
  int cid;
  // pointer to the other clients with the same subscription
  struct ClientNode *next;
};

// a topic node in the database
struct TopicNode {
  // topic id
  int tid;
  char topic[14];
  struct ClientNode *clients;
  struct TopicNode *next;
};

struct Database {
  int tn;
  struct TopicNode *topics;
};

struct Database *new_db() {
  struct Database *db = (struct Database*)malloc(sizeof(struct Database));
  db->tn = 0;
  return db;
}

// searches the topic in the existing records, if it doesn't find it creates one

int get_channel(struct Database *db, char t[14]) {
  for(struct TopicNode *curr = db->topics; curr; curr = curr->next) {
    if(!strcmp(t, curr->topic))
      return curr->tid;
  }
  struct TopicNode *n = (struct TopicNode*)malloc(sizeof(struct TopicNode));
  n->next = db->topics;
  n->tid = db->tn;
  sprintf(n->topic, "%s", t);
  db->topics = n;
  return db->tn++;
}

// adds the client to the appropriate channel

void channel_connect(struct Database *db, int tid, int cid) {
  struct ClientNode *n = (struct ClientNode*)malloc(sizeof(struct ClientNode));
  n->cid = cid;
  for(struct TopicNode *curr = db->topics; curr; curr = curr->next) {
    if(curr->tid == tid) {
      n->next = curr->clients;
      curr->clients = n;
      break;
    }
  }
}

// redirects the captured message to the subscribers 

void distribute_msg(struct Msg *m, struct Database *db) {
  printf("looking for the record %d...\n", m->chid);
  for(struct TopicNode *curr = db->topics; curr; curr = curr->next) {
    printf("checking the record %d\n", curr->tid);
    if(m->chid == curr->tid) {
      for(struct ClientNode *c = curr->clients; c; c = c->next) {
        printf("adressee: %d\n", c->cid);
      }
      return;
    }
  }
}

void client_communicate() {

}

// for debugging purposes only

void overview(struct Database *db) {
  for (struct TopicNode *curr = db->topics; curr; curr = curr->next) {
    printf("at channel: %s, there are:\n", curr->topic);
    for (struct ClientNode *c = curr->clients; c; c = c->next) {
      printf("client: %i\n", c->cid);
    }
  }
  printf("finished\n");
}

#endif // BUS_H
