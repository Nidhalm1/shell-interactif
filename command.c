#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_command(char *line)
{
    // Exemple d'analyse et d'exécution d'une commande
    char *command = parse_command(line);

    if (is_builtin(command))
    {
        execute_builtin(command);
    }
    else
    {
        execute_command(command);
    }

    free(command);
}
