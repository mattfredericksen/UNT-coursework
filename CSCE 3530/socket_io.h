#ifndef SOCKET_IO_H
#define SOCKET_IO_H

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int complete_read(int sock_fd, char **response);
int complete_write(int sock_fd, char* message, size_t message_len);

#endif // SOCKET_IO_H
