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

	n = strtol(argv[1], (char **)NULL, 10);

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	// Tratamento de erro de input não estava funcionando. Botei pra baixo pra poder usar meu_ranque e consertei.
	// Acrescentei também que 0 e 1 são valores válidos, cujo resultado é 0.

	if (n < 2)
	{
		if (meu_ranque == 0)
		{
			if (n < 0)
			{
				printf("Valor inválido! Entre com um valor do maior inteiro\n");
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
			MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &tam_buffer);
			tam_buffer = tam_buffer +  MPI_BSEND_OVERHEAD;
			buffer =(void*) malloc(tam_buffer);
			MPI_Buffer_attach(buffer, tam_buffer);
			MPI_Bsend(&cont, 1, MPI_INT, 0, etiq, MPI_COMM_WORLD);
		}
		else
		{
			total = cont;
			for (int i = 1; i < num_procs; i++)
			{
				MPI_Recv(&parcial, 1, MPI_INT, i, etiq, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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