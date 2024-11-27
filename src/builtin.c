#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "../include/execute.h"
#include "../include/command.h"
#include <errno.h>

int builtin_pwd()
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