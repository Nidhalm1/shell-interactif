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

/**
 * @brief Parse et exécute une commande avec pipe
 *
 * @param argc Nombre d'arguments
 * @param argv Tableau d'arguments
 * @return int Code de retour de la commande
 */
int parse_and_execute_pipe(int argc, char **argv)
{
    int fd_in = STDIN_FILENO; // Initialement, l'entrée est le stdin
    int pipefd[2];            // Pipe pour communication
    int len = argc;
    pid_t pid[256];
    int i = 0;
    int indicePid = 0;
    while (i < len)
    {
        char **s = malloc((len - i + 1) * sizeof(char *)); // Ajustement de la taille
        if (!s)
        {
            perror("malloc");
            return 1;
        }
        int j = 0;

        // Crée une sous-commande jusqu'à trouver un '|'
        for (int k = i; k < len; k++)
        {
            if (argv[k] != NULL)
            {
                if (strcmp(argv[k], "|") == 0)
                {
                    break;
                }
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
        pid[indicePid++] = execute_commandPourPipe(s[0], fd_in, (i + j < len) ? pipefd[1] : STDOUT_FILENO, s);
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
    int j = 0;
    int status;
    while (waitpid(pid[j++], &status, 0) > 0)
    {
        
    }

    return 0;
}
int execute_commandPourPipe(const char *command, int fd0, int fd1, char **argv)
{
    pid_t pid;
    if ((pid = fork()) == 0)
    {
        if (fd0 != STDIN_FILENO)
        {
            if (dup2(fd0, STDIN_FILENO) == -1)
            {
                perror("Erreur lors de dup2 (entrée)");
                exit(1);
            }
            close(fd0);
        }

        // Redirection de la sortie
        if (fd1 != STDOUT_FILENO)
        {
            if (dup2(fd1, STDOUT_FILENO) == -1)
            {
                perror("Erreur lors de dup2 (sortie)");
                exit(1);
            }
            close(fd1);
        }
        if (execvp(command, argv) == -1)
        {
            perror("Erreur lors de execvp");
            exit(1); // Convention : 127 pour une commande introuvable
        }
    }
    else
    {
        return pid;
    }
}
