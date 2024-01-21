#ifndef BUS_H
#define BUS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../msg/types.h"

// a client node in the database
struct cnode {
  // client id
  int cid;
  // pointer to the other clients with the same subscription
  struct cnode *next;
};

// a topic node in the database
struct tnode {
  // topic id
  int tid;
  char topic[14];
  struct cnode *clients;
  struct tnode *next;
};

struct database {
  int tn;
  struct tnode *topics;
};

struct database *new_db() {
  struct database *db = (struct database*)malloc(sizeof(struct database));
  db->tn = 0;
  return db;
}

int get_channel(struct database *db, char t[14]) {
  for(struct tnode *curr = db->topics; curr; curr = curr->next) {
    if(!strcmp(t, curr->topic))
      return curr->tid;
  }
  struct tnode *n = (struct tnode*)malloc(sizeof(struct tnode));
  n->next = db->topics;
  n->tid = db->tn;
  sprintf(n->topic, "%s", t);
  db->topics = n;
  return db->tn++;
}

void channel_connect(struct database *db, int tid, int cid) {
  struct cnode *n = (struct cnode*)malloc(sizeof(struct cnode));
  n->cid = cid;
  for(struct tnode *curr = db->topics; curr; curr = curr->next) {
    if(curr->tid == tid) {
      n->next = curr->clients;
      curr->clients = n;
      break;
    }
  }
}

void overview(struct database *db) {
  for (struct tnode *curr = db->topics; curr; curr = curr->next) {
    printf("at channel: %s, there are:\n", curr->topic);
    for (struct cnode *c = curr->clients; c; c = c->next) {
      printf("client: %i\n", c->cid);
    }
  }
  printf("finished\n");
}

// redirects the captured message to the subscribers 
void distribute_msg(struct msg *m, struct database *db) {
  printf("looking for the record %d...\n", m->chid);
  for(struct tnode *curr = db->topics; curr; curr = curr->next) {
    printf("checking the record %d\n", curr->tid);
    if(m->chid == curr->tid) {
      for(struct cnode *c = curr->clients; c; c = c->next) {
        printf("adressee: %d\n", c->cid);
      }
    }
  }
}

#endif // BUS_H
