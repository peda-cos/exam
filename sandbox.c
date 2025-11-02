/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandbox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: peda-cos <peda-cos@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 19:24:31 by peda-cos          #+#    #+#             */
/*   Updated: 2025/11/02 19:59:54 by peda-cos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Nota 1: Bibliotecas necessarias para manipulacao de processos e sinais */
/* unistd.h: fornece alarm() para definir timeouts */
/* signal.h: fornece sigaction() para capturar sinais como SIGALRM */
/* sys/wait.h: fornece waitpid() e macros como WIFEXITED e WIFSIGNALED */
/* sys/types.h: define tipos como pid_t usado para identificadores de processo */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

/* Nota 2: Handler (manipulador) de sinal para SIGALRM */
/* Esta funcao e chamada quando o alarme dispara (timeout) */
/* O parametro 'sig' contem o numero do sinal recebido */
void	alarm_handler(int sig)
{
	/* Nota 3: (void)sig silencia avisos do compilador sobre parametro nao usado */
	/* O handler pode ficar vazio pois apenas precisamos interromper waitpid() */
	(void)sig;
}

/* Nota 4: Funcao principal que executa uma funcao em ambiente isolado (sandbox) */
/* Parametros: */
/*   - f: ponteiro para a funcao a ser testada */
/*   - timeout: tempo maximo (em segundos) que a funcao pode executar */
/*   - verbose: se true, imprime mensagens sobre o resultado */
/* Retorna: 1 se a funcao e "boa", 0 se e "ruim", -1 em caso de erro */
int	sandbox(void (*f)(void), unsigned int timetout, bool verbose)
{
	/* Nota 5: struct sigaction define como um sinal deve ser tratado */
	/* sa_handler: ponteiro para a funcao que tratara o sinal */
	/* sa_flags: opcoes de comportamento (0 = comportamento padrao) */
	/* sa_mask: conjunto de sinais a bloquear durante a execucao do handler */
	struct sigaction	sa;
	/* Nota 6: pid_t armazena o ID do processo */
	/* Usado para identificar o processo filho criado por fork() */
	pid_t				pid;
	/* Nota 7: Variavel para armazenar o status de saida do processo filho */
	/* Sera analisada pelas macros WIFEXITED, WEXITSTATUS, WIFSIGNALED */
	int					status;

	/* Nota 8: Configuracao do handler para o sinal SIGALRM */
	/* SIGALRM e enviado quando alarm() expira */
	sa.sa_handler = alarm_handler;
	/* Nota 9: sa_flags = 0 significa usar comportamento padrao (sem flags especiais) */
	/* Flags como SA_RESTART poderiam reiniciar syscalls interrompidas */
	sa.sa_flags = 0;
	/* Nota 10: sigemptyset inicializa sa_mask como conjunto vazio */
	/* Significa que nenhum sinal adicional sera bloqueado durante o handler */
	sigemptyset(&sa.sa_mask);
	/* Nota 11: sigaction registra o handler para SIGALRM */
	/* A partir daqui, quando SIGALRM for recebido, alarm_handler sera executado */
	sigaction(SIGALRM, &sa, NULL);
	/* Nota 12: fork() cria um processo filho identico ao pai */
	/* Retorna 0 no processo filho, o PID do filho no pai, ou -1 em caso de erro */
	pid = fork();
	/* Nota 13: Verifica se fork() falhou */
	/* fork() pode falhar se nao houver recursos suficientes no sistema */
	if (pid == -1)
		return (-1);
	/* Nota 14: Bloco executado apenas pelo processo filho (pid == 0) */
	if (pid == 0)
	{
		/* Nota 15: Executa a funcao a ser testada no processo filho */
		/* Isso isola a funcao: se ela crashar, nao afeta o processo pai */
		f();
		/* Nota 16: Se f() retornar normalmente, o filho encerra com codigo 0 */
		/* exit(0) indica sucesso */
		exit(0);
	}
	/* Nota 17: Processo pai: configura alarme para implementar timeout */
	/* alarm(timeout) programa o envio de SIGALRM apos 'timeout' segundos */
	alarm(timeout);
	/* Nota 18: waitpid() espera o processo filho terminar */
	/* Parametros: pid do filho, ponteiro para status, opcoes (0 = bloqueante) */
	/* Retorna o PID do filho se bem-sucedido, ou -1 em caso de erro */
	if (waitpid(pid, &status, 0) == -1)
	{
		/* Nota 19: errno == EINTR significa que waitpid() foi interrompido por um sinal */
		/* No nosso caso, isso acontece quando o alarme dispara (timeout) */
		if (errno == EINTR)
		{
			/* Nota 20: kill() envia SIGKILL ao filho para forcar terminacao imediata */
			/* SIGKILL nao pode ser ignorado nem capturado pelo processo */
			kill(pid, SIGKILL);
			/* Nota 21: waitpid() novamente para coletar o processo filho */
			/* Isso evita deixar processos zumbis (zombie processes) */
			waitpid(pid, NULL, 0);
			/* Nota 22: Se verbose, informa que a funcao excedeu o tempo limite */
			if (verbose)
				printf("Bad function: timed out afeter %u seconds\n", timeout);
			/* Nota 23: Retorna 0 indicando que a funcao e "ruim" (timeout) */
			return (0);
		}
		/* Nota 24: Se waitpid() falhou por outro motivo, retorna erro (-1) */
		return (-1);
	}
	/* Nota 25: WIFEXITED verifica se o processo terminou normalmente (via exit/return) */
	/* Retorna true se o filho chamou exit() ou retornou de main() */
	if (WIFEXITED(status))
	{
		/* Nota 26: WEXITSTATUS extrai o codigo de saida do processo */
		/* Este e o valor passado para exit() ou retornado de main() */
		if (WEXITSTATUS(status) == 0)
		{
			/* Nota 27: Codigo de saida 0 indica sucesso */
			/* A funcao executou completamente sem erros */
			if (verbose)
				printf("Nice function!\n");
			/* Nota 28: Retorna 1 indicando que a funcao e "boa" */
			return (1);
		}
		else
		{
			/* Nota 29: Codigo de saida diferente de 0 indica falha */
			/* A funcao chamou exit() com valor != 0 */
			if (verbose)
				printf("Bad function: exited with code %d\n", WEXITSTATUS(status));
			/* Nota 30: Retorna 0 indicando que a funcao e "ruim" */
			return (0);
		}
	}
	/* Nota 31: WIFSIGNALED verifica se o processo foi terminado por um sinal */
	/* Retorna true se o filho foi morto por SIGSEGV, SIGABRT, etc. */
	if (WIFSIGNALED(status))
	{
		/* Nota 32: WTERMSIG extrai o numero do sinal que matou o processo */
		/* strsignal converte o numero do sinal em string descritiva */
		/* Exemplo: SIGSEGV vira "Segmentation fault" */
		if (verbose)
			printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
		/* Nota 33: Funcoes que crasham (segfault, abort, etc) sao "ruins" */
		return (0);
	}
	/* Nota 34: Se chegamos aqui, algo inesperado aconteceu */
	/* Retorna -1 indicando erro na propria funcao sandbox */
	return (-1);
}
