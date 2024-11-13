#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#endif
#include "../include/execute.h"
#include <errno.h>
#include "../include/command.h"

int execute_command(const char *command, char **argv)
{
    pid_t pid = fork();
    if (pid == 0)
    { // Processus enfant
        execvp(command, argv);
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
