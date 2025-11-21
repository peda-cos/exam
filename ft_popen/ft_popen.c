#include <unistd.h>
#include <stdlib.h>

int ft_popen(const char *file, char *const argv[], char type)
{
    int fd[2], pid;

    if(!file || !argv || (type != 'r' && type != 'w') || pipe(fd) == -1)
        return(-1);
    if((pid = fork()) == -1)
    {
        close(fd[0]);
        close(fd[1]);
    }
    if(pid == 0)
    {
        if(type == 'r')
			dup2(fd[1], 1);
        else
        	dup2(fd[0], 0);
        close(fd[0]);
        close(fd[1]);
        execvp(file, argv);
        exit(1);
    }
    else
    {
        if(type == 'r')
        {
            close(fd[1]);
            return(fd[0]);
        }
        close(fd[0]);
        return(fd[1]);
    }
}
