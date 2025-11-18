#include <unistd.h>
#include <stdlib.h>

int ft_popen(const char *file, char *const argv[], char type)
{
    int p[2];
    int pid;

    if (!file || !argv || (type != 'r' && type != 'w'))
        return (-1);
    if (pipe(p) == -1)
        return (-1);
    pid = fork();
    if (pid == -1)
    {
        close(p[0]);
        close(p[1]);
        return (-1);
    }
    if (pid == 0)
    {
        if (type == 'r')
        {
            dup2(p[1], 1);
            close(p[0]);
            close(p[1]);
        }
        else
        {
            dup2(p[0], 0);
            close(p[0]);
            close(p[1]);
        }
        execvp(file, argv);
        exit(1);
    }
    if (type == 'r')
    {
        close(p[1]);
        return (p[0]);
    }
    close(p[0]);
    return (p[1]);
}
