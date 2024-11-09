#ifndef BUILTIN_H
#define BUILTIN_H

int is_builtin_command(const char *command);
int execute_builtin_command(const char *command);
int builtin_pwd(void);
int builtin_cd(const char *path);
int builtin_ftype(const char *filename);


#endif // BUILTIN_H
