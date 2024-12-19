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

void print_loop_options(loop_options *options)
{
    if (options == NULL)
    {
        printf("Options are NULL.\n");
        return;
    }

    printf("Loop Options:\n");

    printf("  -A (All files): %s\n", options->opt_A ? "Enabled" : "Disabled");
    printf("  -r (Recursive): %s\n", options->opt_r ? "Enabled" : "Disabled");

    if (options->ext != NULL)
    {
        printf("  -e (Extension): %s\n", options->ext);
    }
    else
    {
        printf("  -e (Extension): Not set\n");
    }

    if (options->type != NULL)
    {
        printf("  -t (Type): %s\n", options->type);
    }
    else
    {
        printf("  -t (Type): Not set\n");
    }

    if (options->max != -1)
    {
        printf("  -p (Max processes): %d\n", options->max);
    }
    else
    {
        printf("  -p (Max processes): Not set\n");
    }
}

/**
 * @brief Affiche l'index et la valeur de chaque argument dans un tableau arg[]
 *
 * @param arg Tableau d'arguments
 */
void print_args(char *arg[])
{
    if (arg == NULL)
    {
        printf("Erreur : le tableau arg est NULL.\n");
        return;
    }

    for (int i = 0; arg[i] != NULL; i++)
    {
        printf("Index %d: %s\n", i, arg[i]);
    }
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

void free_loop_options(loop_options *options)
{
    if (options != NULL)
    {
        free(options->ext);
        free(options->type);
        free(options);
    }
}

/**
 * @brief Analyse les options de ligne de commande et remplit une structure loop_options
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return loop_options* Pointeur vers la structure avec les options analysées ou NULL en cas d'erreur
 */
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
    bool can_stock_A = true;
    bool can_stock_r = true;
    bool can_stock_t = true;
    bool can_stock_p = true;
    bool can_stock_e = true;

    for (int i = 4; i < argc; i++)
    {
        if (strcmp(argv[i], "-A") == 0 && can_stock_A)
        {
            opt_struc->opt_A = true;
            can_stock_A = false;
        }
        if (strcmp(argv[i], "-r") == 0 && can_stock_r)
        {
            opt_struc->opt_r = true;
            can_stock_r = false;
        }
        if (strcmp(argv[i], "-e") == 0 && can_stock_e)
        {
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                opt_struc->ext = strdup(argv[++i]);
                can_stock_e = false;
                if (!opt_struc->ext)
                {
                    perror("Erreur d'allocation mémoire pour l'option -e");
                    free(opt_struc);
                    return NULL;
                }
            }
            else
            {
                fprintf(stderr, "Erreur : l'option -e nécessite un argument valide.\n");
                free(opt_struc);
                return NULL;
            }
        }
        if (strcmp(argv[i], "-t") == 0 && can_stock_t)
        {
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                opt_struc->type = strdup(argv[++i]);
                can_stock_t = false;
                if (!opt_struc->type)
                {
                    perror("Erreur d'allocation mémoire pour l'option -t");
                    free(opt_struc);
                    return NULL;
                }
            }
            else
            {
                fprintf(stderr, "Erreur : l'option -t nécessite un argument valide (f, d, l, p).\n");
                free(opt_struc);
                return NULL;
            }
        }
        if (strcmp(argv[i], "-p") == 0 && can_stock_p)
        {
            if (i + 1 < argc && argv[i + 1][0] != '-' && atoi(argv[i + 1]) > 0)
            {
                opt_struc->max = atoi(argv[++i]);
                can_stock_p = false;
            }
            else
            {
                fprintf(stderr, "Erreur : l'option -p nécessite un entier strictement positif.\n");
                free(opt_struc);
                return NULL;
            }
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
        fprintf(stderr, "Erreur : les options sont nulles.\n");
        return 1;
    }

    DIR *dirp = opendir(path);
    if (dirp == NULL)
    {
        fprintf(stderr, "Erreur d'ouverture du répertoire : %s\n", path);
        return 1;
    }

    struct dirent *entry;
    struct stat st;
    char **cmd = NULL;
    size_t cmd_size = 0;

    cmd = get_cmd(argv, size_of_tab, &cmd_size);
    if (cmd == NULL)
    {
        fprintf(stderr, "Erreur : commande non valide ou mal formée.\n");
        closedir(dirp);
        return 1;
    }

    int max_return_code = 0; // Valeur initiale à 0

    // Parcours des entrées dans le répertoire
    while ((entry = readdir(dirp)) != NULL)
    {
        // Ignorer les fichiers cachés si `-A` n'est pas activé
        if (!options->opt_A && entry->d_name[0] == '.')
        {
            continue;
        }

        // Ignorer les entrées spéciales "." et ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char path_file[MAX_LENGTH];
        snprintf(path_file, sizeof(path_file), "%s/%s", path, entry->d_name);

        if (lstat(path_file, &st) == -1)
        {
            perror("Erreur avec lstat");
            continue;
        }

        // Gestion des sous-répertoires si `-r` est activé
        if (options->opt_r && S_ISDIR(st.st_mode))
        {
            char sub_dir[MAX_LENGTH];
            snprintf(sub_dir, sizeof(sub_dir), "%s/%s", path, entry->d_name);

            int sub_return_code = loop_function(sub_dir, argv, size_of_tab, options);
            if (sub_return_code > max_return_code)
            {
                max_return_code = sub_return_code;
            }
        }

        // Filtrage par type `-t`
        if (options->type != NULL)
        {
            if (strcmp(options->type, "d") == 0 && !S_ISDIR(st.st_mode))
            {
                continue;
            }
            if (strcmp(options->type, "f") == 0 && !S_ISREG(st.st_mode))
            {
                continue;
            }
            if (strcmp(options->type, "l") == 0 && !S_ISLNK(st.st_mode))
            {
                continue;
            }
            if (strcmp(options->type, "p") == 0 && !S_ISFIFO(st.st_mode))
            {
                continue;
            }
        }

        // Filtrage par extension `-e`
        if (options->ext != NULL)
        {
            char *ext = get_ext(entry->d_name);
            if (ext == NULL || strcmp(ext, options->ext) != 0)
            {
                free(ext);
                continue;
            }
            free(ext);

            char *filename_without_ext = remove_ext(entry->d_name);
            if (filename_without_ext == NULL)
            {
                perror("Erreur lors de la suppression de l'extension");
                continue;
            }

            snprintf(path_file, sizeof(path_file), "%s/%s", path, filename_without_ext);
            free(filename_without_ext);
        }

        pid_t p = fork();

        if (p == -1)
        {
            perror("Erreur lors du fork");
        }
        else if (p == 0) // Processus enfant
        {
            replace_variables(cmd, cmd_size, path_file, argv[1]);
            int ret = ex_cmd(cmd, cmd_size, path_file, argv[1]);
            free_args(cmd);
            closedir(dirp);
            exit(ret);
        }
        else // Processus parent
        {
            int status;
            wait(&status); // Attendre la fin du processus enfant

            // Mise à jour du code de retour maximum
            if (WIFEXITED(status))
            {
                int return_code = WEXITSTATUS(status);
                if (return_code > max_return_code)
                {
                    max_return_code = return_code;
                }
            }
            else if (WIFSIGNALED(status))
            {
                max_return_code = 255; // Si interrompu par un signal
            }
        }
    }

    free_args(cmd);
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

    // print_argv_line(argv);

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
