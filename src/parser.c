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
#include "../include/structured_command.h"
#include "../include/parser.h"
#include <stdbool.h>

/**
 * @brief Parse et exécute une commande
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return int Code de retour de la commande
 */
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

    // Si la première commande est "For", on lance une boucle
    if (argv[0] != NULL && strcmp(argv[0], "for") == 0)
    {
        loop_options *options = option_struc(argc, argv);
        return loop_function(argv[3], argv, argc, options);
    }

    if (argv[0] != NULL && strcmp(argv[0], "if") == 0)
    {
        return if_function(argc, argv);
    }

    // Parcours des arguments pour détecter une structure de commande
    for (int i = 0; i < argc; i++)
    {
        // Vérifie si l'argument courant est un point-virgule
        if (argv[i] != NULL && strcmp(argv[i], ";") == 0)
        {
            return parse_and_execute_structured(argc, argv);
        }
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

    // Commande externe : appel à la fonction pour exécuter la commande simple
    // si elle contient des redirections
    if (contientRedi(argv, argc) && (strcmp(argv[0], "pwd") == 0 || strcmp(argv[0], "ftype") == 0))
    {
        return redirections(STDIN_FILENO, STDOUT_FILENO, argv, argc);
    }
    else
    {
        return parse_and_execute_simple(argc, argv);
    }
}

bool contientRedi(char **s, int len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (strcmp(s[i], ">") == 0 || strcmp(s[i], "<") == 0 || strcmp(s[i], ">>") == 0 || strcmp(s[i], "2>") == 0 || strcmp(s[i], "2>>") == 0 || strcmp(s[i], ">|") == 0)
        {
            return true;
        }
    }
    return false;
}

void free_args(char **args)
{
    if (args)
    {
        for (int i = 0; args[i] != NULL; i++)
        {
            free(args[i]);
        }
        free(args);
    }
}