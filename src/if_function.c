#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/builtin.h" // Si les fonctions builtin_* sont déclarées ici
#include "../include/command.h" // Si printerr est déclarée ici
#include "../include/parser.h"
#include "../include/loop.h"
#include <sched.h>

/**
 * @brief Trouve la commande entre les accolades
 *
 * @param argv Tableau d'arguments
 * @param size_of_tab Taille du tableau d'arguments
 * @param start_index Index de départ
 * @return char* Commande trouvée
 */
char *find_cmd(char *argv[], size_t size_of_tab, size_t *start_index)
{
    size_t size_of_cmd = 0;
    int brace_count = 0;
    bool find = false;

    for (size_t i = *start_index; i < size_of_tab; i++)
    {
        if (strcmp(argv[i], "{") == 0)
        {
            brace_count++;
            if (brace_count == 1)
            {
                find = true;
                continue; // Skip the first opening brace
            }
        }
        if (find)
        {
            size_of_cmd += strlen(argv[i]) + 1;
        }
        if (strcmp(argv[i], "}") == 0 && find)
        {
            brace_count--;
            if (brace_count == 0)
            {
                break;
            }
        }
    }

    if (size_of_cmd == 0)
    {
        return NULL;
    }

    char *cmd = malloc(size_of_cmd * sizeof(char));
    if (cmd == NULL)
    {
        printerr("Erreur d'allocation mémoire pour cmd\n");
        exit(EXIT_FAILURE);
    }

    size_t parc = 0;
    brace_count = 0;
    find = false;
    for (size_t i = *start_index; i < size_of_tab; i++)
    {
        if (strcmp(argv[i], "{") == 0)
        {
            brace_count++;
            if (brace_count == 1)
            {
                find = true;
                continue; // Skip the first opening brace
            }
        }
        if (find)
        {
            if (parc > 0)
                cmd[parc++] = ' ';
            strcpy(&cmd[parc], argv[i]);
            parc += strlen(argv[i]);
        }
        if (strcmp(argv[i], "}") == 0 && find)
        {
            brace_count--;
            if (brace_count == 0)
            {
                *start_index = i + 1;
                break;
            }
        }
    }
    cmd[parc] = '\0';
    return cmd;
}

/**
 * @brief Exécute une commande de test
 *
 * @param cmd Commande à exécuter
 * @return int Code de retour de la commande
 */
int exec_test(char *cmd)
{
    char *args[128];
    size_t i = 0;

    char *token = strtok(cmd, " ");
    while (token != NULL && i < 127)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    return parse_and_execute(i, args);
}

/**
 * @brief Fonction qui gère la commande if
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return int Code de retour de la commande
 */
int if_function(int argc, char **argv)
{
    if (argc < 4)
    {
        printerr("Usage : if TEST cmd1 [else cmd2]\n");
        return 1;
    }

    // Construire la condition (TEST) à partir des arguments avant '{'
    size_t test_size = 0;
    for (size_t i = 1; i < (size_t)argc; i++)
    {
        if (strcmp(argv[i], "{") == 0)
        {
            break;
        }
        test_size += strlen(argv[i]) + 1;
    }

    if (test_size == 0)
    {
        printerr("Erreur : Condition de test non trouvée.\n");
        return 1;
    }

    char *test_cmd = malloc(test_size * sizeof(char));
    if (test_cmd == NULL)
    {
        printerr("Erreur d'allocation mémoire pour test_cmd\n");
        exit(EXIT_FAILURE);
    }

    size_t parc = 0;
    for (size_t i = 1; i < (size_t)argc; i++)
    {
        if (strcmp(argv[i], "{") == 0)
        {
            break;
        }
        if (parc > 0)
            test_cmd[parc++] = ' ';
        strcpy(&test_cmd[parc], argv[i]);
        parc += strlen(argv[i]);
    }
    test_cmd[parc] = '\0';

    // Exécuter la condition (TEST)
    int test = exec_test(test_cmd);
    free(test_cmd);

    size_t start_index = 0;
    char *cmd1 = find_cmd(argv, argc, &start_index);
    printf("cmd1: %s\n", cmd1);
    if (cmd1 == NULL)
    {
        printerr("Erreur : Commande non trouvée pour le if.\n");
        return 1;
    }

    if (test == 0)
    {
        // Exécuter la première commande
        char *args[128];
        size_t i = 0;

        char *token = strtok(cmd1, " ");
        while (token != NULL && i < 127)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        int ret = parse_and_execute(i, args);
        free(cmd1);
        return ret;
    }
    else
    {
        // Vérifier si une commande 'else' existe
        if (start_index < ((size_t)argc) && strcmp(argv[start_index], "else") == 0)
        {
            start_index++;
            char *cmd2 = find_cmd(argv, argc, &start_index);
            if (cmd2 == NULL)
            {
                printerr("Erreur : Commande non trouvée pour le else.\n");
                free(cmd1);
                return 1;
            }

            char *args[128];
            size_t i = 0;

            char *token = strtok(cmd2, " ");
            while (token != NULL && i < 127)
            {
                args[i++] = token;
                token = strtok(NULL, " ");
            }
            args[i] = NULL;

            int ret = parse_and_execute(i, args);
            free(cmd1);
            free(cmd2);
            return ret;
        }
    }

    free(cmd1);
    return 0;
}