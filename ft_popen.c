/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peda-cos <peda-cos@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 16:02:27 by peda-cos          #+#    #+#             */
/*   Updated: 2025/11/02 19:59:54 by peda-cos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Nota 1: Inclusao de bibliotecas necessarias para manipulacao de processos */
/* unistd.h: fornece acesso a chamadas de sistema POSIX (pipe, fork, dup2, close, execvp) */
/* stdlib.h: fornece funcoes de uso geral como exit() */
/* sys/types.h: define tipos de dados usados em chamadas de sistema como pid_t */
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

/* Nota 2: Funcao ft_popen - implementacao customizada inspirada em popen() da libc */
/* Esta funcao cria um processo filho e estabelece comunicacao via pipe */
/* Parametros: */
/*   - file: caminho do executavel a ser lancado */
/*   - argv: array de argumentos terminado em NULL (formato do execvp) */
/*   - type: 'r' para ler a saida do comando, 'w' para escrever na entrada */
/* Retorna: file descriptor do pipe ou -1 em caso de erro */
int	ft_popen(const char *file, char *const argv[], char type)
{
	/* Nota 3: Declaracao do array de file descriptors para o pipe */
	/* fd[0] sera a extremidade de leitura, fd[1] sera a extremidade de escrita */
	int		fd[2];

	/* Nota 4: pid_t e o tipo usado para armazenar IDs de processos */
	/* Apos o fork(), contera 0 no processo filho e o PID do filho no processo pai */
	pid_t	pid;

	/* Nota 5: Validacao de parametros antes de prosseguir */
	/* Verifica se ponteiros sao validos e se type e 'r' (read) ou 'w' (write) */
	/* Esta validacao previne comportamento indefinido e erros de seguranca */
	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);

	/* Nota 6: Criacao do pipe - canal de comunicacao unidirecional */
	/* pipe() cria dois file descriptors: fd[0] para leitura e fd[1] para escrita */
	/* Dados escritos em fd[1] podem ser lidos em fd[0] */
	/* Retorna -1 em caso de falha (ex: limite de file descriptors atingido) */
	if (pipe(fd) == -1)
		return (-1);

	/* Nota 7: fork() cria um processo filho clonando o processo atual */
	/* O processo filho herda copias dos file descriptors abertos */
	/* Retorna 0 no filho, PID do filho no pai, ou -1 em caso de erro */
	pid = fork();

	/* Nota 8: Tratamento de erro do fork() */
	/* Se fork() falha, precisamos limpar os file descriptors do pipe */
	/* Isso evita vazamento (leak) de recursos do sistema */
	if (pid == -1)
	{
		/* Nota 9: Fechamento de ambas extremidades do pipe */
		/* close() libera o file descriptor, permitindo sua reutilizacao pelo sistema */
		/* IMPORTANTE: sempre fechar FDs nao utilizados para evitar esgotamento de recursos */
		close(fd[0]);
		close(fd[1]);
		return (-1);
	}

	/* Nota 10: Bloco executado APENAS pelo processo filho (pid == 0) */
	/* O filho sera responsavel por executar o comando especificado */
	if (pid == 0)
	{
		/* Nota 11: Configuracao para modo leitura ('r') */
		/* O processo pai quer LER a saida do comando filho */
		if (type == 'r')
		{
			/* Nota 12: dup2() duplica fd[1] sobre STDOUT_FILENO (file descriptor 1) */
			/* Isso redireciona a saida padrao (stdout) do processo para a escrita do pipe */
			/* Assim, tudo que o comando imprimir ira para o pipe em vez do terminal */
			/* STDOUT_FILENO e uma constante definida em unistd.h com valor 1 */
			if (dup2(fd[1], STDOUT_FILENO) == -1)
				exit(1);
		}
		/* Nota 13: Configuracao para modo escrita ('w') */
		/* O processo pai quer ESCREVER na entrada do comando filho */
		else
		{
			/* Nota 14: dup2() duplica fd[0] sobre STDIN_FILENO (file descriptor 0) */
			/* Isso redireciona a entrada padrao (stdin) do processo para a leitura do pipe */
			/* Assim, o comando lerá dados do pipe em vez do teclado */
			/* STDIN_FILENO e uma constante definida em unistd.h com valor 0 */
			if (dup2(fd[0], STDIN_FILENO) == -1)
				exit(1);
		}

		/* Nota 15: Fechamento dos file descriptors originais do pipe no filho */
		/* Apos dup2(), os FDs originais nao sao mais necessarios */
		/* O pipe agora esta acessivel atraves de STDIN (fd 0) ou STDOUT (fd 1) */
		/* Fechar estes FDs previne vazamento e libera recursos */
		close(fd[0]);
		close(fd[1]);

		/* Nota 16: execvp() substitui a imagem do processo filho pelo novo programa */
		/* Parametros: */
		/*   - file: nome ou caminho do executavel (busca no PATH se necessario) */
		/*   - argv: array de strings com argumentos, terminado em NULL */
		/* Se execvp() tem sucesso, o codigo abaixo NUNCA e executado */
		/* O processo e completamente substituido pelo novo programa */
		execvp(file, argv);

		/* Nota 17: Se chegamos aqui, execvp() falhou */
		/* Possíveis causas: arquivo nao encontrado, sem permissao de execucao, etc. */
		/* exit(1) termina o processo filho com codigo de erro */
		/* IMPORTANTE: nunca fazer return aqui, pois o filho deve terminar */
		exit(1);
	}
	/* Nota 18: Bloco executado APENAS pelo processo pai (pid > 0) */
	/* O pai mantem uma extremidade do pipe e retorna o FD para o chamador */
	else
	{
		/* Nota 19: Modo leitura - o pai quer ler a saida do comando */
		if (type == 'r')
		{
			/* Nota 20: Fecha a extremidade de escrita do pipe */
			/* O pai nao vai escrever no pipe, apenas ler */
			/* Fechar a extremidade nao utilizada e crucial: */
			/* 1. Evita vazamento de file descriptors */
			/* 2. Permite que o pipe sinalize EOF quando o filho terminar */
			close(fd[1]);

			/* Nota 21: Retorna a extremidade de leitura (fd[0]) */
			/* O chamador pode usar read() ou funcoes similares neste FD */
			/* para ler a saida do comando executado pelo filho */
			return (fd[0]);
		}
		/* Nota 22: Modo escrita - o pai quer escrever na entrada do comando */
		else
		{
			/* Nota 23: Fecha a extremidade de leitura do pipe */
			/* O pai nao vai ler do pipe, apenas escrever */
			/* Fechamento adequado e essencial para o funcionamento correto */
			close(fd[0]);

			/* Nota 24: Retorna a extremidade de escrita (fd[1]) */
			/* O chamador pode usar write() ou funcoes similares neste FD */
			/* para enviar dados para a entrada (stdin) do comando filho */
			/* IMPORTANTE: o chamador deve fechar este FD quando terminar */
			/* para sinalizar EOF ao processo filho */
			return (fd[1]);
		}
	}
}
