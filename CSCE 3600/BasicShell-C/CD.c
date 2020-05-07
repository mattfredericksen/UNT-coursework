#ifndef CD
#define CD

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>

#endif //CD

/*
David Bugbee, CSCE 3600, David James
Started: 4 / 11 / 20, Due Date: 4 / 27 / 20
CD: Takes in a path and sends the user there. Returns an error if it doesn't exist, and sends the user to their home directory if NULL is sent
*/

void cd(char* path)
{
	if (path == NULL) //If NULL is passed in for the path
		path = getenv("HOME"); //Set the path to the user's home
	if(chdir(path) == -1) //Send the user to that path
		fprintf(stderr, "Path doesn't exist\n"); //Give an error if the change was unsuccessful
}