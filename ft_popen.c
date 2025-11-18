#include <unistd.h>
#include <stdlib.h>

int ft_popen(const char *file, char *const argv[], char type)
{
    int p[2], pid;

    // 1. Validação e Pipe
    if (!file || !argv || (type != 'r' && type != 'w') || pipe(p) == -1)
        return (-1);

    // 2. Fork
    if ((pid = fork()) == -1)
    {
        close(p[0]); close(p[1]); // Limpeza em caso de erro
        return (-1);
    }

    // 3. Filho: Conecta e Executa
    if (pid == 0)
    {
        if (type == 'r') // Se pai lê, filho escreve no STDOUT
            dup2(p[1], 1);
        else             // Se pai escreve, filho lê do STDIN
            dup2(p[0], 0);

        close(p[0]); // Limpeza obrigatória
		close(p[1]); // Limpeza obrigatória
        execvp(file, argv);
        exit(1);
    }

    // 4. Pai: Fecha o não usado e retorna o correto
    if (type == 'r')
    {
        close(p[1]); // Fecha escrita
        return (p[0]); // Retorna leitura
    }
    close(p[0]); // Fecha leitura
    return (p[1]); // Retorna escrita
}
