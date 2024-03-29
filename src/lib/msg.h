#ifndef MSG_H
#define MSG_H

#define MSG_MAX_LENGTH 240
#define TOPIC_MAX_LENGTH 14
#define NAME_MAX_LENGTH 14

struct Msg {
  int client_id;
  int subscription_type;
  int msg_number_requested;
  int channel_id;
  char text[MSG_MAX_LENGTH];
  char name[NAME_MAX_LENGTH];
};

// the internal message: a protocol from the child process
struct Prot {
  int type, client_number, client_id, subscription_type, msg_left;
  char name[NAME_MAX_LENGTH], topic[TOPIC_MAX_LENGTH];
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
  RECORD_ID
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
} DecimalMsgBuf;

typedef struct {
  long type;
  struct Prot imsg;
} InternalMsgBuf;

#endif // !MSG_H
