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

/**
 * @brief Initialise la structure de boucle
 *
 * @return loop_options* Pointeur sur la structure de boucle
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
 * @brief Initialise la structure de boucle
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return loop_options* Pointeur sur la structure de boucle
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
 * @brief Récupère la commande entre les accolades
 *
 * @param argv Tableau d'arguments
 * @param size_of_tab Taille du tableau d'arguments
 * @param cmd_size Taille de la commande
 * @return char** Tableau de commande
 */
char **get_cmd(char *argv[], size_t size_of_tab, size_t *cmd_size)
{
    size_t size_of_cmd = 0;
    bool find = false;

    for (size_t i = 0; i < size_of_tab; i++)
    {
        if (strcmp(argv[i], "{") == 0)
        {
            find = true;
            continue;
        }
        if (strcmp(argv[i], "}") == 0 && find)
        {
            break;
        }
        if (find)
        {
            size_of_cmd++;
        }
    }

    if (size_of_cmd == 0)
    {
        return NULL;
    }

    char **cmd = malloc((size_of_cmd + 1) * sizeof(char *));
    if (cmd == NULL)
    {
        printerr("Erreur d'allocation mémoire pour cmd");
        exit(EXIT_FAILURE);
    }

    size_t parc = 0;
    find = false;
    for (size_t i = 0; i < size_of_tab; i++)
    {
        if (strcmp(argv[i], "{") == 0)
        {
            find = true;
            continue;
        }
        if (strcmp(argv[i], "}") == 0 && find)
        {
            break;
        }
        if (find)
        {
            cmd[parc++] = argv[i];
        }
    }
    cmd[parc] = NULL;
    *cmd_size = size_of_cmd;
    return cmd;
}

/**
 * @brief Fonction qui parcours les fichiers et exécute une commande
 *
 * @param path Chemin du répertoire
 * @param argv Tableau d'arguments
 * @param size_of_tab Taille du tableau d'arguments
 * @param options Options de la boucle
 * @return int 0 si tout c'est bien passé, 1 sinon
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
        printerr("Erreur d'ouverture du répertoire");
        return 1;
    }

    struct dirent *entry;
    struct stat st;
    char **cmd = NULL;
    size_t cmd_size = 0;

    cmd = get_cmd(argv, size_of_tab, &cmd_size);

    int nb_process_runned = 0;
    while ((entry = readdir(dirp)) != NULL)
    {
        if (!options->opt_A && entry->d_name[0] == '.')
        {
            continue;
        }

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

        if (options->opt_r && S_ISDIR(st.st_mode))
        {
            loop_function(path_file, argv, size_of_tab, options);
            continue;
        }

        if (options->ext != NULL)
        {
            char *ext = get_ext(entry->d_name);
            if (ext == NULL || strcmp(ext, options->ext) != 0)
            {
                free(ext);
                continue;
            }
            free(ext);
        }

        if (options->type != NULL)
        {
            if ((strcmp(options->type, "f") == 0 && !S_ISREG(st.st_mode)) ||
                (strcmp(options->type, "d") == 0 && !S_ISDIR(st.st_mode)) ||
                (strcmp(options->type, "l") == 0 && !S_ISLNK(st.st_mode)) ||
                (strcmp(options->type, "p") == 0 && !S_ISFIFO(st.st_mode)))
            {
                continue;
            }
        }

        if (options->max > 0 && nb_process_runned >= options->max)
        {
            wait(NULL);
            nb_process_runned--;
        }

        pid_t p = fork();

        switch (p)
        {
        case -1:
            perror("Erreur lors du fork");
            break;

        case 0:
            ex_cmd(cmd, cmd_size, path_file);
            exit(EXIT_SUCCESS);

        default:
            nb_process_runned++;
            break;
        }

        while (nb_process_runned > 0)
        {
            wait(NULL);
            nb_process_runned--;
        }
    }

    free(cmd);
    closedir(dirp);
    return 0;
}

/**
 * @brief Remplace les variables par une valeur
 *
 * @param argv Tableau d'arguments
 * @param size_of_tab Taille du tableau d'arguments
 * @param replace_var Valeur de remplacement
 */
void replace_variables(char *argv[], size_t size_of_tab, char *replace_var)
{
    for (size_t i = 0; i < size_of_tab; i++)
    {
        if (argv[i][0] == '$')
        {
            argv[i] = replace_var;
        }
    }
}

/**
 * @brief Exécute une commande
 *
 * @param argv Tableau d'arguments
 * @param size_of_tab Taille du tableau d'arguments
 * @param replace_var Valeur de remplacement
 * @return int 0 si tout c'est bien passé, 1 sinon
 */
int ex_cmd(char *argv[], size_t size_of_tab, char *replace_var)
{
    replace_variables(argv, size_of_tab, replace_var);
    return parse_and_execute(size_of_tab, argv);
}

/**
 * @brief Récupère l'extension d'un fichier
 *
 * @param val Nom du fichier
 * @return char* Extension du fichier
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
