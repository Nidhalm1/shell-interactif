#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/execute.h"
#include "../include/builtin.h" // Pour les fonctions builtin_*
#include "../include/command.h" // Pour printerr

/**
 * @brief Parse et exécute une commande simple
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return int Code de retour de la commande
 */
int parse_and_execute_simple(int argc, char **argv)
{
    if (argc == 0 || argv == NULL || argv[0] == NULL)
    {
        printerr("Erreur: commande vide\n");
        return 1;
    }

    // Commandes internes
    if (strcmp(argv[0], "pwd") == 0)
    {
        if (argc > 2)
        {
            printerr("pwd: too many arguments\n");
            return 1;
        }
        if (argc == 2 && strcmp(argv[1], "-p") != 0)
        {
            printerr("pwd: ");
            printerr(argv[1]);
            printerr(": invalid argument\n");
            return 1;
        }
        return builtin_pwd(argv);
    }
    else if (strcmp(argv[0], "cd") == 0)
    {
        if (argc > 2)
        {
            printerr("cd: too many arguments\n");
            return 1;
        }
        const char *path = (argc == 1) ? NULL : argv[1];
        return builtin_cd(path);
    }
    else if (strcmp(argv[0], "ftype") == 0)
    {
        if (argc != 2)
        {
            printerr("ftype: incorrect number of arguments\n");
            return 1;
        }
        if (argv[1] == NULL || strlen(argv[1]) == 0)
        {
            printerr("ftype: no file specified\n");
            return 1;
        }
        return builtin_ftype(argv[1]);
    }

    // Commande externe
    else
    {
        int ret = execute_command(argv[0], STDIN_FILENO, STDOUT_FILENO, argv);
        if (ret == -1)
        {
            printerr("Erreur: échec de l'exécution de la commande externe\n");
            return 1;
        }
        return ret;
    }
}
