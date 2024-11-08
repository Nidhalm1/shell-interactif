#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "execute.h"
#include "prompt.h"
#include <unistd.h>

int print(char *input)
{
    write(STDOUT_FILENO, input, strlen(input));
    write(STDOUT_FILENO, "\n", 1);
    return 0;
}

// Vous pouvez ajouter des fonctions liées aux commandes ici
