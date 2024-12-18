#ifndef LOOP_H
#define LOOP_H

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

#define MAX_LENGTH 1024

typedef struct
{
    bool opt_A;
    bool opt_r;
    char *ext;
    char *type;
    int max;
} loop_options;

int ex_cmd(char *argv[], size_t size_of_tab, char *replace_var, char *);
int add_reference(char ***refs, size_t *size, size_t *capacity, const char *new_ref);
char **get_cmd(char *argv[], size_t size_of_tab, size_t *cmd_size);
char *get_ext(const char *val);
char *remove_ext(const char *file);
loop_options *init_struc();
loop_options *option_struc(int argc, char *argv[]);
int loop_function(char *path, char *argv[], size_t size_of_tab, loop_options *options);
#endif