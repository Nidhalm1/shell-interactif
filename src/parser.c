#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/builtin.h" // Si les fonctions builtin_* sont déclarées ici
#include "../include/command.h" // Si printerr est déclarée ici
#include "../include/simple_commands.h"
#include "../include/piped_commands.h"

int parse_and_execute(int argc, char **argv)
{
    if (argc == 0)
    {
        return 0;
    }
    else if (argc == 1)
    {
        return parse_and_execute_simple(argc, argv);
    }
    for (size_t i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            return parse_and_execute_pipe(argc, argv);
        }
    }
    return parse_and_execute_simple(argc, argv);
}