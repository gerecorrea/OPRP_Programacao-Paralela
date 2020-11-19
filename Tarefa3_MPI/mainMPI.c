#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "toolsv3.h"
#include "matrizv3.h"
#include "matriz-operacoesv3.h"
#include "matriz-operacoes-mpi.h"
#define MPIrun

const int tamLoop = 2;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int main(int argc, char *argv[]) {

	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	// DECLARAÇÃO de VARIÁVEIS
	char filename[100];
	FILE *fmat;
	int nr_line;
	int *vet_line = NULL;
	int N, M, La, Lb;
	double start_time, end_time, runtime;
	double timer_seqMult[2], timer_seqMultBloco[2];
	double timer_MultMPI[2], timer_seqMultBloco_MPI[2];

    mymatriz **mat_mult_MPI = NULL, **mat_mult_MPI_Bloco = NULL;
	mymatriz mat_a, mat_b, mat_c;
	mymatriz **mat_mult = NULL, **mat_multBloco = NULL;

	#ifdef MPIrun
		int rank, size;
	#endif
	
	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

	#ifdef MPIrun
		MPI_Init(&argc, &argv);
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	#endif
	if(!rank){
		if ((argc != 3)) {
		printf("Parametros corretos: mpirun -np <nthreads> %s <matrizA> <matrizB>\n", argv[0]);
		// MPI_Abort(MPI_COMM_WORLD, rc);
		exit(1);
		}
	}

	#ifdef MPIrun
	if (!rank) {
	#endif
		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//                Leitura da Matriz A (arquivo)
		fmat = fopen(argv[1],"r");
		if (fmat == NULL) {
			printf("Error: Na abertura dos arquivos.");
			exit(1);
		}

		//ntasks = size;

		extrai_parametros_matriz(fmat, &N, &La, &vet_line, &nr_line);
		mat_a.matriz = NULL;
		mat_a.lin = N;
		mat_a.col = La;
		if (mmalocar(&mat_a)) {
			printf ("ERROR: Out of memory\n");
		}
		filein_matriz (mat_a.matriz, N, La, fmat, vet_line, nr_line);
		free (vet_line);
		fclose(fmat);
		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

		// %%%%%%%%%%%%%%%%%%%% %%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//               Leitura da Matriz B (arquivo)
		fmat = fopen(argv[2],"r");
		if (fmat == NULL) {
			printf("Error: Na abertura dos arquivos.");
			exit(1);
		}
		extrai_parametros_matriz(fmat, &Lb, &M, &vet_line, &nr_line);
		mat_b.matriz = NULL;
		mat_b.lin = Lb;
		mat_b.col = M;
		if (mmalocar(&mat_b)) {
			printf ("ERROR: Out of memory\n");
		}
		filein_matriz (mat_b.matriz, Lb, M, fmat, vet_line, nr_line);
		free (vet_line);
		fclose(fmat);
		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
	#ifdef MPIrun
	}
	if (!rank) {
	#endif
		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//               Multiplicação Sequencial

		for(int k = 0; k < tamLoop;k++){
			runtime = 0;
			mat_mult = (mymatriz **) malloc (sizeof(mymatriz *)*6);
			for (int ii=0; ii < 6; ii++) {
				printf("\n ##### multiplicar_t%d de Matrizes #####\n", ii);

				start_time = wtime();
				mat_mult[ii] = mmultiplicar(&mat_a, &mat_b, ii);
				end_time = wtime();				

				mimprimir(mat_mult[ii]);
				printf("\tRuntime: %f\n", end_time - start_time); 
				runtime = runtime + (end_time - start_time);
				sprintf(filename, "MATRIZ_SeqC_%d_%d.result", ii,k);
				fmat = fopen(filename,"w");
				fileout_matriz(mat_mult[ii], fmat);
				fclose(fmat);
			}
			timer_seqMult[k] = runtime / 6;

			// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
			//              Comparação dos resultados

			printf("\n ##### Comparação dos resultados da Multiplicação de matrizes #####\n");
			for (int i=1; i<6; i++) {
				printf("[mult_t0 vs mult_t%d]\t", i);
				mcomparar (mat_mult[0],mat_mult[i]);
			}
			// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

		}
		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
	#ifdef MPIrun
	}
		
	if (!rank) {
	#endif

		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//               Multiplicação Sequencial em Bloco
		int n, orientacaoA, orientacaoB = -1;

		mat_multBloco = (mymatriz **) malloc (sizeof(mymatriz *)*6);
		mat_c.matriz = NULL;
		mat_c.lin = mat_a.lin;
		mat_c.col = mat_b.col;
		if (mmalocar(&mat_c)) {
			printf ("ERROR: Out of memory\n");
		}

		n = 2; //definindo o numero de cortes nas matrizes
		orientacaoA = 0; //declarando orientação da matriz A como vertical

		//A orientação deve ser invertida para as matrizes A e B
		if(orientacaoA == 1){
			orientacaoB = 0;
		}else{
			orientacaoB = 1;
		}

		matriz_bloco_t **matrizResA = NULL, **matrizResB = NULL, **matrizResC= NULL;

		matrizResA = (matriz_bloco_t **) malloc (sizeof(matriz_bloco_t *));
		matrizResB = (matriz_bloco_t **) malloc (sizeof(matriz_bloco_t *));

		//if(n > que o possível de cortes mínimos)
		//Exceções para o valor de n
		if(orientacaoA == 0 && (mat_a.col < n || mat_b.lin < n)){
			printf ("\nLimite de parcionamento excedido\n");
			exit(1);
		}
		else if(orientacaoA == 1 && (mat_a.lin < n || mat_b.col < n)){
			printf ("\nLimite de parcionamento excedido\n");
			exit(1);
		}
		if(n < 2 || n > 20){
			printf ("\nLimite de parcionamento excedido\n");
			exit(1);
		}
		
		double start_part, end_part;
		
		for (int i = 0; i < tamLoop; i++){
			timer_seqMultBloco[i] = 0;
			runtime = 0;
			start_part = wtime();
			matrizResA = particionar_matriz(mat_a.matriz, mat_a.lin, mat_a.col, orientacaoA, n);
			matrizResB = particionar_matriz(mat_b.matriz, mat_b.lin, mat_b.col, orientacaoB, n);
			end_part = wtime();

			matrizResC = constroi_submatrizv2 (mat_a.lin, mat_b.col, n);

			//Laço para fazer a multiplicação das submatrizes
			for(int j = 0; j < n; j++){
				printf("\n    ##### Multiplicação das submatrizes %d #####\n", j);
				//Laço para as permutações
				for (int ii=0; ii < 6; ii++) {
					printf("\n ##### multiplicar_t%d de Submatrizes #####\n", ii);
					printf("\tAguarde...\n");
					start_time = wtime();
					mat_multBloco[ii] = multiplicar_submatriz(matrizResA[j], matrizResB[j], matrizResC[j], ii);
					end_time = wtime();
					runtime = runtime + (end_time-start_time);			
				}
				timer_seqMultBloco[i] = timer_seqMultBloco[i] + (runtime / 6);
				runtime = 0;

				// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
				//              Comparação dos resultados

				printf("\n ##### Comparação dos resultados da Multiplicação de submatrizes #####\n");
				for (int i=1; i<6; i++) {
					printf("[mult_t0 vs mult_t%d]\t", i);
					mcomparar (mat_multBloco[0],mat_multBloco[i]);
				}
				// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

			}
			timer_seqMultBloco[i] = timer_seqMultBloco[i] / n;
			timer_seqMultBloco[i] = timer_seqMultBloco[i] + (end_part - start_part);

			//Zerando matriz resultante
			for(int i=0; i<mat_c.lin;i++){
				for(int j=0;j<mat_c.col; j++){
					mat_c.matriz[i][j] = 0;
				}
			}

			//Gerando matriz resultante final
			if(orientacaoA == 0){
				for(int i=0;i<mat_c.lin;i++){
					for(int j=0;j<mat_c.col; j++){
						for(int k=0; k<n; k++){
							mat_c.matriz[i][j] += matrizResC[k]->matriz[i][j];
						}
					}
				}
			}else{
				for(int i=0; i<mat_c.lin; i++){
					for(int j=0; j< mat_c.col;j++){
						mat_c.matriz[i][j] = mat_multBloco[0]->matriz[i][j];
					}
				}
			}

			printf("\n##### Matriz Resultante final #####\n");
			for(int j=0; j<mat_c.col; j++)
				printf("\t(%d)", j);
			printf("\n");
			for(int i=0; i< mat_c.lin;i++){
				printf("(%d)", i);
				for(int j=0; j<mat_c.col;j++){
					printf("\t%d", mat_c.matriz[i][j]);
				}
				printf("\n");
			}

			sprintf(filename, "MATRIZ_SeqBLC%d.result", i);
			fmat = fopen(filename,"w");

			for(int j = 0; j < n; j++){
				fileout_matriz(&mat_c, fmat);
			}

			fclose(fmat);
		}

		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
	#ifdef MPIrun
	}
	#endif
    
	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//               Multiplicaçã normal por MPI

	printf("\n ##### Multiplicação de Matrizes (MPI) #####\n");	
	mat_mult_MPI = (mymatriz **) malloc (sizeof(mymatriz *)*2);
	
	for(int k = 0; k < tamLoop;k++){
		
		if (!rank) {
            printf("##### Multiplicação normal com OpenMPI (mult_openmpi%d) #####\n", k);
            start_time = wtime();
            mat_mult_MPI[k] = multiplica_MPI(&mat_a, &mat_b, rank, size);
            end_time = wtime();
			mimprimir(mat_mult_MPI[k]);	
			timer_MultMPI[k] = end_time - start_time;
			sprintf(filename, "MATRIZ_MPI__%d.result",k);
			fmat = fopen(filename,"w");
			fileout_matriz(mat_mult_MPI[k], fmat);
			fclose(fmat);
		}else{
			multiplica_MPI(&mat_a, &mat_b, rank, size);
		}
		
	}
		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
	//               Multiplicação em bloco por MPI
		
	printf("\n ##### Multiplicação de Matrizes (bloco MPI) #####\n");
	mat_mult_MPI_Bloco = (mymatriz **) malloc (sizeof(mymatriz *)*2);
	
	for(int k = 0; k < tamLoop;k++){
		
		if (!rank) {
            printf("##### Multiplicação em bloco com OpenMPI (mult_openmpi%d) #####\n", k);
            start_time = wtime();
            mat_mult_MPI_Bloco[k] = multiplica_MPI(&mat_a, &mat_b, rank, size);
            end_time = wtime();
			mimprimir(mat_mult_MPI_Bloco[k]);	
			timer_seqMultBloco_MPI[k] = end_time - start_time;
			sprintf(filename, "MATRIZ_MPI_BLOCO__%d.result",k);
			fmat = fopen(filename,"w");
			fileout_matriz(mat_mult_MPI_Bloco[k], fmat);
			fclose(fmat);
		}else{
			multiplica_MPI(&mat_a, &mat_b, rank, size);
		}
		
	}
		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

	if (!rank) {
	//#endif
		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		//              Comparação dos resultados e tempos

		printf("\n ##### Comparação dos resultados da Multiplicação de matrizNormalSeq x MatrizBlocoSeq #####\n");
		printf("matriz_normal_seq_t0 vs matriz_seq_bloco_t0\t");
		mcomparar (mat_mult[0], &mat_c);

		printf("\n ##### Comparação dos resultados da Multiplicação de matrizNormalSeq c/ matrizNormalMPI #####\n");
		printf("matriz_normal_seq_t0 vs matriz_normal_mpi_t0\t");
		mcomparar (mat_mult[0], mat_mult_MPI[0]);
		
		printf("\n ##### Comparação dos resultados da Multiplicação de matrizNormalSeq c/ matrizBlocoMPI #####\n");
		printf("matriz_normal_seq_t0 vs matriz_bloco_MPI_t0\t");
		mcomparar (mat_mult[0], mat_mult_MPI_Bloco[0]);
		
		printf("\n ##### Tempo Médio Matriz Sequencial #####\n");
		double soma = 0;
		for(int i = 0; i < tamLoop; i++){
			soma += timer_seqMult[i];
		}
		soma /= tamLoop;
		printf("\tRuntime: %f\n", soma);

		printf("\n ##### Tempo Médio Matriz Sequencial em Bloco #####\n");
		double somaBlocoSeq = 0;
		for(int i = 0; i < tamLoop; i++){
			somaBlocoSeq += timer_seqMultBloco[i];
		}
		somaBlocoSeq /= tamLoop;
		printf("\tRuntime: %f\n", somaBlocoSeq);

		printf("\n ##### Tempo Médio Matriz em MPI #####\n");
		double somaMPI = 0;
		for(int i = 0; i < tamLoop; i++){
			somaMPI += timer_MultMPI[i];
		}
		somaMPI /= tamLoop;
		printf("\tRuntime: %f\n", somaMPI);

		printf("\n ##### Tempo Médio Matriz em Bloco MPI #####\n");
		double somaMPI_Bloco = 0;
		for(int i = 0; i < tamLoop; i++){
			somaMPI_Bloco += timer_seqMultBloco_MPI[i];
		}
		somaMPI_Bloco /= tamLoop;
		printf("\tRuntime: %f\n", somaMPI_Bloco);

		// ########################################################
	}	

	// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%

	if (!rank) {
	//#endif
		// %%%%%%%%%%%%%%%%%%%%%%%% BEGIN %%%%%%%%%%%%%%%%%%%%%%%%
		
		for (int ii=0; ii < 6; ii++) {
			mmliberar(mat_mult[ii]);
			free (mat_mult[ii]);
		}
		free(mat_mult);

		for (int ii=0; ii < 6; ii++) {
			mmliberar(mat_multBloco[ii]);
			free (mat_multBloco[ii]);
		}
		free(mat_multBloco);

		for (int ii=0; ii < 2; ii++) {
			mmliberar(mat_mult_MPI[ii]);
			free (mat_mult_MPI[ii]);
		}
		free(mat_mult_MPI);

		for (int ii=0; ii < 2; ii++) {
			mmliberar(mat_mult_MPI_Bloco[ii]);
			free (mat_mult_MPI_Bloco[ii]);
		}
		free(mat_mult_MPI_Bloco);

		mmliberar(&mat_a);
		mmliberar(&mat_b);
		mmliberar(&mat_c);
		
		// %%%%%%%%%%%%%%%%%%%%%%%% END %%%%%%%%%%%%%%%%%%%%%%%%
	}
	MPI_Finalize();
	
	return 0;
}
