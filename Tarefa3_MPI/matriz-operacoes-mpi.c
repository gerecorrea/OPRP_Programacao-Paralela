#include "matriz-operacoes-mpi.h"
#include <pthread.h>
#include <mpi.h>
#include <math.h>

mymatriz *multiplica_MPI(mymatriz *mat_a, mymatriz *mat_b, int rank, int size) {
	mymatriz *mat_c = NULL;

  //Com um broadcast envia a quantidade de linhas de A e B para todos os outros processos
  MPI_Bcast(&mat_a->lin, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&mat_b->lin, 1, MPI_INT, 0, MPI_COMM_WORLD);

  mymatriz *matAux_A = (mymatriz*) malloc(sizeof(mymatriz));
  matAux_A->lin = ceil(mat_a->lin/(float)size);
  matAux_A->col = mat_b->lin;
  mmalocar(matAux_A);

  //Utilização de um Scatter para lançar as linhas de A para todos os procesoss
  if (!rank){
      MPI_Scatter(&mat_a->matriz[0][0], matAux_A->lin * matAux_A->col, MPI_INT, &matAux_A->matriz[0][0], matAux_A->lin * matAux_A->col, MPI_INT, 0, MPI_COMM_WORLD);
  }  
  else{
      MPI_Scatter(NULL, matAux_A->lin * matAux_A->col, MPI_INT, &matAux_A->matriz[0][0], matAux_A->lin * matAux_A->col, MPI_INT, 0, MPI_COMM_WORLD);
  }
 
  //Com um broadcast envia a quantidade de colunas de B para todos os outros processos
  MPI_Bcast(&mat_b->col, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  //Realizo a alocação da matriz B
  if(rank)
    mmalocar(mat_b);

  //Agora com um outro broadcast, após realizar a alocação, envio um broadcast da matriz B a todos os outros processos.  
  MPI_Bcast(&mat_b->matriz[0][0], mat_b->lin * mat_b->col, MPI_INT, 0, MPI_COMM_WORLD);
  
  //Com um broadcast realizo o envio da quantidade colunas de A apra todos os outros processos
  MPI_Bcast(&matAux_A->col, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  //Alocação da matriz C com a chamada da multiplicação das matrizes
  mymatriz *matAux_C = mmultiplica_MPI(matAux_A, mat_b);
  
  //Junta-se os resultados obtidos através de um Gather
  if(!rank){
    mat_c = (mymatriz *) malloc (sizeof(mymatriz));
    mat_c->lin = size * matAux_A->lin;
    mat_c->col = mat_b->col;
    mmalocar(mat_c);
    MPI_Gather(&matAux_C->matriz[0][0], matAux_C->lin * matAux_C->col, MPI_INT, &mat_c->matriz[0][0], matAux_C->lin * matAux_C->col, MPI_INT, 0, MPI_COMM_WORLD);
  }else{
    MPI_Gather(&matAux_C->matriz[0][0], matAux_C->lin * matAux_C->col, MPI_INT, NULL, matAux_C->lin * matAux_C->col, MPI_INT, 0, MPI_COMM_WORLD);
  }

  //Retorno a matriz C resultante da operação de multiplicação
  return mat_c;
}

