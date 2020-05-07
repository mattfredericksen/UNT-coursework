#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helpers.h"
#include "shell.h"

#define HISTORY_MAX_LEN 20

struct HistoryNode {
    char** command;  // the command that was executed
    struct HistoryNode* next;
}; typedef struct HistoryNode HistoryNode;

struct CommandHistory {
    HistoryNode* first;
    HistoryNode* last;
    HistoryNode* buffer;  // used by history_execute
    int length;
} static history;


static void free_node(HistoryNode* node) {
    if (!node) return;
    // free anything that was malloc'd
    free_command(node->command);
    free(node);
}

// must be called before any other function can be used
void myhistory_init() {
    history.first = NULL;
    history.last = NULL;
    history.buffer = NULL;
    history.length = 0;
}

void myhistory_add(char **command) {
    HistoryNode* info = malloc(sizeof(HistoryNode));
    if (!info) {
        fprintf(stderr, "myhistory: error: failed to allocate memory\n");
        return;
    }
    info->next = NULL;

    // not necessary, since memory has been
    // allocated by alias_get() in main:
    //     info->command = dup_command(command);
    info->command = command;

    if (history.length == HISTORY_MAX_LEN) {
        // free_node(NULL) does nothing
        free_node(history.buffer);
        // pop the front of the list
        history.buffer = history.first;
        history.first = history.first->next;
        // push the new node
        history.last->next = info;
        history.last = info;
    } else if (history.length == 0) {
        // edge case when no history exists
        history.first = history.last = info;
        history.length = 1;
    } else {
        // history exists and is not full
        history.last->next = info;
        history.last = info;
        history.length += 1;
    }
}

void myhistory_destroy() {
    // delete the entire history list
    while (history.first) {
        HistoryNode* tmp = history.first;
        history.first = history.first->next;
        free_node(tmp);
    }
    free_node(history.buffer);
    history.buffer = NULL;

    history.last = NULL;
    history.length = 0;
}

static void myhistory_print() {
    // print each command in the history
    printf("Command History\n");
    HistoryNode* node = history.first;
    for (int i = 1; node != NULL; node = node->next, i++) {
        printf("[%d]  %c", i, (i < 10 ? ' ' : '\0'));
        print_command(node->command);
    }
}

void myhistory_execute(int index) {
    if (index > history.length || index < 1) {
        printf("myhistory: invalid index \"%d\"\n", index);
        return;
    }
    // correct for cycling history:
    // when history is full, without this part a reference to any
    // index would have been replaced by the next command in history
    HistoryNode* node = (history.buffer ? history.buffer : history.first);

    // retrieve the indexed node
    for (int i = 1; i < index; i++) {
        node = node->next;
    }

    // in order to avoid an infinite loop
    if (!strcmp(node->command[0], "myhistory")) {
        fprintf(stderr, "myhistory: execute: no support for previous myhistory commands\n");
        return;
    }

    // add the command to be executed to the history
    char** command = dup_command(node->command);
    if (!command) {
        fprintf(stderr, "myhistory: execute: failed to allocate memory for command\n");
        return;
    }
    myhistory_add(command);
    // process is defined in "shell.c"
    process(command);
}

void myhistory(char **command) {
    // executes history command
    int length = get_command_length(command);
    if (length == 1) {
        // no arguments == print
        myhistory_print();
    } else if (!strcmp(command[1], "-c")) {
        // -c should have no extra arguments
        if (length == 2) {
            myhistory_destroy();
        } else {
            fprintf(stderr, "myhistory: clear: invalid number of arguments\n");
        }
    } else if (!strcmp(command[1], "-e")) {
        // -e should have exactly one extra argument
        if (length != 3) {
            fprintf(stderr, "myhistory: execute: invalid number of arguments\n");
        } else {
            myhistory_execute(atoi(command[2]));
        }
    } else {
        fprintf(stderr, "myhistory: invalid syntax\n");
    }
}
