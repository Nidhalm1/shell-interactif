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
                /* printf("Exécution de la commande: ");
                 for (int j = start; j < i; j++)
                 {
                     printf("%s ", argv[j]);
                 }
                 printf("\n");*/

                // Vérifier si c'est une commande `for`
                if (strcmp(argv[start], "for") == 0)
                {
                    // Traiter une commande `for` comme une entité structurée
                    int ret = handle_for_command(i - start, argv + start);
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

/**
 * @brief Traite une commande "for" et ses éventuels blocs imbriqués.
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return int Code de retour
 */
int handle_for_command(int argc, char **argv)
{
    // Vérifier la syntaxe minimale de la commande `for`
    if (argc < 5 || strcmp(argv[2], "in") != 0 || strcmp(argv[argc - 1], "}") != 0)
    {
        fprintf(stderr, "Erreur : syntaxe invalide pour la boucle 'for'.\n");
        return 1;
    }

    // Identifier le bloc `{ ... }` à partir de l'accolade ouvrante `{`
    int block_start = -1;
    for (int i = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "{") == 0)
        {
            block_start = i + 1;
            break;
        }
    }

    if (block_start == -1 || block_start >= argc - 1)
    {
        fprintf(stderr, "Erreur : bloc de commande manquant ou mal formé dans 'for'.\n");
        return 1;
    }

    // Exécuter la boucle pour chaque élément après `in`
    for (int i = 3; i < block_start - 1; i++)
    {
        char *variable = argv[1]; // Nom de la variable (par exemple, `f`)
        char *value = argv[i];    // Valeur actuelle de la boucle (par exemple, un fichier ou un chemin)

        // Remplacer la variable dans le bloc de commandes par sa valeur
        replace_variables(argv + block_start, argc - block_start - 1, value, variable);

        // Exécuter récursivement le bloc de commandes
        int ret = parse_and_execute_structured(argc - block_start - 1, argv + block_start);
        if (ret != 0)
        {
            return ret; // Retourner une erreur si une itération échoue
        }
    }

    return 0; // Succès
}
