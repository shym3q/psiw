#!/bin/bash

if [[ ! -e "bin" ]]; then
  mkdir bin
fi

gcc -o bin/chat src/main.c
