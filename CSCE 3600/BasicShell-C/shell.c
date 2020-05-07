#include "shell.h"

#define MAX_CMD_LEN 512


int main(int argc, char *argv[]) {
    char input[MAX_CMD_LEN];
    char* command[MAX_CMD_LEN / 2];
    char** command_sequence[MAX_CMD_LEN / 2];
    bool exit_called = false;
    FILE* source = stdin;
    long file_position;

    // validate command-line arguments
    if (argc > 2) {
        // customize message later
        fprintf(stderr, "Error: too many arguments\n");
        exit(1);
    } else if (argc == 2) {
        source = fopen(argv[1], "r");
        if (!source) {
            perror("Error opening batch file");
            exit(1);
        }
    }

    // setup
    ign_signals(); // ignores the signals
    strcpy(path,getenv("PATH"));
    alias_init();  // required before using alias
    myhistory_init();  // required before using history

    while (!exit_called) {
        do {
            // prompt when reading from stdin
            if (source == stdin) printf("major2> ");

            if (fgets(input, MAX_CMD_LEN, source) == NULL) {
                // EOF or possibly an error
                // cleanup() and exit()
                //printf("end of file\n");
                builtin_exit();
            }
            // echo lines when reading from batch file
            if (source != stdin) printf("%s", input);
        } while (!strcmp(input, "\n"));  // immediately loop if only 'Enter' was pressed

        parse(input, command, command_sequence);

        for(int i = 0; command_sequence[i] != NULL; i++) {
            // memory allocated by alias_get is recycled in history.c
            char** cmd = alias_get(command_sequence[i]);
            if (!cmd) {
                fprintf(stderr, "ERROR: failed to allocate memory for command\n"
                                "\t\"%s\" will not be executed\n", command_sequence[i][0]);
                continue;
            }
            
            if (!strcmp(cmd[0], "exit")) {
                exit_called = true;
                free_command(cmd);
            } else {
                myhistory_add(cmd);

                // horrifying, I know, but it ensures that
                // no exec call ever sees the batch file
                if (source != stdin) {
                    file_position = ftell(source);
                    fclose(source);
                }

                process(cmd);

                if (source != stdin) {
                    source = fopen(argv[1], "r");
                    fseek(source, file_position, 0);
                }
            }
        }
    }

    builtin_exit();
}


void parse(char input[], char* command[], char** command_sequence[]) {
    int i = 0;  // index for command_sequence
    int j = 0;  // index for command

    char* input_r = input;  // only used for strtok_r
    // sentence refers to sections delimited by ';'
    char* sentence = strtok_r(input_r, ";", &input_r);

    while (sentence != NULL) {
        char* sentence_r = sentence;  // only used for strtok_r
        // word refers to whitespace delimited sections of a sentence
        char* word = strtok_r(sentence_r, " \n", &sentence_r);
        // skip if there wasn't anything between ';'
        if (word) {
            // each index in command_sequence points
            // to the beginning of a NULL terminated
            // char* array, which can be used with exec
            command_sequence[i++] = command + j;

            // delineate each word in the sentence
            while (word != NULL) {
                command[j++] = word;
                word = strtok_r(sentence_r, " \n", &sentence_r);
            }
            command[j++] = NULL;
        }
        // proceed to the next sentence
        sentence = strtok_r(input_r, ";", &input_r);
    }
    command_sequence[i] = NULL;
}


void process(char* command[]) {
    if (!strcmp(command[0], "alias")) {
        alias(command);
    } else if (!strcmp(command[0], "cd")) {
        if(command[1] == NULL) //If there is only CD
            cd(NULL);
        else if(command[2] != NULL) //If there is atleast 2 options
            perror("Too many parameters");
        else //Just 1 path
            cd(command[1]);
    } else if (!strcmp(command[0], "myhistory")) {
        myhistory(command);
    } else if (!strcmp(command[0], "path")) {
	    if(command[1] == NULL)
	    {
		    display();
	    }
	    else
        	Paths(command);
    } else if (command_search(command, "<")) {
        redParse(command);
    } else if (command_search(command, ">")) {
        redParse(command);
    } else if (command_search(command, "|")) {
        printf("Piping is currently unsupported\n");
        // pipe_parse(command);
    } else {
        pid_t pid = fork();
        if (pid > 0) {  // parent
            wait(NULL);  // wait for child to finish exec
        } else if (pid == 0) {  // child
            execvp(command[0], command);
            // we only get here if exec fails
            fprintf(stderr,"%s: command not found\n", command[0]);
            exit(1);
        } else if (pid < 0) {  // error
            perror("Failed to fork");
        }
    }
}


bool command_search(char* command[], char* word) {
    // returns true if command contains word
    for (int i = 0; command[i] != NULL; i++) {
        if (!strcmp(command[i], word)) return true;
    }
    return false;
}


void builtin_exit() {
    myhistory_destroy();
    alias_destroy();

    exit(EXIT_SUCCESS);
}
