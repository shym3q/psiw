#ifndef TYPES_H
#define TYPES_H

struct Msg {
  int id;
  int chid;
  char text[20];
};

// the internal message: a protocol from the child process
struct Prot {
  int type, cn, cid;
  char name[14], topic[14];
};

enum MsgTypes {
  GENERIC,
  INTERNAL,
  REGISTER_REQUEST,
  CLIENTS_NUMBER,
  CLIENT_ID,
  SUBSCRIBE_TOPIC,
  CHANNEL_ID,
  CLIENT_MSG,
};

typedef struct {
  long type;
  struct Msg cmsg;
} TextMsgBuf;

typedef struct {
  long type;
} PingBuf;

typedef struct {
  long type;
  int i;
} DecMsgBuf;

typedef struct {
  long type;
  struct Prot imsg;
} InternalMsgBuf;

#endif // !TYPES_H
