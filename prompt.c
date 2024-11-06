#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <string.h>

void get_current_directory(char *buffer, size_t size)
{
    if (getcwd(buffer, size) == NULL)
    {
        write(STDERR_FILENO, "Error: Unable to get current directory\n", 40);
        strcpy(buffer, ""); // Default value
    }
}

char *prompt(int last_return_code)
{
    char color[10];
    char dir[1024];
    get_current_directory(dir, sizeof(dir));

    if (last_return_code == 0)
    {
        strcpy(color, "\033[32m"); // Green
    }
    else
    {
        strcpy(color, "\033[91m"); // Red
    }

    char prompt_string[1024];
    snprintf(prompt_string, sizeof(prompt_string), "\001%s\002[%d]\001\033[00m\002 %s$ ", color, last_return_code, dir);

    char *input = readline(prompt_string);
    if (input == NULL)
    {
        return NULL;
    }
    return input;
}
