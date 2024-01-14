#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

void panic(char *msg) {
  perror(msg);
  exit(-1);
}

#endif // !UTILS_H
