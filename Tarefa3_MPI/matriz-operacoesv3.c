#include "matriz-operacoesv3.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MATRIZ %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
mymatriz *msomar (mymatriz *mat_a, mymatriz *mat_b, int tipo) {
	mymatriz *mat_c = NULL;

	if ((mat_a->lin != mat_b-> lin) || (mat_a->col != mat_b->col)){
		printf ("Erro: Matrizes incompatíveis para soma!\n");
		exit(1);
	}

	mat_c = (mymatriz *) malloc (sizeof(mymatriz));
	mat_c->lin = mat_a->lin;
	mat_c->col = mat_a->col;

	if (mmalocar(mat_c)) {	printf ("ERROR: Out of memory\n"); }

	if (tipo == 1) {
		for (int i=0; i < mat_c->lin; i++)
		  for (int j=0; j < mat_c->col; j++)
					mat_c->matriz[i][j] = mat_a->matriz[i][j]+mat_b->matriz[i][j];
	} else {
		for (int j=0; j < mat_c->col; j++)
			for (int i=0; i < mat_c->lin; i++)
					mat_c->matriz[i][j] = mat_a->matriz[i][j]+mat_b->matriz[i][j];
	}
  return mat_c;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
mymatriz *mmultiplicar (mymatriz *mat_a, mymatriz *mat_b, int tipo) {
	mymatriz *mat_c = NULL;

	if (mat_a->col == mat_b->lin){
		mat_c = (mymatriz *) malloc (sizeof(mymatriz));

		mat_c->lin = mat_a->lin;
		mat_c->col = mat_b->col;

		if (mmalocar(mat_c)) {	printf ("ERROR: Out of memory\n"); }

		for(int i = 0; i< mat_c->lin;i++){
			for(int j = 0; j< mat_c->col; j++){
				mat_c->matriz[i][j] = 0;
			}
		}

		if (tipo == 0){
			for(int i = 0; i < mat_a->lin; i++)
				for(int j = 0; j < mat_a->lin; j++)
					for(int k = 0; k < mat_a->col; k++)
						mat_c->matriz[i][j] = mat_c->matriz[i][j] + (mat_a->matriz[i][k] * mat_b->matriz[k][j]);
			//return mat_c;
		}
		if (tipo == 1){
			for(int i = 0; i < mat_a->lin; i++)
				for(int k = 0; k < mat_a->col; k++)
					for(int j = 0; j < mat_a->lin; j++)
						mat_c->matriz[i][j] = mat_c->matriz[i][j] + (mat_a->matriz[i][k] * mat_b->matriz[k][j]);
			//return mat_c;
		}
		if (tipo == 2){
			for(int j = 0; j < mat_a->lin; j++)
				for(int i = 0; i < mat_a->lin; i++)
					for(int k = 0; k < mat_a->col; k++)
						mat_c->matriz[i][j] = mat_c->matriz[i][j] + (mat_a->matriz[i][k] * mat_b->matriz[k][j]);
			//return mat_c;
		}
		if (tipo == 3){
			for(int j = 0; j < mat_a->lin; j++)
				for(int k = 0; k < mat_a->col; k++)
					for(int i = 0; i < mat_a->lin; i++)
						mat_c->matriz[i][j] = mat_c->matriz[i][j] + (mat_a->matriz[i][k] * mat_b->matriz[k][j]);
			//return mat_c;
		}
		if (tipo == 4){
			for(int k = 0; k < mat_a->col; k++)
				for(int i = 0; i < mat_a->lin; i++)
					for(int j = 0; j < mat_a->lin; j++)
						mat_c->matriz[i][j] = mat_c->matriz[i][j] + (mat_a->matriz[i][k] * mat_b->matriz[k][j]);
			//return mat_c;
		}
		if (tipo == 5){
			for(int k = 0; k < mat_a->col; k++)
				for(int j = 0; j < mat_a->lin; j++)
					for(int i = 0; i < mat_a->lin; i++)
						mat_c->matriz[i][j] = mat_c->matriz[i][j] + (mat_a->matriz[i][k] * mat_b->matriz[k][j]);
			//return mat_c;
		}
		printf("mat c\n\n");
		mimprimir(mat_c);

	}else{
		printf ("Erro: Matrizes incompatíveis para multiplicação!\n");
		exit(1);
	}

  	return mat_c;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% SUBMATRIZ %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

mymatriz *multiplicar_submatriz (matriz_bloco_t *mat_suba, matriz_bloco_t *mat_subb, matriz_bloco_t *mat_subc, int tipo){
	int col_mat_a, lin_mat_b;
	mymatriz *mat_c = NULL;
/*	printf("\n##### Submatriz A #####\n");
	imprimir_submatriz(mat_suba);
	printf("\n##### Submatriz B #####\n");
	imprimir_submatriz(mat_subb);
*/

	mat_c = (mymatriz *) malloc (sizeof(mymatriz));

	col_mat_a = (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio)+1;
	lin_mat_b = (mat_subb->bloco->lin_fim - mat_subb->bloco->lin_inicio)+1;

	//Verificando se as matrizes podem ser multiplicadas
	if (col_mat_a == lin_mat_b){

		mat_c->lin = mat_subc->mat_lin;
		mat_c->col = mat_subc->mat_col;

		if (mmalocar(mat_c)) {	printf ("ERROR: Out of memory\n"); }

		mzerar(mat_c);

		//Permutação dos laços para multiplicação de matrizes
		if(tipo == 0){
			for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
				for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++) {
					for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++) {
						mat_c->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
						mat_subc->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
					}
				}
			}
		}

		if(tipo == 1){
			for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
				for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++) {
					for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++) {
						mat_c->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
					}
				}
			}
		}

		if(tipo == 2){
			for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++) {
				for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
					for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++) {
						mat_c->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
					}
				}
			}
		}

		if(tipo == 3){
			for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++) {
				for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++) {
					for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
						mat_c->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
					}
				}
			}
		}

		if(tipo == 4){
			for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++) {
				for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
					for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++) {
						mat_c->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
					}
				}
			}
		}

		if(tipo == 5){
			for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++) {
				for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++) {
					for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
						mat_c->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
					}
				}
			}
		}

		/*printf("\n##### Resultado = Submatriz C #####\n");
		imprimir_submatrizC(mat_suba, mat_subb, mat_subc);*/
		

	}else{
		printf ("Erro: Submatrizes incompatíveis para multiplicação!\n");
		exit(1);
	}

	return mat_c;
}

mymatriz *mmultiplica_MPI(mymatriz *mat_a, mymatriz *mat_b) {
	mymatriz *mat_c = NULL;

  	if (mat_a->col != mat_b->lin){
		printf ("Erro: Matrizes incompatíveis!\n");
		exit(1);
	}

	mat_c = (mymatriz *) malloc (sizeof(mymatriz));
	mat_c->lin = mat_a->lin;
	mat_c->col = mat_b->col;

  	if (mmalocar(mat_c)) printf ("ERROR: Out of memory\n");

  	mzerar(mat_c);

  	for (int j=0; j<mat_c->col; j++)
      	for (int k=0; k<mat_a->col; k++)
        	for (int i=0; i<mat_c->lin; i++)
          		mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];
  
  	return mat_c;
}