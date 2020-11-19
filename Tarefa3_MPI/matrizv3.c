#include "matrizv3.h"
#include <time.h>

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int malocar (mymatriz *matriz) {
	int **newMatriz = NULL;

    newMatriz = (int **) malloc(matriz->lin*sizeof(int *));
	if (!newMatriz) {
		printf("ERROR: Out of memory\n");
		return 1;
	}
  	for (int i =0; i < matriz->lin; i++) {
		newMatriz[i] = (int *) malloc(sizeof(int)*matriz->col);
		if (!newMatriz) {
			printf("ERROR: Out of memory\n");
			return 1;
		}
	}

	matriz->matriz = newMatriz;
	return 0;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int mmalocar (mymatriz *matriz) {
	int **newMatriz = NULL;

	newMatriz = (int **) malloc(matriz->lin*sizeof(int *));

	if (!newMatriz) {
		printf("ERROR: Out of memory\n");
		return 1;
	}

	newMatriz[0] = (int *) malloc(matriz->lin * matriz->col * sizeof(int));
  	for (int i=1; i < matriz->lin; i++) {
		  	newMatriz[i] = newMatriz[0] + i * matriz->col;
			if (!newMatriz) {
				printf("ERROR: Out of memory\n");
				return 1;
			}
	}

	matriz->matriz = newMatriz;
	return 0;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MATRIZ %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int mgerar(mymatriz *matriz, int valor){
	srand( (unsigned)time(NULL) );

	for (int i=0; i < matriz->lin; i++)
	  for (int j=0; j < matriz->col; j++)
			if (valor == -9999)
				matriz->matriz[i][j] = rand() % 10;
			else
				matriz->matriz[i][j] = valor;

	return 0;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int mimprimir (mymatriz *matriz){
	int linha, coluna;
	linha = matriz->lin;
	coluna = matriz->col;

	if (linha > 15) {
		linha = 15;
	}

	if (coluna > 15) {
		coluna = 15;
	}

	for (int j =0; j < coluna; j++)
		printf("\t(%d)", j);
	printf("\n");
	for (int i=0; i < linha; i++) {
		printf("(%d)", i);
	  for (int j=0; j < coluna; j++){
			printf("\t%d", matriz->matriz[i][j]);
		}
		printf("\n");
	}

	printf("\n \
%%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%%\n \
	WARNING: Impressão truncada em 15x15! \n \
	WARNING: Último elemento matriz[%d][%d] = %d \n \
%%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%% %%%%%%%%%%%%\n", \
matriz->lin-1, matriz->col-1, matriz->matriz[matriz->lin-1][matriz->col-1]);
	return 0;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int mliberar (mymatriz *matriz) {
	for (int i =0; i < matriz->lin; i++) {
		//printf("%p\n", matriz->matriz[i]);
		free(matriz->matriz[i]);
	}
	free(matriz->matriz);
	return 0;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int mmliberar (mymatriz *matriz) {
	free(matriz->matriz[0]);
	free(matriz->matriz);
	return 0;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int mzerar (mymatriz *matriz){
	return mgerar(matriz,0);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int mcomparar (mymatriz *mat_a, mymatriz *mat_b){
	for (int j =0; j < mat_a->col; j++)
	for (int i=0; i < mat_a->lin; i++) {
		for (int j=0; j < mat_a->col; j++){
			if (mat_a->matriz[i][j] != mat_b->matriz[i][j]) {
				printf("O elemento [%d,%d] é diferente nas matrizes analisadas!", i,j);
				return 1;
			}
		}
	}
	printf("\tVERIFICADO: Matrizes identicas\n");
	return 0;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Recebe uma matriz de inteiros e a particiona em n (parâmetro divisor) pedaço de acordo com a orientação (horizontal ou vertical)
matriz_bloco_t **particionar_matriz (int **matriz, int mat_lin, int mat_col, int orientacao, int divisor){
	matriz_bloco_t **vsubmatriz = NULL;
 	int nro_submatrizes, base, corte;
 	bloco_t *bloco_tmp;
	mymatriz *mat = NULL;
	int **newMatriz = NULL;

	//Alocando mymatriz
	mat = (mymatriz *) malloc (sizeof(mymatriz));
	mat->lin = mat_lin;
	mat->col = mat_col;
	
    newMatriz = (int **) malloc(mat->lin*sizeof(int *));
	if (!newMatriz) {
		printf("ERROR: Out of memory\n");	
	}

  	for (int i =0; i < mat->lin; i++) {
		newMatriz[i] = (int *) malloc(sizeof(int)*mat->col);
		if (!newMatriz) {
			printf("ERROR: Out of memory\n");
		}
	}
	mat->matriz = newMatriz;
	//Fim alocação mymatriz

	nro_submatrizes = divisor;
   	vsubmatriz = (matriz_bloco_t **) malloc (nro_submatrizes*sizeof (matriz_bloco_t *));
	
 	if (!vsubmatriz) {
 		printf("ERROR: Out of memory\n");
 		return NULL;
 	}

 	base = 0;
	
	//Laço para particionar a matriz
 	for (int i = 0; i < nro_submatrizes; i++) {
 		vsubmatriz[i] = (matriz_bloco_t *) malloc (sizeof(matriz_bloco_t));
 		bloco_tmp = (bloco_t *) malloc (sizeof(bloco_t));

 		if (!bloco_tmp) {
 			printf("ERROR: Out of memory\n");
 			return NULL;
 		}

 		if (orientacao == 0) { //orientacao vertical
			corte = (mat_lin) / nro_submatrizes;
			bloco_tmp->lin_inicio = 0;
 			bloco_tmp->lin_fim = (mat_lin-1);
 			bloco_tmp->col_inicio = base;

			//Verificando se está na última divisão para pegar o restante dos valores
			if(i == nro_submatrizes - 1){
				bloco_tmp->col_fim = (mat_col-1);
			}else{
				bloco_tmp->col_fim = base + corte - 1;
			}
 		
 			base += corte;
 	
 		} else { //orientacao horizontal
			corte = mat_col/nro_submatrizes;
 			bloco_tmp->lin_inicio = base;
 			bloco_tmp->col_inicio = 0;
 			bloco_tmp->col_fim = (mat_col-1);

			//Verificando se está na última divisão para pegar o restante dos valores
			if(i == nro_submatrizes -1){
				bloco_tmp->lin_fim = (mat_lin-1);
			}else{
				bloco_tmp->lin_fim = base + corte - 1;
			}
 			
 			base += corte;
 		}

		for(int a=0; a< mat_lin; a++){
			for (int b = 0; b < mat_col; b++){
				mat->matriz[a][b] = matriz[a][b];
			}
		}
		vsubmatriz[i]->matriz = matriz;
		vsubmatriz[i]->bloco = bloco_tmp;
 		vsubmatriz[i]->mat_col = mat_col ;
 		vsubmatriz[i]->mat_lin = mat_lin;
 		vsubmatriz[i]->divisor = nro_submatrizes;

 	}
 	return vsubmatriz; 
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Aloca um vetor de matrizes blocos com o número de matrizes igual ao divisor.
matriz_bloco_t **constroi_submatrizv2 (int mat_lin, int mat_col, int divisor){
	matriz_bloco_t **vsubmatriz = NULL;
 	int **matRet = NULL;
 	bloco_t *bloco_tmp;
	mymatriz **mat = NULL;
	mat = (mymatriz **) malloc(divisor* sizeof(mymatriz *));
		
 	vsubmatriz = (matriz_bloco_t **) malloc (divisor*sizeof (matriz_bloco_t *));
	
 	if (!vsubmatriz) {
 		printf("ERROR: Out of memory\n");
 		return NULL;
 	}

    for (int i=0; i < divisor; i++) {

		//Alocando matRet
	    mat[i] = (mymatriz *) malloc (sizeof(mymatriz));
	    mat[i]->lin = mat_lin;
		mat[i]->col = mat_col;

	    matRet = (int **) malloc(mat_lin*sizeof(int *));
		if (!matRet) {
			printf("ERROR: Out of memory\n");
			
		}
  		for (int j =0; j < mat_lin; j++) {
			matRet[j] = (int *) malloc(sizeof(int)*mat_col);
			if (!matRet) {
				printf("ERROR: Out of memory\n");
			
			}
		}
		
		// Fim alocação matRet

 		//Zerando matriz
		for(int k = 0; k< mat_lin;k++){
			for(int j = 0; j< mat_col; j++){
				matRet[k][j] = 0;
			}
		}
		//Fim zerar matriz

		mat[i]->matriz = matRet;
 		vsubmatriz[i] = (matriz_bloco_t *) malloc (sizeof(matriz_bloco_t));
 		bloco_tmp = (bloco_t *) malloc (sizeof(bloco_t));
		
 		if (!bloco_tmp) {
 			printf("ERROR: Out of memory\n");
 			return NULL;
 		}

 		bloco_tmp->lin_inicio = 0;
 		bloco_tmp->lin_fim = (mat_lin-1);
 		bloco_tmp->col_inicio = 0;
 		bloco_tmp->col_fim = (mat_col-1);
		
		vsubmatriz[i]->matriz = matRet;
 		vsubmatriz[i]->bloco = bloco_tmp;
 		vsubmatriz[i]->mat_col = mat_col;
 		vsubmatriz[i]->mat_lin = mat_lin;
 		vsubmatriz[i]->divisor = divisor;
		
 	}
 	return vsubmatriz;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 int imprimir_submatriz (matriz_bloco_t *submatriz){
 	for (int j = submatriz->bloco->col_inicio; j <= submatriz->bloco->col_fim; j++)
	printf("\t(%d)", j);
	printf("\n");
	for (int i= submatriz->bloco->lin_inicio; i <= submatriz->bloco->lin_fim; i++) {
		printf("(%d)", i);
		for (int j= submatriz->bloco->col_inicio; j <= submatriz->bloco->col_fim; j++){
			printf("\t%d", submatriz->matriz[i][j]);
		}
		printf("\n");
	}
	return 0;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int imprimir_submatrizC (matriz_bloco_t *submatrizA, matriz_bloco_t *submatrizB, matriz_bloco_t *submatrizC){
 	for (int j = submatrizB->bloco->col_inicio; j <= submatrizB->bloco->col_fim; j++)
	printf("\t(%d)", j);
	printf("\n");
	for (int i= submatrizA->bloco->lin_inicio; i <= submatrizA->bloco->lin_fim; i++) {
		printf("(%d)", i);
		for (int j= submatrizB->bloco->col_inicio; j <= submatrizB->bloco->col_fim; j++){
			printf("\t%d", submatrizC->matriz[i][j]);
		}
		printf("\n");
	}
	return 0;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
matriz_bloco_t **liberar_submatriz (matriz_bloco_t **submatriz) {
	int nro_submatrizes;
	nro_submatrizes = submatriz[0]->divisor;
	for (int i = 0; i < nro_submatrizes; i++) {
		free (submatriz[i]->bloco);
		free (submatriz[i]);
	}
  	free (submatriz);
  	return NULL;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 int gerar_submatriz (int **mat_origem, matriz_bloco_t *submatriz, bloco_t *bloco) {
   	bloco_t *newblock;

 	submatriz->matriz = mat_origem;

 	newblock = (bloco_t *) malloc (sizeof(bloco_t));
   	newblock->lin_inicio = bloco->lin_inicio;
   	newblock->lin_fim = bloco->lin_fim;
   	newblock->col_inicio = bloco->col_inicio;
   	newblock->col_fim = bloco->col_fim;
   	submatriz->bloco = newblock;
   	return 0;
 }

 // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 int imprimir_bloco (matriz_bloco_t *submatriz) {

   printf("## Bloco (BEGIN) ##\n");
 	printf("Submatriz[lin_ini,col_ini] = %d\n", submatriz->matriz[submatriz->bloco->lin_inicio][submatriz->bloco->col_inicio]);
 	printf("Submatriz[lin_fim,col_fim] = %d\n", submatriz->matriz[submatriz->bloco->lin_fim][submatriz->bloco->col_fim]);
 	printf("Matriz (%p) (l_i,c_i) = (%d,%d) (l_f,c_f) = (%d,%d) \n", submatriz->matriz, submatriz->bloco->lin_inicio, submatriz->bloco->col_inicio, submatriz->bloco->lin_fim, submatriz->bloco->col_fim);
 	printf("mat_lin/mat_col/divisor = (%d,%d,%d) \n", submatriz->mat_lin, submatriz->mat_col, submatriz->divisor);
 	printf("## Bloco (END) ##\n");
 	return 0;
 }

 // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 matriz_bloco_t **constroi_submatriz (int **matriz, int mat_lin, int mat_col, int divisor) {
 	matriz_bloco_t **vsubmatriz = NULL;
 	bloco_t *bloco_tmp;

 	vsubmatriz = (matriz_bloco_t **) malloc (sizeof (matriz_bloco_t *));

 	if (!vsubmatriz) {
 		printf("ERROR: Out of memory\n");
 		return NULL;
 	}

 	vsubmatriz[0] = (matriz_bloco_t *) malloc (sizeof(matriz_bloco_t));
 	bloco_tmp = (bloco_t *) malloc (sizeof(bloco_t));

 	if (!bloco_tmp) {
 		printf("ERROR: Out of memory\n");
 		return NULL;
 	}
 	bloco_tmp->lin_inicio = 0;
 	bloco_tmp->lin_fim = (mat_lin-1);
 	bloco_tmp->col_inicio = 0;
 	bloco_tmp->col_fim = (mat_col-1);

 	vsubmatriz[0]->matriz = matriz;
 	vsubmatriz[0]->bloco = bloco_tmp;
 	vsubmatriz[0]->mat_col = mat_col;
 	vsubmatriz[0]->mat_lin = mat_lin;
 	vsubmatriz[0]->divisor = divisor;

	return vsubmatriz;
 }
