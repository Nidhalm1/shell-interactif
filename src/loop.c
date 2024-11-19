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
#include <bits/getopt_core.h>
#include <../include/builtin.h>
#include <../include/parser.h>

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
        perror("Erreur d'allocation mémoire pour cmd");
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

int loop_function(char *path, char *argv[], size_t size_of_tab, loop_options *options)
{
    if (options == NULL)
    {
        fprintf(stderr, "Option non reconnue.\n");
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
    int count = 0;
    char **cmd = NULL;
    size_t cmd_size = 0;

    cmd = get_cmd(argv, size_of_tab, &cmd_size);

    while ((entry = readdir(dirp)) != NULL)
    {
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
            count += loop_function(path_file, argv, size_of_tab, options);
        }
        else if (S_ISREG(st.st_mode))
        {
            int res = ex_cmd(cmd, cmd_size, path_file);
            if (res == 0)
            {
                count++;
            }
        }
    }

    free(cmd);
    closedir(dirp);
    return 0;
}

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

int ex_cmd(char *argv[], size_t size_of_tab, char *replace_var)
{
    replace_variables(argv, size_of_tab, replace_var);
    return parse_and_execute(size_of_tab, argv);
}