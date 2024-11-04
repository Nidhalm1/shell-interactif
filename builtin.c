#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int is_builtin(char *command)
{
    return (strcmp(command, "cd") == 0 || strcmp(command, "pwd") == 0 || strcmp(command, "exit") == 0);
}

void execute_builtin(char *command)
{
    if (strcmp(command, "cd") == 0)
    {
        builtin_cd(NULL); // À compléter avec le chemin souhaité
    }
    else if (strcmp(command, "pwd") == 0)
    {
        builtin_pwd();
    }
    else if (strcmp(command, "exit") == 0)
    {
        exit(0);
    }
}

void builtin_cd(char *path)
{
    // Commande cd (à compléter)
}

void builtin_pwd()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        perror("pwd");
    }
}
