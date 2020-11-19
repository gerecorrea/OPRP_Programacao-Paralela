#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <omp.h>
#include "matrizv3.h"
#include "matriz-operacoesv3.h"

#ifndef SOME_HEADER_FILE_H
#define SOME_HEADER_FILE_H
typedef struct {
  int lin_inicio;
  int lin_fim;
  int col_inicio;
  int col_fim;
} bloco_t;

typedef struct {
  int **matriz;
  bloco_t *bloco;
} matriz_bloco_t;

typedef struct {
  int **matriz;
  int lin;
  int col;
} mymatriz;

#endif

typedef struct {
      int tid;
      int ntasks;
      int lfim;
      int lini;
      int N;
      int L;
      int M;
      int particao;
      int divisor;
      mymatriz *mat_a;
      mymatriz *mat_b;
      mymatriz *mat_cPar;
      matriz_bloco_t *matA;
      matriz_bloco_t *matB;
      matriz_bloco_t *matC;
} param_t;

void *mult_thread(void *arg);
void *mult_bloco_th(void *arg);
void *multiplicarBlocoThread(void *arg);
mymatriz *multiplicarMultitreads(mymatriz *mat_a, mymatriz *mat_b, int tipo);
mymatriz *multiplicarBlocosMultithreads(matriz_bloco_t *mat_suba, matriz_bloco_t *mat_subb, matriz_bloco_t *mat_subc, int tipo);
mymatriz *multiplica_MPI(mymatriz *mat_a, mymatriz *mat_b, int rank, int size);

