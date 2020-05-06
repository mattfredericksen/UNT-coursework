/* Name: Matthew Fredericksen
 * EUID: mlf0220
 * CSCE 3600.003
 * Date: 4-20-2020
 *
 * This program creates a tic-tac-toe client for use with
 * minor4server.c. The client will not run unless a
 * server is already running on the specified port.
 *
 * compile: gcc minor4client.c -o minor4client
 * usage  : ./minor4client <port>
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
 
int main(int argc, char *argv[])
{
    // validate command-line argument
    if (argc != 2) {
        printf("Provide a single port number as an argument.\n");
        return 1;
    } else if (!atoi(argv[1])) {
        printf("Cannot use '%s' as a port number.\n", argv[1]);
        return 1;
    }

    // socket, port number, fd limit for select,
    // and number of bytes read
	int sockfd, portno, maxfd, nread;
    // buffer for sending and receiving
	char buffer[512] = {};
	// socket thing
	struct sockaddr_in serv_addr;
	// file descriptor set for select()
    fd_set fds;

    // create a socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket error\n");
		exit(EXIT_FAILURE);
	}

	// set server info
	serv_addr.sin_family = AF_INET;
	portno = atoi(argv[1]);
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// connect to the server
	if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("connect error\n");
		exit(EXIT_FAILURE);
	}

	maxfd = sockfd + 1;
	while (1) {
        // set up polling
        // we're only interested in the server socket and stdin (0)
        FD_ZERO(&fds);
        FD_SET(sockfd,&fds);
        FD_SET(0,&fds);

        // wait for some input
        select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0,
                        (struct timeval *) 0);

        // check if we received something from the server
        if(FD_ISSET(sockfd, &fds)) {
            nread = recv(sockfd, buffer, sizeof(buffer), 0);
            // If error or eof, terminate.
            if (nread < 1) break;
            // write what the server sent to stdin
            write(1, buffer, nread);
        }
        // check if we received something from stdin
        if(FD_ISSET(0, &fds)) {
            nread = read(0, buffer, sizeof(buffer));
            // If error or eof, terminate.
            if (nread < 1) break;
            // send what the user typed to the server
            send(sockfd, buffer, nread, 0);
        }
    }
    close(sockfd);
	return 0;
}

