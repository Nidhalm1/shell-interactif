#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include "../include/command.h"

void get_current_directory(char *buffer, size_t size)
{
    if (getcwd(buffer, size) == NULL)
    {
        printerr("Error: Unable to get current directory\n");
    }
}
void troncate(char *dir)
{
    int len = strlen(dir);
    int enlever = len - 25;
    strcpy(dir, dir + enlever);
    dir[25] = '\0';
    for (size_t i = 0; i < 3; i++)
    {
        dir[i] = '.';
    }
}

char *prompt(int last_return_code)
{
    char color[10];
    char reset_color[10];
    char color_dir[10];
    char dir[1024];
    get_current_directory(dir, sizeof(dir));
    if (strlen(dir) > 25)
    {
        troncate(dir);
    }

    if (last_return_code == 0)
    {
        strcpy(color, "\001\033[32m\002"); // Green
    }
    else
    {
        strcpy(color, "\001\033[91m\002"); // Red
    }

    // Couleur pour le répertoire
    strcpy(color_dir, "\001\033[34m\002");   // bleu pour le répertoire
    strcpy(reset_color, "\001\033[00m\002"); // coleur de base normal

    char prompt_string[1070];
    if (last_return_code != 2)
    {
        snprintf(prompt_string, sizeof(prompt_string), "%s[%d]%s%s%s$ ", color, last_return_code, color_dir, dir, reset_color);
    }
    else
    {
        snprintf(prompt_string, sizeof(prompt_string), "%s[%s]%s%s%s$ ", color, "SIG", color_dir, dir, reset_color);
    }
    rl_outstream = stderr;
    char *input = readline(prompt_string);
    if (input == NULL)
    {
        return NULL;
    }

    add_history(input);
    return input;
}
