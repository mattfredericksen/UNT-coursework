/* Name: Matthew Fredericksen
 * EUID: mlf0220
 * Date: 09/28/2020
 *
 * This program is used to accept a web address from a client
 * and respond with the http response of that web address.
 *
 * If the http response code from the web address is "200",
 * the response will be cached, and future requests to the
 * same address will be served from the cache. When the
 * cache size reaches CACHE_LEN, new responses will replace
 * the oldest cached response.
 *
 * An index of cached responses will be created in "list.txt",
 * and cached responses will be stored in files named by a
 * timestamp of when they were received, in the strftime format
 * "%Y%m%d%H%M%S".
 *
 * Valid web addresses start with "www.", not "http://".
 * Web address that do not include "www." may work, but
 * success is not guaranteed. Any domain that is not
 * http (not https) compatible cannot be accessed.
 */

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "socket_io.h"

#define CACHE_NAME "list.txt"
#define CACHE_LEN 5


long int get_file_size(char *filename) {
    // Open file, seek to end, and return byte length.
    // Returns -1 if unsuccessful.

    long int length;
    FILE *fptr = fopen(filename, "r");

    // check if the file exists
    if (!fptr) {
        fprintf(stderr, "Error opening file \"%s\": ", filename);
        perror(NULL);
        return -1;
    }

    // return length of the file
    fseek(fptr, 0L, SEEK_END);
    length = ftell(fptr);
    fclose(fptr);
    return length;
}

void get_timestamp(char *str, size_t len) {
    // copy current timestamp into `str`

    time_t now = time(NULL);
    if (!strftime(str, len, "%Y%m%d%H%M%S", localtime(&now))) {
        fputs("Error creating timestamp", stderr);
    }
}

void get_ip_address(char* host, struct sockaddr_in **webaddr) {
    // Uses getaddrinfo() to get ip address from `host`
    // and store it in `webaddr`.
    // If unsuccessful, sets `*webaddr` to NULL

    struct addrinfo hints, *res;

    memset (&hints, 0, sizeof (hints));
    // specify that request is for IPv4
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, NULL, &hints, &res)) {
        fprintf(stderr, "getaddrinfo() error\n");
        *webaddr = NULL;
    } else {
        *webaddr = (struct sockaddr_in *) res->ai_addr;
    }
}


void print_cache(char *cache[CACHE_LEN][2]) {
    // Prints the contents of the cache after
    // it has been loaded by read_cache().

    int i;

    puts("cache:");
    for (i = 0; i < CACHE_LEN && cache[i][0] != NULL; i++) {
        printf("\t%i: %s %s\n", i+1, cache[i][0], cache[i][1]);
    }
    if (i == 0) {
        puts("\t<empty>");
    }
}

int read_cache(char *cache[CACHE_LEN][2]) {
    // Reads cache from file into `cache` array.
    // Returns number of items in cache.

    // prevents issues when freeing memory later
    memset(cache, 0, sizeof(char*) * CACHE_LEN * 2);

    int i = 0;
    FILE *fptr = fopen(CACHE_NAME, "r");

    if (!fptr) {
        perror("Error opening cache list");
        return 0;
    }

    // read up to `CACHE_LEN` lines, allocating memory automagically with %ms
    // memory is freed later with free_cache()
    while (i < CACHE_LEN) {
        if (2 != fscanf(fptr, "%ms %ms\n", &cache[i][0], &cache[i][1])) {
            break;
        }
        i++;
    }
    fclose(fptr);
    return i;
}

int write_cache(char *cache[CACHE_LEN][2]) {
    // writes contents of `cache` array to cache list file

    FILE *fptr = fopen(CACHE_NAME, "w");

    for (int i=0; i < CACHE_LEN && cache[i][0] != NULL; i++) {
        fprintf(fptr, "%s %s\n", cache[i][0], cache[i][1]);
    }
    fclose(fptr);
}

void free_cache(char *cache[CACHE_LEN][2]) {
    // Free all memory used by chr* within `cache`.
    // Causes segfault if cache is not
    // memset to 0 before loading.

    for (int i=0; i < CACHE_LEN && cache[i][0] != NULL; i++) {
        free(cache[i][0]); free(cache[i][1]);
    }
}

int check_cache(char *return_addr, char *domain, char* path) {
    // Checks if a cached response for `domain`/`path` exists.
    // If found, copy its cache filename into `return_addr`.
    // Otherwise, return -1.

    int n, i;
    char *address = NULL;
    char *cache[CACHE_LEN][2];

    // load the cache list from file
    n = read_cache(cache);

#ifdef DEBUG
    print_cache(cache);
#endif

    // is the cache list empty?
    if (n == 0) {
        return -1;
    }

    // allocates memory for string comparison
    asprintf(&address, "%s/%s", domain, path);

    // check each address in cache list for a match to current request
    for (i = 0; i < n; i++) {
        if (!strcasecmp(address, cache[i][0])) {
            strcpy(return_addr, cache[i][1]);
            break;
        }
    }

    free(address);
    free_cache(cache);
    return i == n ? -1 : 0;
}

int serve_cache(int sock_fd, char *filename) {
    // Sends `filename` into socket at `sock_fd`.
    // Returns 0 on success or -1 on failure.

    int size, bytes, written = 0;

    FILE *fptr = fopen(filename, "r");

    if (!fptr) {
        perror("Error opening cache");
        return -1;
    }

#ifdef DEBUG
    printf("serving cache: %s\n\n", filename);
#endif

    size = get_file_size(filename);
    while (written < size) {
        bytes = sendfile(sock_fd, fileno(fptr), NULL, size);
        if (bytes < 0) {
            perror("Error serving cache");
            return -1;
        }
        written += bytes;
    }
    fclose(fptr);
    return 0;
}

int add_cache(char* response, char* domain, char* path) {
    // Stores http `response` in a new cache file.
    // Adds new cache file name to cache list file.
    // Replaces oldest cached response if cache is full.
    // Returns 0 on success or -1 on failure.

    char *cache[CACHE_LEN][2];
    char timestamp[15];
    int n, i, old = 0;
    FILE *fptr = NULL;

    // write to file
    get_timestamp(timestamp, sizeof(timestamp));
    if (!(fptr = fopen(timestamp, "w"))) {
        fprintf(stderr, "Error opening file \"%s\" for writing: ", timestamp);
        perror(NULL);
        return -1;
    } else {
        fputs(response, fptr);
        fclose(fptr);
    }

    // load cache list from file
    n = read_cache(cache);

    // if cache list is full, find and remove oldest entry
    if (n == CACHE_LEN) {
        for (i = 1; i < CACHE_LEN; i++) {
            if (strcmp(cache[old][1], cache[i][1]) > 0) {
                old = i;
            }
        }
#ifdef DEBUG
        printf("replaced cache index %i: %s\n", old+1, cache[old][0]);
#endif
        remove(cache[old][1]);
        free(cache[old][0]); free(cache[old][1]);
    } else {
        // cache list is not full, append
        old = n;
    }

    // memory allocated here is freed by free_cache()
    asprintf(&cache[old][0], "%s/%s", domain, path);
    asprintf(&cache[old][1], "%s", timestamp);

    write_cache(cache);
    free_cache(cache);
    return 0;
}


int main(int argc, char **argv) {
    int portno;

    // argument validation
    if (argc != 2) {
        puts("Usage: pyserver <port_number>");
        return 1;
    } else if (!(portno = atoi(argv[1]))) {
        printf("Cannot use \"%s\" as a port number.\n", argv[1]);
        return 1;
    }

    char buffer[1024], cache_addr[15], code[4] = {0};
    char *domain = buffer, *path, *pos, *request, *response;
    char *request_template = "GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n";

    int listen_fd, conn_fd, web_fd, n;
    struct sockaddr_in servaddr, *webaddr;


    /* AF_INET - IPv4 IP , Type of socket, protocol*/
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(portno);

    // allows reusing the socket
    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // Binds the above details to the socket
    if (bind(listen_fd,  (struct sockaddr *) &servaddr, sizeof(servaddr))) {
        perror("Socket bind() error");
        return 1;
    }

    // Start listening to incoming connections
    listen(listen_fd, 10);

    // loop until process is terminated or interrupted
    while(1) {
#ifdef DEBUG
        puts("--------------------------------");
        puts("Waiting for client to connect...");
#endif
        // Accepts an incoming connection
        conn_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);

#ifdef DEBUG
        puts("Client connected.");
#endif

        // get input from client
        n = read(conn_fd, buffer, sizeof(buffer));
        if (n <= 0) {
            perror("Error reading from client socket");
            break;
        } else {
            // replace newline with null byte.
            // may cause an issue if message did not
            // originate from a client's stdin.
            buffer[n-1] = '\0';
        }

        // separate domain from resource path
        pos = strchr(buffer, '/');
        if (pos) {
            *pos = '\0';
            path = pos + 1;
        } else {
            path = buffer + n - 1;
        }

#ifdef DEBUG
        printf("client input:\n\tdomain: %s\n\tpath: /%s\n", domain, path);
#endif

        // serve directly if the page is already cached
        if (!check_cache(cache_addr, domain, path)
                && !serve_cache(conn_fd, cache_addr)) {
            close(conn_fd);
            continue;
        }

        // try to get IP address
        get_ip_address(domain, &webaddr);
        if (!webaddr) {
            n = asprintf(&response, "Unable to resolve IP address for \"%s\"\n\n", domain);
            complete_write(conn_fd, response, n);
            close(conn_fd);
#ifdef DEBUG
            puts(response);
#endif
            free(response);
            continue;
        }

        // assuming we always want port 80
        web_fd = socket(AF_INET, SOCK_STREAM, 0);
        webaddr->sin_port = htons(80);
        if (connect(web_fd, (struct sockaddr *) webaddr, sizeof(*webaddr))) {
            perror("Socket connect() error");
            return 1;
        }

        // send HTTP request
        n = asprintf(&request, request_template, path, domain);
#ifdef DEBUG
        printf("sending request:\n%s", request);
#endif
        complete_write(web_fd, request, n);
        free(request);

        // retrieve HTTP response
        n = complete_read(web_fd, &response);

#ifdef DEBUG
        printf("response size: %i bytes\n", n);
#endif

        // check HTTP response code
        strncpy(code, strchr(response, ' ')+1, 3);

#ifdef DEBUG
        printf("response code %s,%s cached\n\n",
               code, strcmp(code, "200") ? " not":"");
#endif

        if (!strcmp(code, "200")) {
            // cache response
            add_cache(response, domain, path);
        }

        // write response to the client
        complete_write(conn_fd, response, n);
        if (close(conn_fd)) {
            perror("Socket close() error");
        }
        free(response);
    }
    return 0;
}
