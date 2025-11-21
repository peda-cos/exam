#include <stdio.h>
#include <unistd.h>

int ft_popen(const char *file, char *const argv[], char type);

int main(void)
{
    char *argv[] = { "/bin/ls", NULL };

    int fd = ft_popen("/bin/ls", argv, 'r');

    char buf[1000];

    int lidos;

    while ((lidos = read(fd, buf, 1000)) > 0)
    {
        buf[lidos] = '\0';
        printf("%s", buf);
    }

    close(fd);
    return (0);
}
