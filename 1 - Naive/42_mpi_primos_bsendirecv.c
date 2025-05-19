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
	void *buffer;
	int tam_buffer;

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
			MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &tam_buffer); // Calcula o tamanho do buffer
			tam_buffer = tam_buffer + MPI_BSEND_OVERHEAD; // Adiciona o espaço extra exigido pelo MPI
			buffer = (void *)malloc(tam_buffer);
			// Anexa o buffer ao comunicador
			MPI_Buffer_attach(buffer, tam_buffer);
			/* Processos enviam cont para o processo 0 com buffer definido 
			e esperam o envio terminar para continuar (bloqueante) */
			MPI_Bsend(&cont, 1, MPI_INT, 0, etiq, MPI_COMM_WORLD);
		}
		else
		{
			total = cont;
			for (int i = 1; i < num_procs; i++)
			{
				/* Processo 0 pretende receber cont dos outros processos, 
				esperam o envio terminar para continuar (não é bloqueante) */
				MPI_Irecv(&parcial, 1, MPI_INT, i, etiq, MPI_COMM_WORLD, &pedido_recebe);
				// Bloqueia a execução até que a recepção esteja concluída pois precisa do valor de parcial
				MPI_Wait(&pedido_recebe, MPI_STATUS_IGNORE);
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