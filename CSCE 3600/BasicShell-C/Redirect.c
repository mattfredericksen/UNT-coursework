#ifndef REDIRECT
#define REDIRECT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#endif /* !REDIRECT */

/*
David Bugbee, CSCE 3600, David James
Started: 4 / 16 / 20, Due Date: 4 / 27 / 20
redOut: Takes in a file, and attempts to redirect output to it.
redIn: Takes in a file, and attempts to redirect input from it
*/

int redOut(char* inFile)
{
	int fd; //Create File Descriptor
	fd = open(inFile, O_CREAT | O_TRUNC | O_WRONLY, 10000); //Open the sent in file
	if(fd != -1) //If no errors
	{
		dup2(fd, 1); //Duplicate Out to stdout
		close(fd); //Close the file
		return 0;
	}
	else
		fprintf(stderr, "File to Redirect Out to does not exist\n"); //Let the user know of error
	return -1;
}

int redIn(char * outFile)
{
	int fd; //Create File Descriptor
	fd = open(outFile, O_RDONLY, 10000); //Open the sent in file
	if(fd != -1) //If no errors
	{
		dup2(fd, 0); //Duplicate In to stdin
		close(fd); //Close the file
		return 0;
	}
	else
		fprintf(stderr, "File to Redirect In to does not exist\n"); //Let the user know of error
	return -1;
}

int redParse(char * command[])
{
	int local = 0;
	int r;
	while(command[local] != NULL)
	{
		if(command[local][0] == '<' || command[local][0] == '>') //Found Redirection
		{
			if(command[local + 1] != NULL)
			{
				pid_t pid = fork(); //Break away for command execution
				if(pid == 0)
				{
					//Redirect
					if(command[local][0] == '<')
						r = redIn(command[local + 1]);
					else if(command[local][0] == '>')
						r = redOut(command[local + 1]);
					
					//Modify for making the system call
					if(r != -1)
					{
					command[local + 1] = NULL;
					command[local] = NULL;
					
						if(execvp(command[0], command) == -1) //Excecute the command
							fprintf(stderr, "Bad Command\n");
					}
					exit(0);
				}
				else if(pid > 0)
					wait((int *)0); //Wait for the child to finish
				else if(pid < 0)
					fprintf(stderr, "Fork Error\n");
			}
			else
			{
				fprintf(stderr, "No File to Redirect to\n");
				r = -1;
			}
			break;
		}
		local++;
	}
	return r;
}