#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/execute.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/builtin.h" // Si les fonctions builtin_* sont déclarées ici
#include "../include/command.h" // Si printerr est déclarée ici
#include <errno.h>

int execute_piped_command(const char *command, int fd0, int fd1, char **argv);

int parse_and_execute_pipe(int argc, char **argv)
{
    int fd_in = STDIN_FILENO; // Initialement, l'entrée est le stdin
    int pipefd[2]; // Pipe pour communication
    int len = argc;
    int i = 0;

    while (i < len)
    {
        char **s = malloc((len - i + 1) * sizeof(char *)); // Ajustement de la taille
        if (!s) {
            perror("malloc");
            return 1;
        }
        int j = 0;

        // Crée une sous-commande jusqu'à trouver un '|'
        for (int k = i; k < len; k++)
        {
            if (strcmp(argv[k], "|") == 0)
            {
                break;
            }
            s[j++] = argv[k];
        }
        s[j] = NULL;

        // Crée un nouveau pipe si ce n'est pas la dernière commande
        if (i + j < len)
        {
            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                free(s);
                return 1;
            }
        }

        // Exécute la commande avec les bons descripteurs
        execute_piped_command(s[0], fd_in, (i + j < len) ? pipefd[1] : STDOUT_FILENO, s);
        free(s);

        // Fermeture de l'extrémité d'écriture dans le processus parent
        if (fd_in != STDIN_FILENO)
            close(fd_in);
        if (pipefd[1] != STDOUT_FILENO)
            close(pipefd[1]);

        // Mise à jour pour la prochaine commande
        if (i + j < len)
            fd_in = pipefd[0];
        i += j + 1;
    }

    // Ferme la dernière entrée de pipe
    if (fd_in != STDIN_FILENO)
        close(fd_in);

    return 0;
}

int execute_piped_command(const char *command, int fd0, int fd1, char **argv)
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
