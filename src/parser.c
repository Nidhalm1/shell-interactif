#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include "../include/builtin.h" // Si les fonctions builtin_* sont déclarées ici
#include "../include/command.h" // Si printerr est déclarée ici
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>

int parse_and_execute(int argc, char **argv)
{

    // Gestion des pipes multiples
    int pipe_count = 0;
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            pipe_count++;
        }
    }

    if (pipe_count > 0)
    {
        int pipefds[2 * pipe_count];
        for (int i = 0; i < pipe_count; i++)
        {
            if (pipe(pipefds + i * 2) == -1)
            {
                printt("pipe");
                printt(": error creating pipe\n");
                return 1;
            }
        }

        int command_start = 0;
        int pipe_index = 0;
        for (int i = 0; i <= argc; i++)
        {
            if (i == argc || strcmp(argv[i], "|") == 0)
            {
                argv[i] = NULL; // Séparer les commandes

                pid_t pid = fork();
                if (pid == -1)
                {
                    printt("fork");
                    printt(": error fork\n");
                    return 1;
                }

                if (pid == 0)
                {
                    // Rediriger stdin si ce n'est pas la première commande
                    if (pipe_index > 0)
                    {
                        if (dup2(pipefds[(pipe_index - 1) * 2], STDIN_FILENO) == -1)
                        {
                            printt("dup2");
                            printt(": error dup2\n");
                            return 1;
                        }
                    }

                    // Rediriger stdout si ce n'est pas la dernière commande
                    if (pipe_index < pipe_count)
                    {
                        if (dup2(pipefds[pipe_index * 2 + 1], STDOUT_FILENO) == -1)
                        {
                            printt("dup2");
                            printt(": error dup2\n");
                            return 1;
                        }
                    }

                    // Fermer tous les descripteurs de pipe
                    for (int j = 0; j < 2 * pipe_count; j++)
                    {
                        close(pipefds[j]);
                    }

                    execvp(argv[command_start], &argv[command_start]);
                    printt("execvp");
                    printt(": error execvp\n");
                    return 1;
                }

                command_start = i + 1;
                pipe_index++;
            }
        }

        // Fermer tous les descripteurs de pipe dans le processus parent
        for (int i = 0; i < 2 * pipe_count; i++)
        {
            close(pipefds[i]);
        }

        // Attendre tous les processus enfants
        for (int i = 0; i <= pipe_count; i++)
        {
            wait(NULL);
        }

        return 0;
    }

    // Commandes internes
    if (argc == 0)
    {
        return 0;
    }

    if (strcmp(argv[0], "pwd") == 0)
    {
        if (argc > 1) // genrer le cas de PWD -P
        {
            printerr("pwd: too many arguments\n");
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
            printerr("cd: too many arguments\n");
            return 1; // Code d'erreur
        }
        else if (argc == 1)
        {
            return builtin_cd(NULL);
        }
        else
        {
            return builtin_cd(argv[1]);
        }
    }
    else if (strcmp(argv[0], "ftype") == 0)
    {
        if (argc > 2)
        {
            printerr("ftype: too many arguments\n");
            return 1; // Code d'erreur
        }
        char *filename = strtok(NULL, " ");
        if (!filename)
        {
            printerr("ftype: no file specified\n");
            return 1; // Code d'erreur
        }
        else
        {
            return builtin_ftype(filename);
        }
    }

    else if (strcmp(argv[0], "For") == 0)
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

    return execute_command(argv[0], argv);
}
