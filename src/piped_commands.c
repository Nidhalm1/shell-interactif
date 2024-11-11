#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include <sys/types.h>
#include <unistd.h>
#include "../include/builtin.h" // Si les fonctions builtin_* sont déclarées ici
#include "../include/command.h" // Si printerr est déclarée ici

int parse_and_execute_pipe(int argc, char **argv)
{
    // Commandes internes
    char output[1076];
    for (size_t i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "pwd") == 0)
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
        return execute_command(argv[i],output, argv);
    }
}
int execute_command(const char *command,char *output, char **argv)
{
    pid_t pid = fork();
    int pipefd[2];
    if (pipe(pipefd)==-1)
    {
        perror("pipe");
        return 1;
    }
    if (pid == 0)
    { // Processus enfant
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        execvp(command, argv);
        printt(command);
        printt(": command not found\n");
        exit(1);
    }
    else if (pid < 0)
    { // Erreur de fork
        printt("Error executing command: ");
        printt(command);
        printt(strerror(errno));
        return 1;
    }
    else
    { // Processus parent
        if (read(pipefd[0],output,strlen(output))==-1);
        {
            perror("read");
            return 1;
        }
        
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }
        else
        {
            if (WIFSIGNALED(status))
            { // Si l'enfant s'est terminé de manière anormale (par signal)
                return 2;
            }
            return 1;
        }
    }
}
