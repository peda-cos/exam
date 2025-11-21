#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int	picoshell(char **cmds[])
{
    int i = 0, fd[2], tmp_fd = 0;
    pid_t pid;

    while (cmds[i])
    {
        if (cmds[i + 1] && pipe(fd) == -1)
        {
            if (tmp_fd)
				close(tmp_fd);
            return (1);
        }
        if ((pid = fork()) == -1)
        {
            if (tmp_fd) close(tmp_fd);
            if (cmds[i + 1])
			{
				close(fd[0]);
				close(fd[1]);
			}
            return (1);
        }
        if (pid == 0)
        {
            if (tmp_fd && dup2(tmp_fd, 0) == -1)
				exit(1);
            if (tmp_fd)
				close(tmp_fd);
            if (cmds[i + 1])
            {
                close(fd[0]);
                if (dup2(fd[1], 1) == -1)
					exit(1);
                close(fd[1]);
            }
            execvp(cmds[i][0], cmds[i]);
            exit(1);
        }
        if (tmp_fd) close(tmp_fd);
        if (cmds[i + 1])
		{
			close(fd[1]);
			tmp_fd = fd[0];
		}
        i++;
    }
    while (wait(NULL) != -1);
    return (0);
}
