
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


char **get_cmd(char *argv[], size_t size_of_tab, size_t *cmd_size) {
    int size_of_cmd = 0;
    bool find = false;

    for (size_t i = 0; i < size_of_tab; i++) {
        if (strcmp(argv[i], "{") == 0) {
            find = true;
        }
        if (strcmp(argv[i], "}") == 0) {
            break;
        }
        if (find && (strcmp(argv[i], "{") != 0 && strcmp(argv[i], "}") != 0)) {
            size_of_cmd++;
        }
    }

    char **cmd = malloc((size_of_cmd + 1) * sizeof(char *));
    if (cmd == NULL) {
        perror("Erreur d'allocation mémoire pour cmd");
        exit(1);
    }

    size_t parc = 0;
    find = false;
    for (size_t i = 0; i < size_of_tab; i++) {
        if (strcmp(argv[i], "{") == 0) {
            find = true;
            continue;
        }
        if (strcmp(argv[i], "}") == 0) {
            break;
        }
        if (find && (strcmp(argv[i], "{") != 0 && strcmp(argv[i], "}") != 0)) {
            cmd[parc++] = argv[i];
        }
    }
    cmd[parc] = NULL;
    *cmd_size = size_of_cmd;
    return cmd;
}

int loop_function(char *path, char *argv[], size_t size_of_tab) {
    DIR *dirp = opendir(path);
    if (dirp == NULL) {
        perror("Aucun répertoire de ce nom");
        return 1;
    }

    struct dirent *entry;
    struct stat st;
    int count = 0;

    while ((entry = readdir(dirp)) != NULL) {
        char path_file[MAX_LENGTH];

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(path_file, sizeof(path_file), "%s/%s", path, entry->d_name);

        if (lstat(path_file, &st) == -1) {
            perror("Erreur avec lstat");
            continue;
        }

        if (S_ISREG(st.st_mode)) {
            size_t cmd_size = 0;
            char **cmd = get_cmd(argv, size_of_tab, &cmd_size);

            int res = ex_cmd(cmd, cmd_size, path_file);
            free(cmd);

            if (res == 0) {
                count++;
            }
        }
    }
    printf("Le nombre de commande reussi est %d\n", count);
    closedir(dirp);
    return 0;
}

int ex_cmd(char *argv[], size_t size_of_tab, char *replace_var) {
    for (size_t i = 0; i < size_of_tab; i++) {
        if (strcmp(argv[i], "$F") == 0) {
            argv[i] = replace_var;
        }
    }

    pid_t proc = fork();
    if (proc == -1) {
        perror("Erreur lors du fork");
        return 1;
    }

    if (proc == 0) {
        if (execvp(argv[0], argv) == -1) {
            perror("Erreur lors de l'exécution de la commande");
            exit(1);
        }
    } else {
        wait(NULL);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory> [options]\n", argv[0]);
        return EXIT_FAILURE;
    }

    return loop_function(argv[1], argv, argc);
}
