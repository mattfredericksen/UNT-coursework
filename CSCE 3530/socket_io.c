#include "socket_io.h"

int complete_read(int sock_fd, char **response) {
    // Reads from a socket until no more data can be read.
    // Concept adapted from https://stackoverflow.com/a/22135885
    // Returns bytes read on success or -1 on failure.

    int size = 2048, received = 0, bytes;
    char *mem = NULL;

    if (!(*response = malloc(size))) {
        perror("Error allocating memory");
        return -1;
    }

    while (1) {
        bytes = read(sock_fd, *response + received, size - received);
        if (bytes < 0) {
            free(*response);
            *response = NULL;
            perror("Socket complete_read() error");
            return bytes;
        }
        if (bytes == 0) {
            break;
        }
        received += bytes;

        // double allocated memory if needed
        if (received == size) {
            size *= 2;
            if (!(mem = realloc(*response, size))) {
                perror("Error allocating memory");
                free(*response);
                *response = NULL;
                return -1;
            } else {
                *response = mem;
            }
        }
    }
    return received;
}

int complete_write(int sock_fd, char* message, size_t message_len) {
    // Writes to a socket until all data has been written.
    // Returns bytes written on success or -1 on failure.

    int bytes, written = 0;
    do {
        bytes = write(sock_fd, message + written, message_len - written);
        if (bytes < 0) {
            perror("Socket complete_write() error");
            return bytes;
        }
        written += bytes;
    } while (bytes > 0);
    return written;
}