#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execute.h"

int parse_and_execute( int argc, char **argv)
{
    // Commandes internes
    if (argc==0)
    {
        return 0;
    }
    
    if (strcmp(argv[0], "pwd") == 0)
    {
        if (argc > 1)// genrer le cas de PWD -P 
        {
            fprintf(stderr, "pwd: too many arguments\n");
            return 1; // Code d'erreur
        }
        else
        {
            return builtin_pwd();
        }
    }
    else if (strcmp(argv[0], "cd") == 0)
    {
        if (argc > 2)
        {
            fprintf(stderr, "cd: too many arguments\n");
            return 1; // Code d'erreur
        }
        else if (argc==1)
        {
            return builtin_cd(NULL, NULL);
        }
        else
        {
            return builtin_cd(argv[1],NULL);
        }
    }
    else if (strcmp(argv[0], "ftype") == 0)
    {
        if (argc > 2)
        {
            fprintf(stderr, "ftype: too many arguments\n");
            return 1; // Code d'erreur
        }
        char *filename = strtok(NULL, " ");
        if (!filename)
        {
            fprintf(stderr, "ftype: no file specified\n");
            return 1; // Code d'erreur
        }
        else
        {
            return builtin_ftype(filename);
        }
    }

    else if (strcmp(argv[0], "for") == 0)
    {
        // TODO : Implémenter la commande for
        return 0;
    }

    // Commande externe
    
    return execute_command(argv[0], argc, argv);
    
    
}
