Neste trabalho de Laboratório de Programação Paralela, serão implementadas modificações nos códigos “mpi_primos.c” (Naive) e “mpi_primosbag.c” (Bag of Tasks), disponíveis em https://github.com/gpsilva2003/MPI. As modificações se tratam de utilizar diferentes formas do MPI_Send e do MPI_Recv para testar os códigos e verificar se há alguma mudança significativa entre eles. Entre os que utilizaremos, estão:

Send’s
  MPI_Send (padrão)
  MPI_Ssend
  MPI_Isend
  MPI_Bsend
  MPI_Rsend

Recv’s
  MPI_Recv (padrão)
  MPI_IRecv

Para o programa Naive, adicionamos toda a parte de Send e Recv e fizemos as alterações necessárias pois esta estava sendo feita com MPI_Reduce e MPI_Gather. Para o programa Bag of tasks apenas modificamos os Send’s e Recv’s com as adições necessárias.

No total, teremos 19 combinações (visto que a combinação Send e Recv do Bag of tasks já havia sido implementada) a fazer e avaliaremos as suas respectivas speedups e eficiências, comparando as execuções sequenciais com as execuções utilizando 4 threads.
