#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/builtin.h" // Si les fonctions builtin_* sont déclarées ici
#include "../include/command.h" // Si printerr est déclarée ici
#include "../include/simple_commands.h"
#include "../include/loop.h"
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/piped_commands.h"
#include "../include/if_function.h"

int parse_and_execute(int argc, char **argv)
{
    // Si aucun argument n'est passé, on sort immédiatement
    if (argc == 0)
    {
        return 0;
    }
    // Si un seul argument, on le traite comme une commande simple
    else if (argc == 1)
    {
        return parse_and_execute_simple(argc, argv);
    }

    // Parcours des arguments pour détecter un pipe (|) et rediriger l'exécution
    for (int i = 0; i < argc; i++)
    {
        // Vérifie si l'argument courant est un pipe
        if (argv[i] != NULL && strcmp(argv[i], "|") == 0)
        {
            return parse_and_execute_pipe(argc, argv);
        }
    }

    // Si la première commande est "For", on lance une boucle
    if (argv[0] != NULL && strcmp(argv[0], "for") == 0)
    {
        loop_options *options = option_struc(argc, argv);
        return loop_function(argv[3], argv, argc, options);
    }

    if (argv[0] != NULL && strcmp(argv[0], "if") == 0)
    {
        // TODO : Implémenter la commande if
        return if_function(argc, argv);
    }

    // Commande externe : appel à la fonction pour exécuter la commande simple
    return parse_and_execute_simple(argc, argv);
}
