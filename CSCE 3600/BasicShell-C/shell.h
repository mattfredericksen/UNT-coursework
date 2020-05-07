#ifndef SHELL
#define SHELL

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pwd.h>
#include <signal.h>
#include "helpers.h"
#include "piping.h"

char path[100000];

void parse(char[], char*[], char**[]);
void process(char*[]);
bool command_search(char*[], char*);

int redOut(char* inFile);
int redIn(char * outFile);
int redParse(char * command[]);
void cd(char* path);

void alias_init();
void alias_destroy();
char** alias_get(char* command[]);
void alias(char* command[]);

void myhistory_init();
void myhistory_destroy();
void myhistory_add(char **command);
void myhistory(char **command);

void Paths(char* command[]);
void display();
void ign_signals();
void dfl_signals();
void signals(char* command[]);
void builtin_exit();

#endif  // SHELL
