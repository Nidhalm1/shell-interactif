#ifndef BUILTIN_H
#define BUILTIN_H

int is_builtin_command(const char *command);
int execute_builtin_command(const char *command);
int builtin_pwd(char **argv);
int builtin_cd(const char *path);
int builtin_ftype(const char *filename);
void execute_commandd(const char *result, int fd0, int fd1, char **argv);

#endif // BUILTIN_H
