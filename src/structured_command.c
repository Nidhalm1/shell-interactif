#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/command.h"
#include "../include/simple_commands.h"
#include "../include/piped_commands.h"
#include "../include/parser.h"
#include "../include/structured_command.h"

/**
 * @brief Parse et exécute une commande structurée, en gérant correctement les boucles imbriquées et les blocs entre accolades.
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return int Code de retour de la commande
 */
int parse_and_execute_structured(int argc, char **argv)
{
    int status = 0;
    int start = 0;
    int brace_count = 0; // Compteur pour gérer les blocs `{}`

    for (int i = 0; i <= argc; i++)
    {
        // Vérifier la fin de la commande ou un séparateur `;` (hors des blocs `{}`)
        if (i == argc || (brace_count == 0 && strcmp(argv[i], ";") == 0))
        {
            argv[i] = NULL; // Séparer la commande actuelle

            if (i > start) // S'il y a une commande à exécuter
            {
                printf("Exécution de la commande: ");
                for (int j = start; j < i; j++)
                {
                    printf("%s ", argv[j]);
                }
                printf("\n");

                // Vérifier si c'est une commande `for`
                if (strcmp(argv[start], "for") == 0)
                {
                    // Traiter une commande `for` comme une entité structurée
                    int ret = parse_and_execute(i - start, argv + start);
                    if (ret != 0)
                    {
                        status = ret; // Mise à jour du statut si erreur
                    }
                }
                else
                {
                    // Sinon, exécuter la commande normalement
                    int ret = parse_and_execute(i - start, argv + start);
                    if (ret != 0)
                    {
                        status = ret; // Mise à jour du statut si erreur
                    }
                }
            }

            start = i + 1; // Passer à la prochaine commande
        }
        else if (i < argc && strcmp(argv[i], "{") == 0)
        {
            brace_count++; // Entrée dans un bloc
        }
        else if (i < argc && strcmp(argv[i], "}") == 0)
        {
            brace_count--; // Sortie d'un bloc

            // Si le compteur d'accolades devient négatif, erreur de syntaxe
            if (brace_count < 0)
            {
                fprintf(stderr, "Erreur : accolades mal appariées.\n");
                return 1;
            }
        }
    }

    // Vérifier si toutes les accolades ont été fermées
    if (brace_count != 0)
    {
        fprintf(stderr, "Erreur : accolades mal fermées ou non appariées.\n");
        return 1;
    }

    return status; // Retourner le statut de la dernière commande
}
