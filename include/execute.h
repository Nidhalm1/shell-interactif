#ifndef EXECUTE_H
#define EXECUTE_H

int execute_command(const char *command, int fd0, int fd1, char **argv);
void supprimeS(char **s, char *ss);
int lenn (char **s);

#endif // EXECUTE_H
