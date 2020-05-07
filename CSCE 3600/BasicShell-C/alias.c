#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "helpers.h"

struct AliasNode {
    char* key;  // the word that will be swapped out
    char** value;  // the phrase that will be swapped in
    int size;  // number of words in the phrase
    struct AliasNode* next;
}; typedef struct AliasNode AliasNode;

struct AliasList {
    AliasNode* first;
    AliasNode* last;
} static aliases;  // keeps track of all aliases for the shell

static void free_node(AliasNode* node) {
    // free everything that was malloc'd
    free(node->key);
    free_command(node->value);
    free(node);
}

void alias_init() {
    aliases.first = NULL;
    aliases.last = NULL;
}

void alias_print() {
    // prints in the same format as bash's alias
    // except it isn't alphabetized
    for (AliasNode* node = aliases.first; node; node = node->next) {
        printf("alias %s='", node->key);
        for (int i = 0; node->value[i]; i++) {
            printf("%s ", node->value[i]);
        }
        printf("\b'\n");
    }
}

void alias_destroy() {
    // frees every alias and resets the `aliases` list
    while (aliases.first) {
        AliasNode* tmp = aliases.first;
        aliases.first = aliases.first->next;
        free_node(tmp);
    }
    aliases.last = NULL;
}

void alias_remove(char* key) {
    // removes a single alias from the `aliases` linked list
    AliasNode* previous = NULL;
    AliasNode* current = NULL;
    for (current = aliases.first; current != NULL; previous = current, current = current->next) {
        // if we find the key in our aliases list
        if (!strcmp(key, current->key)) {
            // if it is the first key
            if (current == aliases.first) {
                // the list is now empty
                aliases.first = aliases.last = NULL;
            }
            // if it is not the first key
            else {
                // adjust predecessor's `next` pointer
                previous->next = current->next;
                // if it is the last key
                if (current == aliases.last) {
                    // update the aliases list `last` pointer
                    aliases.last = previous;
                }
            }
            // the node has been extracted, so free it
            free_node(current);
            return;
        }
    }
    fprintf(stderr, "alias: alias for \"%s\" does not exist\n", key);
}

void alias_add(char* key, char* value[]) {
    AliasNode* new_alias = NULL;

    // check if the alias key already exists
    for (AliasNode* node = aliases.first; node != NULL; node = node->next) {
        if (!strcmp(key, node->key)) {
            // assign the located alias to new_alias
            new_alias = node;
            break;
        }
    }
    // if the alias already existed
    if (new_alias) {
        // keep the key, free the value so it can be replaced
        free_command(new_alias->value);
        if (!(new_alias->value = dup_command(value))) {
            fprintf(stderr, "alias: failed to allocate memory for new alias\n");
            return;
        }
    }
    // if the node did not already exist
    else {
        // initialize a new one
        if (!(new_alias = malloc(sizeof(AliasNode)))
                || !(new_alias->key = strdup(key))
                || !(new_alias->value = dup_command(value))) {
            fprintf(stderr, "alias: failed to allocate memory for new alias\n");
            return;
        }

        new_alias->next = NULL;

        // if our `aliases` list was not empty
        if (aliases.last) {
            // append the new alias to the end
            aliases.last->next = new_alias;
        }
        // if the list was empty
        else {
            // our new alias is the first one
            aliases.first = new_alias;
        }
        // regardless, the last alias is now the new alias
        aliases.last = new_alias;
    }

    new_alias->size = get_command_length(value);
}

void alias(char* command[]) {
    // executes alias command
    int length = get_command_length(command);
    if (length == 1) {
        // no arguments == print
        alias_print();
    } else if (!strcmp(command[1], "-c")) {
        // -c should have no extra arguments
        if (length == 2) {
            alias_destroy();
        } else {
            fprintf(stderr, "alias: invalid number of arguments for clear\n");
        }
    } else if (!strcmp(command[1], "-r")) {
        // -r should have exactly one extra argument
        if (length != 3) {
            fprintf(stderr, "alias: invalid number of arguments for remove\n");
        } else {
            alias_remove(command[2]);
        }
    } else if (strstr(command[1], "='")) {
        // input validation is performed, but is not exhaustive
        int last_length = strlen(command[length - 1]);
        if (command[length - 1][last_length - 1] != '\'') {
            fprintf(stderr, "alias: missing expected single-quote as last character\n");
            return;
        } else {
            // remove trailing quote to prepare for processing
            command[length - 1][last_length - 1] = '\0';
        }

        char* key = strtok(command[1], "=");
        // might fail with invalid quote usage
        command[1] = strtok(NULL, "'");
        alias_add(key, command + 1);
    } else {
        // if you get here, it isn't my fault
        fprintf(stderr, "alias: invalid syntax\n");
    }
}

char** alias_get(char* command[]) {
    // This function returns a malloc'd copy of the
    // command that is passed to it. If an alias is
    // found for the command, substitution occurs.

    // this will be returned after being malloc'd
    char** aliased_command;

    // starting size for malloc is the size of the command
    // +1 for NULL termination
    int size = get_command_length(command) + 1;
    AliasNode* node;

    // find the alias if it exists
    for (node = aliases.first; node != NULL; node = node->next) {
        if (!strcmp(command[0], node->key)) {
            // found an alias, so add it's size for malloc
            // -1 because one word from the original command is removed
            size += node->size - 1;
            break;
        }
    }

    // this will be freed by myhistory
    aliased_command = malloc(sizeof(char*) * size);
    if (!aliased_command) return NULL;
        // fprintf(stderr, "alias: failed to allocate memory for retrieved command\n");

    // populate the new command from the alias and the old command
    int index = 0;
    if (node) {
        for (; node->value[index] != NULL; index++) {
            aliased_command[index] = strdup(node->value[index]);
            if (!aliased_command[index]) {
                // free memory allocated up to this point
                free_command(aliased_command);
                // fprintf(stderr, "alias: failed to allocate memory for aliased command\n");
                return NULL;
            }
        }
    }
    for (int i = (node ? 1 : 0); command[i] != NULL; i++, index++) {
        aliased_command[index] = strdup(command[i]);
        if (!aliased_command[index]) {
            free_command(aliased_command);
            // fprintf(stderr, "alias: failed to allocate memory for command\n");
            return NULL;
        }
    }
    aliased_command[index] = NULL;
    return aliased_command;
}
