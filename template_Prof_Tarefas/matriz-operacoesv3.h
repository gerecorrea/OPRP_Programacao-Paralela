#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <omp.h>
#include <mpi.h>
#include "matrizv3.h"

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
      mymatriz *mat_a;
      mymatriz *mat_b;
      mymatriz *mat_cPar;
} param_t;

// soma sequencial
mymatriz *msomar (mymatriz *mat_a, mymatriz *mat_b, int tipo);

// multiplicação
void * multiplicarth (void *arg);
mymatriz *mpi_multiplicar (mymatriz *mat_a, mymatriz *mat_b);
mymatriz *mmultiplicar (mymatriz *mat_a, mymatriz *mat_b, int tipo);
mymatriz *omp_mmultiplicar (mymatriz *mat_a, mymatriz *mat_b, int tipo);

// multiplicação em bloco
int multiplicar_submatriz (matriz_bloco_t *mat_suba, matriz_bloco_t *mat_subb, matriz_bloco_t *mat_subc);
int omp_multiplicar_submatriz (matriz_bloco_t *mat_suba, matriz_bloco_t *mat_subb, matriz_bloco_t *mat_subc);
