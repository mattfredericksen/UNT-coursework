#!/usr/bin/python3.6

"""
A TicTacToe Game Client

Author:  Matthew Fredericksen
Course:  CSCE 3600.003
Date:  4/30/2020

Requires Python 3

Usage: minor5client.py <host> <port>

This program uses TCP INET socket streams to run a tic-tac-toe
client. A server must be running when the client starts.
"""

import sys
from socket import socket
from select import select
from signal import signal, SIGINT


# close the program if the OS is windows
if sys.platform.startswith('win'):
    print('Error: This program cannot run on Windows because '
          'select.select() will not work with sys.stdin')
    sys.exit()


def __main__():
    # requires hostname and port as arguments
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} <hostname> <port>')
        sys.exit()

    # attempt to convert the port to an integer
    try:
        port = int(sys.argv[2])
    except ValueError:
        print(f'Error: "{sys.argv[2]}" is not a number')
        sys.exit()

    # intercept ^C
    signal(SIGINT, sigint_handler)

    # attempt to connect to the server
    server = socket()
    print('Connecting to Server')
    try:
        server.connect((sys.argv[1], port))
    except ConnectionRefusedError:
        print(f'Unable to connect to server {sys.argv[1]}:{port}')
        sys.exit()

    # receiving input from the user and the server
    sources = [server, sys.stdin]

    while True:
        # wait for input
        read_ready, _, errors = select(sources, [], sources)

        # if the user types something, send it to the server
        if sys.stdin in read_ready:
            server.send(sys.stdin.readline(256).encode())

        # if the server sends something, print it
        if server in read_ready:
            msg = server.recv(256).decode()

            if msg:
                print(msg, end='')  # server message contains closing newline

            # if the message was empty, the server has closed
            else:
                break

        if errors:
            print('Unexpected error, exiting')
            break

    server.close()
    return


def sigint_handler(signum, frame):
    print('\nYou resigned from the game')
    # cleanup isn't necessary, but learning this was interesting
    if 'server' in frame.f_locals:
        frame.f_locals['server'].close()
    sys.exit()


if __name__ == '__main__':
    __main__()
