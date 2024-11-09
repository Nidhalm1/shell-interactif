#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include "execute.h"
#include <errno.h>
#include "command.h"

int builtin_pwd()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        print(cwd);
        return 0;
    }
    return 1;
}

int builtin_cd(const char *path)
{   
    
    if (path == NULL)
    {
        path = getenv("HOME");
    }
    else if (strcmp(path,"-")==0)
    {
        /* code */
    }
    
    if (chdir(path) == -1)
    {
        print(strerror(errno));
        return 1;
    }
    return 0;
}

int builtin_ftype(const char *filename)
{
    struct stat path_stat;
    if (stat(filename, &path_stat) == -1)
    {
        print(strerror(errno));
        return 1;
    }
    if (S_ISDIR(path_stat.st_mode))
    {
        print("directory\n");
    }
    else if (S_ISREG(path_stat.st_mode))
    {
        print("regular file\n");
    }
    else if (S_ISLNK(path_stat.st_mode))
    {
        print("symbolic link\n");
    }
    else if (S_ISFIFO(path_stat.st_mode))
    {
        print("FIFO/pipe\n");
    }
    else
    {
        print("other\n");
    }
    return 0;
}

int execute_command(const char *command, int argc, char **argv)
{
    pid_t pid = fork();
    if (pid == 0)
    { // Processus enfant
        execvp(command, argv);
        printf("%s: command not found\n", command);
        exit(1);
    }
    else if (pid < 0)
    { // Erreur de fork
        print("Error executing command: ");
        print(command);
        print(strerror(errno));
        return 1;
    }
    else
    { // Processus parent
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
           return WEXITSTATUS(status);
        }
        else 
            if (WIFSIGNALED(status)){            // Si l'enfant s'est terminé de manière anormale (par signal)
                return 2;
            }
            return 1;
    }
}
