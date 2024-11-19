#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/command.h"
#include "../include/simple_commands.h"
#include "../include/piped_commands.h"
#include "../include/parser.h"

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
