#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/command.h"
#include "../include/simple_commands.h"
#include "../include/piped_commands.h"
#include "../include/parser.h"

/**
 * @brief Parse et exécute une commande structurée
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return int Code de retour de la commande
 */
int parse_and_execute_structured(int argc, char **argv)
{
    int status = 0;
    int start = 0;

    for (int i = 0; i <= argc; i++)
    {
        if (i == argc || strcmp(argv[i], ";") == 0)
        {
            argv[i] = NULL; // Séparer la commande actuelle

            // Exécuter la commande entre start et i-1
            if (i > start)
            {

                printf("Commande à exécuter : ");
                for (int j = start; j < i; j++)
                {
                    printf("%s ", argv[j]);
                }
                printf("\n");

                int ret = parse_and_execute(i - start, argv + start);
                if (ret != 0)
                {
                    status = ret; // Mise à jour du statut
                }
            }

            start = i + 1; // Prochaine commande
        }
    }

    return status; // Retourne le statut de la dernière commande
}
