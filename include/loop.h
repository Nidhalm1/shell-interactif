
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

int ex_cmd(char *argv[], size_t size_of_tab, char *replace_var);
char **get_cmd(char *argv[], size_t size_of_tab, size_t *cmd_size);
int loop_function(char *path, char *argv[], size_t size_of_tab);

#endif 
