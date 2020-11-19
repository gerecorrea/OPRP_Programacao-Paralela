	#include "matriz-operacoesv3.h"

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
	//Daqui pra baixo nem toquei (nem excluí)
	
	//return msomar(mat_a, mat_b, tipo);

	//printf("c(%d,%d) = a(%d,%d)*b(%d,%d)\n", N, M, N, L, L, M);
	//imprimir_matriz(mat_a, M, N);
	//imprimir_matriz(mat_b, L, M);
	//imprimir_matriz(mat_c, N, N);
	// for (int i=0; i < N; i++)
	// for (int j=0; j < M; j++) {
	// 	//printf("c(%d,%d) = ", i, j);
	// 		for (int k=0; k < L; k++) {
	// 			//printf("%d", k);
	// 			//printf("%d * %d +", mat_a[i][k],mat_b[k][j]);
	// 			mat_c[i][j] += mat_a[i][k]*mat_b[k][j];
	// 		}
	// 		//printf(" = %d\n", mat_c[i][j]);
	// 	}
	//return mat_c;
}


mymatriz *opera_submatrizes(mymatriz *mat_a, mymatriz *mat_b, int divisor){
		//Função criada para trabalhar com as submatrizes de forma a não ser jogado no main		

		//Está funcionando perfeito para matrizes quadradas, mas para não quadradas tá gerado problema, lidar
		
		//QUANTO AO DIVISOR: Qtd de submatrizes (cortes) em relação às originais - EDITE NO MAIN
		//O divisor deve ser menor que a ordem máxima de tamanho das matrizes utilizadas.

		//ORIENTAÇÃO DO CORTE:
		int orientacaoA = 1, orientacaoB = 0; //EDITE AQUI
		//0 para vertical, 1 para horizontal
		
		matriz_bloco_t **submatrizA, **submatrizB, **submatrizC;
		
		//Aqui alocamos inicialmente as duas matrizes A e B para prenchimento a partir das suas originais
		submatrizA=(matriz_bloco_t **)malloc(sizeof(matriz_bloco_t*));
		submatrizB=(matriz_bloco_t **)malloc(sizeof(matriz_bloco_t*));

		//if(divisor > que o possível de cortes mínimos) 
		if(orientacaoA == 1 && (mat_a->col < divisor || mat_b->lin < divisor)){
			printf ("\nQuantidade limite de particionamentos em relação as matrizes de entrada excedido\n");
			exit(1);
		}
		else if(orientacaoA == 0 && (mat_a->lin < divisor || mat_b->col < divisor)){
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
		//1 é corte horizontal, 0 é vertical
		//divisor é a qtd de cortes da matriz
		//submatrizA = particionar_matriz(matriz, qtd linhas matriz, qtd colunas matriz, orientacao do corte, divisor);
		submatrizA = particionar_matriz(mat_a->matriz, mat_a->lin, mat_a->col, orientacaoA, divisor);
		submatrizB = particionar_matriz(mat_b->matriz, mat_b->lin, mat_b->col, orientacaoB, divisor);

		//Aqui geramos uma submatriz C baseada na propriedade de multiplicação, dado as submatrizes A e B criadas
		//submatrizC = constroi_submatrizv2(linhas A, colunas B, divisor);
		submatrizC = constroi_submatrizv2(mat_a->lin, mat_b->col, divisor);
		
		//Aqui realizamos a multiplicação das submatrizes em todas suas partições
		
		printf("\n------------------------------\n\nMultiplicação das submatrizes:\n");
		for(int i=0; i<divisor; i++){
			printf("\n--------------\n\nParticao %d:", i+1);
			multiplicar_submatriz(submatrizA[i], submatrizB[i], submatrizC[i]);
		}
		

		mymatriz *mat_c = NULL;
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

		//PODEMOS GENERALIZAR QUE SE O CORTE FOR VERTICAL (0) NA MATRIZ A, ENTÃO APENAS SOMAMOS AS submatrizC[i]
		//SE FOR CORTE HORIZONTAL (1) NA MATRIZ A, ENTÃO USAR A MULTIPLICAR_SUBMATRIZ (OU OS FOR DELA) QUE TERÁ A MATRIZ RESULTANTE
			//Fazer ela retornar a matriz resultante? Melhor, acho.
		if(orientacaoA == 0){ //se orientacao do corte de A for vertical
			for(int i=0; i<mat_c->lin; i++){
				for(int j=0; j<mat_c->col; j++){
					for(int k=0; k<divisor; k++){
						mat_c->matriz[i][j] += submatrizC[k]->matriz[i][j];
					}
				}
			}
		}
		//submatrizA[i], submatrizB[i], submatrizC[i]
		else{ //orientacaoA == 1, ou seja, for horizontal;
			for (int i = submatrizC[0]->bloco->lin_inicio;	i <= submatrizC[0]->bloco->lin_fim; i++) {
				for (int j = submatrizC[0]->bloco->col_inicio; j <= submatrizC[0]->bloco->col_fim; j++) {
					for (int k = 0; k <= (submatrizA[0]->bloco->col_fim - submatrizA[0]->bloco->col_inicio); k++) {
						mat_c->matriz[i][j] += submatrizA[0]->matriz[i][(k+submatrizA[0]->bloco->col_inicio)] * 
						submatrizB[0]->matriz[(k+submatrizB[0]->bloco->lin_inicio)][j];
					}
				}
			}
		}

		/*
		printf ("\n\n----------------\nMatriz resultante usando blocos:\n");
		//Não dá para usar a função normal imprimir_submatriz porque aí pegará lixo de memória pelas outras posições, então farei aqui mesmo
		for(int j=0; j<mat_c->col; j++){
			printf ("\t(%d)", j);
		}
		printf("\n");
		for(int i= 0; i<mat_c->lin; i++){
			printf ("(%d)", i);
			for(int j=0; j<mat_c->col; j++){
				printf ("\t%d", mat_c->matriz[i][j]);
			}
			printf("\n");
		}*/

		liberar_submatriz(submatrizA);
		liberar_submatriz(submatrizB);
		liberar_submatriz(submatrizC);
		
		return mat_c;
}

 // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int multiplicar_submatriz (matriz_bloco_t *mat_suba, matriz_bloco_t *mat_subb, matriz_bloco_t *mat_subc) {
 	int col_mat_a, lin_mat_b;
	printf("\nSubmatriz A:\n");
	imprimir_submatriz(mat_suba);
	printf("\nSubmatriz B:\n");
	imprimir_submatriz(mat_subb);
	col_mat_a = (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio)+1;
	lin_mat_b = (mat_subb->bloco->lin_fim - mat_subb->bloco->lin_inicio)+1;

	assert ( col_mat_a == lin_mat_b);
	for (int i = mat_subc->bloco->lin_inicio;	i <= mat_subc->bloco->lin_fim; i++) {
		for (int j = mat_subc->bloco->col_inicio; j <= mat_subc->bloco->col_fim; j++) {
			for (int k = 0; k <= (mat_suba->bloco->col_fim - mat_suba->bloco->col_inicio); k++) {
				mat_subc->matriz[i][j] += mat_suba->matriz[i][(k+mat_suba->bloco->col_inicio)] * mat_subb->matriz[(k+mat_subb->bloco->lin_inicio)][j];
			}
		}
	}
 	return 0;
}