#include "shell.h"
char path[100000];

void display()
{
    printf("%s\n",getenv("PATH")); // will display the current path

}
void append_path(char *add)
{
    char* hold = ":";
    //strcat(add,hold); //Adds ':' to the end of add
    strcat(path,hold);//Adds 'add' to the end of path
    strcat(path,add);
    setenv("PATH", path,1); // sets the environemnt variable to path now;
}
void remove_path(char *remove)
{
    strcat(remove,":"); // adds ':' to the end of "remove" to accomadate to the ':'
    char *str2; // used for the pointer variabled grabbed by strstr
    while((str2 = strstr(path,remove)))// continues while str2 is not NULL
    {
        str2 ='\0'; // sets str2 to a NULL variable;
        strcat(path,str2);
    }

    setenv("PATH",path,1); // will change the Path

}
void ign_signals()//will ignore these signals
{
        signal(SIGINT, SIG_IGN);// for ctrl+x
        signal(SIGTSTP, SIG_IGN);// ctrl+z
//      signal(SIGQUIT, SIG_IGN);// CTRL +\'
        signal(SIGTTIN, SIG_IGN); // Delivered to background process that is trying to read input from keyboard
        signal(SIGTTOU, SIG_IGN);// Delivered to background process that is trying to write output to the terminal

}
void dfl_signals() // gives the signals back their power
{
        signal(SIGINT, SIG_DFL);// for ctrl+x
        signal(SIGTSTP, SIG_DFL);// ctrl+z
       // signal(SIGQUIT, SIG_DFL); // CTRL +\'
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);

}
/* not working
void signals(char* command[])
{

	pid_t pid = fork();
        if (pid > 0) {  // parent
	    setpgid(pid,pid);	// puts child in their own Process group
	    tcsetpgrp(0,pid); // set back to foreground	
            wait(NULL);  // wait for child to finish exec
        } else if (pid == 0) {  // child
            dfl_signals(); // put the signals back in 
            setpgid(0,0); // 
            execvp(command[0], command);
            // we only get here if exec fails
            printf("%s: command not found\n", command[0]);
            exit(1);
        } else if (pid < 0) {  // error
            perror("Failed to fork");
        }


}
*/
void Paths(char* command[])
{

	if(strcmp(command[1],"+") == 0)
	{
		append_path(command[2]);
	}
	if(strcmp(command[1],"-") == 0)
	{
		remove_path(command[2]);	
	}
	return;
		 
}

