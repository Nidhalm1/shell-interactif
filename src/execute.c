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

/**
 * @brief Exécute une commande externe
 *
 * @param command Commande à exécuter
 * @param fd0 Descripteur de fichier pour l'entrée
 * @param fd1 Descripteur de fichier pour la sortie
 * @param argv Arguments de la commande
 * @return int Code de retour de la commande
 */
int execute_command(const char *command, int fd0, int fd1, char **argv)
{
    pid_t pid = fork();
    if (pid == 0)
    { // Processus enfant
        if (fd0 != STDIN_FILENO)
        {
            dup2(fd0, STDIN_FILENO);
            close(fd0);
        }
        if (fd1 != STDOUT_FILENO)
        {
            dup2(fd1, STDOUT_FILENO);
            close(fd1);
        }
        execvp(command, argv);
        perror(command); // Affiche une erreur si execvp échoue
        exit(1);
    }
    else if (pid < 0)
    { // Erreur de fork
        perror("fork");
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
