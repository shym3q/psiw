#ifndef BUS_H
#define BUS_H

#include <stdlib.h>

struct node {
  int cid;
  // pointer to the other clients with the same subscription
  struct node *subs;
  struct node *next;
};

struct node *new_db() {
  struct node *db = (struct node*)malloc(sizeof(struct node));
  return db;
}

void add_subcriber(int cid) {

}

// redirects the captured message to the subscribers 
void distribute(int sender, char *msg) {

}

#endif // BUS_H
