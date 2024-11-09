#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/execute.h"
#include "../include/prompt.h"
#include "../include/command.h"

#include <unistd.h>

int printt(char *input)
{
    if (write(STDOUT_FILENO, input, strlen(input)) == -1)
    {
        perror("write");
        return -1;
    }
    return 0;
}
int printerr(char *input){
    if (write(STDERR_FILENO, input, strlen(input)) == -1)
    {
        perror("write");
        return -1;
    }
    return 0;
}

// Vous pouvez ajouter des fonctions liées aux commandes ici
