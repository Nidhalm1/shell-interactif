#ifndef PIPED_COMMANDS_H
#define PIPED_COMMANDS_H

// Parse et exécute une commande avec pipe
int parse_and_execute_pipe(int argc, char **argv);
int execute_commandPourPipe(const char *command, int fd0, int fd1, char **argv);

#endif // PIPED_COMMANDS_H
