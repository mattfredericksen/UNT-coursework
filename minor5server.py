#!/usr/bin/python3.6

"""
A TicTacToe Game Server

Author:  Matthew Fredericksen
Course:  CSCE 3600.003
Date:  4/30/2020

Requires Python 3

Usage: minor5server.py <port>

This program uses TCP INET socket streams to run a
tic-tac-toe server which may be connected to by two clients.
"""

import sys
from socket import socket, SOL_SOCKET, SO_REUSEADDR
from select import select
from signal import signal, SIGINT


def __main__():
    # check for correct number of arguments
    if len(sys.argv) != 2:
        print('Usage: ./minor5server <port>')
        sys.exit()

    # convert the port number argument to an integer
    try:
        port = int(sys.argv[1])
    except ValueError:
        print(f'Error: "{sys.argv[1]}" is not a number')
        sys.exit()

    # intercept ^C
    signal(SIGINT, sigint_handler)

    # the board keeps track of game progress
    board = Board()

    # create and bind an INET socket stream
    print('Network Server Starting')
    s = socket()
    s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    s.bind(("", port))
    s.listen(2)
    print('Waiting on Clients')

    # wait for first player to connect
    player_x = Player(s, 'X')
    player_x.send('You are player X. Waiting on player O to connect...')

    # wait for second player to connect
    player_o = Player(s, 'O')
    player_o.send('You are player O')

    # no longer accepting new connections
    s.close()
    players = {player_x, player_o}

    # send the blank game board to both players
    for player in players:
        player.send(str(board))

    current_turn = {player_x}
    player_x.send('Your turn')

    playing = True
    while playing:
        # wait for a player to send something
        read_ready, _, errors = select(players, [], players)

        # each time a player sends something
        for player in read_ready:
            try:
                # retrieve and process user input
                moved = player(board, player in current_turn)
            except Player.DisconnectedError:
                # player resigned or client closed connection
                player.quit()
                players.remove(player)
                for remaining in players:
                    remaining.quit(f'You win, {player.symbol} resigned')
                playing = False
                break

            if moved:
                # swap turns and send the updated board to both players
                current_turn ^= players
                for p in players:
                    p.send(str(board))

                # if the board has a winner,
                # then it was the player who just moved
                if board.has_winner():
                    player.quit('You win')
                    for loser in current_turn:
                        loser.quit('You lose')
                    playing = False
                    break

                # tie game occurs after 9 moves with no winner
                elif board.is_tie_game():
                    for p in players:
                        p.quit('Tie game')
                    playing = False
                    break

                # if no winner and not a tie game
                else:
                    for p in current_turn:
                        p.send('Your turn')

        if errors:
            print('Unexpected errors, exiting')
            return

    print('Game Ended')
    return


class Board:
    """Tracks the progress of a tic-tac-toe game"""

    class OccupiedError(Exception):
        """Raised when attempting to occupy an occupied position"""
        pass

    class BadPositionError(Exception):
        """Raised when board position coordinates are invalid"""
        pass

    def __init__(self):
        # two-dimensional list of strings, the board itself
        self.board = [[' '] * 3 for _ in range(3)]

        # integer indicating the number of occupied positions
        self.move_count = 0

    def __str__(self):
        """Generate and return a user-friendly representation of the board"""
        return '\n  board\n' \
               '  1 2 3\n' \
               ' +-+-+-\n' \
               f'A|{"|".join(self.board[0])}\n' \
               ' +-+-+-\n' \
               f'B|{"|".join(self.board[1])}\n' \
               ' +-+-+-\n' \
               f'C|{"|".join(self.board[2])}'

    def __getitem__(self, key) -> None:
        raise NotImplementedError

    def __setitem__(self, key: str, value: str) -> None:
        """Used to occupy a new position on the board

        Parameters:
            key: board position (e.g. 'B2')
            value: character with which to occupy the position

        Raises:
            OccupiedError: if the specified position is already occupied
            BadPositionError: if the specified position is not on the board
        """

        if len(key) != 2  \
                or key[0] not in 'ABC'  \
                or key[1] not in '123':
            raise self.BadPositionError

        # convert letter to number index
        row = ord(key[0].upper()) - ord('A')
        # convert 1-based to 0-based index
        col = int(key[1]) - 1

        if self.board[row][col] != ' ':
            raise self.OccupiedError

        self.board[row][col] = value
        self.move_count += 1

    def has_winner(self) -> bool:
        """Returns True if the board has a winner. False otherwise."""
        for row in self.board:
            if len(set(row)) == 1 and row[0] != ' ':
                return True
        # zip(*list) rotates a matrix
        for column in zip(*self.board):
            if len(set(column)) == 1 and column[0] != ' ':
                return True
        # I hope this is confusing, at least for a moment
        if self.board[1][1] != ' ' \
                and (len({self.board[i][i] for i in range(3)}) == 1
                     or len({self.board[i][2 - i] for i in range(3)}) == 1):
            return True
        return False

    def is_tie_game(self) -> bool:
        """Returns True if the board is fully occupied. False otherwise."""
        return self.move_count >= 9


class Player:
    """Represents a player (client socket) in a tic-tac-toe game"""

    class DisconnectedError(Exception):
        """Raised when a player disconnects or wants to disconnect"""
        pass

    def __init__(self, sock: socket, symbol: str):
        """Accepts an incoming socket connection to create a new player"""
        self.socket, self.address = sock.accept()
        self.symbol = symbol
        print(f'{symbol}<Connected')

    def fileno(self):
        """Required for use with select.select()"""
        return self.socket.fileno()

    def send(self, message: str) -> None:
        """Essentially wraps socket.send()"""
        print(f'{self.symbol}>{message}')
        self.socket.send((message + '\n').encode())

    def recv(self) -> str:
        """Essentially wraps socket.recv()"""
        msg = self.socket.recv(256).decode()
        if msg:
            print(f'{self.symbol}<{msg}', end='')
        return msg

    def quit(self, message: str = '') -> None:
        """Closes the player's socket with an optional closing message"""
        if message:
            self.send(message)
        self.socket.close()

    def __call__(self, board: Board, can_move: bool) -> bool:
        """Player plays the play if the player can play

        Returns: True if the player occupied a new position. False otherwise.
        """
        # ignore case and extra whitespace
        command = self.recv().strip().upper()

        # if we get an empty message, or the player has resigned
        if not command or command == 'R':
            raise Player.DisconnectedError

        if command == '?':
            self.send('\n?-Display this help\n'
                      'R-Resign\n'
                      'M<R><C>-Move where <R> is a row A, B, or C '
                      'and <C> is a column 1, 2, or 3\n'
                      '\tExample Moves: MA1 MC3 MB1')

        elif command.startswith('M'):
            if can_move:
                try:
                    board[command[1:]] = self.symbol
                    return True
                except Board.BadPositionError:
                    self.send('Invalid Move')
                except Board.OccupiedError:
                    self.send('That spot is already taken')
            else:
                self.send('It\'s not your turn')

        # if no previous matches, the command is invalid
        else:
            self.send('Invalid command')
        return False


def sigint_handler(signum, frame) -> None:
    print()  # ^C on previous line
    while frame.f_back and 'players' not in frame.f_locals:
        frame = frame.f_back
    if 'players' in frame.f_locals:
        for player in frame.f_locals['players']:
            player.quit('Server closed')
    sys.exit()


if __name__ == '__main__':
    __main__()
