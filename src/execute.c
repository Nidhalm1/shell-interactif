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
int execute_command(const char *command, int fd0, int fd1, char **argv)
{
    int fd2 = STDERR_FILENO; // pour les erreurs
    pid_t pid = fork();
    if (pid == 0)
    { // Processus enfant
        int len = lenn(argv);
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
            supprimeS(argv, argv[fichierEntree]);
            supprimeS(argv, argv[fichierEntree - 1]);
        }

        // Gestion de la sortie
        if (fichierSortie != -1)
        {
            if (ecrase)
            {
                fd1 = open(argv[fichierSortie], O_WRONLY | O_CREAT | O_TRUNC, 0777);
            }
            else if (ajoute)
            {
                fd1 = open(argv[fichierSortie], O_WRONLY | O_CREAT | O_APPEND, 0777);
            }
            else
            {
                fd1 = open(argv[fichierSortie], O_WRONLY | O_CREAT | O_EXCL, 0777);
            }
            if (fd1 == -1)
            {
                perror("Erreur d'ouverture de fichier de sortie");
                exit(1);
            }
            supprimeS(argv, argv[fichierSortie]);
            supprimeS(argv, argv[fichierSortie - 1]);
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
                fd2 = open(argv[fichierErr], O_WRONLY | O_CREAT | O_EXCL, 0777);
            }
            if (fd2 == -1)
            {
                perror("Erreur d'ouverture du fichier d'erreur");
                exit(1);
            }
            supprimeS(argv, argv[fichierErr]);
            supprimeS(argv, argv[fichierErr - 1]);
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

        // Exécuter la commande
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

void supprimeS(char **s, char *ss)
{
    int len = lenn(s);
    for (int i = 0; i < len; i++)
    {
        if (strcmp(s[i], ss) == 0)
        {
            free(s[i]);
            memmove(s + i, s + i + 1, len - i - 1);
            s[len - 1] = NULL;
            break;
        }
    }
}
int lenn(char **s)
{
    int lenn = 0;
    while (s[lenn] != NULL)
    {
        lenn++;
    }
    return lenn;
}
