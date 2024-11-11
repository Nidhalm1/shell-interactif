#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/builtin.h" // Si les fonctions builtin_* sont déclarées ici
#include "../include/command.h" // Si printerr est déclarée ici
#include "../include/simple_commands.h"
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>
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
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            return parse_and_execute_pipe(argc, argv);
        }
    }
    if (strcmp(argv[0], "For") == 0)
    {
    char * argv_bis [(argc - 3)+1];
    int opt;
    argv_bis[0]="./loop";
    argv_bis[1]=argv[3];
    int parc=2;
    optind = 3;
    // Analyse des options -A, -r, -e <arg>, -p <arg>
    while ((opt = getopt(argc, argv, "Are:t:p:")) != -1) {
        switch (opt) {
            case 'A':
                argv_bis[parc++]="-A";
                break;
            case 'r':
                argv_bis[parc++]="-r";
                break;
            case 'e':
                 argv_bis[parc++]="-e";
                 argv_bis[parc++]=optarg;
                break;
            case 't':
                 argv_bis[parc++]="-t";
                 argv_bis[parc++]=optarg;
                break;
            case 'p':
                 argv_bis[parc++]="-p";
                 argv_bis[parc++]=optarg;
                break;
            default:
                break;
        }
    }
    // stocker les commandes restantes après les options
    for (int i = optind; i < argc; i++) {
         argv_bis[parc++]=argv[i];
    }
    pid_t proc;

    switch (proc=fork())
    {
        case -1:
        return 1;
        break;
    case 0:
        if (execvp(argv_bis[0],argv_bis)==-1){
            perror("Erreur lors de l'exécution de la commande");
            exit(1);
        }
        break;
    
    default:
         wait(NULL);
        break;
    }
    return 0;

    }

    // Commande externe
    return parse_and_execute_simple(argc, argv);
}