/* Name: Matthew Fredericksen
 * EUID: mlf0220
 * Date: 09/28/2020
 *
 * This program is used to send a web address to a proxy server
 * and print the http response send by that server.
 *
 * Valid web addresses start with "www.", not "http://".
 * Web address that do not include "www." may work, but
 * success is not guaranteed. Any domain that is not
 * http (not https) compatible cannot be accessed.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "socket_io.h"

int main (int argc, char **argv) {
    int portno;

    // argument validation
    if (argc != 2) {
        printf("Usage: client <port_number>\n");
        return 1;
    } else if (!(portno = atoi(argv[1]))) {
        printf("Cannot use \"%s\" as a port number.\n", argv[1]);
        return 1;
    }

    int sockfd, n;
    char buffer[1024], *response;
    struct sockaddr_in servaddr;

    /* AF_INET - IPv4 IP , Type of socket, protocol*/
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(portno);

    /* Convert IPv4 and IPv6 addresses from text to binary form */
    inet_pton(AF_INET,"129.120.151.94", &(servaddr.sin_addr));

    /* Connect to the server */
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) {
        perror("Socket connect() error");
        return 1;
    }

    // get input from user, leaving extra room for server's response
    printf("url: ");
    fgets(buffer, sizeof(buffer), stdin);

    // send what the user typed to the server
    complete_write(sockfd, buffer, strlen(buffer));

    // retrieve the server's response
    n = complete_read(sockfd, &response);

    // If error or eof, terminate.
    if (n < 1) {
        puts("Error reading server response");
    } else {
        // print the response
        printf("%.*s\n\n", n, response);
    }
    free(response);

    // close the socket
    if (close(sockfd)) {
        perror("Socket close() error");
    }

    return 0;
}
