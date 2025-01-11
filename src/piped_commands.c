#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int parse_and_execute_pipe(int argcc, char **s)
{
    int argc = argcc;
    pid_t parent_pid = getpid();
    char **s_copy = malloc((argc + 1) * sizeof(char *));
    if (s_copy == NULL) {
        perror("malloc");
        exit(1);
    }
    for (int i = 0; i < argc; i++) {
        s_copy[i] = strdup(s[i]);
        if (s_copy[i] == NULL) {
            perror("strdup");
            exit(1);
        }
    }
    s_copy[argc] = NULL;
    pid_t pid = fork();
    if (pid == 0)
    {
        int i;
        int in_fd = STDIN_FILENO;
        int fd[2];

        for (i = 0; i < argc; i++)
        {
            if (strcmp(s_copy[i], "|") == 0)
            {
                s_copy[i] = NULL;
                if (pipe(fd) == -1) {
                    perror("pipe");
                    exit(1);
                }
                pid = fork();
                if (pid == 0)
                {
                    dup2(in_fd, STDIN_FILENO);
                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[0]);
                    close(fd[1]);
                    execvp(s_copy[0], s_copy);
                    perror("execvp");
                    exit(1);
                }
                else if (pid > 0)
                {
                    close(fd[1]);
                    if (in_fd != STDIN_FILENO)
                        close(in_fd);
                    in_fd = fd[0];
                    s_copy += i + 1;
                    argc -= (i + 1);
                    i = -1;
                }
                else
                {
                    perror("fork");
                    exit(1);
                }
            }
        }
        if (in_fd != STDIN_FILENO)
        {
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        execvp(s_copy[0], s_copy);
        perror("execvp");
        exit(1);
    }
    // Attendre tous les processus enfants
    while (wait(NULL) > 0)
    {
        /* code */
    }
    return 0;
    void free_argg(char **s, int size) {
        for (int i = 0; i < size; i++) {
            if (s[i] != NULL) {
                free(s[i]);
            }
        }
        free(s);
    }
}
