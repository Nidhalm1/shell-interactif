#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include "execute.h"
#include <errno.h>

int builtin_pwd()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
        return 0;
    }
    return 1;
}

int builtin_cd(const char *path)
{
    if (path == NULL || strcmp(path, "-") == 0)
    {
        path = getenv("HOME");
    }
    if (chdir(path) == -1)
    {
        fprintf(stderr, "cd: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}

int builtin_ftype(const char *filename)
{
    struct stat path_stat;
    if (stat(filename, &path_stat) == -1)
    {
        fprintf(stderr, "ftype: cannot stat '%s'\n", filename);
        return 1;
    }
    if (S_ISDIR(path_stat.st_mode))
    {
        printf("directory\n");
    }
    else if (S_ISREG(path_stat.st_mode))
    {
        printf("regular file\n");
    }
    else if (S_ISLNK(path_stat.st_mode))
    {
        printf("symbolic link\n");
    }
    else if (S_ISFIFO(path_stat.st_mode))
    {
        printf("named pipe\n");
    }
    else
    {
        printf("other\n");
    }
    return 0;
}

int execute_command(const char *command, int argc, char **argv)
{

    pid_t pid = fork();
    if (pid == 0)
    { // Processus enfant
        execlp(command, command, NULL);
        fprintf(stderr, "Error executing command: %s\n", command);
        exit(1);
    }
    else if (pid < 0)
    { // Erreur de fork
        fprintf(stderr, "Error executing command: %s\n", command);
        return 1;
    }
    else
    { // Processus parent
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}
