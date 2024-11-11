#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/builtin.h" // Si les fonctions builtin_* sont déclarées ici
#include "../include/command.h" // Si printerr est déclarée ici
#include "../include/simple_commands.h"

int parse_and_execute(int argc, char **argv)
{
    if (argc==0)
    {
        return 0;
    }
    else if (argc==1)
    {
        return parse_and_execute_simple(argc,argv);
    }
    int len = strlen(argv);
    for (size_t i = 0; i <len; i++)
    {
        if (argv[i]="|")
        {
            return parse_and_execute_pipe(argc,argv);
        }
    }
    return parse_and_execute_simple(argc,argv);
}