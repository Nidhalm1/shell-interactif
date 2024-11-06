#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <string.h>

void get_current_directory(char *buffer, size_t size)
{
    if (getcwd(buffer, size) == NULL)
    {
        fprintf(stderr, "Error: Unable to get current directory\n");
        strcpy(buffer, ""); // Valeur par défaut
    }
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

    char *prompt_string = malloc(1024);
    snprintf(prompt_string, 1024, "\001%s\002[%d]\001\033[00m\002 %s$ ", color, last_return_code, dir);
    printf("%s", prompt_string);
    free(prompt_string);
    char *input = malloc(1024);
    if (fgets(input, 1024, stdin) == NULL)
    {
        free(input);
        return NULL;
    }
    input[strcspn(input, "\n")] = '\0';
    return input;
}
