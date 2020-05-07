#ifndef MAJOR2_PIPING
#define MAJOR2_PIPING

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void pipe_parse(char* command[]);

#endif