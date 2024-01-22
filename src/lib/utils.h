#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

void panic(char *msg) {
  perror(msg);
  exit(-1);
}

void handle_timeout(int signum) {
  printf("timeout occured\n");
  exit(-1);
}

#endif // !UTILS_H
