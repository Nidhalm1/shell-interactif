#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/execute.h"
#include "../include/builtin.h" // Si les fonctions builtin_* sont déclarées ici
#include "../include/command.h" // Si printerr est déclarée ici

/**
 * @brief Parse et exécute une commande simple
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return int Code de retour de la commande
 */
int parse_and_execute_simple(int argc, char **argv)
{
    // Commandes internes
    if (strcmp(argv[0], "pwd") == 0)
    {
        if (!contientRedi(argv, argc))//elle contient pas des redirections
        {
            if (argc > 2)
            {
                printerr("pwd: too many arguments\n");
                return 1; // Code d'erreur
            }
            if (argc == 2)
            {
                if (strcmp(argv[1], "-p") != 0)
                {
                    printerr("pwd: ");
                    printerr(argv[1]);
                    printerr(": invalid␣argument\n");
                    return 1;
                }
                return builtin_pwd(argv);
            }
        }
        else{

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
        char *filename = argv[1];
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

    // Commande externe

    return execute_command(argv[0], STDIN_FILENO, STDOUT_FILENO, argv);
}
