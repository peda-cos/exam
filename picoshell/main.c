#include <stdlib.h>
#include <stdio.h>

int picoshell(char **cmds[]);

int main(void)
{
    char *cmd1[] = { "/bin/ls", NULL };

    char *cmd2[] = { "/bin/cat", "-e", NULL };

    char **cmds[] = { cmd1, cmd2, NULL };

    int ret = picoshell(cmds);

    printf("Retorno da picoshell: %d\n", ret);
    return ret;
}
