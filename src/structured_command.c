#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/command.h"
#include "../include/simple_commands.h"
#include "../include/piped_commands.h"
#include "../include/parser.h"
#include "../include/structured_command.h"

/**
 * @brief Parse et exécute une commande structurée, incluant les boucles `for`, les blocs `{}` et les commandes séparées par `;`.
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return int Code de retour de la commande
 */
int parse_and_execute_structured(int argc, char **argv)
{
    int status = 0;      // Code de retour global
    int start = 0;       // Position de départ d'une commande
    int brace_count = 0; // Compteur pour gérer les blocs `{}`

    for (int i = 0; i <= argc; i++)
    {
        // Si on rencontre un point-virgule ou la fin des arguments (i == argc)
        if (i == argc || (strcmp(argv[i], ";") == 0 && brace_count == 0))
        {
            argv[i] = NULL; // Marquer la fin de la commande actuelle

            // Exécuter la commande si elle n'est pas vide
            if (i > start)
            {
                // Afficher la commande complète
                printf("Exécution de la commande : ");
                for (int j = start; j < i; j++)
                {
                    printf("%s ", argv[j]);
                }
                printf("\n");

                // Gérer les blocs `{}` ou les boucles `for`
                if (strcmp(argv[start], "for") == 0)
                {
                    char *var = argv[start + 1];
                    if (strcmp(argv[start + 2], "in") != 0)
                    {
                        fprintf(stderr, "Erreur : syntaxe invalide pour la boucle for.\n");
                        return 1;
                    }

                    // Parcourir les valeurs après `in`
                    for (int j = start + 3; j < i; j++)
                    {
                        if (strcmp(argv[j], "{") == 0)
                        {
                            // Début du bloc à exécuter
                            int block_start = j + 1;
                            int block_end = i - 1;

                            if (strcmp(argv[block_end], "}") != 0)
                            {
                                fprintf(stderr, "Erreur : accolades mal fermées dans la boucle for.\n");
                                return 1;
                            }

                            // Forker un processus pour chaque exécution du bloc
                            for (int k = start + 3; k < j; k++)
                            {
                                setenv(var, argv[k], 1); // Définir la variable d'environnement

                                for (int l = block_start; l < block_end; l++)
                                {
                                    if (strcmp(argv[l], ";") == 0 || l == block_end - 1)
                                    {
                                        argv[l] = NULL; // Terminer la sous-commande

                                        // Créer un processus fils pour exécuter la sous-commande
                                        pid_t pid = fork();
                                        if (pid == -1)
                                        {
                                            perror("Erreur lors du fork");
                                            return 1;
                                        }
                                        else if (pid == 0)
                                        {
                                            // Exécuter la commande dans le processus fils
                                            printf("Exécution de la sous-commande : ");
                                            for (int m = block_start; m <= l; m++)
                                            {
                                                printf("%s ", argv[m]);
                                            }
                                            printf("\n");

                                            // Appel récursif pour exécuter la sous-commande
                                            status = parse_and_execute(l - block_start, &argv[block_start]);
                                            exit(status); // Sortir avec le code de retour de la commande
                                        }
                                        else
                                        {
                                            // Attendre la fin du processus fils
                                            int child_status;
                                            waitpid(pid, &child_status, 0);
                                            if (WIFEXITED(child_status))
                                            {
                                                status = WEXITSTATUS(child_status);
                                            }
                                        }

                                        block_start = l + 1; // Avancer le début pour la prochaine sous-commande
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
                else
                {
                    // Traiter comme une commande classique
                    status = parse_and_execute(i - start, &argv[start]);
                }
            }

            start = i + 1; // Avancer le pointeur de début pour la commande suivante
        }
        else if (i < argc && strcmp(argv[i], "{") == 0)
        {
            brace_count++; // Incrémenter le compteur pour une accolade ouvrante
        }
        else if (i < argc && strcmp(argv[i], "}") == 0)
        {
            brace_count--; // Décrémenter le compteur pour une accolade fermante
        }
    }

    // Vérifier si toutes les accolades ont été fermées
    if (brace_count != 0)
    {
        fprintf(stderr, "Erreur : accolades mal fermées ou non appariées.\n");
        return 1;
    }

    return status; // Retourner le code de statut de la dernière commande
}