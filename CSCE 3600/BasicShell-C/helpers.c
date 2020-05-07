#include "helpers.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int get_command_length(char* command[]) {
    // does not include NULL in length
    int len;
    for (len = 0; command[len] != NULL; len++) { }
    return len;
}

// only used when command was copied with dup_command()
void free_command(char* command[]) {
    if (!command) return;

    for (int i = 0; command[i]; i++) {
        free(command[i]);
    }
    free(command);
}

char** dup_command(char* command[]) {
    // duped commands must be freed later
    // if char** fails to allocate:
    //     return NULL
    // if char* within char** fails to allocate:
    //     free all memory allocated to to that point
    //     return NULL

    int len = get_command_length(command);
    char** new_command = malloc(sizeof(char*) * (len + 1));
    if (!new_command) return NULL;

    for (int i = 0; i < len; i++) {
        new_command[i] = strdup(command[i]);
        if (!new_command[i]) {
            free_command(new_command);
            return NULL;
        }
    }
    new_command[len] = NULL;

    return new_command;
}

void print_command(char* command[]) {
    for (int i = 0; command[i] != NULL; i++) {
        printf("%s ", command[i]);
    }
    printf("\n");
}

void print_cmd_seq(char** cmd_seq[]) {
    // helper function for testing
    printf("Command Sequence:\n");
    for (int i = 0; cmd_seq[i] != NULL; i++) {
        printf("\t[");
        for (int j = 0; cmd_seq[i][j] != NULL; j++) {
            printf("\"%s\", ", cmd_seq[i][j]);
        }
        printf("NULL]\n");
    }
}
