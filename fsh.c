#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "parser.h"
#include "execute.h"
#include "prompt.h"

void sigint_handler(int sig)
{
    (void)sig; // Paramètre inutilisé
    // Peut-être ajouter un message si interruption souhaitée
}

int argc(char *input)
{
    int count = 0;
    for (int i = 0; i < strlen(input); i++)
    {
        if (input[i] == ' ')
        {
            count++;
        }
    }
    return count + 1;
}

char **argv(char *input)
{
    int arg_count = argc(input);
    char **args = malloc((arg_count + 1) * sizeof(char *));
    char *input_copy = strdup(input);
    char *arg = strtok(input_copy, " ");
    int i = 0;
    while (arg != NULL)
    {
        args[i] = strdup(arg);
        arg = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;
    free(input_copy);
    return args;
}

int main()
{
    signal(SIGINT, sigint_handler); // Ignorer SIGINT
    int last_return_code = 0;

    while (1)
    {
        // Affichage du prompt
        char *input = prompt(last_return_code);
        if (!input)
        {
            break; // Quitter si l'utilisateur saisit Ctrl-D
        }

        // Parser et exécuter la commande
        printf("Parsing and executing: %d\n", argc(input));
        last_return_code = parse_and_execute(input, argc(input), argv(input));
        free(input);
    }

    return 0;
}
