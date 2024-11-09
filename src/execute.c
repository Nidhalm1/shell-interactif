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

int builtin_pwd()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printt(cwd);
        return 0;
    }
    return 1;
}

int builtin_cd(const char *path)
{   
    char current_dir[1076];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        printt(strerror(errno));
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
            printt("cd: OLDPWD not set\n");
            return 1;
        }
        printt(oldpwd);
        path = oldpwd;
    }
    
    if (chdir(path) == -1)
    {
        printt(strerror(errno));
        return 1;
    }
    setenv("OLDPWD", current_dir, 1);
    return 0;
}

int builtin_ftype(const char *filename)
{
    struct stat path_stat;
    if (stat(filename, &path_stat) == -1)
    {
        printt(strerror(errno));
        return 1;
    }
    if (S_ISDIR(path_stat.st_mode))
    {
        printt("directory\n");
    }
    else if (S_ISREG(path_stat.st_mode))
    {
        printt("regular file\n");
    }
    else if (S_ISLNK(path_stat.st_mode))
    {
        printt("symbolic link\n");
    }
    else if (S_ISFIFO(path_stat.st_mode))
    {
        printt("FIFO/pipe\n");
    }
    else
    {
        printt("other\n");
    }
    return 0;
}

int execute_command(const char *command,char **argv)
{
    pid_t pid = fork();
    if (pid == 0)
    { // Processus enfant
        execvp(command, argv);
        printt(command);
        printt(": command not found\n");
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
        if (WIFEXITED(status)) {
           return WEXITSTATUS(status);
        }
        else{
            if (WIFSIGNALED(status)){            // Si l'enfant s'est terminé de manière anormale (par signal)
                return 2;
            }
            return 1;
        }
    }
}
