#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int primo(long int n)
{ /* mpi_primos.c  */
	int i;

	for (i = 3; i < (int)(sqrt(n) + 1); i += 2)
	{
		if (n % i == 0)
			return 0;
	}
	return 1;
}

int main(int argc, char *argv[])
{
	double t_inicial, t_final;
	int cont = 0, total = 0, parcial = 0, etiq = 0;
	long int i, n;
	int meu_ranque, num_procs, inicio, salto;

	if (argc < 2)
	{
		printf("Valor inválido! Entre com um valor do maior inteiro\n");
		return 0;
	}
	else
	{
		n = strtol(argv[1], (char **)NULL, 10);
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Request pedido_recebe;

	// Acrescentei tratamento para 0 e 1 serem aceitos e darem 0, mas valores negativos serem inválidos.

	if (n < 2)
	{
		if (meu_ranque == 0)
		{
			if (n < 0)
			{
				printf("Números negativos não podem ser primos! Entre com um valor do maior inteiro positivo\n");
			}
			else
			{
				printf("Quant. de primos entre 1 e n: 0 \n");
			}
		}
		MPI_Finalize();
		return 0;
	}

	t_inicial = MPI_Wtime();
	inicio = 3 + meu_ranque * 2;
	salto = num_procs * 2;
	for (i = inicio; i <= n; i += salto)
	{
		if (primo(i) == 1)
			cont++;
	}

	if (num_procs > 1)
	{
		if (meu_ranque != 0)
		{
			for (int i = 1; i < num_procs; i++)
			{
				// Barreira para sincronizar os processos
				MPI_Barrier(MPI_COMM_WORLD);
				if (i == meu_ranque)
					/* Processos enviam cont para o processo 0 quando há um 
					processo de recebimento ativo em 0 (bloqueante) */
					MPI_Rsend(&cont, 1, MPI_INT, 0, etiq, MPI_COMM_WORLD);
			}
		}
		else
		{
			total = cont;
			for (int i = 1; i < num_procs; i++)
			{
				// Processo 0 recebe cont dos outros processos (bloqueante)
				MPI_Recv(&parcial, 1, MPI_INT, i, etiq, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
				/* DEADLOCK!!!! Recv bloqueia o processo 0 que não executa a barreira, 
				logo os outros processos não podem enviar */
				MPI_Barrier(MPI_COMM_WORLD);
				total += parcial;
			}
		}
	}
	else
	{
		total = cont;
	}

	t_final = MPI_Wtime();

	if (meu_ranque == 0)
	{
		total += 1; /* Acrescenta o dois, que também é primo */
		printf("Quant. de primos entre 1 e n: %d \n", total);
		printf("Tempo de execucao: %1.3f \n", t_final - t_inicial);
	}
	MPI_Finalize();
	return (0);
}