#ifndef MAJOR2_HELPERS_C
#define MAJOR2_HELPERS_C

int get_command_length(char* command[]);
char** dup_command(char* command[]);
void print_command(char* command[]);
void print_cmd_seq(char** cmd_seq[]);
void free_command(char* command[]);

#endif //MAJOR2_HELPERS_C
