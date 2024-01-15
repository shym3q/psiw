#ifndef TYPES_H
#define TYPES_H

typedef enum MessageType {
  UNKNOWN,
  REGISTER_REQUEST,
  CLIENTS_NUMBER,
  CLIENT_ID,
  SUBSCRIBE_TOPIC
} msg_type;

typedef struct MsgBuf {
  long type;
  char text[1024];
} msg_buf;

typedef struct Ping {
  long type;
} pingbuf;

typedef struct DecMsgBuf {
  long type;
  int i;
} dec_msgbuf;

#endif // !TYPES_H
