#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../include/loop.h"
#include <../include/builtin.h>
#include <../include/parser.h>
#include <../include/execute.h>
#include <../include/command.h>

bool apply_filters(const struct dirent *entry, const struct stat *st, loop_options *options)
{
    // Filtrer les fichiers cachés
    if (!options->opt_A && entry->d_name[0] == '.')
    {
        return false;
    }

    // Filtrer par type
    if (options->type != NULL)
    {
        char type = options->type[0];
        if ((type == 'f' && !S_ISREG(st->st_mode)) ||
            (type == 'd' && !S_ISDIR(st->st_mode)) ||
            (type == 'l' && !S_ISLNK(st->st_mode)) ||
            (type == 'p' && !S_ISFIFO(st->st_mode)))
        {
            return false;
        }
    }

    // Filtrer par extension
    if (options->ext != NULL)
    {
        char *ext = strrchr(entry->d_name, '.');
        if (!ext || strcmp(ext + 1, options->ext) != 0)
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief Initialise une structure loop_options
 *
 * @return loop_options* Pointeur vers la structure initialisée
 */
loop_options *init_struc()
{
    loop_options *opt_struc = malloc(sizeof(loop_options));
    if (opt_struc == NULL)
    {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }
    opt_struc->opt_A = false;
    opt_struc->opt_r = false;
    opt_struc->ext = NULL;
    opt_struc->type = NULL;
    opt_struc->max = -1;
    return opt_struc;
}

/**
 * @brief Analyse les options de ligne de commande et remplit une structure loop_options
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return loop_options* Pointeur vers la structure avec les options analysées ou NULL en cas d'erreur
 */
loop_options *option_struc(int argc, char *argv[])
{
    loop_options *opt_struc = init_struc();
    int opt;
    optind = 4;
    while ((opt = getopt(argc, argv, "Are:t:p:")) != -1)
    {
        switch (opt)
        {
        case 'A':
            opt_struc->opt_A = true;
            break;
        case 'r':
            opt_struc->opt_r = true;
            break;
        case 'e':
            opt_struc->ext = optarg;
            break;
        case 't':
            opt_struc->type = optarg;
            break;
        case 'p':
            opt_struc->max = atoi(optarg);
            break;
        case '?':
            free(opt_struc);
            return NULL;
        default:
            break;
        }
    }

    return opt_struc;
}

/**
 * @brief Extrait une commande entre accolades `{}` en détectant les imbrications.
 *
 * @param argv Tableau d'arguments
 * @param size_of_tab Taille du tableau d'arguments
 * @param cmd_size Pointeur pour stocker la taille de la commande extraite
 * @return char** Tableau contenant la commande extraite ou NULL si erreur
 */
char **get_cmd(char *argv[], size_t size_of_tab, size_t *cmd_size)
{
    if (argv == NULL || cmd_size == NULL || size_of_tab == 0)
    {
        fprintf(stderr, "Erreur : paramètres invalides.\n");
        return NULL;
    }

    int brace_count = 0;
    size_t start_index = 0, end_index = 0;
    bool in_block = false;

    // Recherche des indices des accolades ouvrantes et fermantes
    for (size_t i = 0; i < size_of_tab; i++)
    {
        if (strcmp(argv[i], "{") == 0)
        {
            if (brace_count == 0)
            {
                start_index = i + 1; // Position après la première accolade ouvrante
                in_block = true;
            }
            brace_count++;
        }
        else if (strcmp(argv[i], "}") == 0)
        {
            brace_count--;
            if (brace_count == 0 && in_block)
            {
                end_index = i; // Position de la dernière accolade fermante
                break;
            }
        }

        // Vérifier si on dépasse la limite du tableau
        if (brace_count < 0)
        {
            fprintf(stderr, "Erreur : accolades mal appariées (trop de fermantes).\n");
            return NULL;
        }
    }

    // Vérification des accolades
    if (!in_block || brace_count != 0 || end_index <= start_index)
    {
        fprintf(stderr, "Erreur : accolades mal formées ou absentes.\n");
        return NULL;
    }

    // Taille du bloc à extraire
    *cmd_size = end_index - start_index;

    if (*cmd_size == 0)
    {
        fprintf(stderr, "Erreur : aucune commande entre accolades.\n");
        return NULL;
    }

    // Allocation de mémoire pour la commande extraite
    char **cmd = malloc((*cmd_size + 1) * sizeof(char *));
    if (cmd == NULL)
    {
        perror("Erreur d'allocation mémoire");
        return NULL;
    }

    // Copie des arguments entre les accolades
    for (size_t i = 0; i < *cmd_size; i++)
    {
        cmd[i] = strdup(argv[start_index + i]);
        if (cmd[i] == NULL)
        {
            perror("Erreur d'allocation mémoire");
            // Libération en cas d'échec
            for (size_t j = 0; j < i; j++)
            {
                free(cmd[j]);
            }
            free(cmd);
            return NULL;
        }
    }

    cmd[*cmd_size] = NULL; // Terminaison du tableau
    return cmd;
}
/**
 * @brief Parcourt un répertoire et exécute des commandes sur les fichiers correspondant aux options
 *
 * @param path Chemin du répertoire
 * @param argv Tableau d'arguments
 * @param size_of_tab Taille du tableau d'arguments
 * @param options Structure contenant les options de filtrage
 * @return int Code de retour (0 en cas de succès, 1 en cas d'échec)
 */
int loop_function(char *path, char *argv[], size_t size_of_tab, loop_options *options)
{
    if (options == NULL)
    {
        printerr("Option non reconnue.\n");
        return 1;
    }

    DIR *dirp = opendir(path);
    if (dirp == NULL)
    {
        perror("Erreur d'ouverture du répertoire");
        return 1;
    }

    struct dirent *entry;
    struct stat st;
    char **cmd = NULL;
    size_t cmd_size = 0;

    cmd = get_cmd(argv, size_of_tab, &cmd_size);
    if (cmd == NULL)
    {
        closedir(dirp);
        return 1;
    }

    int nb_process_runned = 0;
    int max_return_code = 0;

    while ((entry = readdir(dirp)) != NULL)
    {
        char full_path[MAX_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (lstat(full_path, &st) == -1)
        {
            perror("Erreur avec lstat");
            continue;
        }

        if (!apply_filters(entry, &st, options))
        {
            continue;
        }

        // Substituer la variable de boucle
        char **substituted_args = malloc((cmd_size + 1) * sizeof(char *));
        if (!substituted_args)
        {
            perror("malloc");
            continue;
        }

        for (size_t i = 0; i < cmd_size; i++)
        {
            substituted_args[i] = strdup(cmd[i]);
            replace_variables(&substituted_args[i], 1, full_path, argv[1]);
        }
        substituted_args[cmd_size] = NULL;

        // Exécuter les commandes
        pid_t p = fork();
        switch (p)
        {
        case -1:
            perror("Erreur lors du fork");
            break;

        case 0:
            parse_and_execute(cmd_size, substituted_args);
            exit(EXIT_SUCCESS);

        default:
            nb_process_runned++;
            break;
        }

        if (options->max > 0 && nb_process_runned >= options->max)
        {
            int status;
            wait(&status);
            nb_process_runned--;
            if (WIFEXITED(status))
            {
                int ret_code = WEXITSTATUS(status);
                if (ret_code > max_return_code)
                {
                    max_return_code = ret_code;
                }
            }
        }

        // Libérer les arguments substitués
        for (size_t i = 0; i < cmd_size; i++)
        {
            free(substituted_args[i]);
        }
        free(substituted_args);
    }

    while (nb_process_runned > 0)
    {
        int status;
        wait(&status);
        nb_process_runned--;
        if (WIFEXITED(status))
        {
            int ret_code = WEXITSTATUS(status);
            if (ret_code > max_return_code)
            {
                max_return_code = ret_code;
            }
        }
    }

    free(cmd);
    closedir(dirp);
    return max_return_code;
}

/**
 * @brief Remplace les variables dans un tableau d'arguments par une valeur donnée
 *
 * @param argv Tableau d'arguments
 * @param size_of_tab Taille du tableau
 * @param replace_var Valeur à insérer
 * @param loop_var Nom de la variable à remplacer
 */
void replace_variables(char *argv[], size_t size_of_tab, char *replace_var, char *loop_var)
{
    size_t var_len = strlen(loop_var);
    char var_placeholder[256];
    snprintf(var_placeholder, sizeof(var_placeholder), "$%s", loop_var);

    for (size_t i = 0; i < size_of_tab; i++)
    {
        char *pos = strstr(argv[i], var_placeholder);
        while (pos != NULL)
        {
            // Calculer la nouvelle taille nécessaire
            size_t before_len = pos - argv[i];
            size_t after_len = strlen(pos + var_len + 1);
            size_t new_len = before_len + strlen(replace_var) + after_len + 1;

            char *new_arg = malloc(new_len);
            if (new_arg == NULL)
            {
                printerr("Erreur d'allocation mémoire pour la variable remplacée\n");
                exit(EXIT_FAILURE);
            }

            // Copier la partie avant la variable
            strncpy(new_arg, argv[i], before_len);
            new_arg[before_len] = '\0';

            // Ajouter la valeur de remplacement
            strcat(new_arg, replace_var);

            // Ajouter la partie après la variable
            strcat(new_arg, pos + var_len + 1);

            // Remplacer l'argument original
            free(argv[i]);
            argv[i] = strdup(new_arg);
            free(new_arg);

            // Rechercher d'autres occurrences dans le même argument
            pos = strstr(argv[i], var_placeholder);
        }
    }
}

/**
 * @brief Affiche une ligne de commande complète
 *
 * @param argv Tableau d'arguments
 */
void print_argv_line(char *argv[])
{
    for (size_t i = 0; argv[i] != NULL; i++)
    {
        printf("%s", argv[i]);
        if (argv[i + 1] != NULL)
        {
            printf(" ");
        }
    }
    printf("\n");
}

/**
 * @brief Exécute une commande après remplacement des variables
 *
 * @param argv Tableau d'arguments
 * @param size_of_tab Taille du tableau d'arguments
 * @param replace_var Valeur pour la substitution
 * @param loop_var Nom de la variable à remplacer
 * @return int Code de retour de la commande
 */
int ex_cmd(char *argv[], size_t size_of_tab, char *replace_var, char *loop_var)

{
    replace_variables(argv, size_of_tab, replace_var, loop_var);

    print_argv_line(argv);

    return parse_and_execute(size_of_tab, argv);
}

/**
 * @brief Obtient l'extension d'un fichier
 *
 * @param val Nom du fichier
 * @return char* Extension du fichier ou NULL si aucune extension
 */
char *get_ext(const char *val)
{
    char *cpy = strdup(val);
    if (cpy == NULL)
    {
        return NULL;
    }

    char *token = strtok(cpy, ".");
    char *save = NULL;

    while (token != NULL)
    {
        save = token;
        token = strtok(NULL, ".");
    }

    char *ext = save ? strdup(save) : NULL;
    free(cpy);
    return ext;
}

/**
 * @brief Supprime l'extension d'un fichier
 *
 * @param file Nom du fichier
 * @return char* Nom du fichier sans extension
 */
char *remove_ext(const char *file)
{
    if (file == NULL)
    {
        return NULL;
    }
    const char *last_dot = strrchr(file, '.');
    if (last_dot == NULL || last_dot == file)
    {
        return strdup(file);
    }
    size_t new_file_length = last_dot - file;
    char *new_file = malloc(new_file_length + 1);
    if (new_file == NULL)
    {
        return NULL;
    }
    strncpy(new_file, file, new_file_length);
    new_file[new_file_length] = '\0';
    return new_file;
}
