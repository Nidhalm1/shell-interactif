#ifndef PIPED_COMMANDS_H
#define PIPED_COMMANDS_H

int parse_and_execute_pipe(int argc, char **argv);
int execute_piped_command(const char *command, int fd0, int fd1, char **argv);

#endif