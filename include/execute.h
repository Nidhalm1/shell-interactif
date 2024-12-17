#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdbool.h>

// Exécute une commande externe
int execute_command(const char *command, int fd0, int fd1, char **argv);
// Obtient la longueur d'un tableau d'arguments
int lenn(char **s);

#endif // EXECUTE_H
