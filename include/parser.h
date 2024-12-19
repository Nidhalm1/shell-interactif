#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdbool.h>

// Parse et exécute une commande
int parse_and_execute(int argc, char **argv);

// Extrait une commande entre accolades
char **get_cmd(char *argv[], size_t size_of_tab, size_t *cmd_size);

// Remplace les variables dans un tableau d'arguments
void replace_variables(char *argv[], size_t size_of_tab, char *replace_var, char *loop_var);

// Affiche une ligne de commande complète
void print_argv_line(char *argv[]);

// Compte le nombre d'arguments dans une chaîne de caractères
int argc(char *input);

// Crée un tableau d'arguments à partir d'une chaîne de caractères
char **argv(char *input);

// Libère la mémoire allouée pour les arguments
void free_args(char **args);

// Vérifie si la commande contient une redirection
bool contientRedi(char **s, int len);

// Exécute une commande après remplacement des variables
int ex_cmd(char *argv[], size_t size_of_tab, char *replace_var, char *loop_var);

// Obtient l'extension d'un fichier
char *get_ext(const char *val);

// Supprime l'extension d'un fichier
char *remove_ext(const char *file);

#endif // PARSER_H
