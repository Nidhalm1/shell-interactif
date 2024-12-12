#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "../include/parser.h"
#include "../include/execute.h"
#include "../include/prompt.h"
#include "../include/command.h"

void sigint_handler(int sig)
{
    (void)sig; // Paramètre inutilisé
    // Peut-être ajouter un message si interruption souhaitée
}

/**
 * @brief Compte le nombre d'arguments dans une chaîne de caractères
 *
 * @param input Chaîne de caractères
 * @return int Nombre d'arguments
 */
int argc(char *input)
{
    int count = 0;
    int len = strlen(input);
    for (int i = 0; i < len; i++)
    {
        if (input[i] == ' ')
        {
            count++;
        }
    }
    return count + 1;
}

/**
 * @brief Crée un tableau d'arguments à partir d'une chaîne de caractères
 *
 * @param input Chaîne de caractères
 * @return char** Tableau d'arguments
 */
char **argv(char *input)
{
    int arg_count = argc(input);
    char **args = malloc((arg_count + 1) * sizeof(char *));
    if (args == NULL)
    {
        perror("malloc");
        return NULL;
    }
    char *input_copy = strdup(input);
    if (input_copy == NULL)
    {
        perror("strdup");
        free(args); // Libérer args si strdup échoue
        return NULL;
    }
    char *arg = strtok(input_copy, " ");
    int i = 0;
    while (arg != NULL)
    {
        args[i] = strdup(arg);
        if (args[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(args[j]);
            }
            free(args);
            free(input_copy);
            return NULL;
        }
        arg = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;
    free(input_copy);
    return args;
}

/**
 * @brief Libère la mémoire allouée pour les arguments
 *
 * @param args Tableau d'arguments
 */
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

/**
 * @brief Fonction principale du shell
 *
 * @return int Code de retour du shell
 */
int main()
{
    signal(SIGINT, sigint_handler);  // Ignorer SIGINT
    signal(SIGTERM, sigint_handler); // Ignorer SIGTERM
    int last_return_code = 0;
    char **args;

    while (1)
    {
        // Affichage du prompt
        char *input = prompt(last_return_code);
        if (!input)
        {
            return last_return_code; // Quitter si l'utilisateur saisit Ctrl-D
        }
        args = argv(input);
        if (input != NULL)
        {
            if (args[0] && (strcmp(args[0], "exit") == 0)) // Vérifier si args[0] est valide
            {
                if (args[2] != NULL){

                    printerr("exit: too many arguments\n");
                    free(input); // Libérer la mémoire allouée pour l'entrée
                    free_args(args);
                    last_return_code = 1;
                    continue;

                }
                free(input); // Libérer la mémoire allouée pour l'entrée
                if (args[1] == NULL)
                {
                    free_args(args);
                    return last_return_code; // Quitte le shell avec le code 0
                }
                else if (args[1] != NULL)
                {
                    int exit_code = atoi(args[1]);
                    free_args(args);
                    return exit_code; // Quitte le shell avec le code passé en argument
                }
            }
        }

        if (input != NULL && strcmp(input, "") == 0)
        {
            free(input);
            continue;
        }
        // Parser et exécuter la commande
        last_return_code = parse_and_execute(argc(input), args);
        free(input);
        free_args(args);
    }

    return 0;
}
