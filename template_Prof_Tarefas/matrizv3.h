#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  int mat_col;
  int mat_lin;
  int divisor;
} matriz_bloco_t;

typedef struct {
  int **matriz;
  int lin;
  int col;
} mymatriz;
#endif

int mzerar (mymatriz *matriz);
int malocar (mymatriz *matriz);
int mliberar (mymatriz *matriz);
int mimprimir (mymatriz *matriz);
int mgerar(mymatriz *matriz, int valor);
int mcomparar (mymatriz *mat_a, mymatriz *mat_b);

int imprimir_bloco (matriz_bloco_t *submatriz);
int imprimir_submatriz (matriz_bloco_t *submatriz);
int gerar_submatriz (int **mat_origem, matriz_bloco_t *submatriz, bloco_t *bloco);

matriz_bloco_t **liberar_submatriz (matriz_bloco_t **submatriz);
matriz_bloco_t **constroi_submatrizv3 (mymatriz *matriz, int divisor);
matriz_bloco_t **particionar_matrizv3 (mymatriz *matriz, int orientacao, int divisor);
