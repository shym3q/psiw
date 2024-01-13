// What client can do:
// -> send message with type and priority

#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

typedef struct msgbuf {
  long type;
  char text[1024];
} msg;

void send_to_server() {

}

// -> receive message in synchronious or asynchronious way
// synchronious: on demand
// asynchronious: automatically

void receive_from_server() {

}

// The client tries to establish a connection with the server.

void establish_connection() {
  // opening own channel for upcoming messages with unique id
  key_t ch = ftok("channel-key", 65); //TODO: to actually open that queue

  // announcing the landing on the server 
  int mid = msgget(0x123, 0600 | IPC_CREAT);
  msg m;
  strcpy(m.text, "hello world");
  m.type = 5;
  //TODO: including in landing message for the server the id of self-created channel (which will be treated as a client id)
  msgsnd(mid, &m, strlen(m.text) + 1, 0);
}

void subscribe(){

}

int main(int argc, char *argv[])
{
  establish_connection();
  return 0;
}
