#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../include/execute.h"
#include "../include/command.h"
#include <errno.h>

int builtin_pwd(char **argv)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printt(cwd);
        printt("\n");
        return 0;
    }
    return 1;
}

int builtin_cd(const char *path)
{
    char current_dir[1076];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        printt("cd: ");
        printt(strerror(errno));
        printt("\n");
        return 1;
    }
    if (path == NULL)
    {
        path = getenv("HOME");
    }
    else if (strcmp(path, "-") == 0)
    {
        char *oldpwd = getenv("OLDPWD");
        if (oldpwd == NULL)
        {
            printt("cd: OLDPWD not set");
            return 1;
        }
        path = oldpwd;
    }

    if (chdir(path) == -1)
    {
        printt(strerror(errno));
        printt("\n");
        return 1;
    }
    setenv("OLDPWD", current_dir, 1);
    return 0;
}

int builtin_ftype(const char *filename)
{
    struct stat path_stat;
    if (lstat(filename, &path_stat) == -1)
    {
        printerr("ftype: ");
        perror(strerror(errno));
        printerr("\n");
        return 1;
    }
    if (S_ISLNK(path_stat.st_mode))
    {
        printt("symbolic link\n");
    }
    else if (S_ISDIR(path_stat.st_mode))
    {
        printt("directory\n");
    }
    else if (S_ISREG(path_stat.st_mode))
    {
        printt("regular file\n");
    }
    else if (S_ISFIFO(path_stat.st_mode))
    {
        printt("named pipe\n");
    }
    else
    {
        printt("other\n");
    }
    return 0;
}

// Fonction pour exécuter une commande interne avec redirection
int redirections(int fd0, int fd1, char **argv, int argc)
{
    int fd2 = STDERR_FILENO; // pour les erreurs
    int len = lenn(argv);
    int fichierEntree = -1, fichierSortie = -1, fichierErr = -1, ecrase = 0, ajoute = 0, ecraserr = 0, ajouterr = 0;

    // Sauvegarder les descripteurs de fichiers originaux
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stderr = dup(STDERR_FILENO);

    if (saved_stdin == -1 || saved_stdout == -1 || saved_stderr == -1)
    {
        perror("Erreur lors de la sauvegarde des descripteurs");
        return 1;
    }

    // Analyse des redirections dans les arguments
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

    // Gestion de l'entrée (redirection <)
    if (fichierEntree != -1)
    {
        fd0 = open(argv[fichierEntree], O_RDONLY);
        if (fd0 == -1)
        {
            perror("Erreur d'ouverture de fichier d'entrée");
            goto cleanup;
        }
        if (dup2(fd0, STDIN_FILENO) == -1)
        {
            perror("Erreur de redirection d'entrée");
            goto cleanup;
        }
        close(fd0);
    }

    // Gestion de la sortie (redirection >, >>, >|, etc.)
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
            goto cleanup;
        }
        if (dup2(fd1, STDOUT_FILENO) == -1)
        {
            perror("Erreur de redirection de sortie");
            goto cleanup;
        }
        close(fd1);
    }

    // Gestion de la redirection d'erreur (redirection 2>)
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
            goto cleanup;
        }
        if (dup2(fd2, STDERR_FILENO) == -1)
        {
            perror("Erreur de redirection d'erreur");
            goto cleanup;
        }
        close(fd2);
    }

    // Exécuter la commande
    int ret = parse_and_execute_simple(argc, argv);

    // Restauration des descripteurs de fichiers d'origine après l'exécution
cleanup:
    if (dup2(saved_stdin, STDIN_FILENO) == -1)
    {
        perror("Erreur lors de la restauration de stdin");
        ret = 1;
    }
    if (dup2(saved_stdout, STDOUT_FILENO) == -1)
    {
        perror("Erreur lors de la restauration de stdout");
        ret = 1;
    }
    if (dup2(saved_stderr, STDERR_FILENO) == -1)
    {
        perror("Erreur lors de la restauration de stderr");
        ret = 1;
    }

    // Fermeture des descripteurs sauvegardés
    close(saved_stdin);
    close(saved_stdout);
    close(saved_stderr);

    return ret;
}
