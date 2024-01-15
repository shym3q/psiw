#ifndef TYPES_H
#define TYPES_H

struct msg {
  int id;
  char text[1024];
};

typedef enum {
  GENERIC,
  REGISTER_REQUEST,
  CLIENTS_NUMBER,
  CLIENT_ID,
  SUBSCRIBE_TOPIC
} msg_type;

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

#endif // !TYPES_H
