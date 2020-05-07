/* Name: Matthew Fredericksen
 * EUID: mlf0220
 * CSCE 3600.003
 * 4-20-2020
 *
 * This program creates a tic-tac-toe server on
 * the specified port using TCP sockets. Two clients
 * are required to connect for the game to being.
 *
 * compile: gcc minor4server.c -o minor4server
 * usage  : ./minor4server <port>
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

struct Player {
    int fd;
    char symbol;
    bool my_turn;
}; typedef struct Player Player;

void send_and_print(Player *player, char* message) {
    // print message to be sent in console
    printf("%c>%s", player->symbol, message);
    // and send it to the player (ignore SIGPIPE)
    send(player->fd, message, strlen(message), MSG_NOSIGNAL);
}

void send_board(char board[3][3], char* buffer, Player* p1, Player* p2) {
    // formats the tic-tac-toe board and sends it to the players
    char board_rows[8][9] = {"  board\n",
                             "  1 2 3\n",
                             " +-+-+-\n",
                             "A| | | \n",
                             " +-+-+-\n",
                             "B| | | \n",
                             " +-+-+-\n",
                             "C| | | \n"};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j]) {
                board_rows[i*2 + 3][j*2 + 2] = board[i][j];
            }
        }
    }
    strcpy(buffer, "\n");
    for (int i = 0; i < 8; i++) {
        // no one cares about Schlemiel the Painter
        strcat(buffer, board_rows[i]);
    }
    send_and_print(p1, buffer);
    send_and_print(p2, buffer);
}

char check_for_winner(char board[3][3]) {
    // returns the symbol of the winning player,
    // or 0 if there is no winner
    for (int i = 0; i < 3; i++) {
        // vertical win
        if (!board[i][0]) continue; // so we don't match nulls
        if (board[i][0] == board[i][1] &&
            board[i][0] == board[i][2]) {
            return board[i][0];
        }
    }
    for (int i = 0; i < 3; i++) {
        // horizontal win
        if (!board[0][i]) continue; // so we don't match nulls
        if     (board[0][i] == board[1][i] &&
                board[0][i] == board[2][i]) {
            return board[0][i];
        }
    }
    // diagonal win
    if (!board[1][1]) return 0; // so we don't match nulls
    if (   (board[0][0] == board[1][1] &&
            board[0][0] == board[2][2]) ||
           (board[0][2] == board[1][1] &&
            board[0][2] == board[2][0])) {
        return board[1][1];
    }
    // no win
    return 0;
}

bool check_board_is_full(char board[3][3]) {
    // check if every board spot is occupied
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (!board[i][j]) return false;
        }
    }
    return true;
}

void end_game(char winner, Player* p1, Player* p2) {
    // winner is the symbol for the winning player
    if (winner == p1->symbol) {
        send_and_print(p1, "You win\n");
        send_and_print(p2, "You lose\n");
    } else if (winner == p2->symbol) {
        send_and_print(p1, "You lose\n");
        send_and_print(p2, "You win\n");
    } else {
        send_and_print(p1, "Tie Game\n");
        send_and_print(p2, "Tie Game\n");
    }
    close(p1->fd);
    close(p2->fd);
}

bool process_player_input(Player* player, char* buffer, int buffer_len, Player* other_player, char board[3][3]) {
    // If error or eof, end game gracefully
    if (buffer_len < 1) {
        close(player->fd);
        snprintf(buffer, 32, "Player %c quit unexpectedly\n", player->symbol);
        send_and_print(other_player, buffer);
        close(other_player->fd);
        return 1;
    }

    // print player input on server console
    printf("%c<%.*s", player->symbol, buffer_len, buffer);

    // handle user input according to input length
    if (buffer[0] == '\n') {
        // do nothing
    } else if (buffer_len == 2) {
        if (buffer[0] == 'R') {
            // RESIGN COMMAND
            close(player->fd);
            snprintf(buffer, 22, "You win, %c resigned\n", player->symbol);
            send_and_print(other_player, buffer);
            close(other_player->fd);
            return 1;
        } else if (buffer[0] == '?') {
            // HELP COMMAND
            send_and_print(player,
                    "\n"
                    "?-Display this help\n"
                    "R-Resign\n"
                    "M<R><C>-Move where <R> is a row A, B, or C "
                    "and <C> is a column 1, 2, or 3\n"
                    "\tExample Moves: MA1 MC3 MB1\n");
        } else {
            // NO MATCHING 2-LENGTH COMMAND
            send_and_print(player, "Invalid command\n");
        }
    } else if (buffer_len == 4) {
        // match any case
        buffer[0] = toupper(buffer[0]);
        buffer[1] = toupper(buffer[1]);
        if (buffer[0] != 'M') {
            // 4-LENGTH COMMAND MUST BEGIN WITH 'M'
            send_and_print(player, "Invalid command\n");
        } else if (!player->my_turn) {
            // MUST HAVE PERMISSION TO EXECUTE 4-LENGTH COMMAND
            send_and_print(player, "It's not your turn\n");
        } else if (buffer[1] < 'A' || buffer[1] > 'C' ||
                   buffer[2] < '1' || buffer[2] > '3') {
            // 4-LENGTH COMMAND MUST HAVE VALID ARGUMENTS
            send_and_print(player, "Invalid Move\n"
                                   "  Move should be M<R><C> with no spaces\n"
                                   "  Example: MA1 or MB3\n");
        } else if (board[buffer[1] - 'A'][buffer[2] - '1']) {
            // CANNOT MOVE ON AN OCCUPIED LOCATION
            send_and_print(player, "That spot is already taken\n");
        } else {
            // congratulations, the input is valid
            // place the player's symbol on the board
            board[buffer[1] - 'A'][buffer[2] - '1'] = player->symbol;
            // send the updated board to both players
            send_board(board, buffer, player, other_player);
            // if we have a winner, notify players and end the game
            char winner = check_for_winner(board);
            if (winner) {
                end_game(winner, player, other_player);
                return 1;
            } else if (check_board_is_full(board)) {
                // if we don't have a winner, but the board is full,
                // then end the game as a tie
                end_game(0, player, other_player);
                return 1;
            }
            // if we get here, we didn't have a winner or tie.
            // alternate turns
            player->my_turn = false;
            other_player->my_turn = true;
            send_and_print(other_player, "Your turn\n");
        }
    } else {
        // not a 1-, 2-, or 4-length command? INVALID!!!
        send_and_print(player, "Invalid command\n");
    }
    return 0;
}

int main(int argc, char *argv[])
{
    // exit the program if we don't have a port number
    if (argc != 2) {
        printf("Provide a single port number as an argument.\n");
        return 1;
    } else if (!atoi(argv[1])) {
        printf("Cannot use '%s' as a port number.\n", argv[1]);
        return 1;
    }

    // variable declarations
    char board[3][3] = {};  // 3x3 tic-tac-toe board
    char buffer[512];  // buffer for sending and receiving
    int listenfd,  // socket file descriptor for accepting clients
	       maxfd,  // maximum file descriptor value for select()
	       nread,  // number of bytes read from client socket
	       cli_size,  // the client size?
	       portno;  // port number the server will listen on
    fd_set fds;  // file descriptor set for select()
    Player player_x, player_o;  // structs for player information
	struct sockaddr_in serv_addr, cli_addr;

	// symbols will be used when sending messages to players
	// and when players move on the board
    player_x.symbol = 'X';
	player_o.symbol = 'O';

	printf("Network Server Starting\n");

	// create a tcp internet socket
	// exit if an error occurs
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("socket error\n");
		exit(EXIT_FAILURE);
	}

	// zero out memory
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(buffer, '0', sizeof(buffer));

	// configure socket stuff
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	portno = atoi(argv[1]);
	serv_addr.sin_port = htons(portno);

	// allow binding of a previously used address, I think
    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // attempt to bind the socket to the port, I think
	if (bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
		printf("bind error\n");
		exit(EXIT_FAILURE);
	}

	// begin listening for players to join
	// buffer size 2 because we know only
	// two players will try to connect
	if (listen(listenfd, 2) == -1) {
		printf("listen error\n");
		exit(EXIT_FAILURE);
	}

	// wait for first player (X) to connect
	printf("Waiting on Clients\n");
    cli_size = sizeof(cli_addr);
    if ((player_x.fd = accept(listenfd, (struct sockaddr*) &cli_addr, &cli_size)) == -1) {
        printf("accept error\n");
        exit(EXIT_FAILURE);
    }
    printf("X<Connected\n");
    send_and_print(&player_x, "You are player X. Waiting on player O to connect\n");

    // wait for second player (O) to connect
    if ((player_o.fd = accept(listenfd, (struct sockaddr*) &cli_addr, &cli_size)) == -1) {
        printf("accept error\n");
        exit(EXIT_FAILURE);
    }
    printf("O<Connected\n");
    send_and_print(&player_o, "You are player O\n");

    // send the initial tic-tac-toe board to both players
    send_board(board, buffer, &player_x, &player_o);
    // check which player's file descriptor is larger for select()
    maxfd = (player_x.fd > player_o.fd ? player_x.fd : player_o.fd) + 1;

    player_x.my_turn = true;
    player_o.my_turn = false;
    send_and_print(&player_x, "Your turn\n");
    while (1) {
        // Set up polling using select.
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(player_x.fd, &fds);
        FD_SET(player_o.fd, &fds);

        // wait for some input
        select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);

        // check if player X has sent input
        if (FD_ISSET(player_x.fd, &fds)) {
            nread = recv(player_x.fd, buffer, sizeof(buffer), 0);
            // returns 0 unless game-ending conditions have been met
            if (process_player_input(&player_x, buffer, nread, &player_o, board)) {
                break;
            }
        }
        // check if player O has sent input
        if (FD_ISSET(player_o.fd, &fds)) {
            nread = recv(player_o.fd, buffer, sizeof(buffer), 0);
            // returns 0 unless game-ending conditions have been met
            if (process_player_input(&player_o, buffer, nread, &player_x, board)) {
                break;
            }
        }
    }
    printf("Game Ended\n");
    return 0;
}

