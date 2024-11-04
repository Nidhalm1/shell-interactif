#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void execute_command(char *command)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // Enfant
        execlp(command, command, (char *)NULL);
        perror("Execution error");
        exit(1);
    }
    else if (pid > 0)
    {
        // Parent
        wait(NULL); // Attente de la fin du processus enfant
    }
    else
    {
        perror("Fork error");
    }
}
