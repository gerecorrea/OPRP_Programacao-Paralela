#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

typedef struct{ //struct para threads
    int tid; //id da thread
    int ntasks; //qtd de threads o usuário passou
    int lini; //linha de início de execução da thread
    int lfim; //linha de fim de execução da thread
    int tipo; //tipo de permutação
    int divisor; //qtd de partições para acesso pela thread
    int N;
    int L; 
    int M; 
    mymatriz *mat_a; //endereço para matriz a
    mymatriz *mat_b; //...
    mymatriz *mat_c; //...
} param_t;

mymatriz *msomar (mymatriz *mat_a, mymatriz *mat_b, int tipo);
mymatriz *mmultiplicar (mymatriz *mat_a, mymatriz *mat_b, int tipo);
mymatriz *opera_submatrizes(mymatriz *mat_a, mymatriz *mat_b, int divisor, unsigned short tipo);

int multiplicar_submatriz (matriz_bloco_t *mat_suba, matriz_bloco_t *mat_subb, matriz_bloco_t *mat_subc, unsigned short tipo);

void *multiplicarThread (void *arg); //para multiplicação por threads
void *multiplicarBlocoThread (void *arg);

mymatriz *multiplicar_OMP (mymatriz *mat_a, mymatriz *mat_b, int tipo);
mymatriz *multiplicaPorBlocos_OMP(mymatriz *mat_a, mymatriz *mat_b, int divisor, unsigned short tipo);