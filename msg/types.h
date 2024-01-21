#ifndef TYPES_H
#define TYPES_H

struct msg {
  int id;
  int chid;
  char text[20];
};

// the internal message: a protocol from the child process
struct prot {
  int type, cn, cid;
  char name[14], topic[14];
};

enum msg_type {
  GENERIC,
  INTERNAL,
  REGISTER_REQUEST,
  CLIENTS_NUMBER,
  CLIENT_ID,
  SUBSCRIBE_TOPIC,
  CLIENT_MSG,
};

typedef struct {
  long type;
  struct msg cmsg;
} t_msgbuf;

typedef struct {
  long type;
} pingbuf;

typedef struct {
  long type;
  int i;
} dec_msgbuf;

typedef struct {
  long type;
  struct prot imsg;
} i_msgbuf;

#endif // !TYPES_H
