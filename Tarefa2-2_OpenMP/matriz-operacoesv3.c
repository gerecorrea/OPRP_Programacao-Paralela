	#include "matriz-operacoesv3.h"

//#include <omp.h>

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% MATRIZ %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
mymatriz *msomar (mymatriz *mat_a, mymatriz *mat_b, int tipo) {
	mymatriz *mat_c = NULL;
	/*
	if ((mat_a->lin != mat_b-> lin) || (mat_a->col != mat_b->col)){
		printf ("Erro: Matrizes incompatíveis!\n");
		exit(1);
	}*/

	mat_c = (mymatriz *) malloc (sizeof(mymatriz));
	mat_c->lin = mat_a->lin;
	mat_c->col = mat_a->col;

	if (malocar(mat_c)) {	printf ("ERROR: Out of memory\n"); }

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

	if(mat_a->col != mat_b->lin){
		printf ("Erro: Matrizes incompatíveis!\n");
		exit(1);
	}

	mat_c = (mymatriz *) malloc (sizeof(mymatriz));
	mat_c->lin = mat_a->lin;
	mat_c->col = mat_b->col;

	if (malocar(mat_c))
		printf ("ERROR: Out of memory\n");

	//Excluindo lixo de memória
	for(int i=0; i<mat_c->lin; i++){
		for(int j=0; j<mat_c->col; j++){
			mat_c->matriz[i][j] = 0;
		}
	}

	//ijk, ikj, jik, jki, kij, kji
	//obs: k pode ser igual a coluna da matriz a ou linha da matriz b (mesmo valor)
	if (tipo == 0) { //ijk ok
		for (int i=0; i < mat_c->lin; i++)
		  for (int j=0; j < mat_c->col; j++)
		  	for (int k=0; k < mat_a->col; k++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	} else if (tipo == 1){// ikj
		for (int i=0; i < mat_c->lin; i++)
		  for (int k=0; k < mat_a->col; k++)
		  	for (int j=0; j < mat_c->col; j++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	}else if (tipo == 2){ //jik
		for (int j=0; j < mat_c->col; j++)
		  for (int i=0; i < mat_c->lin; i++)
		  	for (int k=0; k < mat_a->col; k++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	}else if (tipo == 3){ //jki
		for (int j=0; j < mat_c->col; j++)
		  for (int k=0; k < mat_a->col; k++)
		  	for (int i=0; i < mat_c->lin; i++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	}else if (tipo == 4){ //kij
		for (int k=0; k < mat_a->col; k++)
		  for (int i=0; i < mat_c->lin; i++)
		  	for (int j=0; j < mat_c->col; j++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	}else{ //kji
		for (int k=0; k < mat_a->col; k++)
		  for (int j=0; j < mat_c->col; j++)
		  	for (int i=0; i < mat_c->lin; i++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	}
	
	return mat_c;
}


mymatriz *opera_submatrizes(mymatriz *mat_a, mymatriz *mat_b, int divisor, unsigned short tipo){
		//Função criada para trabalhar com as submatrizes de forma a não ser jogado no main		
		
		//QUANTO AO DIVISOR: Qtd de submatrizes (cortes) em relação às originais - EDITE NO MAIN
		//O divisor deve ser menor que a ordem máxima de tamanho das matrizes utilizadas.

		//ORIENTAÇÃO DO CORTE:
		int orientacaoA = 0, orientacaoB = 1; //EDITE AQUI
		//0 para vertical, 1 para horizontal
		
		matriz_bloco_t **submatrizA, **submatrizB, **submatrizC;
		
		//Aqui alocamos inicialmente as duas matrizes A e B para prenchimento a partir das suas originais
		submatrizA=(matriz_bloco_t **)malloc(sizeof(matriz_bloco_t*));
		submatrizB=(matriz_bloco_t **)malloc(sizeof(matriz_bloco_t*));

		//if(divisor > que o possível de cortes mínimos) 
		if(orientacaoA == 0 && (mat_a->col < divisor || mat_b->lin < divisor)){
			printf ("\nQuantidade limite de particionamentos em relação as matrizes de entrada excedido\n");
			exit(1);
		}
		else if(orientacaoA == 1 && (mat_a->lin < divisor || mat_b->col < divisor)){
			printf ("\nQuantidade limite de particionamentos em relação as matrizes de entrada excedido\n");
			exit(1);
		}
		else if(orientacaoA == orientacaoB){
			printf ("As duas matrizes não podem possuir a mesma orientação de corte!\n");
			exit(1);
		}
		else if (mat_a->col != mat_b->lin){
			printf ("Matrizes impossíveis de serem multiplicadas.\n");
			exit(1);
		}
		
		//Em particionar_matriz particionamos em submatriz as matrizes originais A e B, dado a orientação do corte e quantidade de partições
		submatrizA = particionar_matriz(mat_a->matriz, mat_a->lin, mat_a->col, orientacaoA, divisor);
		submatrizB = particionar_matriz(mat_b->matriz, mat_b->lin, mat_b->col, orientacaoB, divisor);

		//Aqui geramos uma submatriz C baseada na propriedade de multiplicação, dado as submatrizes A e B criadas
		submatrizC = constroi_submatrizv2(mat_a->lin, mat_b->col, divisor);
		
		//Aqui realizamos a multiplicação das submatrizes em todas suas partições
		//printf("\n------------------------------\n\nMultiplicação das submatrizes:\n");
		for(int i=0; i<divisor; i++){
			//printf("\n--------------\n\nParticao %d:", i+1);
			//Enviar o tipo (permutação) como parâmetro aqui.
			multiplicar_submatriz(submatrizA[i], submatrizB[i], submatrizC[i], tipo);
		}
		

		mymatriz *mat_c = NULL;
		mat_c = (mymatriz *) malloc (sizeof(mymatriz));
		mat_c->lin = mat_a->lin;
		mat_c->col = mat_b->col;

		if (malocar(mat_c))
			printf ("ERROR: Out of memory\n");

		//Excluindo lixo de memória e setando em zero por conta da soma
		for(int i=0; i<mat_c->lin; i++){
			for(int j=0; j<mat_c->col; j++){
				mat_c->matriz[i][j] = 0;
			}
		}

		//Se o corte na matriz A for vertical, logo efetuamos somente a soma das matrizes geradas
		//Caso seja o corte seja o contrário, efetuamos a multiplicação.
		if(orientacaoA == 0){
			for(int i=0; i<mat_c->lin; i++){
				for(int j=0; j<mat_c->col; j++){
					for(int k=0; k<divisor; k++){
						mat_c->matriz[i][j] += submatrizC[k]->matriz[i][j];
					}
				}
			}
		}
		else{ //orientacaoA == 1;
			for (int i = submatrizC[0]->bloco->lin_inicio;	i <= submatrizC[0]->bloco->lin_fim; i++) {
				for (int j = submatrizC[0]->bloco->col_inicio; j <= submatrizC[0]->bloco->col_fim; j++) {
					for (int k = 0; k <= (submatrizA[0]->bloco->col_fim - submatrizA[0]->bloco->col_inicio); k++) {
						mat_c->matriz[i][j] += submatrizA[0]->matriz[i][(k+submatrizA[0]->bloco->col_inicio)] * 
						submatrizB[0]->matriz[(k+submatrizB[0]->bloco->lin_inicio)][j];
					}
				}
			}
		}

		liberar_submatriz(submatrizA);
		liberar_submatriz(submatrizB);
		liberar_submatriz(submatrizC);
		
		return mat_c;
}

 // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int multiplicar_submatriz (matriz_bloco_t *mat_suba, matriz_bloco_t *mat_subb, matriz_bloco_t *mat_subc, unsigned short tipo) {
 	int col_mat_a, lin_mat_b;
	//printf("\nSubmatriz A:\n");
	//imprimir_submatriz(mat_suba);
	//printf("\nSubmatriz B:\n");
	//imprimir_submatriz(mat_subb);
	col_mat_a = (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio)+1;
	lin_mat_b = (mat_subb->bloco->lin_fim - mat_subb->bloco->lin_inicio)+1;
	assert ( col_mat_a == lin_mat_b);

	//Permutações dos tipos possíveis - tudo ok
	//ijk, ikj, jik, jki, kij, kji
	if(tipo == 0){
		for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
			for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++) {
				for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++) {
					mat_subc->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
				}
			}
		}
	}
	else if(tipo == 1){
		for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
			for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++) {
				for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++) {
					mat_subc->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
				}
			}
		}
	}
	else if(tipo == 2){
		for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++){
			for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
				for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++) {
					mat_subc->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
				}
			}
		}
	}
	//ijk, ikj, jik, jki, kij, kji
	else if(tipo == 3){
		for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++)  {
			for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++){
				for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
					mat_subc->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
				}
			}
		}
	}
	else if(tipo == 4){
		for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++){
			for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
				for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++) {
					mat_subc->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
				}
			}
		}
	}
	else{
		for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++) {
			for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++) {
				for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
					mat_subc->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
				}
			}
		}
	}
 	return 0;
}

//NOVAS FUNÇÕES

void *multiplicarThread(void *arg){
	param_t *p = (param_t*) arg;
	
	//ijk, ikj, jik, jki, kij, kji
	if(p->tipo == 0){
		for(int i = p->lini; i<p->lfim; i++){
			for(int j=0; j<p->M; j++){
				for(int k=0; k<p->L; k++){
					p->mat_c->matriz[i][j] += p->mat_a->matriz[i][k] * p->mat_b->matriz[k][j];
				}
			}
		}
	}
	else if(p->tipo == 1){
		for(int i = p->lini; i<p->lfim; i++){
			for(int k=0; k<p->L; k++){
				for(int j=0; j<p->M; j++){
					p->mat_c->matriz[i][j] += p->mat_a->matriz[i][k] * p->mat_b->matriz[k][j];
				}
			}
		}
	}
	else if(p->tipo == 2){
		for(int j=0; j<p->M; j++){
			for(int i = p->lini; i<p->lfim; i++){
				for(int k=0; k<p->L; k++){
					p->mat_c->matriz[i][j] += p->mat_a->matriz[i][k] * p->mat_b->matriz[k][j];
				}
			}
		}
	}
	else if(p->tipo == 3){
		for(int j=0; j<p->M; j++){
			for(int k=0; k<p->L; k++){
				for(int i = p->lini; i<p->lfim; i++){
					p->mat_c->matriz[i][j] += p->mat_a->matriz[i][k] * p->mat_b->matriz[k][j];
				}
			}
		}
	}
	else if(p->tipo == 4){
		for(int k=0; k<p->L; k++){
			for(int i = p->lini; i<p->lfim; i++){
				for(int j=0; j<p->M; j++){
					p->mat_c->matriz[i][j] += p->mat_a->matriz[i][k] * p->mat_b->matriz[k][j];
				}
			}
		}
	}
	else{
		for(int k=0; k<p->L; k++){
			for(int j=0; j<p->M; j++){
				for(int i = p->lini; i<p->lfim; i++){
					p->mat_c->matriz[i][j] += p->mat_a->matriz[i][k] * p->mat_b->matriz[k][j];
				}
			}
		}
	}
	return 0;
}

void *multiplicarBlocoThread(void *arg){
		param_t *p = (param_t*) arg;
		//ORIENTAÇÃO DO CORTE:
		int orientacaoA = 0, orientacaoB = 1; //EDITE AQUI
		//0 para vertical, 1 para horizontal
		
		matriz_bloco_t **submatrizA, **submatrizB, **submatrizC;
		
		//Aqui alocamos inicialmente as duas matrizes A e B para prenchimento a partir das suas originais
		submatrizA=(matriz_bloco_t **)malloc(sizeof(matriz_bloco_t*));
		submatrizB=(matriz_bloco_t **)malloc(sizeof(matriz_bloco_t*));

		//if(divisor > que o possível de cortes mínimos) 
		if(orientacaoA == 1 && (p->mat_a->col < p->divisor || p->mat_b->lin < p->divisor)){
			printf ("\nQuantidade limite de particionamentos em relação as matrizes de entrada excedido\n");
			exit(1);
		}
		else if(orientacaoA == 0 && (p->mat_a->lin < p->divisor || p->mat_b->col < p->divisor)){
			printf ("\nQuantidade limite de particionamentos em relação as matrizes de entrada excedido\n");
			exit(1);
		}
		else if(orientacaoA == orientacaoB){
			printf ("As duas matrizes não podem possuir a mesma orientação de corte!\n");
			exit(1);
		}
		else if (p->mat_a->col != p->mat_b->lin){
			printf ("Matrizes impossíveis de serem multiplicadas.\n");
			exit(1);
		}
		
		//Em particionar_matriz particionamos em submatriz as matrizes originais A e B, dado a orientação do corte e quantidade de partições
		submatrizA = particionar_matriz(p->mat_a->matriz, p->mat_a->lin, p->mat_a->col, orientacaoA, p->divisor);
		submatrizB = particionar_matriz(p->mat_b->matriz, p->mat_b->lin, p->mat_b->col, orientacaoB, p->divisor);

		//Aqui geramos uma submatriz C baseada na propriedade de multiplicação, dado as submatrizes A e B criadas
		submatrizC = constroi_submatrizv2(p->mat_a->lin, p->mat_b->col, p->divisor);
		
		//Aqui realizamos a multiplicação das submatrizes em todas suas partições
		for(int i=0; i<p->divisor; i++){
			multiplicar_submatriz(submatrizA[i], submatrizB[i], submatrizC[i], p->tipo);
		}
		
		//?
		p->mat_c->lin = p->mat_a->lin;
		p->mat_c->col = p->mat_b->col;

		//Se o corte na matriz A for vertical, logo efetuamos somente a soma das matrizes geradas
		//Caso seja o corte seja o contrário, efetuamos a multiplicação.
		if(orientacaoA == 0){
			for(int i=0; i<p->mat_c->lin; i++){
				for(int j=0; j<p->mat_c->col; j++){
					for(int k=0; k<p->divisor; k++){
						p->mat_c->matriz[i][j] += submatrizC[k]->matriz[i][j];
					}
				}
			}
		}
		else{ //orientacaoA == 1;
			for (int i = submatrizC[0]->bloco->lin_inicio;	i <= submatrizC[0]->bloco->lin_fim; i++) {
				for (int j = submatrizC[0]->bloco->col_inicio; j <= submatrizC[0]->bloco->col_fim; j++) {
					for (int k = 0; k <= (submatrizA[0]->bloco->col_fim - submatrizA[0]->bloco->col_inicio); k++) {
						p->mat_c->matriz[i][j] += submatrizA[0]->matriz[i][(k+submatrizA[0]->bloco->col_inicio)] * 
						submatrizB[0]->matriz[(k+submatrizB[0]->bloco->lin_inicio)][j];
					}
				}
			}
		}

		liberar_submatriz(submatrizA);
		liberar_submatriz(submatrizB);
		liberar_submatriz(submatrizC);
		return 0;
		
}

mymatriz *multiplicar_OMP (mymatriz *mat_a, mymatriz *mat_b, int tipo) {
	mymatriz *mat_c = NULL;

	if(mat_a->col != mat_b->lin){
		printf ("Erro: Matrizes incompatíveis!\n");
		exit(1);
	}

	mat_c = (mymatriz *) malloc (sizeof(mymatriz));
	mat_c->lin = mat_a->lin;
	mat_c->col = mat_b->col;

	if (malocar(mat_c))
		printf ("ERROR: Out of memory\n");

	//Excluindo lixo de memória
	for(int i=0; i<mat_c->lin; i++){
		for(int j=0; j<mat_c->col; j++){
			mat_c->matriz[i][j] = 0;
		}
	}

	//ijk, ikj, jik, jki, kij, kji
	//obs: k pode ser igual a coluna da matriz a ou linha da matriz b (mesmo valor)
	if (tipo == 0) { //ijk ok
		for (int i=0; i < mat_c->lin; i++)
		  for (int j=0; j < mat_c->col; j++)
		  	for (int k=0; k < mat_a->col; k++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	} else if (tipo == 1){// ikj
		for (int i=0; i < mat_c->lin; i++)
		  for (int k=0; k < mat_a->col; k++)
		  	for (int j=0; j < mat_c->col; j++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	}else if (tipo == 2){ //jik
		for (int j=0; j < mat_c->col; j++)
		  for (int i=0; i < mat_c->lin; i++)
		  	for (int k=0; k < mat_a->col; k++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	}else if (tipo == 3){ //jki
		for (int j=0; j < mat_c->col; j++)
		  for (int k=0; k < mat_a->col; k++)
		  	for (int i=0; i < mat_c->lin; i++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	}else if (tipo == 4){ //kij
		for (int k=0; k < mat_a->col; k++)
		  for (int i=0; i < mat_c->lin; i++)
		  	for (int j=0; j < mat_c->col; j++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	}else{ //kji
		for (int k=0; k < mat_a->col; k++)
		  for (int j=0; j < mat_c->col; j++)
		  	for (int i=0; i < mat_c->lin; i++)
				mat_c->matriz[i][j] += mat_a->matriz[i][k] * mat_b->matriz[k][j];	
	}
	
	return mat_c;
}

mymatriz *multiplicaPorBlocos_OMP(mymatriz *mat_a, mymatriz *mat_b, int divisor, unsigned short tipo){
		//Função criada para trabalhar com as submatrizes de forma a não ser jogado no main		
		
		//QUANTO AO DIVISOR: Qtd de submatrizes (cortes) em relação às originais - EDITE NO MAIN
		//O divisor deve ser menor que a ordem máxima de tamanho das matrizes utilizadas.

		//ORIENTAÇÃO DO CORTE:
		int orientacaoA = 0, orientacaoB = 1; //EDITE AQUI
		//0 para vertical, 1 para horizontal
		
		matriz_bloco_t **submatrizA, **submatrizB, **submatrizC;
		
		//Aqui alocamos inicialmente as duas matrizes A e B para prenchimento a partir das suas originais
		submatrizA=(matriz_bloco_t **)malloc(sizeof(matriz_bloco_t*));
		submatrizB=(matriz_bloco_t **)malloc(sizeof(matriz_bloco_t*));

		//if(divisor > que o possível de cortes mínimos) 
		if(orientacaoA == 0 && (mat_a->col < divisor || mat_b->lin < divisor)){
			printf ("\nQuantidade limite de particionamentos em relação as matrizes de entrada excedido\n");
			exit(1);
		}
		else if(orientacaoA == 1 && (mat_a->lin < divisor || mat_b->col < divisor)){
			printf ("\nQuantidade limite de particionamentos em relação as matrizes de entrada excedido\n");
			exit(1);
		}
		else if(orientacaoA == orientacaoB){
			printf ("As duas matrizes não podem possuir a mesma orientação de corte!\n");
			exit(1);
		}
		else if (mat_a->col != mat_b->lin){
			printf ("Matrizes impossíveis de serem multiplicadas.\n");
			exit(1);
		}
		
		//Em particionar_matriz particionamos em submatriz as matrizes originais A e B, dado a orientação do corte e quantidade de partições
		submatrizA = particionar_matriz(mat_a->matriz, mat_a->lin, mat_a->col, orientacaoA, divisor);
		submatrizB = particionar_matriz(mat_b->matriz, mat_b->lin, mat_b->col, orientacaoB, divisor);

		//Aqui geramos uma submatriz C baseada na propriedade de multiplicação, dado as submatrizes A e B criadas
		submatrizC = constroi_submatrizv2(mat_a->lin, mat_b->col, divisor);
		
		//Aqui realizamos a multiplicação das submatrizes em todas suas partições
		//printf("\n------------------------------\n\nMultiplicação das submatrizes:\n");
		for(int i=0; i<divisor; i++){
			//printf("\n--------------\n\nParticao %d:", i+1);
			//Enviar o tipo (permutação) como parâmetro aqui.
			multiplicar_submatriz(submatrizA[i], submatrizB[i], submatrizC[i], tipo);
		}
		

		mymatriz *mat_c = NULL;
		mat_c = (mymatriz *) malloc (sizeof(mymatriz));
		mat_c->lin = mat_a->lin;
		mat_c->col = mat_b->col;

		if (malocar(mat_c))
			printf ("ERROR: Out of memory\n");

		//Excluindo lixo de memória e setando em zero por conta da soma
		for(int i=0; i<mat_c->lin; i++){
			for(int j=0; j<mat_c->col; j++){
				mat_c->matriz[i][j] = 0;
			}
		}

		//Se o corte na matriz A for vertical, logo efetuamos somente a soma das matrizes geradas
		//Caso seja o corte seja o contrário, efetuamos a multiplicação.
		if(orientacaoA == 0){
			for(int i=0; i<mat_c->lin; i++){
				for(int j=0; j<mat_c->col; j++){
					for(int k=0; k<divisor; k++){
						mat_c->matriz[i][j] += submatrizC[k]->matriz[i][j];
					}
				}
			}
		}
		else{ //orientacaoA == 1;
			for (int i = submatrizC[0]->bloco->lin_inicio;	i <= submatrizC[0]->bloco->lin_fim; i++) {
				for (int j = submatrizC[0]->bloco->col_inicio; j <= submatrizC[0]->bloco->col_fim; j++) {
					for (int k = 0; k <= (submatrizA[0]->bloco->col_fim - submatrizA[0]->bloco->col_inicio); k++) {
						mat_c->matriz[i][j] += submatrizA[0]->matriz[i][(k+submatrizA[0]->bloco->col_inicio)] * 
						submatrizB[0]->matriz[(k+submatrizB[0]->bloco->lin_inicio)][j];
					}
				}
			}
		}

		liberar_submatriz(submatrizA);
		liberar_submatriz(submatrizB);
		liberar_submatriz(submatrizC);
		
		return mat_c;
}

