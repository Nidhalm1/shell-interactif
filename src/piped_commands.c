#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
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
#include "../include/parser.h"
#include <fcntl.h>

/**
 * @brief Exécute une commande externe
 *
 * @param command Commande à exécuter
 * @param fd0 Descripteur de fichier pour l'entrée
 * @param fd1 Descripteur de fichier pour la sortie
 * @param argv Arguments de la commande
 * @return int Code de retour de la commande
 */
int parse_and_execute_pipe(int argcc, char **s)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        int argc = argcc;
        int original_argc = argcc; // Sauvegarder la valeur originale de argc
        pid_t parent_pid = getpid();
        char *s_copy[argc + 1]; // Tableau statique
        for (int i = 0; i < argc; i++)
        {
            s_copy[i] = s[i]; // Ne duplique que les pointeurs
        }
        s_copy[argc] = NULL; // Terminaison
        int i;
        int in_fd = STDIN_FILENO;
        int fd[2];
        char **cmd = s_copy; // Utiliser un pointeur séparé

        for (i = 0; i < argc; i++)
        {
            if (strcmp(cmd[i], "|") == 0)
            {
                cmd[i] = NULL;
                if (pipe(fd) == -1)
                {
                    perror("pipe");
                    exit(1);
                }
                execCommandPipe(cmd[0], in_fd, fd[1], cmd, i);
                close(fd[1]);
                if (in_fd != STDIN_FILENO)
                    close(in_fd);
                in_fd = fd[0];
                cmd += i + 1;
                argc -= (i + 1);
                i = -1;
            }
        }
        if (in_fd != STDIN_FILENO)
        {
            if (dup2(in_fd, STDIN_FILENO) == -1)
            {
                perror("dup2 stdin");
                exit(1);
            }
            close(in_fd);
        }

        size_t cmd_len = 0;
        for (int j = 0; cmd[j] != NULL; j++)
        {
            cmd_len++;
        }
        execCommandPipe(cmd[0], STDIN_FILENO, STDOUT_FILENO, cmd, cmd_len);
        wait(NULL);
        exit(0);
    }
    // Attendre tous les processus enfants
    while (wait(NULL) > 0)
    {
        /* code */
    }
    return 0;
}
void free_argg(char **s, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (s[i] != NULL)
        {
            free(s[i]);
        }
    }
    free(s);
}
/**
 * @brief Exécute une commande externe
 *
 * @param command Commande à exécuter
 * @param fd0 Descripteur de fichier pour l'entrée
 * @param fd1 Descripteur de fichier pour la sortie
 * @param argv Arguments de la commande
 * @return int Code de retour de la commande
 */
int execCommandPipe(const char *command, int fd0, int fd1, char **argv, int argc)
{
    int fd2 = STDERR_FILENO; // pour les erreurs
    pid_t pid = fork();
    if (pid == 0)
    { // Processus enfant
        int len = argc;
        int fichierEntree = -1, fichierSortie = -1, fichierErr = -1, ecrase = 0, ajoute = 0, ecraserr = 0, ajouterr = 0;

        // Analyse des redirections
        for (int i = 0; i < len; i++)
        {
            if (strcmp("<", argv[i]) == 0)
            {
                fichierEntree = i + 1;
            }
            else if (strcmp(">", argv[i]) == 0)
            {
                fichierSortie = i + 1;
            }
            else if (strcmp(">|", argv[i]) == 0)
            {
                fichierSortie = i + 1;
                ecrase = 1;
            }
            else if (strcmp(">>", argv[i]) == 0)
            {
                fichierSortie = i + 1;
                ajoute = 1;
            }
            else if (strcmp("2>", argv[i]) == 0)
            {
                fichierErr = i + 1;
            }
            else if (strcmp("2>|", argv[i]) == 0)
            {
                fichierErr = i + 1;
                ecraserr = 1;
            }
            else if (strcmp("2>>", argv[i]) == 0)
            {
                fichierErr = i + 1;
                ajouterr = 1;
            }
        }

        // Gestion de l'entrée
        if (fichierEntree != -1)
        {
            fd0 = open(argv[fichierEntree], O_RDONLY);
            if (fd0 == -1)
            {
                perror("Erreur d'ouverture de fichier d'entrée");
                exit(1);
            }
        }

        // Gestion de la sortie
        if (fichierSortie != -1)
        {
            if (ecrase)
            {
                fd1 = open(argv[fichierSortie], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            else if (ajoute)
            {
                fd1 = open(argv[fichierSortie], O_WRONLY | O_CREAT | O_APPEND, 0644);
            }
            else
            {
                fd1 = open(argv[fichierSortie], O_WRONLY | O_CREAT | O_EXCL, 0644);
            }
            if (fd1 == -1)
            {
                perror("Erreur d'ouverture de fichier de sortie");
                exit(1);
            }
        }

        // Gestion de la redirection d'erreur
        if (fichierErr != -1)
        {
            if (ecraserr)
            {
                fd2 = open(argv[fichierErr], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            else if (ajouterr)
            {
                fd2 = open(argv[fichierErr], O_WRONLY | O_CREAT | O_APPEND, 0644);
            }
            else
            {
                fd2 = open(argv[fichierErr], O_WRONLY | O_CREAT | O_EXCL, 0644);
            }
            if (fd2 == -1)
            {
                perror("Erreur d'ouverture du fichier d'erreur");
                exit(1);
            }
        }

        // Rediriger les flux
        if (fd0 != STDIN_FILENO)
        {
            if (dup2(fd0, STDIN_FILENO) == -1)
            {
                perror("dup2 stdin");
                exit(1);
            }
            close(fd0);
        }
        if (fd1 != STDOUT_FILENO)
        {
            if (dup2(fd1, STDOUT_FILENO) == -1)
            {
                perror("dup2 stdout");
                exit(1);
            }
            close(fd1);
        }
        if (fd2 != STDERR_FILENO)
        {
            if (dup2(fd2, STDERR_FILENO) == -1)
            {
                perror("dup2 stderr");
                exit(1);
            }
            close(fd2);
        }
        // Créer une copie du tableau argv sans les redirections
        int new_argc = 0;
        char **new_argv = malloc((len + 1) * sizeof(char *));
        for (int i = 0; i < len; i++)
        {
            if ((fichierEntree != -1 && (i == fichierEntree || i == fichierEntree - 1)) ||
                (fichierErr != -1 && (i == fichierErr || i == fichierErr - 1)) ||
                (fichierSortie != -1 && (i == fichierSortie || i == fichierSortie - 1)))
            {
                continue;
            }
            new_argv[new_argc++] = argv[i];
        }
        new_argv[new_argc] = NULL;

        // Exécuter la commande
        execvp(command, new_argv);
        perror(command); // Affiche une erreur si execvp échoue
        free_args(new_argv);
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
        waitpid(pid, &status,WNOHANG);
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status))
        { // Si l'enfant s'est terminé de manière anormale
            return 2;
        }
        else
        {
            return 1;
        }
        
        
    }
}
