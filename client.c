// What client can do:
// -> send message with type and priority

#include <string.h>
#include <sys/msg.h>

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

void join() {
  int mid = msgget(0x123, 0600 | IPC_CREAT);
  msg m;
  strcpy(m.text, "hello world");
  m.type = 5;
  msgsnd(mid, &m, strlen(m.text) + 1, 0);
}

void add_topic(){

}

int main(int argc, char *argv[])
{
  join();
  return 0;
}
