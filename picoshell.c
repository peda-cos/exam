/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peda-cos <peda-cos@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 18:43:51 by peda-cos          #+#    #+#             */
/*   Updated: 2025/11/02 19:54:16 by peda-cos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Nota 1: unistd.h fornece acesso as chamadas de sistema POSIX como fork, pipe, dup2, close e execvp */
#include <unistd.h>
/* Nota 2: stdlib.h e necessario para a funcao exit que termina processos filhos em caso de erro */
#include <stdlib.h>
/* Nota 3: sys/wait.h contem wait() para aguardar terminacao de processos filhos */
#include <sys/wait.h>
/* Nota 4: stdio.h e string.h sao incluidos mas nao utilizados nesta implementacao */
#include <stdio.h>
#include <string.h>

/* Nota 5: Esta funcao implementa um pipeline de comandos similar ao operador | do shell
 * Recebe um array de comandos onde cada comando e um array de strings (formato execvp)
 * Exemplo: cmds[0] = {"ls", "-l", NULL}, cmds[1] = {"grep", "txt", NULL}, cmds[2] = NULL
 * O pipeline conecta a saida (stdout) de cada comando a entrada (stdin) do proximo */
int	picoshell(char **cmds[])
{
	/* Nota 6: pid_t e o tipo usado para armazenar IDs de processos retornados por fork() */
	pid_t	pid;
	/* Nota 7: fd[2] e um array para o pipe: fd[0] e a extremidade de leitura, fd[1] e a de escrita
	 * Quando criamos um pipe, os dados escritos em fd[1] podem ser lidos de fd[0] */
	int		fd[2];
	/* Nota 8: last_fd armazena o descritor de arquivo da extremidade de leitura do pipe anterior
	 * Isso permite conectar a saida do comando anterior a entrada do comando atual */
	int		last_fd;
	/* Nota 9: i e o indice para iterar sobre o array de comandos cmds */
	int		i;

	/* Nota 10: Inicializamos last_fd com -1 para indicar que nao ha pipe anterior
	 * O primeiro comando ira ler de stdin (nao de um pipe) */
	last_fd = -1;
	/* Nota 11: Comecamos do primeiro comando (indice 0) */
	/* Nota 11: Comecamos do primeiro comando (indice 0) */
	i = 0;
	/* Nota 12: Loop principal que itera sobre cada comando ate encontrar NULL (fim do array)
	 * Cada iteracao cria um novo processo filho para executar um comando do pipeline */
	while (cmds[i])
	{
		/* Nota 13: Se existe um proximo comando (cmds[i + 1]), precisamos criar um pipe
		 * para conectar a saida do comando atual a entrada do proximo comando
		 * pipe(fd) retorna -1 em caso de erro (ex: limite de descritores atingido) */
		if (cmds[i + 1] && pipe(fd) == -1)
			return (1);
		/* Nota 14: fork() cria um processo filho identico ao pai
		 * Retorna 0 no processo filho, o PID do filho no pai, ou -1 em erro
		 * Apos fork(), temos dois processos executando o mesmo codigo */
		pid = fork();
		/* Nota 15: Se fork() falhar, precisamos fazer limpeza dos recursos
		 * Se um pipe foi criado para este comando, devemos fecha-lo antes de retornar */
		/* Nota 15: Se fork() falhar, precisamos fazer limpeza dos recursos
		 * Se um pipe foi criado para este comando, devemos fecha-lo antes de retornar */
		if (pid == -1)
		{
			/* Nota 16: Se criamos um pipe (ha um proximo comando), ambas extremidades devem ser fechadas
			 * Fechar descritores evita vazamento de recursos (resource leak) no sistema operacional */
			if (cmds[i + 1])
			{
				close(fd[0]);
				close(fd[1]);
			}
			/* Nota 17: Retornamos 1 para indicar erro conforme especificacao da funcao */
			return (1);
		}
		/* Nota 18: Este bloco e executado apenas pelo processo filho (pid == 0)
		 * O processo filho ira configurar redirecionamentos e executar o comando */
		if (pid == 0)
		{
			/* Nota 19: Se last_fd != -1, existe um pipe anterior (nao e o primeiro comando)
			 * Precisamos redirecionar stdin para ler do pipe anterior */
			if (last_fd != -1)
			{
				/* Nota 20: dup2(last_fd, STDIN_FILENO) duplica last_fd para o descritor 0 (stdin)
				 * Agora quando o comando ler de stdin, estara lendo do pipe anterior
				 * Se dup2 falhar, terminamos o processo filho com exit(1) */
				if (dup2(last_fd, STDIN_FILENO) == -1)
					exit(1);
				/* Nota 21: Apos duplicar, fechamos o descritor original pois nao e mais necessario
				 * Manter descritores desnecessarios abertos e ma pratica e pode causar problemas */
				close(last_fd);
			}
			/* Nota 22: Se existe um proximo comando, precisamos redirecionar stdout
			 * para escrever no pipe que criamos, conectando este comando ao proximo */
			if (cmds[i + 1])
			{
				/* Nota 23: Fechamos a extremidade de leitura do pipe pois este processo so vai escrever
				 * Cada extremidade do pipe deve ser fechada nos processos que nao a utilizam */
				close(fd[0]);
				/* Nota 24: dup2(fd[1], STDOUT_FILENO) duplica fd[1] para o descritor 1 (stdout)
				 * Agora quando o comando escrever para stdout, ira escrever no pipe
				 * O proximo comando podera ler esses dados de sua stdin */
				if (dup2(fd[1], STDOUT_FILENO) == -1)
					exit(1);
				/* Nota 25: Fechamos fd[1] apos duplicar, pois stdout agora aponta para o pipe */
				close(fd[1]);
			}
			/* Nota 26: execvp() substitui a imagem do processo atual pelo novo programa
			 * cmds[i][0] e o nome/caminho do comando, cmds[i] e o array de argumentos completo
			 * Se execvp retornar, significa que houve erro (comando nao encontrado, sem permissao, etc)
			 * O 'p' em execvp significa que a funcao busca o comando no PATH */
			execvp(cmds[i][0], cmds[i]);
			/* Nota 27: Esta linha so e executada se execvp falhar
			 * Terminamos o processo filho com status 1 indicando erro */
			exit(1);
		}
		/* Nota 28: Este codigo e executado apenas pelo processo pai (pid != 0)
		 * O pai e responsavel por gerenciar os pipes entre comandos */

		/* Nota 29: Se havia um pipe anterior (last_fd != -1), o pai deve fecha-lo
		 * O filho que precisava dele ja fez dup2 e tem sua propria copia
		 * Fechar no pai evita que o proximo comando fique esperando dados indefinidamente */
		if (last_fd != -1)
			close(last_fd);
		/* Nota 30: Se criamos um pipe para este comando, gerenciamos suas extremidades
		 * O pai precisa fechar a extremidade de escrita e guardar a de leitura */
		if (cmds[i + 1])
		{
			/* Nota 31: Fechamos fd[1] (escrita) pois o pai nao vai escrever neste pipe
			 * Apenas o filho atual vai escrever nele. Se nao fecharmos, o proximo comando
			 * pode nunca receber EOF e ficar esperando mais dados eternamente */
			close(fd[1]);
			/* Nota 32: Salvamos fd[0] (leitura) em last_fd para o proximo comando usar
			 * O proximo filho ira dup2 este descritor para sua stdin */
			last_fd = fd[0];
		}
		/* Nota 33: Avancamos para o proximo comando no array */
		i++;
	}
	/* Nota 34: Apos criar todos os processos filhos, o pai deve aguardar que terminem
	 * wait(NULL) bloqueia ate que um filho termine e retorna seu PID
	 * Retorna -1 quando nao ha mais filhos, entao o loop termina */
	while (wait(NULL) > 0)
	{
		/* Nota 35: O corpo do loop esta vazio (apenas um ponto e virgula)
		 * Isso e valido em C - queremos apenas aguardar, sem processar o status de saida
		 * Esta pratica garante que nao criaremos processos zumbis (zombies) */
		;
	}
	/* Nota 36: Retornamos 0 para indicar sucesso
	 * Todos os comandos foram executados e todos os processos filhos terminaram */
	return (0);
}
