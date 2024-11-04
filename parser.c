#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"

int parse_and_execute(char *input)
{
    char *command = strtok(input, " ");
    if (command == NULL)
        return 0;

    // Vérification des commandes internes
    if (strcmp(command, "exit") == 0)
    {
        exit(0);
    }
    else if (strcmp(command, "pwd") == 0)
    {
        return builtin_pwd();
    }
    else if (strcmp(command, "cd") == 0)
    {
        return builtin_cd(strtok(NULL, " "));
    }
    else if (strcmp(command, "ftype") == 0)
    {
        return builtin_ftype(strtok(NULL, " "));
    }

    // Exécuter une commande externe
    return execute_command(command);
}
