#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_prompt()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("[fsh]%s$ ", cwd); // Prompt simple
    }
    else
    {
        perror("getcwd error");
    }
}
