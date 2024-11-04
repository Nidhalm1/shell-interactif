#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void get_current_directory(char *buffer, size_t size)
{
    getcwd(buffer, size);
}

char *prompt(int last_return_code)
{
    char color[10];
    char dir[1024];
    get_current_directory(dir, sizeof(dir));

    if (last_return_code == 0)
    {
        strcpy(color, "\033[32m"); // Vert
    }
    else
    {
        strcpy(color, "\033[91m"); // Rouge
    }

    // Format du prompt
    char *prompt_string = malloc(1024);
    snprintf(prompt_string, 1024, "\001%s\002[%d]\001\033[00m\002 %s$ ", color, last_return_code, dir);
    return readline(prompt_string);
}
