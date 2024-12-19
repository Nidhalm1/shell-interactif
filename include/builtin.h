#ifndef BUILTIN_H
#define BUILTIN_H

// Exécute la commande builtin 'pwd'
int builtin_pwd();

// Exécute la commande builtin 'cd'
int builtin_cd(const char *path);

// Exécute la commande builtin 'ftype'
int builtin_ftype(const char *filename);

// Exécute une commande interne avec redirections
int redirections(int fd0, int fd1, char **argv, int argc);

#endif // BUILTIN_H
