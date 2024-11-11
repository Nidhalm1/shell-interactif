#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/builtin.h" // Si les fonctions builtin_* sont déclarées ici
#include "../include/command.h" // Si printerr est déclarée ici

int parse_and_execute_simple(int argc, char **argv)
{
    // Commandes internes
    if (strcmp(argv[0], "pwd") == 0)
    {
        if (argc > 1) // genrer le cas de PWD -P
        {
            printerr("pwd: too many arguments\n");
            return 1; // Code d'erreur
        }
        else
        {
            return builtin_pwd();
        }
    }
    else if (strcmp(argv[0], "cd") == 0)
    {
        if (argc > 2)
        {
            printerr("cd: too many arguments\n");
            return 1; // Code d'erreur
        }
        else if (argc == 1)
        {
            return builtin_cd(NULL);
        }
        else
        {
            return builtin_cd(argv[1]);
        }
    }
    else if (strcmp(argv[0], "ftype") == 0)
    {
        if (argc > 2)
        {
            printerr("ftype: too many arguments\n");
            return 1; // Code d'erreur
        }
        char *filename = strtok(NULL, " ");
        if (!filename)
        {
            printerr("ftype: no file specified\n");
            return 1; // Code d'erreur
        }
        else
        {
            return builtin_ftype(filename);
        }
    }

    else if (strcmp(argv[0], "for") == 0)
    {
        // TODO : Implémenter la commande for
        return 0;
    }

    // Commande externe

    return execute_command(argv[0], argv);
}


