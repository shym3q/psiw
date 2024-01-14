#ifndef TYPES_H
#define TYPES_H

typedef enum MessageType {
  REGISTER_REQUEST=1,
  CLIENTS_NUMBER,
  CLIENT_ID
} msg_type;

typedef struct MsgBuf {
  long type;
  char text[1024];
} msg_buf;

#endif // !TYPES_H
