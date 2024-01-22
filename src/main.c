#include "server/server.c"
#include "client/client.c"

int main(int argc, char *argv[]) {
  if(argc == 2) {
    if(strcmp(argv[1], "server") == 0)
      server_init();
    if(strcmp(argv[1], "client") == 0)
      client_init();
  }
  return 0;
}
